//
//  EditorViewController.m
//  Iyan3D
//
//  Created by Sankar on 18/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>

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
#define IMPORT_VIDEO 2
#define IMPORT_TEXT 3
#define IMPORT_LIGHT 4
#define IMPORT_OBJFILE 5
#define IMPORT_ADDBONE 6
#define CHANGE_TEXTURE 7


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
#define ASSET_TEXT_RIG 10
#define ASSET_TEXT 11
#define ASSET_VIDEO 12
#define ASSET_ADDITIONAL_LIGHT 900

#define ADD_BUTTON_TAG 99

#define UNDEFINED_OBJECT -1

#define CHOOSE_RIGGING_METHOD 8
#define OWN_RIGGING 1
#define HUMAN_RIGGING 2
#define DATA_LOSS_ALERT 999

#define OK_BUTTON_INDEX 1
#define CANCEL_BUTTON_INDEX 0
#define DELETE_OBJECT 200


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
        renderBgColor = Vector4(0.1,0.1,0.1,1.0);
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    isSelected=NO;
    [_center_progress setHidden:NO];
    [_center_progress startAnimating];
    [_rigCancelBtn setHidden:YES];
    [_rigAddToSceneBtn setHidden:YES];
    
#if !(TARGET_IPHONE_SIMULATOR)
    if (iOSVersion >= 8.0)
        isMetalSupported = (MTLCreateSystemDefaultDevice() != NULL) ? true : false;
#endif
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    screenHeight = screenRect.size.height;
    constants::BundlePath = (char*)[[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSASCIIStringEncoding];
    totalFrames = 24;
    [[UIApplication sharedApplication] setStatusBarHidden:NO];
    if ([Utility IsPadDevice])
        [self.framesCollectionView registerNib:[UINib nibWithNibName:@"FrameCellNew" bundle:nil] forCellWithReuseIdentifier:@"FRAMECELL"];
    else
        [self.framesCollectionView registerNib:[UINib nibWithNibName:@"FrameCellNewPhone" bundle:nil] forCellWithReuseIdentifier:@"FRAMECELL"];
    assetsInScenes = [NSMutableArray array];
    if ([[AppHelper getAppHelper] userDefaultsForKey:@"indicationType"])
        [self.framesCollectionView setTag:[[[AppHelper getAppHelper] userDefaultsForKey:@"indicationType"] longValue]];
    else
        [self.framesCollectionView setTag:1];
    
    [self.framesCollectionView reloadData];
    [self.objectList reloadData];
    [self initScene];
    [self changeAllButtonBG];
    [self setupEnableDisableControls];
    [self createDisplayLink];
    [self undoRedoButtonState:DEACTIVATE_BOTH];
    if([[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]){
        [self toolbarPosition:(int)[[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]];
        NSLog(@"Toolbar position : %@ ",[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]);
    } else {
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:0] withKey:@"toolbarPosition"];
        [self toolbarPosition:0];
    }
    
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSLog(@"Document Path : %@ ",documentsDirectory);
    [_center_progress stopAnimating];
    [_center_progress setHidden:YES];
    if (editorScene->actionMan->actions.size() > 0)
        [self.undoBtn setEnabled:YES];
}



