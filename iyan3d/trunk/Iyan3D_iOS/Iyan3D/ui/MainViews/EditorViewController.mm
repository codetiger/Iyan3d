//
//  EditorViewController.m
//  Iyan3D
//
//  Created by Sankar on 18/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "EditorViewController.h"
#import "FrameCellNew.h"
#import "AppDelegate.h"
#import "WEPopoverContentViewController.h"
#import "SceneSelectionControllerNew.h"
#import "Constants.h"

@implementation EditorViewController

#define EXPORT_POPUP 1
#define ANIMATION_POPUP 2
#define IMPORT_POPUP 3
#define INFO_POPUP 4
#define VIEW_POPUP 5

#define EXPORT_IMAGE 0
#define EXPORT_VIDEO 1

#define APPLY_ANIMATION 0
#define SAVE_ANIMATION 1

#define IMPORT_MODELS 0
#define IMPORT_IMAGES 1
#define IMPORT_TEXT 2
#define IMPORT_LIGHT 3
#define IMPORT_OBJFILE 4
#define IMPORT_ADDBONE 5

#define ABOUT 0
#define HELP 1
#define TUTORIAL 2
#define SETTINGS 3
#define CONTACT_US 4

#define FRONT_VIEW 0
#define TOP_VIEW 1
#define LEFT_VIEW 2
#define BACK_VIEW 3
#define RIGHT_VIEW 4
#define BOTTOM_VIEW 5

#define RENDER_IMAGE 0
#define RENDER_VIDEO 1
#define RENDER_GIF 23

#define ADD_OBJECT 100
#define REMOVE_OBJECT 200

#define HIDE_PROGRESS 0
#define SHOW_PROGRESS 1

#define ASSET_ANIMATION 4
#define ASSET_RIGGED 1
#define ASSET_BACKGROUNDS 2
#define ASSET_ACCESSORIES 3
#define ASSET_OBJ 6
#define ASSET_CAMERA 7
#define ASSET_LIGHT 8
#define ASSET_IMAGE 9
#define ASSET_TEXT 10
#define ASSET_ADDITIONAL_LIGHT 900

#define ADD_BUTTON_TAG 99

#define UNDEFINED_OBJECT -1

#define OK_BUTTON_INDEX 1
#define CANCEL_BUTTON_INDEX 0

BOOL missingAlertShown;

#pragma mark NativeMethods

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil SceneItem:(SceneItem*)scene selectedindex:(int)index
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        currentScene = scene;
        IndexOfSelected = index;
        cache = [CacheSystem cacheSystem];
        constants::BundlePath = (char*)[[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSASCIIStringEncoding];
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* cachesDir = [paths objectAtIndex:0];
        constants::CachesStoragePath = (char*)[cachesDir cStringUsingEncoding:NSASCIIStringEncoding];
        isViewLoaded = false;
        lightCount = 1;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    [_center_progress setHidden:NO];
    [_center_progress startAnimating];
    
    #if !(TARGET_IPHONE_SIMULATOR)
        if (iOSVersion >= 8.0)
            isMetalSupported = false;//(MTLCreateSystemDefaultDevice() != NULL) ? true : false;
    #endif
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    CGFloat screenWidth = screenRect.size.width;
    screenHeight = screenRect.size.height;
    constants::BundlePath = (char*)[[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSASCIIStringEncoding];

    totalFrames = 24;
    [[UIApplication sharedApplication] setStatusBarHidden:NO];
    
    if ([Utility IsPadDevice])
         [self.framesCollectionView registerNib:[UINib nibWithNibName:@"FrameCellNew" bundle:nil] forCellWithReuseIdentifier:@"FRAMECELL"];
    else
         [self.framesCollectionView registerNib:[UINib nibWithNibName:@"FrameCellNewPhone" bundle:nil] forCellWithReuseIdentifier:@"FRAMECELL"];

    assetsInScenes = [NSMutableArray array];
    
    [self.objectList reloadData];    
    [self initScene];
    [self createDisplayLink];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    [_center_progress stopAnimating];
    [_center_progress setHidden:YES];
}



- (void)initScene
{
    float screenScale = [[UIScreen mainScreen] scale];
    renderViewMan = [[RenderViewManager alloc] init];
    renderViewMan.delegate = self;
    if (isMetalSupported) {
        [[AppDelegate getAppDelegate] initEngine:METAL ScreenWidth:SCREENWIDTH ScreenHeight:SCREENHEIGHT ScreenScale:screenScale renderView:_renderView];
         smgr = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        editorScene = new SGEditorScene(METAL, smgr, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
    }
    else {
        [renderViewMan setupLayer:_renderView];
        [renderViewMan setupContext];
        
        [[AppDelegate getAppDelegate] initEngine:OPENGLES2 ScreenWidth:SCREENWIDTH ScreenHeight:SCREENHEIGHT ScreenScale:screenScale renderView:_renderView];
        smgr = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        editorScene = new SGEditorScene(OPENGLES2, smgr, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
        editorScene->screenScale = screenScale;
        
        [renderViewMan setupDepthBuffer:_renderView];
        [renderViewMan setupRenderBuffer];
        [renderViewMan setupFrameBuffer:smgr];
    }
    if(editorScene) {
        if([Utility IsPadDevice]) {
            editorScene->topLimit = 64.0 * editorScene->screenScale;
            editorScene->rightLimit = self.leftView.frame.size.width * editorScene->screenScale;// 160.0;
        } else {
            editorScene->topLimit = 48.0 * editorScene->screenScale;
            editorScene->rightLimit = self.leftView.frame.size.width * editorScene->screenScale;//110.0;
        }
    }
    missingAlertShown = false;
    [renderViewMan setUpPaths];
    [renderViewMan setUpCallBacks:editorScene];
    editorScene->downloadMissingAssetCallBack = &downloadMissingAssetCallBack;
    [renderViewMan addGesturesToSceneView];
    [self loadScene];
}

- (bool) isMetalSupportedDevice
{
    return isMetalSupported;
}

- (void) reloadFrames
{
    [_framesCollectionView reloadData];
}

- (void) removeTempNodeFromScene
{
    if(editorScene && editorScene->loader)
        editorScene->loader->removeTempNodeIfExists();        
}

- (void) load3DTex:(int)type AssetId:(int)assetId TypedText:(NSString*)typedText FontSize:(int)fontSize BevelValue:(float)bevelRadius TextColor:(Vector4)colors
          FontPath:(NSString*)fontFileName isTempNode:(bool)isTempNode{

    NSMutableDictionary *textDetails = [[NSMutableDictionary alloc] init];
    [textDetails setObject:[NSNumber numberWithInt:assetId] forKey:@"AssetId"];
    [textDetails setObject:typedText forKey:@"typedText"];
    [textDetails setObject:[NSNumber numberWithInt:fontSize] forKey:@"fontSize"];
    [textDetails setObject:[NSNumber numberWithInt:bevelRadius] forKey:@"bevelRadius"];
    [textDetails setObject:[NSNumber numberWithFloat:colors.x] forKey:@"red"];
    [textDetails setObject:[NSNumber numberWithFloat:colors.y] forKey:@"green"];
    [textDetails setObject:[NSNumber numberWithFloat:colors.z] forKey:@"blue"];
    [textDetails setObject:[NSNumber numberWithFloat:colors.w] forKey:@"alpha"];
    [textDetails setObject:fontFileName forKey:@"fontFileName"];
    [textDetails setObject:[NSNumber numberWithBool:isTempNode] forKey:@"isTempNode"];
    assetAddType = IMPORT_ASSET_ACTION;
    [self performSelectorOnMainThread:@selector(load3dTextOnMainThread:) withObject:textDetails waitUntilDone:YES];

}

- (void) load3dTextOnMainThread:(NSMutableDictionary*)fontDetails{
    int assetId = [[fontDetails objectForKey:@"AssetId"]intValue];
    std::wstring assetName = [self getwstring:[fontDetails objectForKey:@"typedText"]];
    bool isTempNode = [[fontDetails objectForKey:@"isTempNode"]boolValue];
    int fontSize = [[fontDetails objectForKey:@"fontSize"]intValue];
    float bevalValue = [[fontDetails objectForKey:@"bevelRadius"]floatValue];
    
    NSLog(@"Is Temp Node : %d", isTempNode);

    [renderViewMan loadNodeInScene:ASSET_TEXT AssetId:assetId AssetName:assetName Width:fontSize Height:bevalValue isTempNode:isTempNode More:fontDetails ActionType:assetAddType];
}

- (void) importAdditionalLight{
    if(ShaderManager::lightPosition.size() < 5) {
        assetAddType = IMPORT_ASSET_ACTION;
        //editorScene->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, ASSET_ADDITIONAL_LIGHT + lightCount , "Light"+ to_string(lightCount));
        [self addLightToScene:[NSString stringWithFormat:@"Light%d",lightCount] assetId:ASSET_ADDITIONAL_LIGHT + lightCount ];
        lightCount++;
    } else {
        UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Scene cannot contain more than five lights." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [closeAlert show];
    }
}

-(void) addLightToScene:(NSString*)lightName assetId:(int)assetId
{
    [renderViewMan loadNodeInScene:ASSET_ADDITIONAL_LIGHT AssetId:assetId AssetName:[self getwstring:lightName] Width:20 Height:50 isTempNode:NO More:nil ActionType:assetAddType];
}

- (void) loadNodeInScene:(AssetItem*)assetItem ActionType:(ActionType)actionType
{
    assetAddType = actionType;
    [self performSelectorOnMainThread:@selector(loadNode:) withObject:assetItem waitUntilDone:YES];
    
}

- (void)loadNodeForImage:(NSMutableDictionary*)nsDict
{
    std::wstring saltedFileName = [self getwstring:[nsDict objectForKey:@"AssetName"]];
    int type = [[nsDict objectForKey:@"type"]intValue];
    int assetId = [[nsDict objectForKey:@"AssetId"]intValue];
    int imgWidth = [[nsDict objectForKey:@"Width"]intValue];
    int imgHeight = [[nsDict objectForKey:@"Height"]intValue];
    int isTempNode = [[nsDict objectForKey:@"isTempNode"]intValue];
    [renderViewMan loadNodeInScene:type AssetId:assetId AssetName:saltedFileName Width:imgWidth Height:imgHeight isTempNode:isTempNode More:nil ActionType:assetAddType];
}

- (void) loadNode:(AssetItem*) asset
{
    [renderViewMan loadNodeInScene:asset.type AssetId:asset.assetId AssetName:[self getwstring:asset.name] Width:0 Height:0 isTempNode:asset.isTempAsset More:nil ActionType:assetAddType];
}

- (void)createDisplayLink
{
    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateRenderer)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void) updateRenderer
{
    if(editorScene) {
        if (renderViewMan.checkCtrlSelection) {
            editorScene->selectMan->checkCtrlSelection(renderViewMan.touchMovePosition[0]);
            renderViewMan.checkCtrlSelection = false;
        }
        if (renderViewMan.checkTapSelection) {
            editorScene->selectMan->checkSelection(renderViewMan.tapPosition);
            renderViewMan.checkTapSelection = false;
            [renderViewMan showPopOver:editorScene->selectedNodeId];
        }
        if(renderViewMan.makePanOrPinch)
            [renderViewMan panOrPinchProgress];
        editorScene->renderAll();
        if(!isMetalSupported)
            [renderViewMan presentRenderBuffer];
    }
}

#pragma Touches Actions

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    editorScene->setLightingOff();
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    editorScene->setLightingOn();
}

#pragma Scene Loading

- (void)loadScene
{
    if (![self loadFromFile]) {
        if(renderViewMan){
            [renderViewMan addCameraLight];
        }
    }
    editorScene->updater->setDataForFrame(editorScene->currentFrame);
}

- (bool)loadFromFile
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* srcFilePath = [NSString stringWithFormat:@"%@/Projects", documentsDirectory];
    NSString* filePath = [NSString stringWithFormat:@"%@/%@.sgb", srcFilePath, currentScene.sceneFile];
    std::string* SGBFilePath = new std::string([filePath UTF8String]);
    
    NSDictionary* fileAttributes = [[NSFileManager defaultManager] attributesOfItemAtPath:filePath error:nil];
    NSNumber* fileSizeNumber = [fileAttributes objectForKey:NSFileSize];
    long long fileSize = [fileSizeNumber longLongValue];
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        if (fileSize == 0 || !editorScene->loadSceneData(SGBFilePath)) {
            [self.view endEditing:YES];
            UIAlertView* fileCorruptAlert = [[UIAlertView alloc] initWithTitle:@"File Corrupted" message:@"The project you are trying to open is corrupted." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil, nil];
            [fileCorruptAlert show];
            delete SGBFilePath;
            return false;
        }
    }
    else {
        delete SGBFilePath;
        return false;
    }
    
    totalFrames = editorScene->totalFrames;
    [self.framesCollectionView reloadData];
    [self reloadSceneObjects];
    delete SGBFilePath;
    return true;
}