- (void)initScene
{
    float screenScale = [[UIScreen mainScreen] scale];
    renderViewMan = [[RenderViewManager alloc] init];
    renderViewMan.delegate = self;
    [renderViewMan setupLayer:_renderView];
    
    if (isMetalSupported) {
        [[AppDelegate getAppDelegate] initEngine:METAL ScreenWidth:SCREENWIDTH ScreenHeight:SCREENHEIGHT ScreenScale:screenScale renderView:_renderView];
        smgr = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        editorScene = new SGEditorScene(METAL, smgr, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
        [renderViewMan setUpPaths:smgr];
        editorScene->screenScale = screenScale;
    }
    else {
        [renderViewMan setupContext];
        [[AppDelegate getAppDelegate] initEngine:OPENGLES2 ScreenWidth:SCREENWIDTH ScreenHeight:SCREENHEIGHT ScreenScale:screenScale renderView:_renderView];
        smgr = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        editorScene = new SGEditorScene(OPENGLES2, smgr, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
        editorScene->screenScale = screenScale;
        [renderViewMan setUpPaths:smgr];
        [renderViewMan setupDepthBuffer:_renderView];
        [renderViewMan setupRenderBuffer];
        [renderViewMan setupFrameBuffer];
    }
    if ([[AppHelper getAppHelper] userDefaultsForKey:@"cameraPreviewSize"]){
        editorScene->camPreviewScale=[[[AppHelper getAppHelper] userDefaultsForKey:@"cameraPreviewSize"]floatValue];
    }
    missingAlertShown = false;
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

- (void)changeAllButtonBG{
    if(editorScene->isNodeSelected)
    {
        if(editorScene->controlType==MOVE){
            [self.moveBtn setBackgroundColor:[UIColor colorWithRed:123.0f / 255.0f green:123.0f / 255.0f blue:127.0f / 255.0f alpha:1]];
            [self.rotateBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
            [self.scaleBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
            
        }
        if(editorScene->controlType==ROTATE){
            [self.rotateBtn setBackgroundColor:[UIColor colorWithRed:123.0f / 255.0f green:123.0f / 255.0f blue:127.0f / 255.0f alpha:1]];
            [self.moveBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
            [self.scaleBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
        }
        if(editorScene->controlType==SCALE){
            [self.scaleBtn setBackgroundColor:[UIColor colorWithRed:123.0f / 255.0f green:123.0f / 255.0f blue:127.0f / 255.0f alpha:1]];
            [self.rotateBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
            [self.moveBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
        }
    }
    else if (editorScene->selectedNodeIds.size()>0)
    {
        if(editorScene->controlType==MOVE){
            [self.moveBtn setBackgroundColor:[UIColor colorWithRed:123.0f / 255.0f green:123.0f / 255.0f blue:127.0f / 255.0f alpha:1]];
            [self.rotateBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
            [self.scaleBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
        }
        if(editorScene->controlType==ROTATE){
            [self.rotateBtn setBackgroundColor:[UIColor colorWithRed:123.0f / 255.0f green:123.0f / 255.0f blue:127.0f / 255.0f alpha:1]];
            [self.moveBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
            [self.scaleBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
        }
        if(editorScene->controlType==SCALE){
            [self.scaleBtn setBackgroundColor:[UIColor colorWithRed:123.0f / 255.0f green:123.0f / 255.0f blue:127.0f / 255.0f alpha:1]];
            [self.rotateBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
            [self.moveBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
        }
    }
    else
    {
        [self.moveBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
        [self.rotateBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
        [self.scaleBtn setBackgroundColor:[UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1]];
    }
}

-(void)setupEnableDisableControls{
    if(editorScene->isNodeSelected)
    {
        if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT || editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT){
            [self.moveBtn setEnabled:true];
            [self.rotateBtn setEnabled:false];
            [self.optionsBtn setEnabled:true];
            [self.scaleBtn setEnabled:false];
            return;
        }
        if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_CAMERA){
            [self.moveBtn setEnabled:true];
            [self.rotateBtn setEnabled:true];
            [self.optionsBtn setEnabled:true];
            [self.scaleBtn setEnabled:false];
            return;
        }
        [self.moveBtn setEnabled:true];
        [self.optionsBtn setEnabled:true];
        [self.rotateBtn setEnabled:true];
        [self.scaleBtn setEnabled:true];
        return;
    }
    else if (editorScene->selectedNodeIds.size()>0)
    {
        if(!editorScene->allObjectsScalable()){
            [self.moveBtn setEnabled:true];
            [self.rotateBtn setEnabled:false];
            [self.scaleBtn setEnabled:false];
            [self.optionsBtn setEnabled:false];
            return;
        }
        [self.moveBtn setEnabled:true];
        [self.rotateBtn setEnabled:true];
        [self.scaleBtn setEnabled:true];
        [self.optionsBtn setEnabled:false];
        return;
        
    }
    else if (editorScene->isRigMode){
        [self.moveBtn setEnabled:true];
        [self.optionsBtn setEnabled:false];
        [self.rotateBtn setEnabled:true];
        [self.scaleBtn setEnabled:true];
        [self.undoBtn setEnabled:false];
        [self.redoBtn setEnabled:false];
        [self.importBtn setEnabled:false];
        [self.exportBtn setEnabled:false];
        [self.animationBtn setEnabled:false];
        [self.optionsBtn setEnabled:false];
        [self.objectList setUserInteractionEnabled:false];
        [self.playBtn setEnabled:false];
        return;
    }
    else
    {
        [self.optionsBtn setEnabled:true];
        [self.moveBtn setEnabled:false];
        [self.rotateBtn setEnabled:false];
        [self.scaleBtn setEnabled:false];
        [self.importBtn setEnabled:true];
        [self.exportBtn setEnabled:true];
        [self.animationBtn setEnabled:true];
        [self.objectList setUserInteractionEnabled:true];
        [self.playBtn setEnabled:true];
        return;
    }
}

- (void)undoRedoButtonState:(int)state
{
    switch (state) {
        case DEACTIVATE_UNDO:
            if ([self.undoBtn isEnabled])
                [self.undoBtn setEnabled:NO];
            if (![self.redoBtn isEnabled])
                [self.redoBtn setEnabled:YES];
            break;
        case DEACTIVATE_REDO:
            if ([self.redoBtn isEnabled])
                [self.redoBtn setEnabled:NO];
            if (![self.undoBtn isEnabled])
                [self.undoBtn setEnabled:YES];
            break;
        case DEACTIVATE_BOTH:
            if ([self.undoBtn isEnabled])
                [self.undoBtn setEnabled:NO];
            if ([self.redoBtn isEnabled])
                [self.redoBtn setEnabled:NO];
            break;
        default:
            if (![self.undoBtn isEnabled])
                [self.undoBtn setEnabled:YES];
            if (![self.redoBtn isEnabled])
                [self.redoBtn setEnabled:YES];
            break;
    }
    
    if (editorScene) {
        if (editorScene->actionMan->actions.size() > 0 && editorScene->actionMan->currentAction > 0)
            [self.undoBtn setEnabled:YES];
        if (editorScene->actionMan->actions.size() > 0 && editorScene->actionMan->currentAction < (editorScene->actionMan->actions.size()))
            [self.redoBtn setEnabled:YES];
    }
}


- (void) removeTempNodeFromScene
{
    if(editorScene && editorScene->loader)
        editorScene->loader->removeTempNodeIfExists();
}

- (void) load3DTex:(int)type AssetId:(int)assetId TextureName:(NSString*)textureName TypedText:(NSString*)typedText FontSize:(int)fontSize BevelValue:(float)bevelRadius TextColor:(Vector4)colors
          FontPath:(NSString*)fontFileName isTempNode:(bool)isTempNode{
    NSMutableDictionary *textDetails = [[NSMutableDictionary alloc] init];
    [textDetails setObject:[NSNumber numberWithInt:assetId] forKey:@"AssetId"];
    [textDetails setObject:textureName forKey:@"textureName"];
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
    NSString* textureName = [fontDetails objectForKey:@"textureName"];
    
    [renderViewMan loadNodeInScene:ASSET_TEXT_RIG AssetId:assetId AssetName:assetName TextureName:textureName Width:fontSize Height:bevalValue isTempNode:isTempNode More:fontDetails ActionType:assetAddType VertexColor:Vector4(-1)];
    if (!isTempNode) {
        [self undoRedoButtonState:DEACTIVATE_BOTH];
    }
}

- (void) importAdditionalLight
{
    if(ShaderManager::lightPosition.size() < 5) {
        assetAddType = IMPORT_ASSET_ACTION;
        //editorScene->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, ASSET_ADDITIONAL_LIGHT + lightCount , "Light"+ to_string(lightCount));
        [self addLightToScene:[NSString stringWithFormat:@"Light%d",lightCount] assetId:ASSET_ADDITIONAL_LIGHT + lightCount ];
        lightCount++;
        [self undoRedoButtonState:DEACTIVATE_BOTH];
    } else {
        UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Scene cannot contain more than five lights." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [closeAlert show];
    }
}

- (Vector2) getVideoResolution:(NSString*) videoPath
{
    Vector2 res = Vector2(0.0, 0.0);
    if([[NSFileManager defaultManager] fileExistsAtPath:videoPath]) {
        NSURL *videoURL = [NSURL fileURLWithPath:videoPath];
        AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:videoURL options:nil];
        AVAssetImageGenerator *gen = [[AVAssetImageGenerator alloc] initWithAsset:asset];
        gen.appliesPreferredTrackTransform = YES;
        NSError *err = NULL;
        CMTime midpoint = CMTimeMake(0, 24);
        CGImageRef imageRef = [gen copyCGImageAtTime:midpoint actualTime:NULL error:&err];
        res.x = CGImageGetWidth(imageRef);
        res.y = CGImageGetHeight(imageRef);
    }
    return res;
}

-(void) addLightToScene:(NSString*)lightName assetId:(int)assetId
{
    [renderViewMan loadNodeInScene:ASSET_ADDITIONAL_LIGHT AssetId:assetId AssetName:[self getwstring:lightName] TextureName:(@"") Width:20 Height:50 isTempNode:NO More:nil ActionType:assetAddType VertexColor:Vector4(-1)];
}

- (void) loadNodeInScene:(AssetItem*)assetItem ActionType:(ActionType)actionType
{
    assetAddType = actionType;
    assetItem.textureName = [NSString stringWithFormat:@"%d%@",assetItem.assetId,@"-cm"];
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
    [renderViewMan loadNodeInScene:type AssetId:assetId AssetName:saltedFileName TextureName:(@"") Width:imgWidth Height:imgHeight isTempNode:isTempNode More:nil ActionType:assetAddType VertexColor:Vector4(-1)];
    if (!isTempNode) {
        [self undoRedoButtonState:DEACTIVATE_BOTH];
    }
}

- (void) loadNode:(AssetItem*) asset
{
    NSLog(@"Asset Id : %d ", asset.assetId);
    [renderViewMan loadNodeInScene:asset.type AssetId:asset.assetId AssetName:[self getwstring:asset.name] TextureName:(asset.textureName) Width:0 Height:0 isTempNode:asset.isTempAsset More:nil ActionType:assetAddType VertexColor:Vector4(-1)];
    if (!asset.isTempAsset) {
        [self undoRedoButtonState:DEACTIVATE_BOTH];
    }
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
            if(editorScene->isRigMode){
                editorScene->selectMan->checkSelectionForAutoRig(renderViewMan.tapPosition);
            }
            else{
                editorScene->selectMan->checkSelection(renderViewMan.tapPosition);
                [self changeAllButtonBG];
                [self setupEnableDisableControls];
            }
            
            [self reloadFrames];
            renderViewMan.checkTapSelection = false;
            [self highlightObjectList];
            if(editorScene->isRigMode && editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)(RIG_MODE_MOVE_JOINTS)){
                [_addJointBtn setHidden:(editorScene->rigMan->isSkeletonJointSelected)?NO:YES];
            }
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
    
    if(editorScene->isRigMode && editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)(RIG_MODE_PREVIEW)){
        [_addJointBtn setHidden:(editorScene->rigMan->isSGRJointSelected)?NO:YES];
    }
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
        // if (editorScene->nodes[i]->getType() == NODE_TEXT_SKIN)
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
    if (editorScene->isKeySetForFrame.find((int)indexPath.row) != editorScene->isKeySetForFrame.end() && (indexPath.row) == editorScene->currentFrame) {
        cell.backgroundColor = [UIColor blackColor];
        cell.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                 green:156.0f / 255.0f
                                                  blue:156.0f / 255.0f
                                                 alpha:1.0f]
        .CGColor;
        cell.layer.borderWidth = 2.0f;
    }
    else if (editorScene->isKeySetForFrame.find((int)indexPath.row) != editorScene->isKeySetForFrame.end()) {
        cell.backgroundColor = [UIColor blackColor];
        cell.layer.borderColor = [UIColor blackColor].CGColor;
        cell.layer.borderWidth = 2.0f;
    }
    else {
        cell.backgroundColor = [UIColor colorWithRed:61.0f / 255.0f
                                               green:62.0f / 255.0f
                                                blue:63.0f / 255.0f
                                               alpha:1.0f];
        cell.layer.borderColor = [UIColor colorWithRed:61.0f / 255.0f
                                                 green:62.0f / 255.0f
                                                  blue:63.0f / 255.0f
                                                 alpha:1.0f]
        .CGColor;
        cell.layer.borderWidth = 2.0f;
    }
    if ((indexPath.row) == editorScene->currentFrame) {
        cell.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                 green:156.0f / 255.0f
                                                  blue:156.0f / 255.0f
                                                 alpha:1.0f]
        .CGColor;
        cell.layer.borderWidth = 2.0f;
    }
    
    //    cell.backgroundColor = [UIColor colorWithRed:55.0f / 255.0f
    //                                                   green:56.0f / 255.0f
    //                                                    blue:57.0f / 255.0f
    //                                                   alpha:1.0f];
    
    cell.layer.borderWidth = 0.0f;
    if (_framesCollectionView.tag==1) {
        cell.framesLabel.text = [NSString stringWithFormat:@"%d", (int)indexPath.row + 1];
    }
    else if (_framesCollectionView.tag==2){
        cell.framesLabel.text = [NSString stringWithFormat:@"%.2fs", ((float)indexPath.row + 1) / 24];
    }
    
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
    [self undoRedoButtonState:DEACTIVATE_BOTH];
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
    if(editorScene->nodes.size() > indexPath.row){
        if(editorScene->nodes[indexPath.row]->getType() == NODE_CAMERA)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Camera_Pad.png"];
        else if(editorScene->nodes[indexPath.row]->getType() == (NODE_LIGHT || NODE_ADDITIONAL_LIGHT))
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Light_Pad.png"];
        else if(editorScene->nodes[indexPath.row]->getType() == NODE_TEXT_SKIN)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Text_Pad"];
        else if(editorScene->nodes[indexPath.row]->getType() == NODE_IMAGE)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Image_Pad"];
        else
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Models_Pad.png"];
    }
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
- (NSIndexPath *)tableView:(UITableView *)tableView willDeselectRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSLog(@"Deselected");
    [self objectSelectionCompleted:(int)indexPath.row];
    return indexPath;
}
-(void)tableView:(UITableView *)tableView deSelectRowAtIndexPath:(NSIndexPath *)indexPath{
    
    [self.objectList deselectRowAtIndexPath:indexPath animated:YES];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

#pragma mark - Button Actions

- (IBAction)backToScenes:(id)sender {
    if(editorScene->isPlaying){
        [self stopPlaying];
        return;
    }
    [self performSelectorOnMainThread:@selector(saveAnimationData) withObject:nil waitUntilDone:YES];
    [self loadSceneSelectionView];
}

- (IBAction)playButtonAction:(id)sender {
    [self playAnimation];
}


- (IBAction)moveLastAction:(id)sender {
    if((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode) != RIG_MODE_PREVIEW){
        if(editorScene->rigMan->sceneMode == RIG_MODE_OBJVIEW){
            UIAlertView *closeAlert = [[UIAlertView alloc]initWithTitle:@"Select Bone Structure" message:@"You can either start with a complete Human Bone structure or with a single bone?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Single Bone", @"Human Bone Structure", nil];
            [closeAlert setTag:CHOOSE_RIGGING_METHOD];
            [closeAlert show];
        }
        else if(editorScene->rigMan->sceneMode + 1 == RIG_MODE_PREVIEW){
            //[[UIApplication sharedApplication] beginIgnoringInteractionEvents];
            NSString *tempDir = NSTemporaryDirectory();
            NSString *sgrFilePath = [NSString stringWithFormat:@"%@r-%@.sgr", tempDir, @"autorig"];
            string path = (char*)[sgrFilePath cStringUsingEncoding:NSUTF8StringEncoding];
            NSLog(@"Temp Path %s ",path.c_str());
            [self performSelectorOnMainThread:@selector(exportSgr:) withObject:sgrFilePath waitUntilDone:YES];
            [_rigAddToSceneBtn setHidden:NO];
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode+1));
        }
        else
        {
            [_rigAddToSceneBtn setHidden:YES];
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode+1));
        }
    }
    [_addJointBtn setHidden:YES];
}

- (void)exportSgr:(NSString*)pathStr
{
    string path = (char*)[pathStr cStringUsingEncoding:NSUTF8StringEncoding];
    editorScene->rigMan->exportSGR(path);
    
}

- (IBAction)moveFirstAction:(id)sender {
    if((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode) != RIG_MODE_OBJVIEW){
        if((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode-1) == RIG_MODE_OBJVIEW) {
            [self.view endEditing:YES];
            UIAlertView *dataLossAlert = [[UIAlertView alloc]initWithTitle:@"Warning" message:@"Are you sure you want to go back to previous mode? By pressing Yes all your changes will be discarded." delegate:self cancelButtonTitle:@"NO" otherButtonTitles:@"Yes", nil];
            [dataLossAlert setTag:DATA_LOSS_ALERT];
            [dataLossAlert show];
        } else {
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode-1));
        }
    }
    [_rigAddToSceneBtn setHidden:YES];
    [_addJointBtn setHidden:YES];
    
}

- (IBAction)rigCancelAction:(id)sender
{
    editorScene->rigMan->deallocAutoRig(NO);
    editorScene->enterOrExitAutoRigMode(false);
    [self setupEnableDisableControls];
    [self autoRigViewButtonHandler:YES];
    selectedNodeId = -1;
}

- (IBAction)rigAddToSceneAction:(id)sender
{
    
    if(editorScene->rigMan->deallocAutoRig(YES)){
        editorScene->enterOrExitAutoRigMode(false);
        [self setupEnableDisableControls];
    }
    [self addrigFileToCacheDirAndDatabase:[NSString stringWithFormat:@"%s%@",editorScene->nodes[selectedNodeId]->textureName.c_str(),@".png"]];
    [self autoRigViewButtonHandler:YES];
    [self updateAssetListInScenes];
    selectedNodeId = -1;
}


- (IBAction)addJoinAction:(id)sender {
    if(editorScene->rigMan->isSkeletonJointSelected)
        editorScene->rigMan->addNewJoint();
}

- (IBAction)publishBtnAction:(id)sender {
    [animationsliderVC publishBtnaction];
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
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1){
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
                              permittedArrowDirections:UIPopoverArrowDirectionLeft
                                              animated:YES];
    }
    else{
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
}

- (IBAction)loginBtnAction:(id)sender {
    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"googleAuthentication"]||[[AppHelper getAppHelper] userDefaultsBoolForKey:@"facebookauthentication"] || [[AppHelper getAppHelper] userDefaultsBoolForKey:@"twitterauthentication"] )
    {
        if ([Utility IsPadDevice])
        {
            _loggedInVc = [[LoggedInViewController alloc] initWithNibName:@"LoggedInViewController" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_loggedInVc];
            self.popoverController.popoverContentSize = CGSizeMake(305, 525);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [_loggedInVc.view setClipsToBounds:YES];
            _loggedInVc.delegare=self;
            self.popUpVc.delegate=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];
        }
        else
        {
            _loggedInVc = [[LoggedInViewController alloc] initWithNibName:@"LoggedInViewControllerPhone" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_loggedInVc];
            self.popoverController.popoverContentSize = CGSizeMake(230.0, 250.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            _loggedInVc.delegare=self;
            [_loggedInVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];
        }
        
    }
    else
    {
        if ([Utility IsPadDevice])
        {
            loginVc = [[LoginViewController alloc] initWithNibName:@"LoginViewController" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:loginVc];
            self.popoverController.popoverContentSize = CGSizeMake(302 , 240.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [loginVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            loginVc.delegare=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];
            
        }
        else
        {
            loginVc = [[LoginViewController alloc] initWithNibName:@"LoginViewControllerPhone" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:loginVc];
            self.popoverController.popoverContentSize = CGSizeMake(228.00, 274.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [loginVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            loginVc.delegare=self;
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
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1){
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
                              permittedArrowDirections:UIPopoverArrowDirectionLeft
                                              animated:YES];
    }
    else{
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
}

- (IBAction)importBtnAction:(id)sender
{
    BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1);
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
                          permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight
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
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1){
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
                              permittedArrowDirections:UIPopoverArrowDirectionLeft
                                              animated:YES];
        [_popUpVc UpdateObjectList:assetsInScenes];
    }
    else
    {
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
        [_popUpVc UpdateObjectList:assetsInScenes];
    }
}


- (IBAction)optionsBtnAction:(id)sender
{
    
    if(!editorScene->isNodeSelected || (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_UNDEFINED))
    {
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1);
        _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"optionsBtn"];
        [_popUpVc.view setClipsToBounds:YES];
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        self.popoverController.popoverContentSize = CGSizeMake(205.0, 42);
        self.popoverController.delegate =self;
        self.popUpVc.delegate=self;
        CGRect rect = _optionsBtn.frame;
        rect = [self.view convertRect:rect fromView:_optionsBtn.superview];
        [self.popoverController presentPopoverFromRect:rect
                                                inView:self.view
                              permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight
                                              animated:YES];
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
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1);
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
                              permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight
                                              animated:NO];
    }
    else if(editorScene->isNodeSelected && (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_CAMERA))
    {
        float fovValue = editorScene->cameraFOV;
        NSInteger resolutionType = editorScene->cameraResolutionType;
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1);
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
                              permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight
                                              animated:NO];
        
    }
    else
    {
        float brightnessValue = editorScene->nodes[editorScene->selectedNodeId]->props.brightness;
        float specularValue = editorScene->nodes[editorScene->selectedNodeId]->props.shininess;
        bool isLightningValue = editorScene->nodes[editorScene->selectedNodeId]->props.isLighting;
        bool isVisibleValue = editorScene->nodes[editorScene->selectedNodeId]->props.isVisible;
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1);
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
                              permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight
                                              animated:NO];
    }
}

- (IBAction)moveBtnAction:(id)sender
{
    editorScene->controlType = MOVE;
    editorScene->updater->updateControlsOrientaion();
    [self changeAllButtonBG];
}

- (IBAction)rotateBtnAction:(id)sender
{
    if(editorScene->hasNodeSelected() || editorScene->allObjectsScalable())
    {
        editorScene->controlType = ROTATE;
        editorScene->updater->updateControlsOrientaion();
        [self changeAllButtonBG];
    }
    
}

- (IBAction)scaleBtnAction:(id)sender
{
    if(self.rigScreenLabel.isHidden){
        
        bool status = false;
        
        if((editorScene->selectedNodeIds.size() > 0) && (editorScene->allObjectsScalable()))
            status = true;
        else if(!(editorScene->selectedNodeIds.size() > 0) && (editorScene->isRigMode || (editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_CAMERA && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_LIGHT && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_ADDITIONAL_LIGHT)))
            status = true;
        
        if(status){
            editorScene->controlType = SCALE;
            editorScene->updater->updateControlsOrientaion();
            NSLog(@"Function is here 2");
            editorScene->renHelper->setControlsVisibility(false);
            Vector3 currentScale = editorScene->getSelectedNodeScale();
            _scaleProps = [[ScaleViewController alloc] initWithNibName:@"ScaleViewController" bundle:nil updateXValue:currentScale.x updateYValue:currentScale.y updateZValue:currentScale.z];
            _scaleProps.delegate = self;
            BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1);
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_scaleProps];
            self.popoverController.popoverContentSize = CGSizeMake(270, 177);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [_popUpVc.view setClipsToBounds:YES];
            CGRect rect = _scaleBtn.frame;
            rect = [self.view convertRect:rect fromView:_scaleBtn.superview];
            [self.popoverController presentPopoverFromRect:rect
                                                    inView:self.view
                                  permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight
                                                  animated:NO];
            [self changeAllButtonBG];
        }
    }
    else{
        //        float scale = autoRigObject->getSelectedJointScale();
        float scale = 10.0;
        scaleAutoRig=[[ScaleForAutoRigViewController alloc] initWithNibName:@"ScaleForAutoRigViewController" bundle:nil updateScale:scale];
        scaleAutoRig.delegate = self;
        self.popoverController = [[WEPopoverController alloc]initWithContentViewController:scaleAutoRig];
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        self.popoverController.popoverContentSize = CGSizeMake(390, 100.0);
        self.popoverController.delegate =self;
        CGRect rect = _scaleBtn.frame;
        rect = [self.view convertRect:rect fromView:_scaleBtn.superview];
        [self.popoverController presentPopoverFromRect:rect
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionRight
                                              animated:YES];
    }
}

- (IBAction)undoBtnAction:(id)sender
{
    int returnValue2 = NOT_SELECTED;
    
    ACTION_TYPE actionType = (ACTION_TYPE)editorScene->undo(returnValue2);
    [self undoRedoButtonState:actionType];
    
    NSLog(@"Undo Return Value : %d",returnValue2);
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
        case ADD_MULTI_ASSET_BACK:{
            [self undoMultiAssetDeleted:returnValue2];
            break;
        }
        case ADD_TEXT_IMAGE_BACK: {
            std::wstring name = editorScene->nodes[editorScene->nodes.size() - 1]->name;
            [self updateAssetListInScenes];
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
                assetObject.isTempAsset = NO;
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
    [self undoRedoButtonState:DEACTIVATE_BOTH];
    [self updateAssetListInScenes];
}

- (void) undoMultiAssetDeleted:(int)size
{
    for (int i = 0; i < size; i++)
        [self undoBtnAction:nil];
    editorScene->actionMan->currentAction--;
}

- (void) redoMultiAssetDeleted:(int)size
{
    for (int i = 0; i < size; i++)
        [self redoBtnAction:nil];
    editorScene->actionMan->currentAction++;
}

- (IBAction)redoBtnAction:(id)sender
{
    int returnValue = editorScene->redo();
    [self undoRedoButtonState:returnValue];
    if (returnValue == DO_NOTHING) {
        //DO NOTHING
    }
    else if (returnValue == DELETE_ASSET) {
        if (editorScene->selectedNodeId < assetsInScenes.count) {
            [renderViewMan removeNodeFromScene:editorScene->selectedNodeId IsUndoOrRedo:YES];
        }
    }
    else if (returnValue == ADD_TEXT_IMAGE_BACK) {
        std::wstring name = editorScene->nodes[editorScene->nodes.size() - 1]->name;
        [self updateAssetListInScenes];
    }
    else if (returnValue == SWITCH_MIRROR) {
        //self.mirrorSwitch.on = animationScene->getMirrorState();
    }
    else if(returnValue == ACTION_MULTI_NODE_DELETED_BEFORE){
        SGAction &recentAction = editorScene->actionMan->actions[editorScene->actionMan->currentAction-1];
        [self redoMultiAssetDeleted:recentAction.objectIndex];
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
                assetObject.isTempAsset = NO;
                [self loadNodeInScene:assetObject ActionType:assetAddType];
            }
        }
    }
    if (editorScene->actionMan->actions.size() <= 0 || editorScene->actionMan->currentAction >= editorScene->actionMan->actions.size())
        [self undoRedoButtonState:DEACTIVATE_REDO];
    
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->currentFrame inSection:0];
    [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    [self HighlightFrame];
    [self.framesCollectionView reloadData];
    [self updateAssetListInScenes];
    
}