- (void) reloadSceneObjects{
    NSUInteger assetsCount = editorScene->nodes.size();
    for (int i = 0; i < assetsCount; i++) {
        NSString* assetName = [self stringWithwstring:editorScene->nodes[i]->name];
    encoding:[NSString defaultCStringEncoding];
       // if (editorScene->nodes[i]->getType() == NODE_TEXT)
       //     assetName = [NSString stringWithFormat:@"TEXT: %@", assetName];
       // else if (editorScene->nodes[i]->getType() == NODE_IMAGE)
        //    assetName = [NSString stringWithFormat:@"IMAGE: %@", assetName];
        [assetsInScenes addObject:assetName];
    }
    [self.objectList reloadData];
}

#pragma mark - Frames Collection View Deligates

- (NSInteger)collectionView:(UICollectionView*)collectionView numberOfItemsInSection:(NSInteger)section
{
    return totalFrames;
}

- (FrameCellNew*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    FrameCellNew* cell = [self.framesCollectionView dequeueReusableCellWithReuseIdentifier:@"FRAMECELL" forIndexPath:indexPath];
    cell.backgroundColor = [UIColor colorWithRed:55.0f / 255.0f
                                                   green:56.0f / 255.0f
                                                    blue:57.0f / 255.0f
                                                   alpha:1.0f];
    
    cell.layer.borderWidth = 0.0f;
    cell.framesLabel.text = [NSString stringWithFormat:@"%d", (int)indexPath.row + 1];
    cell.framesLabel.adjustsFontSizeToFitWidth = YES;
    cell.framesLabel.minimumFontSize = 3.0;
    return cell;
}

- (void)collectionView:(UICollectionView*)collectionView didSelectItemAtIndexPath:(NSIndexPath*)indexPath
{
    NSIndexPath* toPath2 = [NSIndexPath indexPathForItem:indexPath.row inSection:0];
    UICollectionViewCell* todatasetCell2 = [self.framesCollectionView cellForItemAtIndexPath:toPath2];
    todatasetCell2.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                       green:156.0f / 255.0f
                                                        blue:156.0f / 255.0f
                                                       alpha:1.0f].CGColor;
    todatasetCell2.layer.borderWidth = 2.0f;
    [self NormalHighLight];
    editorScene->previousFrame = editorScene->currentFrame;
    editorScene->currentFrame = indexPath.row;
    editorScene->actionMan->switchFrame(editorScene->currentFrame);
    [self HighlightFrame];
}

#pragma mark - Object Selection Table View Deligates

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [assetsInScenes count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *simpleTableIdentifier = @"Cell";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:simpleTableIdentifier];
    }
    cell.textLabel.font=[cell.textLabel.font fontWithSize:13];
    cell.textLabel.text = [assetsInScenes objectAtIndex:indexPath.row];
    if([cell.textLabel.text isEqualToString:@"CAMERA"])
        cell.imageView.image = [UIImage imageNamed:@"My-objects-Camera_Pad.png"];
    else if([cell.textLabel.text isEqualToString:@"LIGHT"])
        cell.imageView.image = [UIImage imageNamed:@"My-objects-Light_Pad.png"];
    else if([cell.textLabel.text hasPrefix:@"TEXT"])
        cell.imageView.image = [UIImage imageNamed:@"My-objects-Text_Pad"];
    else if([cell.textLabel.text hasPrefix:@"IMAGE"])
        cell.imageView.image = [UIImage imageNamed:@"My-objects-Image_Pad"];
    else if([cell.textLabel.text hasPrefix:@"LIGHT"])
        cell.imageView.image = [UIImage imageNamed:@"My-objects-Light_Pad.png"];
    else
        cell.imageView.image = [UIImage imageNamed:@"My-objects-Models_Pad.png"];
    return cell;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        [renderViewMan removeNodeFromScene:(int)indexPath.row IsUndoOrRedo:NO];
        [tableView reloadData];
    }
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellEditingStyleDelete;
}

- (BOOL)tableView:(UITableView *)tableview shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath *)indexPath
{
    return YES;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.row > 1)
        return YES;
    
    return NO;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    [self objectSelectionCompleted:(int)indexPath.row];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

#pragma mark - Button Actions

- (IBAction)backToScenes:(id)sender {
    [self performSelectorOnMainThread:@selector(saveAnimationData) withObject:nil waitUntilDone:YES];
    [self loadSceneSelectionView];
}

- (IBAction)playButtonAction:(id)sender {
    [self playAnimation];
}

- (void) playAnimation{
    isPlaying = !isPlaying;
    editorScene->isPlaying = isPlaying;
    
    if (editorScene->selectedNodeId != NOT_SELECTED) {
        editorScene->selectMan->unselectObject(editorScene->selectedNodeId);
        [self.framesCollectionView reloadData];
    }
    if (isPlaying) {
        //[self setInterfaceVisibility:YES];
        
        [self.playBtn setImage:[UIImage imageNamed:@"Pause_Pad.png"] forState:UIControlStateNormal];
        playTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0f / 24.0f) target:self selector:@selector(playTimerTarget) userInfo:nil repeats:YES];
    }
    else {
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    }
}

- (void) playTimerTarget{
    if (editorScene->currentFrame + 1 < editorScene->totalFrames) {
        [self NormalHighLight];
        editorScene->currentFrame++;
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->currentFrame inSection:0];
        [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
        [self HighlightFrame];
        editorScene->setLightingOff();
        editorScene->actionMan->switchFrame((float)editorScene->currentFrame);
    }
    else if (editorScene->currentFrame + 1 >= editorScene->totalFrames) {
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    }
    else if (editorScene->currentFrame == editorScene->totalFrames) {
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    }
    editorScene->updater->setDataForFrame(editorScene->currentFrame);}

- (void) stopPlaying{
    editorScene->setLightingOn();
    [playTimer invalidate];
    playTimer = nil;
    if (isPlaying) {
        isPlaying = false;
        editorScene->isPlaying = false;
    }
    editorScene->actionMan->switchFrame(editorScene->currentFrame);
}

- (void)NormalHighLight
{
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->currentFrame inSection:0];
    UICollectionViewCell* todatasetCell = [self.framesCollectionView cellForItemAtIndexPath:toPath];
    todatasetCell.layer.borderColor = [UIColor colorWithRed:61.0f / 255.0f green:62.0f / 255.0f blue:63.0f / 255.0f alpha:1.0f].CGColor;
    todatasetCell.layer.borderWidth = 2.0f;
}

- (void)HighlightFrame
{
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->currentFrame inSection:0];
    UICollectionViewCell* todatasetCell = [self.framesCollectionView cellForItemAtIndexPath:toPath];
    todatasetCell.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                      green:156.0f / 255.0f
                                                       blue:156.0f / 255.0f
                                                      alpha:1.0f]
    .CGColor;
    todatasetCell.layer.borderWidth = 2.0f;
}


- (IBAction)editFunction:(id)sender
{
    if([self.objectList isEditing]) {
        [self.editobjectBtn setTitle:@"Edit" forState:UIControlStateNormal];
        [self.objectList setEditing:NO animated:YES];
    } else {
        [self.editobjectBtn setTitle:@"Done" forState:UIControlStateNormal];
        [self.objectList setEditing:YES animated:YES];
    }
}

- (IBAction)addFrames:(id)sender
{

    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"addFrames"];
    [_popUpVc.view setClipsToBounds:YES];
     
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popoverController.popoverContentSize = CGSizeMake(205.0, 130.0);
    self.popoverController.delegate =self;
    self.popUpVc.delegate=self;
    [self.popoverController presentPopoverFromRect:_addFrameBtn.frame
                                            inView:self.view
                          permittedArrowDirections:UIPopoverArrowDirectionUp
                                          animated:YES];
}

- (IBAction)exportAction:(id)sender
{

     _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"exportBtn"];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(204.0, 85.0);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    [_popUpVc.view setClipsToBounds:YES];
    self.popUpVc.delegate=self;
    self.popoverController.delegate =self;
    CGRect rect = _exportBtn.frame;
    rect = [self.view convertRect:rect fromView:_exportBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self.view
                          permittedArrowDirections:UIPopoverArrowDirectionRight
                                          animated:YES];
    }

- (IBAction)loginBtnAction:(id)sender {
    isLoggedin=NO;
    if(isLoggedin){
        if ([Utility IsPadDevice]){
            _loggedInVc = [[LoggedInViewController alloc] initWithNibName:@"LoggedInViewController" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_loggedInVc];
            self.popoverController.popoverContentSize = CGSizeMake(305, 525);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [_loggedInVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];
        }
        else {
            _loggedInVc = [[LoggedInViewController alloc] initWithNibName:@"LoggedInViewControllerPhone" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_loggedInVc];
            self.popoverController.popoverContentSize = CGSizeMake(230.0, 250.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [_loggedInVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];
        }
        
    }
    else{
        if ([Utility IsPadDevice]){
            _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"loginBtn"];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
            self.popoverController.popoverContentSize = CGSizeMake(300, 360.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [_popUpVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];

        }
        else{
            _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewControllerPhone" bundle:nil clickedButton:@"loginBtn"];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
            self.popoverController.popoverContentSize = CGSizeMake(228.00, 274.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [_popUpVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];

        }
    
    }
}

- (IBAction)animationBtnAction:(id)sender
{

    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"animationBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(204.0, 85.0);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popoverController.delegate =self;
    self.popUpVc.delegate=self;
    CGRect rect = _animationBtn.frame;
    rect = [self.view convertRect:rect fromView:_animationBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self.view
                          permittedArrowDirections:UIPopoverArrowDirectionRight
                                          animated:YES];
}

- (IBAction)importBtnAction:(id)sender
{

    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"importBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popoverController.popoverContentSize = CGSizeMake(205.0, 260.0);
    self.popoverController.delegate =self;
    self.popUpVc.delegate=self;
    CGRect rect = _importBtn.frame;
    rect = [self.view convertRect:rect fromView:_importBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self.view
                          permittedArrowDirections:UIPopoverArrowDirectionRight
                                          animated:YES];
}
- (IBAction)infoBtnAction:(id)sender
{
    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"infoBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(180.0, 213.0);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popoverController.delegate =self;
    self.popUpVc.delegate=self;
    CGRect rect = _infoBtn.frame;
    rect = [self.view convertRect:rect fromView:_infoBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self.view
                          permittedArrowDirections:UIPopoverArrowDirectionUp
                                          animated:YES];
}

- (IBAction)viewBtn:(id)sender
{

    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"viewBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
     self.popoverController.popoverContentSize = CGSizeMake(182.0, 265.0);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popoverController.delegate =self;
    self.popUpVc.delegate=self;
    [self.popoverController presentPopoverFromRect:_viewBtn.frame
                                            inView:self.view
                          permittedArrowDirections:UIPopoverArrowDirectionUp
                                          animated:YES];

}

- (IBAction)lastFrameBtnAction:(id)sender
{
    [self NormalHighLight];
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:totalFrames-1 inSection:0];
    [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    editorScene->previousFrame = editorScene->currentFrame;
    editorScene->currentFrame = editorScene->totalFrames - 1;
    editorScene->actionMan->switchFrame(editorScene->currentFrame);
    [self HighlightFrame];
}

- (IBAction)firstFrameBtnAction:(id)sender
{
    [self NormalHighLight];
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:0 inSection:0];
    [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    editorScene->previousFrame = editorScene->currentFrame;
    editorScene->currentFrame = 0;
    editorScene->actionMan->switchFrame(editorScene->currentFrame);
    [self HighlightFrame];
}

- (IBAction)myObjectsBtnAction:(id)sender {
    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"myObjectsBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popoverController.popoverContentSize = CGSizeMake(205.0, 260.0);
    self.popoverController.delegate =self;
    self.popUpVc.delegate=self;
    CGRect rect = _myObjectsBtn.frame;
    rect = [self.view convertRect:rect fromView:_myObjectsBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self.view
                          permittedArrowDirections:UIPopoverArrowDirectionRight
                                          animated:YES];
}


- (IBAction)optionsBtnAction:(id)sender
{
  
    if(!editorScene->isNodeSelected || (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_UNDEFINED))
    {
        return;
    }

    if(editorScene->isNodeSelected && (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT || editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT))
    {
        Quaternion lightProps;
        if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT)
            lightProps = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(editorScene->currentFrame,editorScene->nodes[editorScene->selectedNodeId]->rotationKeys,true);
        if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT){
            Vector3 mainLight = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(editorScene->currentFrame, editorScene->nodes[editorScene->selectedNodeId]->scaleKeys);
            lightProps = Quaternion(mainLight.x,mainLight.y,mainLight.z,1.0);
        }
        
        _lightProp = [[LightProperties alloc] initWithNibName:@"LightProperties" bundle:nil LightColor:lightProps];
        _lightProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_lightProp];
        self.popoverController.popoverContentSize = CGSizeMake(270, 300);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [_lightProp.view setClipsToBounds:YES];
        CGRect rect = _optionsBtn.frame;
        rect = [self.view convertRect:rect fromView:_optionsBtn.superview];
        [self.popoverController presentPopoverFromRect:rect
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionRight
                                              animated:NO];
    }
    else if(editorScene->isNodeSelected && (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_CAMERA))
    {
        float fovValue = editorScene->cameraFOV;
        NSInteger resolutionType = editorScene->cameraResolutionType;
        _camProp = [[CameraSettings alloc] initWithNibName:@"CameraSettings" bundle:nil FOVvalue:fovValue ResolutionType:resolutionType];
        _camProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_camProp];
        self.popoverController.popoverContentSize = CGSizeMake(300 , 200);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [_camProp.view setClipsToBounds:YES];
        CGRect rect = _optionsBtn.frame;
        rect = [self.view convertRect:rect fromView:_optionsBtn.superview];
        [self.popoverController presentPopoverFromRect:rect
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionRight
                                              animated:NO];
   
    }
    else
    {
        float brightnessValue = editorScene->nodes[editorScene->selectedNodeId]->props.brightness;
        float specularValue = editorScene->nodes[editorScene->selectedNodeId]->props.shininess;
        bool isLightningValue = editorScene->nodes[editorScene->selectedNodeId]->props.isLighting;
        bool isVisibleValue = editorScene->nodes[editorScene->selectedNodeId]->props.isVisible;
        _meshProp = [[MeshProperties alloc] initWithNibName:@"MeshProperties" bundle:nil BrightnessValue:brightnessValue SpecularValue:specularValue LightningValue:isLightningValue Visibility:isVisibleValue];
        _meshProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_meshProp];
        self.popoverController.popoverContentSize = CGSizeMake(300 , 300);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [_meshProp.view setClipsToBounds:YES];
        CGRect rect = _optionsBtn.frame;
        rect = [self.view convertRect:rect fromView:_optionsBtn.superview];
        [self.popoverController presentPopoverFromRect:rect
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionRight
                                              animated:NO];
    }
}