#pragma mark EditorScene Functions

- (void) playAnimation{
    isPlaying = !isPlaying;
    editorScene->isPlaying = isPlaying;
    [self showOrHideRightView:YES];
    if (editorScene->selectedNodeId != NOT_SELECTED) {
        editorScene->selectMan->unselectObject(editorScene->selectedNodeId);
        [self.framesCollectionView reloadData];
    }
    if (isPlaying) {
        //[self setInterfaceVisibility:YES];
        if(editorScene->selectedNodeIds.size() > 0)
            editorScene->selectMan->unselectObjects();
        [self setupEnableDisableControlsPlayAnimation];
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
    NSLog(@"Stop playing");
    [self.playBtn setImage:[UIImage imageNamed:@"Play_Pad.png"] forState:UIControlStateNormal];
    if(_leftView.isHidden)
        [self showOrHideRightView:NO];
    editorScene->setLightingOn();
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->currentFrame inSection:0];
    UICollectionViewCell* todatasetCell = [self.framesCollectionView cellForItemAtIndexPath:toPath];
    todatasetCell.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                      green:156.0f / 255.0f
                                                       blue:156.0f / 255.0f
                                                      alpha:1.0f]
    .CGColor; // highlight selection
    todatasetCell.layer.borderWidth = 2.0f;
    
    [playTimer invalidate];
    playTimer = nil;
    if (isPlaying) {
        isPlaying = false;
        editorScene->isPlaying = false;
    }
    [self setupEnableDisableControlsPlayAnimation];
    editorScene->actionMan->switchFrame(editorScene->currentFrame);
}

-(void) myAnimation:(BOOL)showorHide
{
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1)
    {
        self.publishBtn.frame = CGRectMake(self.rightView.frame.size.width,self.scaleBtn.frame.origin.y , 110, 40);
        [self.publishBtn setHidden:showorHide];
        
    }
    else
    {
        self.publishBtn.frame = CGRectMake( self.view.frame.size.width-self.rightView.frame.size.width, self.scaleBtn.frame.origin.y, 110, 40);
        [self.publishBtn setHidden:showorHide];
        
    }
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


-(void)highlightObjectList
{
    if(editorScene->selectedNodeId==-1){
        for(int i=0;i<[assetsInScenes count];i++){
            NSIndexPath *path = [NSIndexPath indexPathForRow:i inSection:0];
            if([Utility IsPadDevice]){
                [self.objectList deselectRowAtIndexPath:path animated:YES];
            }
            else{
                [_popUpVc updateDescelect:path];
            }
            
        }
    }
    
    if(!editorScene->isMultipleSelection)
    {
        NSIndexPath *path = [NSIndexPath indexPathForRow:editorScene->selectedNodeId inSection:0];
        if(editorScene->selectedNodeId!=-1)
        {
            isSelected=YES;
            if([Utility IsPadDevice]){
                [self.objectList selectRowAtIndexPath:path animated:YES scrollPosition:editorScene->selectedNodeId];
            }
            else
            {
                [_popUpVc updateSelection:path ScrollPosition:editorScene->selectedNodeId];
            }
            prevSelection=path;
        }
        if(editorScene->selectedNodeId==-1)
        {
            if(isSelected)
            {
                if([Utility IsPadDevice]){
                    [self.objectList deselectRowAtIndexPath:path animated:YES];
                }
                else{
                    [_popUpVc updateDescelect:path];
                }
                isSelected=NO;
                
            }
            
        }
        
    }
    else
    {
        for(int i=0; i < editorScene->selectedNodeIds.size(); i++){
            NSIndexPath *path = [NSIndexPath indexPathForRow:editorScene->selectedNodeIds[i] inSection:0];
            if([Utility IsPadDevice]){
                [self.objectList selectRowAtIndexPath:path animated:YES scrollPosition:editorScene->selectedNodeIds[i]];
            }
            else
            {
                [_popUpVc updateSelection:path ScrollPosition:editorScene->selectedNodeIds[i]];
            }
        }
        
        
    }
}


- (void) presentViewControllerInCurrentView:(UIViewController*) vcToPresent
{
    [self presentViewController:vcToPresent animated:YES completion:nil];
}

#pragma mark - Rendering ViewController Delegates

- (void) saveScene
{
    [self performSelectorOnMainThread:@selector(saveAnimationData) withObject:nil waitUntilDone:YES];
}

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
    [self showOrHideRightView:NO];
    
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
    if(editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size())
        return;
    editorScene->actionMan->changeObjectScale(Vector3(XValue, YValue, ZValue), false);
    [_framesCollectionView reloadData];
}
- (void)scaleValueForAction:(float)XValue YValue:(float)YValue ZValue:(float)ZValue
{
    if(editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size())
        return;
    editorScene->actionMan->changeObjectScale(Vector3(XValue, YValue, ZValue), true);
    [_framesCollectionView reloadData];
    [self undoRedoButtonState:DEACTIVATE_BOTH];
}

- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd
{
    if(editorScene)
        editorScene->selectMan->unselectObjects();
    BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1);
    
    if(showView)
        [self.leftView addSubview:subViewToAdd];
    
    [self.leftView setHidden:(!showView)];
    CATransition *transition = [CATransition animation];
    transition.duration = 0.5;
    transition.type = kCATransitionPush;
    if (status) {
        transition.subtype = (showView) ? kCATransitionFromRight : kCATransitionFromLeft;
    }
    else{
        transition.subtype = (showView) ? kCATransitionFromLeft : kCATransitionFromRight;
    }
    [transition setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
    [self.leftView.layer addAnimation:transition forKey:nil];
    
    CATransition* transition1 = [CATransition animation];
    transition1.duration = 0.5;
    transition1.type = kCATransitionPush;
    if (status) {
        transition1.subtype = (showView) ? kCATransitionFromRight : kCATransitionFromLeft;
    }
    else{
        transition1.subtype = (showView) ? kCATransitionFromLeft : kCATransitionFromRight;
    }
    [transition1 setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
    [self.rightView.layer addAnimation:transition1 forKey:kCATransition];
    
    [self.rightView setHidden:showView];
    
}
- (void) showOrHideRightView:(BOOL)showView{
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1){
        CATransition* transition1 = [CATransition animation];
        transition1.duration = 0.5;
        transition1.type = kCATransitionPush;
        transition1.subtype = (showView) ? kCATransitionFromRight : kCATransitionFromLeft;
        [transition1 setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
        [self.rightView.layer addAnimation:transition1 forKey:kCATransition];
        
        [self.rightView setHidden:showView];
    }
    else{
        CATransition* transition1 = [CATransition animation];
        transition1.duration = 0.5;
        transition1.type = kCATransitionPush;
        transition1.subtype = (showView) ? kCATransitionFromLeft : kCATransitionFromRight;
        [transition1 setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
        [self.rightView.layer addAnimation:transition1 forKey:kCATransition];
        
        [self.rightView setHidden:showView];
    }
}

-(void)pickedImageWithInfo:(NSDictionary*)info type:(BOOL)isTempNode
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
    
    assetAddType = IMPORT_ASSET_ACTION;
    [self performSelectorOnMainThread:@selector(loadNodeForImage:) withObject:imageDetails waitUntilDone:YES];
    [self.center_progress stopAnimating];
    [self.center_progress setHidden:YES];
}

-(void)pickedVideoWithInfo:(NSString*)path FileName:(NSString*)fileName IsTemp:(BOOL)isTemp
{
    Vector2 resolution = [self getVideoResolution:path];
    NSMutableDictionary *videoDetail = [[NSMutableDictionary alloc] init];
    [videoDetail setObject:[NSNumber numberWithInt:ASSET_VIDEO] forKey:@"type"];
    [videoDetail setObject:[NSNumber numberWithInt:0] forKey:@"AssetId"];
    [videoDetail setObject:fileName forKey:@"AssetName"];
    [videoDetail setObject:[NSNumber numberWithFloat:resolution.x] forKey:@"Width"];
    [videoDetail setObject:[NSNumber numberWithFloat:resolution.y] forKey:@"Height"];
    [videoDetail setObject:[NSNumber numberWithBool:isTemp] forKey:@"isTempNode"];
    [self performSelectorOnMainThread:@selector(loadNodeForImage:) withObject:videoDetail waitUntilDone:YES];
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
    editorScene->renHelper->renderAndSaveImage((char*)[imageFilePath cStringUsingEncoding:NSUTF8StringEncoding], shaderType, false, removeWatermark,renderBgColor);
}

- (void) presentPopOver:(CGRect )arect{
    
    UIAlertController * view=   [UIAlertController
                                 alertControllerWithTitle:nil
                                 message:nil
                                 preferredStyle:UIAlertControllerStyleActionSheet];
    
    if(!editorScene->hasNodeSelected() && !(editorScene->selectedNodeIds.size() > 0))
    {
        UIAlertAction* movaCam = [UIAlertAction
                                  actionWithTitle:@"Move Camera"
                                  style:UIAlertActionStyleDefault
                                  handler:^(UIAlertAction * action)
                                  {
                                      editorScene->nodes[0]->setPosition(editorScene->viewCamera->getPosition(), editorScene->currentFrame);
                                      Vector3 nodeRot = editorScene->viewCamera->getViewMatrix().getRotationInDegree();
                                      Vector3 initDir = (editorScene->viewCamera->getTarget() - editorScene->nodes[0]->getNodePosition()).normalize();
                                      Vector3 targetDir = (editorScene->viewCamera->getTarget() - editorScene->viewCamera->getPosition()).normalize();
                                      Quaternion rotQ = MathHelper::rotationBetweenVectors(targetDir, initDir);
                                      rotQ = MathHelper::RotateNodeInWorld(nodeRot, rotQ);
                                      Vector3 rotation = MathHelper::toEuler(rotQ);
                                      rotation.x -= 180.0;
                                      rotation.y -= 180.0;
                                      rotation.z -= 180.0;
                                      
                                      editorScene->nodes[0]->setRotation(Quaternion(rotation * DEGTORAD), editorScene->currentFrame);
                                      editorScene->nodes[0]->setRotationOnNode(rotQ);
                                      /*
                                       Vector3 camera = editorScene->nodes[0]->getNodePosition();
                                       Vector3 viewCamera = editorScene->viewCamera->getPosition();
                                       Vector3 rotDir = (viewCamera - camera).normalize();
                                       Quaternion rotQ = MathHelper::rotationBetweenVectors(rotDir, Vector3(1.0,1.0,1.0));
                                       editorScene->nodes[0]->setRotation(rotQ, editorScene->currentFrame);
                                       */
                                      NSLog(@"\nCamera Rotation : %f %f %f ",editorScene->viewCamera->getViewMatrix().getRotationInDegree().x,
                                            editorScene->viewCamera->getRotationInRadians().x,
                                            editorScene->viewCamera->getTarget().x);
                                      
                                      editorScene->updater->setDataForFrame(editorScene->currentFrame);
                                  }];
        
        [view addAction:movaCam];
    }
    else
    {
        UIAlertAction* deleteButton = [UIAlertAction
                                       actionWithTitle:@"Delete"
                                       style:UIAlertActionStyleDefault
                                       handler:^(UIAlertAction * action)
                                       {
                                           if(editorScene->selectedNodeIds.size() > 0){
                                               editorScene->loader->removeSelectedObjects();
                                               [self undoRedoButtonState:DEACTIVATE_BOTH];
                                           }
                                           else{
                                               [renderViewMan removeNodeFromScene:editorScene->selectedNodeId IsUndoOrRedo:NO];
                                               [self undoRedoButtonState:DEACTIVATE_BOTH];
                                           }
                                           [self updateAssetListInScenes];
                                           
                                       }];
        [view addAction:deleteButton];
        
        if(editorScene->selectedNodeId!=-1 && editorScene->selectedNodeIds.size() <= 0 && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_ADDITIONAL_LIGHT && editorScene->selectedNodeId!=-1){
            UIAlertAction* duplicateButton = [UIAlertAction
                                              actionWithTitle:@"Duplicate"
                                              style:UIAlertActionStyleDefault
                                              handler:^(UIAlertAction * action)
                                              {
                                                  [self createDuplicateAssets];
                                                  [self updateAssetListInScenes];
                                              }];
            
            [view addAction:duplicateButton];
        }
        if(editorScene->selectedNodeId!=-1 && editorScene->selectedNodeIds.size() <= 0 && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_ADDITIONAL_LIGHT && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_IMAGE && editorScene->selectedNodeId!=-1){
            UIAlertAction* changeTexture = [UIAlertAction
                                            actionWithTitle:@"Change Texture"
                                            style:UIAlertActionStyleDefault
                                            handler:^(UIAlertAction * action)
                                            {
                                                selectedNodeId = editorScene->selectedNodeId;
                                                [self changeTextureForAsset];
                                            }];
            [view addAction:changeTexture];
        }
        
        
    }
    
    
    UIPopoverPresentationController *popover = view.popoverPresentationController;
    popover.sourceRect = arect;
    popover.sourceView=self.renderView;
    popover.permittedArrowDirections = UIPopoverArrowDirectionAny;
    [self presentViewController:view animated:YES completion:nil];
}

#pragma Duplicate Actions

- (void) createDuplicateAssetsForAnimation {
    
    int selectedAssetId  = NOT_EXISTS;
    int selectedNode = NOT_EXISTS;
    NODE_TYPE selectedNodeType = NODE_UNDEFINED;
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        selectedAssetId = editorScene->nodes[editorScene->selectedNodeId]->assetId;
        selectedNodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
        selectedNode = editorScene->selectedNodeId;
    }
    
    if((selectedNodeType == NODE_RIG || selectedNodeType ==  NODE_SGM || selectedNodeType ==  NODE_OBJ) && selectedAssetId != NOT_EXISTS)
    {
        AssetItem *assetItem = [cache GetAsset:selectedAssetId];
        assetItem.textureName = [NSString stringWithFormat:@"%d%@",assetItem.assetId,@"-cm"];
        [self performSelectorOnMainThread:@selector(loadNode:) withObject:assetItem waitUntilDone:YES];
        
    }
    else if((selectedNodeType == NODE_TEXT_SKIN || selectedNodeType == NODE_TEXT) && selectedAssetId != NOT_EXISTS){
        NSString *typedText = [self stringWithwstring:editorScene->nodes[editorScene->selectedNodeId]->name];
        NSString *fontName = [NSString stringWithCString:editorScene->nodes[editorScene->selectedNodeId]->optionalFilePath.c_str()
                                                encoding:[NSString defaultCStringEncoding]];
        Vector4 color = Vector4(editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.x,editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.y,editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.z,0.0);
        float bevalValue = editorScene->nodes[editorScene->selectedNodeId]->props.nodeSpecificFloat;
        int fontSize = editorScene->nodes[editorScene->selectedNodeId]->props.fontSize;
        
        [self load3DTex:(selectedNodeType == NODE_TEXT) ? ASSET_TEXT : ASSET_TEXT_RIG AssetId:0 TextureName:[NSString stringWithCString:editorScene->nodes[selectedNode]->textureName.c_str()
                                                                                                                               encoding:[NSString defaultCStringEncoding]] TypedText:typedText FontSize:fontSize BevelValue:bevalValue TextColor:color FontPath:fontName isTempNode:NO];
    }
}


- (void) createDuplicateAssets {
    
    int selectedAssetId  = NOT_EXISTS;
    int selectedNode = NOT_EXISTS;
    NODE_TYPE selectedNodeType = NODE_UNDEFINED;
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        selectedAssetId = editorScene->nodes[editorScene->selectedNodeId]->assetId;
        selectedNodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
        selectedNode = editorScene->selectedNodeId;
    }
    
    if((selectedNodeType == NODE_RIG || selectedNodeType ==  NODE_SGM || selectedNodeType ==  NODE_OBJ) && selectedAssetId != NOT_EXISTS)
    {
        assetAddType = IMPORT_ASSET_ACTION;
        AssetItem *assetItem = [cache GetAsset:selectedAssetId];
        assetItem.textureName = [NSString stringWithCString:editorScene->nodes[selectedNode]->textureName.c_str()
                                                   encoding:[NSString defaultCStringEncoding]];
        [self performSelectorOnMainThread:@selector(loadNode:) withObject:assetItem waitUntilDone:YES];
        editorScene->animMan->copyPropsOfNode(selectedNode, editorScene->nodes.size()-1);
        
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
    else if((selectedNodeType == NODE_TEXT_SKIN || selectedNodeType == NODE_TEXT) && selectedAssetId != NOT_EXISTS){
        NSString *typedText = [self stringWithwstring:editorScene->nodes[editorScene->selectedNodeId]->name];
        NSString *fontName = [NSString stringWithCString:editorScene->nodes[editorScene->selectedNodeId]->optionalFilePath.c_str()
                                                encoding:[NSString defaultCStringEncoding]];
        Vector4 color = Vector4(editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.x,editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.y,editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.z,0.0);
        float bevalValue = editorScene->nodes[editorScene->selectedNodeId]->props.nodeSpecificFloat;
        int fontSize = editorScene->nodes[editorScene->selectedNodeId]->props.fontSize;
        [self load3DTex:(selectedNodeType == NODE_TEXT) ? ASSET_TEXT : ASSET_TEXT_RIG AssetId:0 TextureName:[NSString stringWithCString:editorScene->nodes[selectedNode]->textureName.c_str()
                                                                                                                               encoding:[NSString defaultCStringEncoding]] TypedText:typedText FontSize:fontSize BevelValue:bevalValue TextColor:color FontPath:fontName isTempNode:NO];
        //   [self load3DTex:(selectedNodeType == NODE_TEXT) ? ASSET_TEXT : ASSET_TEXT_RIG AssetId:0 TypedText:typedText FontSize:fontSize BevelValue:bevalValue TextColor:color FontPath:fontName isTempNode:NO];
        editorScene->animMan->copyPropsOfNode(selectedNode, editorScene->nodes.size()-1);
    }
}

- (void) changeTextureForAsset{
    if([Utility IsPadDevice]){
        [self.popoverController dismissPopoverAnimated:YES];
        objVc =[[ObjSidePanel alloc] initWithNibName:@"ObjSidePanel" bundle:Nil Type:CHANGE_TEXTURE];
        objVc.objSlideDelegate=self;
        [self showOrHideLeftView:YES withView:objVc.view];
    }
    else{
        [self.popoverController dismissPopoverAnimated:YES];
        objVc =[[ObjSidePanel alloc] initWithNibName:@"ObjSidePanelPhone" bundle:Nil Type:CHANGE_TEXTURE];
        objVc.objSlideDelegate=self;
        [self showOrHideLeftView:YES withView:objVc.view];
    }
    
}

-(void) updateAssetListInScenes
{
    [assetsInScenes removeAllObjects];
    for(int i = 0; i < editorScene->nodes.size(); i++){
        NSString* name = [self stringWithwstring:editorScene->nodes[i]->name];
        if(editorScene->nodes[i]->getType() == NODE_CAMERA){
            [assetsInScenes addObject:@"CAMERA"];
        }
        else if(editorScene->nodes[i]->getType() == NODE_LIGHT){
            [assetsInScenes addObject:@"LIGHT"];
        }
        else if(editorScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT){
            [assetsInScenes addObject:name];
        }
        else if(editorScene->nodes[i]->getType() == NODE_IMAGE){
            [assetsInScenes addObject:[NSString stringWithFormat:@"Image : %@",name]];
        }
        else if(editorScene->nodes[i]->getType() == NODE_TEXT_SKIN){
            [assetsInScenes addObject:[NSString stringWithFormat:@"Text : %@",name]];
        }
        else{
            [assetsInScenes addObject:name];
        }
    }
    
    [self.objectList reloadData];
    
}


- (void)objectSelectionCompleted:(int)assetIndex
{
    editorScene->selectMan->selectObject(assetIndex);
    [self setupEnableDisableControls];
    
}

#pragma mark PopUpViewConrollerDelegate