- (IBAction)moveBtnAction:(id)sender
{
    editorScene->controlType = MOVE;
    editorScene->updater->updateControlsOrientaion();
}

- (IBAction)rotateBtnAction:(id)sender
{
    editorScene->controlType = ROTATE;
    editorScene->updater->updateControlsOrientaion();
}

- (IBAction)scaleBtnAction:(id)sender
{
    if(editorScene->isNodeSelected && (editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_CAMERA && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_LIGHT && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_ADDITIONAL_LIGHT)){
        
        editorScene->controlType = SCALE;
        editorScene->updater->updateControlsOrientaion();

        /*
        
        editorScene->renHelper->setControlsVisibility(false);
        Vector3 currentScale = editorScene->getSelectedNodeScale();
        _scaleProps = [[ScaleViewController alloc] initWithNibName:@"ScaleViewController" bundle:nil updateXValue:currentScale.x updateYValue:currentScale.y updateZValue:currentScale.z];
        _scaleProps.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_scaleProps];
        self.popoverController.popoverContentSize = CGSizeMake(270, 177);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [_popUpVc.view setClipsToBounds:YES];
        CGRect rect = _scaleBtn.frame;
        rect = [self.view convertRect:rect fromView:_scaleBtn.superview];
        [self.popoverController presentPopoverFromRect:rect
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionRight
                                              animated:NO];
         */
    }
}

- (IBAction)undoBtnAction:(id)sender
{
    int returnValue2 = NOT_SELECTED;
    NSLog(@"Undo Return Value : %d",returnValue2);
    ACTION_TYPE actionType = (ACTION_TYPE)editorScene->undo(returnValue2);    
    switch (actionType) {
        case DO_NOTHING: {
            break;
        }
        case DELETE_ASSET: {
            int nodeIndex = returnValue2;
            if (nodeIndex < assetsInScenes.count) {
                [renderViewMan removeNodeFromScene:nodeIndex IsUndoOrRedo:YES];
            }
            break;
        }
        case ADD_TEXT_IMAGE_BACK: {
            std::wstring name = editorScene->nodes[editorScene->nodes.size() - 1]->name;
            [self updateAssetListInScenes:ASSET_TEXT assetName:[NSString stringWithFormat:@"TEXT %@",[self stringWithwstring:name]] actionType:(int)ADD_OBJECT removeObjectAtIndex:(int)UNDEFINED_OBJECT];
            break;
        }
        case ADD_ASSET_BACK: {
            int assetId = returnValue2;
            assetAddType = UNDO_ACTION;
            if(assetId > 900 && assetId < 1000) {
                int numberOfLight = assetId - 900;
                [self addLightToScene:[NSString stringWithFormat:@"Light%d",numberOfLight] assetId:assetId];
            } else {
                AssetItem* assetObject = [cache GetAsset:assetId];
                [self loadNodeInScene:assetObject ActionType:assetAddType];
            }
            break;
        }
        case SWITCH_FRAME: {
            NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->currentFrame inSection:0];
            [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
            [self HighlightFrame];
            [self.framesCollectionView reloadData];
            break;
        }
        case RELOAD_FRAMES: {
            [self.framesCollectionView reloadData];
            break;
        }
        case SWITCH_MIRROR:
            //self.mirrorSwitch.on = animationScene->getMirrorState();
            break;
        default: {
            break;
        }
    }
}

- (IBAction)redoBtnAction:(id)sender
{
    int returnValue = editorScene->redo();
    
    NSLog(@"Redo Return Value : %d ",returnValue);

    if (returnValue == DO_NOTHING) {
        //DO NOTHING
    }
    else if (returnValue == DELETE_ASSET) {
        if (editorScene->selectedNodeId < assetsInScenes.count) {
            NSLog(@"Seleced Node Id %d",editorScene->selectedNodeId);
            [renderViewMan removeNodeFromScene:editorScene->selectedNodeId IsUndoOrRedo:YES];
        }
    }
    else if (returnValue == ADD_TEXT_IMAGE_BACK) {
        std::wstring name = editorScene->nodes[editorScene->nodes.size() - 1]->name;
        [self updateAssetListInScenes:ASSET_TEXT assetName:[NSString stringWithFormat:@"TEXT %@",[self stringWithwstring:name]] actionType:(int)ADD_OBJECT removeObjectAtIndex:(int)UNDEFINED_OBJECT];
    }
    else if (returnValue == SWITCH_MIRROR) {
        //self.mirrorSwitch.on = animationScene->getMirrorState();
    }
    else {
        if (returnValue != DEACTIVATE_UNDO && returnValue != DEACTIVATE_REDO && returnValue != DEACTIVATE_BOTH) {
            //importPressed = NO;
            assetAddType = REDO_ACTION;
            int assetId = returnValue;
            if(assetId > 900 && assetId < 1000) {
                int numberOfLight = assetId - 900;
                [self addLightToScene:[NSString stringWithFormat:@"Light%d",numberOfLight] assetId:assetId];
            } else {
                AssetItem* assetObject = [cache GetAsset:assetId];
                [self loadNodeInScene:assetObject ActionType:assetAddType];
            }
        }
    }
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->currentFrame inSection:0];
        [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
        [self HighlightFrame];
        [self.framesCollectionView reloadData];

}




- (void) presentViewControllerInCurrentView:(UIViewController*) vcToPresent
{
    [self presentViewController:vcToPresent animated:YES completion:nil];
}

#pragma mark - Rendering ViewController Delegates

- (void) changeLightProps:(Quaternion)lightProps Distance:(float)distance isStoredProperty:(BOOL)isStored{
    
    if(!isStored)
        editorScene->actionMan->changeLightProperty(lightProps.x, lightProps.y, lightProps.z, lightProps.w,true);
    else if(isStored)
        editorScene->actionMan->storeLightPropertyChangeAction(lightProps.x, lightProps.y, lightProps.z, lightProps.w);
}

- (void)applyAnimationToSelectedNode:(NSString*)filePath SelectedNodeId:(int)originalId SelectedFrame:(int)selectedFrame
{
    editorScene->animMan->copyKeysOfNode(originalId, editorScene->nodes.size()-1);
    editorScene->nodes[originalId]->node->setVisible(false);
    editorScene->currentFrame = selectedFrame;
    editorScene->actionMan->switchFrame(selectedFrame);   
    std::string *filePathStr = new std::string([filePath UTF8String]);
    editorScene->animMan->applyAnimations(*filePathStr, editorScene->selectedNodeId);
    totalFrames = editorScene->totalFrames;
    [_framesCollectionView reloadData];
    [self playAnimation];
    
}

- (void) removeTempAnimation{
    totalFrames = editorScene->totalFrames;
    [_framesCollectionView reloadData];
}