- (void) animationBtnDelegateAction:(int)indexValue
{
    if(indexValue==APPLY_ANIMATION){
        [self.popoverController dismissPopoverAnimated:YES];
        if (editorScene->selectedNodeId <= 1 || (!(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG) && !(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT_SKIN)) || editorScene->isJointSelected) {
            [self.view endEditing:YES];
            UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Please select a text or character to apply the animation." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [closeAlert show];
        }
        else{
            ANIMATION_TYPE animType = (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT_SKIN) ? TEXT_ANIMATION : RIG_ANIMATION;
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
        if (editorScene->selectedNodeId <= 1 || (!(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG) && !(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT_SKIN)) || editorScene->isJointSelected) {
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
    
    switch (indexValue) {
        case IMPORT_MODELS:
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
            break;
        case IMPORT_VIDEO:
        case IMPORT_IMAGES:
            if([Utility IsPadDevice])
            {
                [self.popoverController dismissPopoverAnimated:YES];
                self.imagePicker = [[UIImagePickerController alloc] init];
                self.imagePicker.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
                if(indexValue == IMPORT_VIDEO)
                    self.imagePicker.mediaTypes = [[NSArray alloc] initWithObjects:(NSString *)kUTTypeMovie,nil];
                [self.imagePicker setNavigationBarHidden:YES];
                [self.imagePicker setToolbarHidden:YES];
                importImageViewVC = [[ImportImageNew alloc] initWithNibName:@"ImportImageNew" bundle:nil Type:(indexValue == IMPORT_VIDEO) ? IMPORT_VIDEO : IMPORT_IMAGES];
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
                if(indexValue == IMPORT_VIDEO)
                    self.imagePicker.mediaTypes = [[NSArray alloc] initWithObjects:(NSString *)kUTTypeMovie,nil];
                [self.imagePicker setNavigationBarHidden:YES];
                [self.imagePicker setToolbarHidden:YES];
                importImageViewVC = [[ImportImageNew alloc] initWithNibName:@"ImportImageNewPhone" bundle:nil Type:(indexValue == IMPORT_VIDEO) ? IMPORT_VIDEO : IMPORT_IMAGES];
                [self showOrHideLeftView:YES withView:importImageViewVC.view];
                [self.imagePicker.view setFrame:CGRectMake(0, 0, importImageViewVC.imagesView.frame.size.width, importImageViewVC.imagesView.frame.size.height)];
                self.imagePicker.delegate=importImageViewVC;
                importImageViewVC.delegate = self;
                [importImageViewVC.imagesView addSubview:self.imagePicker.view];
            }
            break;
        case IMPORT_LIGHT:
            [self.popoverController dismissPopoverAnimated:YES];
            [self importAdditionalLight];
            break;
        case IMPORT_OBJFILE:
            if([Utility IsPadDevice]){
                [self.popoverController dismissPopoverAnimated:YES];
                objVc =[[ObjSidePanel alloc] initWithNibName:@"ObjSidePanel" bundle:Nil Type:IMPORT_OBJFILE];
                objVc.objSlideDelegate=self;
                [self showOrHideLeftView:YES withView:objVc.view];
            }
            else{
                [self.popoverController dismissPopoverAnimated:YES];
                objVc =[[ObjSidePanel alloc] initWithNibName:@"ObjSidePanelPhone" bundle:Nil Type:IMPORT_OBJFILE];
                objVc.objSlideDelegate=self;
                [self showOrHideLeftView:YES withView:objVc.view];
            }
            break;
        case IMPORT_ADDBONE:{
            NSLog(@"Bundle Path %s " , constants::BundlePath.c_str());
            [self.popoverController dismissPopoverAnimated:YES];
            NODE_TYPE selectedNodeType = NODE_UNDEFINED;
            if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
                selectedNodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
                
                if(selectedNodeType != NODE_SGM && selectedNodeType != NODE_TEXT) {
                    UIAlertView* error = [[UIAlertView alloc] initWithTitle:@"Alert" message:@"Bones cannot be added to this model." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
                    [error show];
                }
                else {
                    [self.addJointBtn setHidden:YES];
                    [self.moveLast setHidden:NO];
                    [self.moveFirst setHidden:NO];
                    [self.rigScreenLabel setHidden:NO];
                    selectedNodeId = editorScene->selectedNodeId;
                    editorScene->enterOrExitAutoRigMode(true);
                    editorScene->rigMan->sgmForRig(editorScene->nodes[selectedNodeId]);
                    editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(RIG_MODE_OBJVIEW));
                    editorScene->rigMan->boneLimitsCallBack = &boneLimitsCallBack;
                    [self setupEnableDisableControls];
                    [_rigCancelBtn setHidden:NO];
                }
            }
            else
            {
                UIAlertView* error = [[UIAlertView alloc] initWithTitle:@"Alert" message:@"Please Select any Node to add Bone" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
                [error show];
            }
            break;
        }
        case IMPORT_TEXT:{
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
            break;
        }
        default:
            break;
    }
}

- (void) exportBtnDelegateAction:(int)indexValue{
    if(indexValue==EXPORT_IMAGE){
        renderBgColor = Vector4(0.1,0.1,0.1,1.0);
        if([Utility IsPadDevice])
        {
            [self.popoverController dismissPopoverAnimated:YES];
            RenderingViewController* renderingView;
            renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewController" bundle:nil StartFrame:0 EndFrame:totalFrames renderOutput:RENDER_IMAGE caMresolution:0];  //FOR TESTING
            renderingView.delegate = self;
            renderingView.projectName=@"Scene 1";  //FOR TESTING
            renderingView.modalPresentationStyle = UIModalPresentationFormSheet;
            [self showOrHideRightView:YES];
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
        renderBgColor = Vector4(0.1,0.1,0.1,1.0);
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
            settingsVc.delegate=self;
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
                    settingsVc.delegate=self;
                    [settingsVc.view setClipsToBounds:YES];
                    settingsVc.modalPresentationStyle = UIModalPresentationFormSheet;
                    [self presentViewController:settingsVc animated:YES completion:nil];
                    settingsVc.view.superview.backgroundColor = [UIColor clearColor];
                }
                else
                {
                    [self.popoverController dismissPopoverAnimated:YES];
                    settingsVc = [[SettingsViewController alloc]initWithNibName:@"SettingsViewControllerPhone2x"bundle:nil];
                    settingsVc.delegate=self;
                    [settingsVc.view setClipsToBounds:YES];
                    [self presentViewController:settingsVc animated:YES completion:nil];
                    settingsVc.view.superview.backgroundColor = [UIColor clearColor];
                    
                }
            }
            else{
                [self.popoverController dismissPopoverAnimated:YES];
                settingsVc = [[SettingsViewController alloc]initWithNibName:@"SettingsViewControllerPhone"bundle:nil];
                [settingsVc.view setClipsToBounds:YES];
                settingsVc.delegate=self;
                settingsVc.modalPresentationStyle = UIModalPresentationFormSheet;
                [self presentViewController:settingsVc animated:YES completion:nil];
                settingsVc.view.superview.backgroundColor = [UIColor clearColor];
            }
            
        }
    }
    
}

-(void) optionBtnDelegate:(int)indexValue
{
    NSLog(@"Mova camera Clicked");
    [self.popoverController dismissPopoverAnimated:YES];
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
    int type = editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT_SKIN ? 1 : 0;
    
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
            //BOOL assetPurchaseStatus = [[CacheSystem cacheSystem] checkDownloadedAsset:stoi(fileName)];
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
            if (([file intValue] >= 20000 && ([file intValue] < 30000)) || ([file intValue] >= 60000 && ([file intValue] < 60010))) {
                NSString* rigPath = [NSString stringWithFormat:@"%@/Resources/Sgm/%d.sgm", documentsDirectory, [file intValue]];
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
            NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/mesh/%s.%@", fileName.c_str(), extension];
            if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
                downloadFile(url, filePath);
            NSString* texfilePath = [NSString stringWithFormat:@"%@/%s-cm.png", cacheDirectory, fileName.c_str()];
            NSString* texurl = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/meshtexture/%s.png", fileName.c_str()];
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
        case NODE_TEXT:
        case NODE_TEXT_SKIN: {
            AssetItem* assetObj = [[CacheSystem cacheSystem] GetAssetByName:[NSString stringWithUTF8String:fileName.c_str()]];
            BOOL assetPurchaseStatus = [[CacheSystem cacheSystem] checkDownloadedAsset:assetObj.assetId];
            NSString* extension = [[NSString stringWithUTF8String:fileName.c_str()] pathExtension];
            
            NSString* filePath = [NSString stringWithFormat:@"%@/%s", cacheDirectory, fileName.c_str()];
            NSString* docFilePath = [NSString stringWithFormat:@"%@/Resources/Fonts/%s", documentsDirectory, fileName.c_str()];
            if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
                if (![[NSFileManager defaultManager] fileExistsAtPath:docFilePath]) {
                    NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/font/%d.%@", assetObj.assetId, extension];
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

#pragma mark Login Delegate

-(void)googleSigninDelegate{
    isLoggedin= true;
}

-(void)dismisspopover{
    [self.popoverController dismissPopoverAnimated:YES];
}

#pragma mark LoggedinViewController Delegate

-(void)dismissView{
    NSLog(@"delegate called");
    [_loggedInVc dismissViewControllerAnimated:YES completion:nil];
    [self.popoverController dismissPopoverAnimated:YES];
    
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
    if (alertView.tag == CHOOSE_RIGGING_METHOD) {
        if(buttonIndex == HUMAN_RIGGING) {
            editorScene->rigMan->skeletonType = SKELETON_HUMAN;
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode+1));
        }else if(buttonIndex == OWN_RIGGING) {
            editorScene->rigMan->skeletonType = SKELETON_OWN;
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode+1));
        }
    }
    if (alertView.tag == DATA_LOSS_ALERT) {
        if(buttonIndex == CANCEL_BUTTON_INDEX) {
            
        } else if(buttonIndex == OK_BUTTON_INDEX) {
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode-1));
        }
    }
}


#pragma mark SettingsViewControllerDelegate