- (void)resumeRenderingAnimationScene
{
//    animationScene->isExportingImages = false;
//    animationScene->setDataForFrame(animationScene->currentFrame);
    CATransition* transition1 = [CATransition animation];
    transition1.duration = 0.5;
    transition1.type = kCATransitionPush;
    transition1.subtype = kCATransitionFromRight;
    [transition1 setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
    [self.rightView.layer addAnimation:transition1 forKey:kCATransition];
    [self.rightView setHidden:NO];
    
}

- (void)clearFolder:(NSString*)dirPath
{
    NSFileManager* fileManager = [NSFileManager defaultManager];
    NSError* error;
    NSArray* cacheFiles = [fileManager contentsOfDirectoryAtPath:dirPath error:&error];
    for (NSString* file in cacheFiles) {
        error = nil;
        NSString* filePath = [dirPath stringByAppendingPathComponent:file];
        [fileManager removeItemAtPath:filePath error:&error];
    }
}

#pragma mark - Other Delegate Functions

- (void)scalePropertyChanged:(float)XValue YValue:(float)YValue ZValue:(float)ZValue
{
    
    NSLog(@"Scale Values : %f %f %f",XValue ,YValue,ZValue);
    
    if(editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size())
        return;
    editorScene->actionMan->changeObjectScale(Vector3(XValue, YValue, ZValue), false);
    //isFileSaved = !(animationScene->changeAction = true);
    [_framesCollectionView reloadData];
}
- (void)scaleValueForAction:(float)XValue YValue:(float)YValue ZValue:(float)ZValue
{
    
    NSLog(@"Scale Values : %f %f %f",XValue ,YValue,ZValue);

    if(editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size())
        return;
    editorScene->actionMan->changeObjectScale(Vector3(XValue, YValue, ZValue), false);
    //[self showTipsViewForAction:OBJECT_MOVED];
    //isFileSaved = !(animationScene->changeAction = true);
    [_framesCollectionView reloadData];
    //[self undoRedoButtonState:DEACTIVATE_BOTH];
}

- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd
{
    if(showView)
        [self.leftView addSubview:subViewToAdd];
    
    [self.leftView setHidden:(!showView)];
    CATransition *transition = [CATransition animation];
    transition.duration = 0.5;
    transition.type = kCATransitionPush;
    transition.subtype = (showView) ? kCATransitionFromLeft : kCATransitionFromRight;
    [transition setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
    [self.leftView.layer addAnimation:transition forKey:nil];
    
    CATransition* transition1 = [CATransition animation];
    transition1.duration = 0.5;
    transition1.type = kCATransitionPush;
    transition1.subtype = (showView) ? kCATransitionFromLeft : kCATransitionFromRight;
    [transition1 setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
    [self.rightView.layer addAnimation:transition1 forKey:kCATransition];
    
    [self.rightView setHidden:showView];
}

-(void)pickedImageWithInfo:(NSDictionary*)info type:(BOOL)isTempNode;
{

    [self.center_progress setHidden:NO];
    [self.center_progress startAnimating];
    UIImage* imageData = [info objectForKey:UIImagePickerControllerOriginalImage];
    imgSalt = [[info objectForKey:UIImagePickerControllerReferenceURL] absoluteString];
    
    float imgW = imageData.size.width;
    float imgH = imageData.size.height;
    float bigSide = (imgW >= imgH) ? imgW : imgH;
    float target = 0;
    
    if (bigSide <= 128)
        target = 128;
    else if (bigSide <= 256)
        target = 256;
    else if (bigSide <= 512)
        target = 512;
    else
        target = 1024;
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(target, target), NO, 1.0);
    [imageData drawInRect:CGRectMake(0, 0, target, target)];
    UIImage* nImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cachesDirectory = [paths objectAtIndex:0];
    NSString* fileNameSalt = [Utility getMD5ForString:imgSalt];
    NSString* saveFileName = [NSString stringWithFormat:@"%@/%@.png", cachesDirectory, fileNameSalt];
    NSData* data = UIImagePNGRepresentation(nImage);
    [data writeToFile:saveFileName atomically:YES];
    
    fileNameSalt = [Utility getMD5ForString:imgSalt];
    
    NSMutableDictionary *imageDetails = [[NSMutableDictionary alloc] init];
        [imageDetails setObject:[NSNumber numberWithInt:ASSET_IMAGE] forKey:@"type"];
        [imageDetails setObject:[NSNumber numberWithInt:0] forKey:@"AssetId"];
        [imageDetails setObject:fileNameSalt forKey:@"AssetName"];
        [imageDetails setObject:[NSNumber numberWithFloat:imgW] forKey:@"Width"];
        [imageDetails setObject:[NSNumber numberWithFloat:imgH] forKey:@"Height"];
        [imageDetails setObject:[NSNumber numberWithBool:isTempNode] forKey:@"isTempNode"];
    

    [self performSelectorOnMainThread:@selector(loadNodeForImage:) withObject:imageDetails waitUntilDone:YES];
    [self.center_progress stopAnimating];
    [self.center_progress setHidden:YES];
}

- (void)playTimer
{/*
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:animationScene->currentFrame inSection:0];
        [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
  */
}

#if TARGET_RT_BIG_ENDIAN
const NSStringEncoding kEncoding_wchar_t =
CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32BE);
#else
const NSStringEncoding kEncoding_wchar_t =
CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
#endif

-(NSString*) stringWithwstring:(const std::wstring&)ws
{
    char* data = (char*)ws.data();
    unsigned size = (int)ws.size() * sizeof(wchar_t);
    
    NSString* result = [[NSString alloc] initWithBytes:data length:size encoding:kEncoding_wchar_t];
    return result;
}

-(std::wstring) getwstring:(NSString*) sourceString
{
    NSData* asData = [sourceString dataUsingEncoding:kEncoding_wchar_t];
    return std::wstring((wchar_t*)[asData bytes], [asData length] / sizeof(wchar_t));
}


#pragma mark RenderManager Delegate

- (void) setShaderTypeForRendering:(int)shaderType
{
    editorScene->updater->resetMaterialTypes((shaderType == 0) ? false : true);
}

- (void)renderFrame:(int)frame withType:(int)shaderType andRemoveWatermark:(bool)removeWatermark
{   
    editorScene->renHelper->isExportingImages = true;
    editorScene->updater->setDataForFrame(frame);
    NSString* tempDir = NSTemporaryDirectory();
    NSString* imageFilePath = [NSString stringWithFormat:@"%@/r-%d.png", tempDir, frame];
    editorScene->renHelper->renderAndSaveImage((char*)[imageFilePath cStringUsingEncoding:NSUTF8StringEncoding], shaderType, false, removeWatermark);
}

- (void) presentPopOver:(CGRect )arect{
    
    UIAlertController * view=   [UIAlertController
                                 alertControllerWithTitle:nil
                                 message:nil
                                 preferredStyle:UIAlertControllerStyleActionSheet];
    UIAlertAction* deleteButton = [UIAlertAction
                                   actionWithTitle:@"Delete"
                                   style:UIAlertActionStyleDefault
                                   handler:^(UIAlertAction * action)
                                   {
                                       [renderViewMan removeNodeFromScene:editorScene->selectedNodeId IsUndoOrRedo:NO];
                                   }];
    if(editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_ADDITIONAL_LIGHT){
        UIAlertAction* duplicateButton = [UIAlertAction
                                          actionWithTitle:@"Duplicate"
                                          style:UIAlertActionStyleDefault
                                          handler:^(UIAlertAction * action)
                                          {
                                              [self createDuplicateAssets];
                                          }];
        
        [view addAction:duplicateButton];
    }
    [view addAction:deleteButton];
    UIPopoverPresentationController *popover = view.popoverPresentationController;
    popover.sourceRect = arect;
    popover.sourceView=self.renderView;
    popover.permittedArrowDirections = UIPopoverArrowDirectionAny;
    [self presentViewController:view animated:YES completion:nil];
}

#pragma Duplicate Actions

- (void) createDuplicateAssetsForAnimation {
    
    int selectedAssetId  = NOT_EXISTS;
    NODE_TYPE selectedNodeType = NODE_UNDEFINED;
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        selectedAssetId = editorScene->nodes[editorScene->selectedNodeId]->assetId;
        selectedNodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
    }
    
    if((selectedNodeType == NODE_RIG || selectedNodeType ==  NODE_SGM || selectedNodeType ==  NODE_OBJ) && selectedAssetId != NOT_EXISTS)
    {
        AssetItem *assetItem = [cache GetAsset:selectedAssetId];
        [self performSelectorOnMainThread:@selector(loadNode:) withObject:assetItem waitUntilDone:YES];
        
    }
      else if(selectedNodeType == NODE_TEXT && selectedAssetId != NOT_EXISTS){
        NSString *typedText = [self stringWithwstring:editorScene->nodes[editorScene->selectedNodeId]->name];
        NSString *fontName = [NSString stringWithCString:editorScene->nodes[editorScene->selectedNodeId]->optionalFilePath.c_str()
                                                encoding:[NSString defaultCStringEncoding]];
        Vector4 color = Vector4(editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.x,editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.y,editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.z,0.0);
        float bevalValue = editorScene->nodes[editorScene->selectedNodeId]->props.nodeSpecificFloat;
        int fontSize = editorScene->nodes[editorScene->selectedNodeId]->props.fontSize;
        [self load3DTex:ASSET_TEXT AssetId:0 TypedText:typedText FontSize:fontSize BevelValue:bevalValue TextColor:color FontPath:fontName isTempNode:NO];
    }
}


- (void) createDuplicateAssets {
    
    int selectedAssetId  = NOT_EXISTS;
    NODE_TYPE selectedNodeType = NODE_UNDEFINED;
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        selectedAssetId = editorScene->nodes[editorScene->selectedNodeId]->assetId;
        selectedNodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
    }
    
    if((selectedNodeType == NODE_RIG || selectedNodeType ==  NODE_SGM || selectedNodeType ==  NODE_OBJ) && selectedAssetId != NOT_EXISTS)
    {
        assetAddType = IMPORT_ASSET_ACTION;
        AssetItem *assetItem = [cache GetAsset:selectedAssetId];
        [self performSelectorOnMainThread:@selector(loadNode:) withObject:assetItem waitUntilDone:YES];

    }
    else if(selectedNodeType == NODE_IMAGE && selectedAssetId != NOT_EXISTS){
        NSString* fileName = [self stringWithwstring:editorScene->nodes[editorScene->selectedNodeId]->name];
        float imgW = editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.x;
        float imgH = editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.y;
        NSMutableDictionary *imageDetails = [[NSMutableDictionary alloc] init];
        [imageDetails setObject:[NSNumber numberWithInt:ASSET_IMAGE] forKey:@"type"];
        [imageDetails setObject:[NSNumber numberWithInt:0] forKey:@"AssetId"];
        [imageDetails setObject:fileName forKey:@"AssetName"];
        [imageDetails setObject:[NSNumber numberWithFloat:imgW] forKey:@"Width"];
        [imageDetails setObject:[NSNumber numberWithFloat:imgH] forKey:@"Height"];
        [imageDetails setObject:[NSNumber numberWithBool:NO] forKey:@"isTempNode"];
        [self performSelectorOnMainThread:@selector(loadNodeForImage:) withObject:imageDetails waitUntilDone:YES];
    }
    else if(selectedNodeType == NODE_TEXT && selectedAssetId != NOT_EXISTS){
        NSString *typedText = [self stringWithwstring:editorScene->nodes[editorScene->selectedNodeId]->name];
        NSString *fontName = [NSString stringWithCString:editorScene->nodes[editorScene->selectedNodeId]->optionalFilePath.c_str()
                                                    encoding:[NSString defaultCStringEncoding]];
        Vector4 color = Vector4(editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.x,editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.y,editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.z,0.0);
        float bevalValue = editorScene->nodes[editorScene->selectedNodeId]->props.nodeSpecificFloat;
        int fontSize = editorScene->nodes[editorScene->selectedNodeId]->props.fontSize;
        [self load3DTex:ASSET_TEXT AssetId:0 TypedText:typedText FontSize:fontSize BevelValue:bevalValue TextColor:color FontPath:fontName isTempNode:NO];
    }
}

-(void) updateAssetListInScenes :(int)nodeType assetName:(NSString *)assetName actionType:(int)action removeObjectAtIndex:(int)index {
    if(action == ADD_OBJECT){
        if(nodeType == ASSET_CAMERA){
            [assetsInScenes addObject:@"CAMERA"];
        }
        else if(nodeType == ASSET_LIGHT){
            [assetsInScenes addObject:@"LIGHT"];
        }
        else if(nodeType == ASSET_IMAGE){
            NSString *name = [NSString stringWithFormat:@"Image : %@",assetName];
            [assetsInScenes addObject:name];
        }
        else if(nodeType == ASSET_TEXT){
            [assetsInScenes addObject:assetName];
        }
        else{
            [assetsInScenes addObject:assetName];
        }
    }
    else if (action == REMOVE_OBJECT){
        [assetsInScenes removeObjectAtIndex:index];
    }
    [self.objectList reloadData];
}


- (void)objectSelectionCompleted:(int)assetIndex
{
    editorScene->selectMan->selectObject(assetIndex);
   
}

#pragma mark PopUpViewConrollerDelegate

- (void) animationBtnDelegateAction:(int)indexValue
{
    if(indexValue==APPLY_ANIMATION){
        [self.popoverController dismissPopoverAnimated:YES];
        if (editorScene->selectedNodeId <= 1 || (!(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG) && !(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT)) || editorScene->isJointSelected) {
            [self.view endEditing:YES];
            UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Please select a text or character to apply the animation." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [closeAlert show];
        }
        else{
            ANIMATION_TYPE animType = (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT) ? TEXT_ANIMATION : RIG_ANIMATION;
            if([Utility IsPadDevice]){
                animationsliderVC =[[AnimationSelectionSlider alloc] initWithNibName:@"AnimationSelectionSlider" bundle:Nil withType:animType  EditorScene:editorScene FirstTime:YES];
                animationsliderVC.delegate = self;
                [self showOrHideLeftView:YES withView:animationsliderVC.view];
            }
            else{
                animationsliderVC =[[AnimationSelectionSlider alloc] initWithNibName:@"AnimationSelectionSliderPhone" bundle:Nil withType:animType EditorScene:editorScene FirstTime:YES];
                animationsliderVC.delegate = self;
                [self showOrHideLeftView:YES withView:animationsliderVC.view];
            }
        }
    }
    else if(indexValue==SAVE_ANIMATION){
        [self.popoverController dismissPopoverAnimated:YES];
        if (editorScene->selectedNodeId <= 1 || (!(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG) && !(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT)) || editorScene->isJointSelected) {
            [self.view endEditing:YES];
            UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Please select a text or character to save the animation as a template." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [closeAlert show];
        }
        else {
            [self.view endEditing:YES];
            UIAlertView* addAnimAlert = [[UIAlertView alloc] initWithTitle:@"Save your Animation as template" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
            [addAnimAlert setAlertViewStyle:UIAlertViewStylePlainTextInput];
            [[addAnimAlert textFieldAtIndex:0] setPlaceholder:@"Animation Name"];
            [[addAnimAlert textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
            [addAnimAlert setTag:ADD_BUTTON_TAG];
            [addAnimAlert show];
            [[addAnimAlert textFieldAtIndex:0] becomeFirstResponder];
        }
    }
}

- (void) importBtnDelegateAction:(int)indexValue{
    if(indexValue==IMPORT_MODELS){
        if([Utility IsPadDevice]){
        [self.popoverController dismissPopoverAnimated:YES];
        assetSelectionSlider =[[AssetSelectionSidePanel alloc] initWithNibName:@"AssetSelectionSidePanel" bundle:Nil];
        assetSelectionSlider.assetSelectionDelegate = self;
        [self showOrHideLeftView:YES withView:assetSelectionSlider.view];
        }
        else{
            [self.popoverController dismissPopoverAnimated:YES];
            assetSelectionSlider =[[AssetSelectionSidePanel alloc] initWithNibName:@"AssetSelectionSidePanelPhone" bundle:Nil];
            assetSelectionSlider.assetSelectionDelegate = self;
            [self showOrHideLeftView:YES withView:assetSelectionSlider.view];
        }
    }
    else if(indexValue==IMPORT_IMAGES)
    {
        if([Utility IsPadDevice])
        {
            [self.popoverController dismissPopoverAnimated:YES];
            self.imagePicker = [[UIImagePickerController alloc] init];
            self.imagePicker.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
            [self.imagePicker setNavigationBarHidden:YES];
            [self.imagePicker setToolbarHidden:YES];
            importImageViewVC = [[ImportImageNew alloc] initWithNibName:@"ImportImageNew" bundle:nil];
            [self showOrHideLeftView:YES withView:importImageViewVC.view];
            [self.imagePicker.view setFrame:CGRectMake(0, 0, importImageViewVC.imagesView.frame.size.width, importImageViewVC.imagesView.frame.size.height)];
            self.imagePicker.delegate=importImageViewVC;
            importImageViewVC.delegate = self;
            [importImageViewVC.imagesView addSubview:self.imagePicker.view];
        }
        else
        {
            [self.popoverController dismissPopoverAnimated:YES];
            self.imagePicker = [[UIImagePickerController alloc] init];
            self.imagePicker.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
            [self.imagePicker setNavigationBarHidden:YES];
            [self.imagePicker setToolbarHidden:YES];
            importImageViewVC = [[ImportImageNew alloc] initWithNibName:@"ImportImageNewPhone" bundle:nil];
            [self showOrHideLeftView:YES withView:importImageViewVC.view];
            [self.imagePicker.view setFrame:CGRectMake(0, 0, importImageViewVC.imagesView.frame.size.width, importImageViewVC.imagesView.frame.size.height)];
            self.imagePicker.delegate=importImageViewVC;
            importImageViewVC.delegate = self;
            [importImageViewVC.imagesView addSubview:self.imagePicker.view];
        }
    }
    else if(indexValue==IMPORT_TEXT){
        if([Utility IsPadDevice])
        {
            [self.popoverController dismissPopoverAnimated:YES];
            textSelectionSlider =[[TextSelectionSidePanel alloc] initWithNibName:@"TextSelectionSidePanel" bundle:Nil];
            textSelectionSlider.textSelectionDelegate = self;
            [self showOrHideLeftView:YES withView:textSelectionSlider.view];
        }
        else
        {
            [self.popoverController dismissPopoverAnimated:YES];
            textSelectionSlider = [[TextSelectionSidePanel alloc] initWithNibName:@"TextSelectionSidePanelPhone" bundle:Nil];
            textSelectionSlider.textSelectionDelegate = self;
            [self showOrHideLeftView:YES withView:textSelectionSlider.view];
        }
    }
    else if(indexValue==IMPORT_LIGHT){
        [self.popoverController dismissPopoverAnimated:YES];
        [self importAdditionalLight];
    }
    else if(indexValue==IMPORT_OBJFILE){
        if([Utility IsPadDevice]){
            [self.popoverController dismissPopoverAnimated:YES];
            objVc =[[ObjSidePanel alloc] initWithNibName:@"ObjSidePanel" bundle:Nil];
            objVc.delegate=self;
            [self showOrHideLeftView:YES withView:objVc.view];
        }
        else{
            [self.popoverController dismissPopoverAnimated:YES];
            objVc =[[ObjSidePanel alloc] initWithNibName:@"ObjSidePanelPhone" bundle:Nil];
            objVc.delegate=self;
            [self showOrHideLeftView:YES withView:objVc.view];
        }
    }
    else if(indexValue==5){
        NSLog(@"ADD BONE Button");
    }
}

- (void) exportBtnDelegateAction:(int)indexValue{
    if(indexValue==EXPORT_IMAGE){
        if([Utility IsPadDevice])
        {
            [self.popoverController dismissPopoverAnimated:YES];
            RenderingViewController* renderingView;
            renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewController" bundle:nil StartFrame:0 EndFrame:totalFrames renderOutput:RENDER_IMAGE caMresolution:0];  //FOR TESTING
            renderingView.delegate = self;
            renderingView.projectName=@"Scene 1";  //FOR TESTING
            renderingView.modalPresentationStyle = UIModalPresentationFormSheet;
            CATransition* transition1 = [CATransition animation];
            transition1.duration = 0.5;
            transition1.type = kCATransitionPush;
            transition1.subtype = kCATransitionFromLeft;
            [transition1 setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
            [self.rightView.layer addAnimation:transition1 forKey:kCATransition];
            [self.rightView setHidden:YES];
            dispatch_async(dispatch_get_main_queue(), ^ {
            [self presentViewControllerInCurrentView:renderingView];
            });
            renderingView.view.superview.backgroundColor = [UIColor clearColor];
        
        }
        else
        {
            [self.popoverController dismissPopoverAnimated:YES];
            RenderingViewController* renderingView;
            renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewControllerPhone" bundle:nil StartFrame:0 EndFrame:totalFrames renderOutput:RENDER_IMAGE caMresolution:0];  //FOR TESTING
            renderingView.delegate = self;
            renderingView.projectName=@"Scene 1";  //FOR TESTING
            renderingView.modalPresentationStyle = UIModalPresentationFormSheet;
            CATransition* transition1 = [CATransition animation];
            transition1.duration = 0.5;
            transition1.type = kCATransitionPush;
            transition1.subtype = kCATransitionFromLeft;
            [transition1 setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
            [self.rightView.layer addAnimation:transition1 forKey:kCATransition];
            [self.rightView setHidden:YES];
            dispatch_async(dispatch_get_main_queue(), ^ {
                [self presentViewControllerInCurrentView:renderingView];
            });
            renderingView.view.superview.backgroundColor = [UIColor clearColor];
        }
    
    }
    else if(indexValue==EXPORT_VIDEO){
         [self.popoverController dismissPopoverAnimated:YES];
        RenderingViewController* renderingView;
        if ([Utility IsPadDevice])
            renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewController" bundle:nil StartFrame:0 EndFrame:totalFrames renderOutput:RENDER_VIDEO caMresolution:0];  //FOR TESTING
        else
            renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewControllerPhone" bundle:nil StartFrame:0 EndFrame:totalFrames renderOutput:RENDER_VIDEO caMresolution:0];  //FOR TESTING
        renderingView.delegate = self;
        renderingView.projectName=@"Scene 1";  //FOR TESTING
        renderingView.modalPresentationStyle = UIModalPresentationFormSheet;
        CATransition* transition1 = [CATransition animation];
        transition1.duration = 0.5;
        transition1.type = kCATransitionPush;
        transition1.subtype = kCATransitionFromLeft;
        [transition1 setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
        [self.rightView.layer addAnimation:transition1 forKey:kCATransition];
        [self.rightView setHidden:YES];
        dispatch_async(dispatch_get_main_queue(), ^ {
            [self presentViewControllerInCurrentView:renderingView];
        });
        
        if ([Utility IsPadDevice]) {
            renderingView.view.superview.backgroundColor = [UIColor clearColor];
            NSLog(@"Videos Clicked");
        }
    }
}

- (void) viewBtnDelegateAction:(int)indexValue{
    if(indexValue==FRONT_VIEW){
        [self.popoverController dismissPopoverAnimated:YES];
       editorScene->updater->changeCameraView(VIEW_FRONT);
    }
    else if(indexValue==TOP_VIEW){
         [self.popoverController dismissPopoverAnimated:YES];
        editorScene->updater->changeCameraView(VIEW_TOP);
    }
    else if(indexValue==LEFT_VIEW){
         [self.popoverController dismissPopoverAnimated:YES];
        editorScene->updater->changeCameraView(VIEW_LEFT);
    }
    else if(indexValue==BACK_VIEW){
         [self.popoverController dismissPopoverAnimated:YES];
        editorScene->updater->changeCameraView(VIEW_BACK);
    }
    else if(indexValue==RIGHT_VIEW){
         [self.popoverController dismissPopoverAnimated:YES];
        editorScene->updater->changeCameraView(VIEW_RIGHT);
    }
    else if(indexValue==BOTTOM_VIEW){
         [self.popoverController dismissPopoverAnimated:YES];
        editorScene->updater->changeCameraView(VIEW_BOTTOM);
    }

}
- (void) addFrameBtnDelegateAction:(int)indexValue{
    if(indexValue==0){
            [self.popoverController dismissPopoverAnimated:YES];
            totalFrames++;
            NSIndexPath* toPath = [NSIndexPath indexPathForItem:totalFrames-1 inSection:0];
            [self.framesCollectionView reloadData];
            [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    }
    else if (indexValue==1){
            [self.popoverController dismissPopoverAnimated:YES];
            totalFrames+=24;
            NSIndexPath* toPath = [NSIndexPath indexPathForItem:totalFrames-1 inSection:0];
            [self.framesCollectionView reloadData];
            [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    }
    else if(indexValue==2){
            [self.popoverController dismissPopoverAnimated:YES];
            totalFrames+=240;
            NSIndexPath* toPath = [NSIndexPath indexPathForItem:totalFrames-1 inSection:0];
            [self.framesCollectionView reloadData];
            [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    }
}
- (void) loginBtnAction{
  
    if([Utility IsPadDevice]){
        [self.popoverController dismissPopoverAnimated:YES];
        loginVc = [[LoginViewController alloc] initWithNibName:@"LoginViewController" bundle:nil];
        [loginVc.view setClipsToBounds:YES];
        loginVc.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:loginVc animated:YES completion:nil];
        loginVc.view.superview.backgroundColor = [UIColor clearColor];
        NSLog(@"Frame Height :%f",loginVc.view.frame.size.height);

    }
    else{
    [self.popoverController dismissPopoverAnimated:YES];
    loginVc = [[LoginViewController alloc] initWithNibName:@"LoginViewControllerPhone" bundle:nil];
    [loginVc.view setClipsToBounds:YES];
    loginVc.modalPresentationStyle = UIModalPresentationFormSheet;
    [self presentViewController:loginVc animated:YES completion:nil];
    loginVc.view.superview.backgroundColor = [UIColor clearColor];
    NSLog(@"Frame Height :%f",loginVc.view.frame.size.height);
    }
    
}
- (void) infoBtnDelegateAction:(int)indexValue{
    if(indexValue==3){
        if([Utility IsPadDevice]){
            [self.popoverController dismissPopoverAnimated:YES];
            settingsVc = [[SettingsViewController alloc]initWithNibName:@"SettingsViewController"bundle:nil];
            [settingsVc.view setClipsToBounds:YES];
            settingsVc.modalPresentationStyle = UIModalPresentationFormSheet;
            [self presentViewController:settingsVc animated:YES completion:nil];
            settingsVc.view.superview.backgroundColor = [UIColor clearColor];
        }
        else{
            
            NSLog(@"Screen Height %f",screenHeight);
            if(screenHeight>320){
                if(screenHeight<400)
                {
                    [self.popoverController dismissPopoverAnimated:YES];
                    settingsVc = [[SettingsViewController alloc]initWithNibName:@"SettingsViewControllerPhone2x"bundle:nil];
                    [settingsVc.view setClipsToBounds:YES];
                    settingsVc.modalPresentationStyle = UIModalPresentationFormSheet;
                    [self presentViewController:settingsVc animated:YES completion:nil];
                    settingsVc.view.superview.backgroundColor = [UIColor clearColor];
                }
                else
                {
                    [self.popoverController dismissPopoverAnimated:YES];
                    settingsVc = [[SettingsViewController alloc]initWithNibName:@"SettingsViewControllerPhone2x"bundle:nil];
                    [settingsVc.view setClipsToBounds:YES];
                    [self presentViewController:settingsVc animated:YES completion:nil];
                    settingsVc.view.superview.backgroundColor = [UIColor clearColor];
                    
                }
            }
            else{
                [self.popoverController dismissPopoverAnimated:YES];
                settingsVc = [[SettingsViewController alloc]initWithNibName:@"SettingsViewControllerPhone"bundle:nil];
                [settingsVc.view setClipsToBounds:YES];
                settingsVc.modalPresentationStyle = UIModalPresentationFormSheet;
                [self presentViewController:settingsVc animated:YES completion:nil];
                settingsVc.view.superview.backgroundColor = [UIColor clearColor];
            }
            
        }
    }

}

#pragma Save Delegates

- (void)saveAnimationData
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* filePath = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, currentScene.sceneFile];
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath])
        [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
    std::string* outputFilePath = new std::string([filePath UTF8String]);
    editorScene->saveSceneData(outputFilePath);
    delete outputFilePath;
    
    [self performSelectorOnMainThread:@selector(saveThumbnail) withObject:nil waitUntilDone:YES];
}

- (void) myObjectsBtnDelegateAction:(int)indexValue{
    [self.popoverController dismissPopoverAnimated:YES];
    [self objectSelectionCompleted:indexValue];
}

- (void)saveThumbnail
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* imageFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, currentScene.sceneFile];
    editorScene->saveThumbnail((char*)[imageFilePath cStringUsingEncoding:NSUTF8StringEncoding]);
}

- (void)saveUserAnimation:(NSString*)assetName
{
    int assetId = ANIMATIONS_ID + [cache getNextAnimationAssetId];
    int type = editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT ? 1 : 0;
    
    bool status = editorScene->animMan->storeAnimations(assetId);
    
    if (status) {
        AnimationItem* animItem = [[AnimationItem alloc] init];
        animItem.assetId = assetId;
        animItem.assetName = assetName;
        animItem.published = 0;
        animItem.rating = 0;
        animItem.boneCount = (int)editorScene->nodes[editorScene->selectedNodeId]->joints.size();
        animItem.isViewerRated = 0;
        animItem.type = type;
        animItem.userId = [[AppHelper getAppHelper] userDefaultsForKey:@"identifierForVendor"];
        animItem.userName = @"anonymous";
        animItem.keywords = [NSString stringWithFormat:@" %@", animItem.assetName];
        animItem.modifiedDate = @"";
        [cache UpdateMyAnimation:animItem];
    }
    else {
        NSLog(@"Error saving animation");
    }
}

#pragma Download Missing Assets
bool downloadMissingAssetCallBack(std::string fileName, NODE_TYPE nodeType)
{
    
    NSLog(@"File Name %s Node Type %d",fileName.c_str(),nodeType);
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [docPaths objectAtIndex:0];
    
    switch (nodeType) {
        case NODE_SGM:
        case NODE_RIG: {
            BOOL assetPurchaseStatus = [[CacheSystem cacheSystem] checkDownloadedAsset:stoi(fileName)];
                NSString* extension = (nodeType == NODE_SGM) ? @"sgm" : @"sgr";
                NSString* name = [NSString stringWithCString:fileName.c_str() encoding:[NSString defaultCStringEncoding]];
                NSString* file = [name stringByDeletingPathExtension];
                if ([file intValue] >= 40000 && ([file intValue] < 50000)) {
                    NSString* rigPath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.sgr", documentsDirectory, [file intValue]];
                    if (![[NSFileManager defaultManager] fileExistsAtPath:rigPath]) {
                        if (!missingAlertShown) {
                            [[AppHelper getAppHelper] missingAlertView];
                            missingAlertShown = true;
                        }
                        return false;
                    }
                    else
                        return true;
                }
                // TODO for sgr in documents directory
                NSString* filePath = [NSString stringWithFormat:@"%@/%s.%@", cacheDirectory, fileName.c_str(), extension];
                NSString* url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/mesh/%s.%@", fileName.c_str(), extension];
                if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
                    downloadFile(url, filePath);
                NSString* texfilePath = [NSString stringWithFormat:@"%@/%s-cm.png", cacheDirectory, fileName.c_str()];
                NSString* texurl = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/meshtexture/%s.png", fileName.c_str()];
                if (![[NSFileManager defaultManager] fileExistsAtPath:texfilePath])
                    downloadFile(texurl, texfilePath);
                
                if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
                    if (!missingAlertShown) {
                        [[AppHelper getAppHelper] missingAlertView];
                        missingAlertShown = true;
                    }
                    return false;
                }
                else
                    return true;
            break;
        }
        case NODE_TEXT: {
            AssetItem* assetObj = [[CacheSystem cacheSystem] GetAssetByName:[NSString stringWithUTF8String:fileName.c_str()]];
            BOOL assetPurchaseStatus = [[CacheSystem cacheSystem] checkDownloadedAsset:assetObj.assetId];
            NSString* extension = [[NSString stringWithUTF8String:fileName.c_str()] pathExtension];
            
            NSString* filePath = [NSString stringWithFormat:@"%@/%s", cacheDirectory, fileName.c_str()];
            NSString* docFilePath = [NSString stringWithFormat:@"%@/Resources/Fonts/%s", documentsDirectory, fileName.c_str()];
            if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
                if (![[NSFileManager defaultManager] fileExistsAtPath:docFilePath]) {
                    NSString* url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/font/%d.%@", assetObj.assetId, extension];
                    if ([assetObj.iap isEqualToString:@"0"] || assetPurchaseStatus)
                        downloadFile(url, filePath);
                    
                    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
                        if (!missingAlertShown) {
                            [[AppHelper getAppHelper] missingAlertView];
                            missingAlertShown = true;
                        }
                        return false;
                    }
                    else
                        return true;
                }
                else
                    return true;
            }
            else
                return true;
            break;
        }
        case NODE_IMAGE: {
            NSString* imagePath = [NSString stringWithFormat:@"%@/%s.png", cacheDirectory, fileName.c_str()];
            if (![[NSFileManager defaultManager] fileExistsAtPath:imagePath]) {
                if (!missingAlertShown) {
                    [[AppHelper getAppHelper] missingAlertView];
                    missingAlertShown = true;
                }
                return false;
            }
            else
                return true;
            break;
        }
        case NODE_OBJ: {
            NSString* objPath = [NSString stringWithFormat:@"%@/Resources/Objs/%s.obj", documentsDirectory, fileName.c_str()];
            if (![[NSFileManager defaultManager] fileExistsAtPath:objPath]) {
                if (!missingAlertShown) {
                    [[AppHelper getAppHelper] missingAlertView];
                    missingAlertShown = true;
                }
                return false;
            }
            else
                return true;
            break;
        }
        default:
            break;
    }
}