-(void)frameCountDisplayMode:(int)selctedIndex{
    if(selctedIndex==0){
        NSLog(@"Frame Vount Display");
        [self.framesCollectionView setTag:1];
        [self.framesCollectionView reloadData];
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithLong:self.framesCollectionView.tag] withKey:@"indicationType"];
    }
    else
    {
        NSLog(@"Frame Durations Display");
        [self.framesCollectionView setTag:2];
        [self.framesCollectionView reloadData];
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithLong:self.framesCollectionView.tag] withKey:@"indicationType"];
    }
}

-(void)cameraPreviewSize:(int)selctedIndex{
    if(selctedIndex==0)
    {
        editorScene->camPreviewScale=1.0;
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithFloat:editorScene->camPreviewScale] withKey:@"cameraPreviewSize"];
    }
    else
    {
        editorScene->camPreviewScale=2.0;
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithFloat:editorScene->camPreviewScale] withKey:@"cameraPreviewSize"];
    }
}

-(void)cameraPreviewPosition:(int)selctedIndex{
    NSLog(@"Camera preview Origin: X Value: %f Y Value: %f ",self.renderView.bounds.size.width,self.renderView.bounds.size.height);
    float camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
    if(selctedIndex==0){
        camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
        if(editorScene->camPreviewScale==1.0){
            editorScene->camPreviewOrigin.x=0;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height-camPrevRatio;
            editorScene->camPreviewEnd.x=-editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height-camPrevRatio;
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            
        }
    }
    if(selctedIndex==1){
        camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
        if(editorScene->camPreviewScale==1.0){
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height;
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height;
        }
    }
    
    if(selctedIndex==2){
        if(editorScene->camPreviewScale==1.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=self.renderView.frame.size.width-self.rightView.frame.size.width-camPrevRatio;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height-camPrevRatio;
            editorScene->camPreviewEnd.x=-editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            
            
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=self.renderView.frame.size.width-self.rightView.frame.size.width;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height-camPrevRatio;
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            NSLog(@"Camera preview orgin.x : %f",editorScene->camPreviewOrigin.x);
            
        }
    }
    if(selctedIndex==3){
        
        if(editorScene->camPreviewScale==1.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=self.view.frame.size.width-self.rightView.frame.size.width;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height;
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=self.view.frame.size.width-self.rightView.frame.size.width;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height;
        }
    }
    
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:selctedIndex] withKey:@"cameraPreviewPosition"];
}
-(void)toolbarPosition:(int)selctedIndex
{
    CGRect frame = self.rightView.frame;
    frame.origin.x = (selctedIndex==1) ? 0 : self.view.frame.size.width-self.rightView.frame.size.width;
    frame.origin.y = self.topView.frame.size.height; // new y coordinate
    self.rightView.frame = frame;
    CGRect frame1 = self.leftView.frame;
    frame1.origin.x = (selctedIndex==1) ? self.view.frame.size.width-self.leftView.frame.size.width : 0;
    frame1.origin.y = self.topView.frame.size.height; // new y coordinate
    self.leftView.frame = frame1;
    if(editorScene) {
        editorScene->topLeft = Vector2((selctedIndex==0) ? 0.0 : self.rightView.frame.size.width, self.topView.frame.size.height) * editorScene->screenScale;
        editorScene->topRight = Vector2((selctedIndex==0) ? self.view.frame.size.width-self.rightView.frame.size.width : self.view.frame.size.width, self.topView.frame.size.height) * editorScene->screenScale;
        editorScene->bottomLeft = Vector2((selctedIndex==0) ? 0.0 : self.rightView.frame.size.width, self.view.frame.size.height) * editorScene->screenScale;
        editorScene->bottomRight = Vector2((selctedIndex==0) ? self.view.frame.size.width-self.rightView.frame.size.width : self.view.frame.size.width, self.view.frame.size.height) * editorScene->screenScale;
        
        editorScene->renHelper->movePreviewToCorner();
    }
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:selctedIndex] withKey:@"toolbarPosition"];
}


- (void)setupEnableDisableControlsPlayAnimation{
    if(editorScene->isPlaying){
        [self.addFrameBtn setEnabled:false];
        [self.loginBtn setEnabled:false];
        [self.viewBtn setEnabled:false];
        [self.infoBtn setEnabled:false];
    }
    else {
        [self.lastFrameBtn setEnabled:true];
        [self.addFrameBtn setEnabled:true];
        [self.loginBtn setEnabled:true];
        [self.viewBtn setEnabled:true];
        [self.infoBtn setEnabled:true];
    }
}

#pragma AutoRig Delegates

- (void) autoRigViewButtonHandler:(bool)disable
{
    [self.addJointBtn setHidden:disable];
    [self.moveLast setHidden:disable];
    [self.moveFirst setHidden:disable];
    [self.rigScreenLabel setHidden:disable];
    [_rigAddToSceneBtn setHidden:disable];
    [_rigCancelBtn setHidden:disable];
}

- (void)importObjAndTexture:(int)indexPathOfOBJ TextureName:(NSString*)textureFileName VertexColor:(Vector3)color haveTexture:(BOOL)isHaveTexture IsTempNode:(BOOL)isTempNode
{
    NSArray* basicShapes = [NSArray arrayWithObjects:@"Cone",@"cube",@"Cylinder",@"Plane",@"Sphere",@"Torus",nil];
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSArray *dirFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirPath error:nil];
    NSArray* filesList = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.obj'"]];
    NSFileManager* fm = [[NSFileManager alloc]init];
    std::string *objStr = NULL;
    std::string *textureStr = new std::string([(!isHaveTexture) ? @"White-texture" : textureFileName UTF8String]);
    NSString* assetName = (indexPathOfOBJ < 6 ) ? [basicShapes objectAtIndex:indexPathOfOBJ] : [[filesList objectAtIndex:indexPathOfOBJ - 6]stringByDeletingPathExtension];
    int assetId = (!isTempNode) ? -1 : 123456;
    int assetIdReturn = 0;
    NSArray* basicShapesId = [NSArray arrayWithObjects:@"60001",@"60002",@"60003",@"60004",@"60005",@"60006",nil];
    if(indexPathOfOBJ < 6){//Total Basic Shapes 6
        assetId = [[basicShapesId objectAtIndex:indexPathOfOBJ]intValue];
    }
    else{
        objStr = new std::string([[[filesList objectAtIndex:indexPathOfOBJ - 6]stringByDeletingPathExtension] UTF8String]);
        editorScene->objMan->loadAndSaveAsSGM(*objStr, *textureStr, 123456,!isHaveTexture,color);
    }
    
    NSString* texTo = [NSString stringWithFormat:@"%@/Resources/Textures/%d-cm.png",docDirPath,assetId];
    [fm removeItemAtPath:texTo error:nil];
    
    if(!isTempNode){
        assetIdReturn =  [self addSgmFileToCacheDirAndDatabase:assetName];
    }
    
    NSString* texFrom = [NSString stringWithFormat:@"%@/%@.png",docDirPath,(!isHaveTexture)?@"Resources/Sgm/White-texture":textureFileName];
    texTo = [NSString stringWithFormat:@"%@/Resources/Textures/%d-cm.png",docDirPath,assetIdReturn];
    [fm copyItemAtPath:texFrom toPath:texTo error:nil];
    
    if (!isTempNode){
        NSString* sgmFrom = [NSString stringWithFormat:@"%@%@%d.sgm",docDirPath,@"/Resources/Sgm/",(indexPathOfOBJ < 6) ? assetId : 123456];
        NSString* sgmTo = [NSString stringWithFormat:@"%@/Resources/Sgm/%d.sgm",docDirPath,assetIdReturn];
        [fm copyItemAtPath:sgmFrom toPath:sgmTo error:nil];
    }
    NSMutableDictionary* dict = [[NSMutableDictionary alloc]init];
    [dict setObject:assetName forKey:@"name"];
    [dict setObject:[NSNumber numberWithInt:(isTempNode) ? assetId : assetIdReturn] forKey:@"assetId"];
    [dict setObject:[NSNumber numberWithFloat:color.x] forKey:@"x"];
    [dict setObject:[NSNumber numberWithFloat:color.y] forKey:@"y"];
    [dict setObject:[NSNumber numberWithFloat:color.z] forKey:@"z"];
    NSString *textureName;
    if(isTempNode)
        textureName = textureFileName;
    else
        textureName = [NSString stringWithFormat:@"%d%@",assetIdReturn,@"-cm"];
    [dict setObject:[NSString stringWithFormat:@"%@",(isHaveTexture) ? textureName : @"-1"] forKey:@"textureName"];
    [dict setObject:[NSNumber numberWithBool:isTempNode] forKey:@"isTempNode"];
    [dict setObject:[NSNumber numberWithBool:isHaveTexture] forKey:@"isHaveTexture"];
    [self performSelectorOnMainThread:@selector(loadObjOrSGM:) withObject:dict waitUntilDone:YES];
}

-(int)addSgmFileToCacheDirAndDatabase:(NSString*)fileName
{
    NSArray* directoryPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectoryPath = [directoryPath objectAtIndex:0];
    NSDate *currDate = [NSDate date];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc]init];
    [dateFormatter setDateFormat:@"YY-MM-DD HH:mm:ss"];
    NSString *dateString = [dateFormatter stringFromDate:currDate];
    AssetItem* sgmAsset = [[AssetItem alloc] init];
    sgmAsset.assetId = 20000 + [cache getNextObjAssetId];
    sgmAsset.type = NODE_SGM;
    sgmAsset.name = [NSString stringWithFormat:@"%@%d",fileName,[cache getNextObjAssetId]];
    sgmAsset.iap = 0;
    sgmAsset.keywords = [NSString stringWithFormat:@" %@",fileName];
    sgmAsset.boneCount = 0;
    NSString *filePath = [NSString stringWithFormat:@"%@/%@", documentsDirectoryPath, fileName];
    sgmAsset.hash = [self getMD5ForNonReadableFile:filePath];
    sgmAsset.modifiedDate = dateString;
    [cache UpdateAsset:sgmAsset];
    [cache AddDownloadedAsset:sgmAsset];
    return sgmAsset.assetId;
}