void downloadFile(NSString* url, NSString* fileName)
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName])
        return;
    
    NSData* data = [NSData dataWithContentsOfURL:[NSURL URLWithString:url]];
    
    if (data)
        [[AppHelper getAppHelper] writeDataToFile:data FileName:fileName];
}

#pragma mark Camerasettings

- (void)cameraPropertyChanged:(float)fov Resolution:(NSInteger)resolution{
    NSLog(@"Fov value : %f Resolution type: %ld",fov,(long)resolution);
    if (editorScene->cameraResolutionType != resolution) {
        
        editorScene->actionMan->changeCameraProperty(fov, (int)resolution, true);
    } //switch action
    else
        editorScene->actionMan->changeCameraProperty(fov, (int)resolution, false); //slider action
}

#pragma mark Meshproperties Delegate

- (void)meshPropertyChanged:(float)brightness Specular:(float)specular Lighting:(BOOL)light Visible:(BOOL)visible
{
    if(editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size())
        return;
    
    if (editorScene->nodes[editorScene->selectedNodeId]->props.isLighting != light || editorScene->nodes[editorScene->selectedNodeId]->props.isVisible != visible) { //switch action
        editorScene->actionMan->changeMeshProperty(brightness, specular, light, visible, true);
    }
    else { //slider action
        editorScene->actionMan->changeMeshProperty(brightness, specular, light, visible, false);
    }
    
}

#pragma Switch to SceneSelection
- (void) loadSceneSelectionView
{
    if([Utility IsPadDevice]) {
        SceneSelectionControllerNew* sceneSelectionView = [[SceneSelectionControllerNew alloc] initWithNibName:@"SceneSelectionControllerNew" bundle:nil];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:sceneSelectionView];
    } else {
        SceneSelectionControllerNew* sceneSelectionView = [[SceneSelectionControllerNew alloc] initWithNibName:@"SceneSelectionControllerNewPhone" bundle:nil];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:sceneSelectionView];
    }
    
    [self performSelectorOnMainThread:@selector(removeSGEngine) withObject:nil waitUntilDone:YES];
    [self removeFromParentViewController];
}

- (void)removeSGEngine
{
    if (displayLink) {
        [displayLink invalidate];
        displayLink = nil;
    }
    if (editorScene) {
        delete editorScene;
        editorScene = NULL;
    }
}




#pragma Show Or Hide Progress

-(void) showOrHideProgress:(BOOL) value{
    NSLog(@"Value %d ",value);
    if(value == SHOW_PROGRESS){
        [_center_progress setHidden:NO];
        [_center_progress startAnimating];
    }
    else if(value == HIDE_PROGRESS){
        [_center_progress stopAnimating];
        [_center_progress setHidden:YES];
    }
}

#pragma mark -
#pragma AlertView
- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
       alertView.delegate = nil;
}
- (void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (alertView.tag == ADD_BUTTON_TAG) {
        NSCharacterSet* set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];
        
        if (buttonIndex == CANCEL_BUTTON_INDEX) {
        }
        else if (buttonIndex == OK_BUTTON_INDEX) {
            NSString* name = [[alertView textFieldAtIndex:0].text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
            if ([name length] == 0) {
                [self.view endEditing:YES];
                UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"Animation name cannot be empty." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                [errorAlert show];
            }
            else {
                [self.view endEditing:YES];
                if ([name rangeOfCharacterFromSet:set].location != NSNotFound) {
                    UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"Animation Name cannot contain any special characters." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                    [errorAlert show];
                }
                else {
                    [self saveUserAnimation:[alertView textFieldAtIndex:0].text];
                }
            }
        }
    }

}



- (void)setupEnableDisableControls{
    
}

- (void)dealloc
{
    if(smgr)
        delete smgr;
    assetsInScenes = nil;
    importImageViewVC.delegate = nil;
    importImageViewVC = nil;
    animationsliderVC.delegate = nil;
    animationsliderVC = nil;
    textSelectionSlider.textSelectionDelegate = nil;
    textSelectionSlider = nil;
    assetSelectionSlider.assetSelectionDelegate = nil;
    assetSelectionSlider = nil;
    loginVc = nil;
    lightProperties.delegate = nil;
    lightProperties = nil;
    currentScene = nil;
    cache = nil;
    [playTimer invalidate];
    playTimer = nil;
    settingsVc = nil;
    renderViewMan.delegate = nil;
    renderViewMan = nil;
    objVc.delegate = nil;
    objVc = nil;
    currentScene = nil;
    settingsVc = nil;
    screenHeight = nil;
}

@end