-(int)addrigFileToCacheDirAndDatabase:(NSString*)texturemainFileNameRig
{
    NSString *tempDir = NSTemporaryDirectory();
    NSString *sgrFilePath = [NSString stringWithFormat:@"%@r-%@.sgr", tempDir, @"autorig"];
    NSDate *currDate = [NSDate date];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc]init];
    [dateFormatter setDateFormat:@"YY-MM-DD HH:mm:ss"];
    NSString *dateString = [dateFormatter stringFromDate:currDate];
    AssetItem* objAsset = [[AssetItem alloc] init];
    objAsset.assetId = 40000 + [cache getNextAutoRigAssetId];
    objAsset.type = 1;
    objAsset.name = [NSString stringWithFormat:@"autorig%d",[cache getNextAutoRigAssetId]];
    objAsset.iap = 0;
    objAsset.keywords = [NSString stringWithFormat:@" %@ , %@",texturemainFileNameRig,@"autorig"];
    objAsset.boneCount = 0;
    objAsset.hash = [self getMD5ForNonReadableFile:sgrFilePath];
    objAsset.modifiedDate = dateString;
    objAsset.price = @"FREE";
    [cache UpdateAsset:objAsset];
    [cache AddDownloadedAsset:objAsset];
    [self storeRiginCachesDirectory:objAsset.name assetId:objAsset.assetId];
    [self storeRigTextureinCachesDirectory:texturemainFileNameRig assetId:objAsset.assetId];
    assetAddType = IMPORT_ASSET_ACTION;
    objAsset.isTempAsset = NO;
    objAsset.textureName = [NSString stringWithFormat:@"%d%@",objAsset.assetId,@"-cm"];
    [self performSelectorOnMainThread:@selector(loadNode:) withObject:objAsset waitUntilDone:YES];
    editorScene->animMan->copyKeysOfNode(selectedNodeId, editorScene->nodes.size()-1);
    editorScene->updater->setDataForFrame(editorScene->currentFrame);
}

-(void) storeRiginCachesDirectory:(NSString*)desFilename assetId:(int)localAssetId
{
    NSString *tempDir = NSTemporaryDirectory();
    NSString *sgrFilePath = [NSString stringWithFormat:@"%@r-%@.sgr", tempDir, @"autorig"];
    NSArray* desDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [desDirPath objectAtIndex:0];
    NSString* rigDirPath = [NSString stringWithFormat:@"%@/Resources/Rigs",docDirPath];
    NSString* desFilePath = [NSString stringWithFormat:@"%@/%d.sgr",rigDirPath,localAssetId];
    NSData *objData = [NSData dataWithContentsOfFile:sgrFilePath];
    [objData writeToFile:desFilePath atomically:YES];
}

-(void) storeRigTextureinCachesDirectory:(NSString*)fileName assetId:(int)localAssetId
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString* srcTextureFilePath = [NSString stringWithFormat:@"%@/%@",docDirPath,fileName];
    if (![[NSFileManager defaultManager] fileExistsAtPath:srcTextureFilePath]){
        srcTextureFilePath = [NSString stringWithFormat:@"%@/%@",cacheDirectory,fileName];
        if (![[NSFileManager defaultManager] fileExistsAtPath:srcTextureFilePath]){
            srcTextureFilePath = [NSString stringWithFormat:@"%@/Resources/Sgm/%@",docDirPath,fileName];
            if (![[NSFileManager defaultManager] fileExistsAtPath:srcTextureFilePath])
                return;
        }
    }
    
    NSString* desFilePath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d-cm.png",docDirPath,localAssetId];
    NSString* desFilePathForDisplay = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.png",docDirPath,localAssetId];
    
    UIImage *image =[UIImage imageWithContentsOfFile:srcTextureFilePath];
    NSData *imageData = [self convertAndScaleImage:image size:-1];
    NSData *imageDataforDisplay = [self convertAndScaleImage:image size:128];
    // image File size should be exactly 128 for display.
    [imageData writeToFile:desFilePath atomically:YES];
    [imageDataforDisplay writeToFile:desFilePathForDisplay atomically:YES];
}

- (NSData*) convertAndScaleImage:(UIImage*)image size:(int)textureRes
{
    float target = 0;
    if(textureRes == -1) {
        
        float imgW = image.size.width;
        float imgH = image.size.height;
        float bigSide = (imgW >= imgH) ? imgW : imgH;
        //Convert texture image size should be the 2 to the power values for convinent case.
        if(bigSide <= 128)
            target = 128;
        else if(bigSide <= 256)
            target = 256;
        else if(bigSide <= 512)
            target = 512;
        else
            target = 1024;
    }
    else
        target = (float)textureRes;
    
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(target, target), NO, 1.0);
    [image drawInRect:CGRectMake(0, 0, target, target)];
    UIImage* nImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    NSData* data = UIImagePNGRepresentation(nImage);
    return data;
}

-(NSString*) getMD5ForNonReadableFile:(NSString*) path
{
    NSData* data = [NSData dataWithContentsOfFile:path];
    unsigned char result[CC_MD5_DIGEST_LENGTH];
    CC_MD5([data bytes], (int)[data length], result);
    NSMutableString *hashString = [NSMutableString stringWithCapacity:CC_MD5_DIGEST_LENGTH * 2];
    for(int i = 0; i < CC_MD5_DIGEST_LENGTH; ++i) {
        [hashString appendFormat:@"%02x", result[i]];
    }
    return [NSString stringWithString:hashString];
}

- (void) loadObjOrSGM:(NSMutableDictionary*)dict
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    Vector4 color = Vector4([[dict objectForKey:@"x"]floatValue],[[dict objectForKey:@"y"]floatValue],[[dict objectForKey:@"z"]floatValue],1.0);
    int assetId = [[dict objectForKey:@"assetId"]intValue];
    wstring assetName = [self getwstring:[dict objectForKey:@"name"]];
    BOOL isTempNode = [[dict objectForKey:@"isTempNode"]boolValue];
    BOOL isHaveTexture = [[dict objectForKey:@"isHaveTexture"]boolValue];
    
    [renderViewMan loadNodeInScene:NODE_SGM AssetId:assetId AssetName:assetName TextureName:([dict objectForKey:@"textureName"]) Width:0 Height:0 isTempNode:isTempNode More:dict ActionType:IMPORT_ASSET_ACTION VertexColor:color];
    
    if(!isTempNode){
        if(!isHaveTexture){
            editorScene->selectMan->selectObject(editorScene->nodes.size()-1);
            [self changeTexture:@"0-cm" VertexColor:Vector3(1.0) IsTemp:YES]; // for White Texture Thumbnail
            editorScene->selectMan->unselectObject(editorScene->nodes.size()-1);
        }
        if(assetId >= 20000 && assetId <= 30000){
            NSMutableArray* nodes = [[NSMutableArray alloc]init];
            for(int i = 0; i < editorScene->nodes.size(); i++){
                if(i != editorScene->nodes.size()-1 && editorScene->nodes[i]->props.isVisible == true){
                    editorScene->nodes[i]->node->setVisible(false);
                    [nodes addObject:[NSNumber numberWithInt:i]];
                }
            }
            editorScene->renHelper->isExportingImages = true;
            editorScene->updater->setDataForFrame(editorScene->currentFrame);
            NSString* imageFilePath = [NSString stringWithFormat:@"%@/Resources/Sgm/%d.png", docDirPath, assetId];
            renderBgColor = Vector4(0.1,0.1,0.1,1.0);
            editorScene->renHelper->renderAndSaveImage((char*)[imageFilePath cStringUsingEncoding:NSUTF8StringEncoding], 0, false, YES,renderBgColor);
            
            for(int i = 0; i < [nodes count]; i++){
                editorScene->nodes[[[nodes objectAtIndex:i]intValue]]->node->setVisible(true);
            }
            [nodes removeAllObjects];
            nodes = nil;
        }
        if(!isHaveTexture){
            editorScene->selectMan->selectObject(editorScene->nodes.size()-1);
            [self changeTexture:[dict objectForKey:@"textureName"] VertexColor:Vector3(color.x,color.y,color.z) IsTemp:YES]; // back to original Texture
            editorScene->selectMan->unselectObject(editorScene->nodes.size()-1);
        }
    }
}

void boneLimitsCallBack(){
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    EditorViewController *autoRigVC = (EditorViewController*)[[appDelegate window] rootViewController];
    [autoRigVC boneLimitsAlert];
}

- (void) boneLimitsAlert
{
    [self.view endEditing:YES];
    UIAlertView *boneLimitMsg = [[UIAlertView alloc]initWithTitle:@"Information" message:@"The maximum bones per object cannot exceed 57." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [boneLimitMsg show];
}

#pragma mark Autorig ScaleView Delegate

- (void) scalePropertyChangedInRigView:(float)scaleValue{
    if((editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)(RIG_MODE_EDIT_ENVELOPES)) &&  editorScene->rigMan->isSkeletonJointSelected){
        editorScene->rigMan->changeEnvelopeScale(Vector3(scaleValue), false);
    }
}

- (void) scaleValueForAction:(float)scaleValue{
    
}


#pragma mark change Texture Delegates

- (void) changeTexture:(NSString*)textureName VertexColor:(Vector3)color IsTemp:(BOOL)isTemp
{
    
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* srcTextureFilePath = [NSString stringWithFormat:@"%@/%@.png",docDirPath,textureName];
    std::string *texture = new std::string([textureName UTF8String]);
    if ([[NSFileManager defaultManager] fileExistsAtPath:srcTextureFilePath]){
        NSString* desFilePath = [NSString stringWithFormat:@"%@/Resources/Textures/%@.png",docDirPath,(isTemp) ? @"temp" : textureName];
        UIImage *image =[UIImage imageWithContentsOfFile:srcTextureFilePath];
        NSData *imageData = [self convertAndScaleImage:image size:-1];
        [imageData writeToFile:desFilePath atomically:YES];
        *texture = *new std::string([(isTemp)?@"temp" : textureName UTF8String]);
    }
    editorScene->selectMan->selectObject(selectedNodeId);
    if(!(editorScene->selectedNodeIds.size() > 0) && editorScene->hasNodeSelected()){
        editorScene->changeTexture(*texture, Vector3(color),isTemp,NO);
        editorScene->selectMan->unselectObject(selectedNodeId);
        editorScene->setLightingOn();
    }
}

- (void) removeTempTextureAndVertex
{
    editorScene->removeTempTextureAndVertex(selectedNodeId);
}

#pragma Rendering Delegates

- (void) changeRenderingBgColor:(Vector3)vertexColor
{
    renderBgColor = Vector4(vertexColor,1.0);
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
    objVc.objSlideDelegate = nil;
    objVc = nil;
    currentScene = nil;
    settingsVc = nil;
    screenHeight = nil;
}

@end


