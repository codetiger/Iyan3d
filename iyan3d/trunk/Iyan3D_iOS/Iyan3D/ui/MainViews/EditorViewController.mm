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
#define RENDER_GIF 2


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    #if !(TARGET_IPHONE_SIMULATOR)
        if (iOSVersion >= 8.0)
            isMetalSupported = (MTLCreateSystemDefaultDevice() != NULL) ? true : false;
    #endif

    constants::BundlePath = (char*)[[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSASCIIStringEncoding];

    totalFrames = 24;
    
    
    if ([Utility IsPadDevice])
         [self.framesCollectionView registerNib:[UINib nibWithNibName:@"FrameCellNew" bundle:nil] forCellWithReuseIdentifier:@"FRAMECELL"];
    else
         [self.framesCollectionView registerNib:[UINib nibWithNibName:@"FrameCellNewPhone" bundle:nil] forCellWithReuseIdentifier:@"FRAMECELL"];

    assetsInScenes = [NSMutableArray array];
    
    [self.objectList reloadData];
    
}

- (void)viewDidAppear:(BOOL)animated
{
    if(!editorScene) {
        [self initScene];
        [self createDisplayLink];
        if(renderViewMan){
            [renderViewMan addCameraLight];
        }
    }
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
    [renderViewMan setUpPaths];
    [renderViewMan setUpCallBacks:editorScene];
    [renderViewMan addGesturesToSceneView];
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

- (void) loadNodeInScene:(AssetItem*)assetItem
{
    [self performSelectorOnMainThread:@selector(loadNode:) withObject:assetItem waitUntilDone:YES];
}
     
- (void) loadNode:(AssetItem*) asset
{
    [renderViewMan loadNodeInScene:asset.type AssetId:asset.assetId AssetName:[self getwstring:asset.name] Width:0 Height:0 isTempNode:asset.isTempAsset];
}

- (void)createDisplayLink
{
    CADisplayLink* displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateRenderer)];
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
}

#pragma mark - Object Selection Table View Deligates

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [assetsInScenes count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSLog(@"Table View Created");
    static NSString *simpleTableIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];
    
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:simpleTableIdentifier];
    }
    cell.textLabel.font=[cell.textLabel.font fontWithSize:13];
    cell.textLabel.text = [assetsInScenes objectAtIndex:indexPath.row];
    if([cell.textLabel.text isEqualToString:@"CAMERA"])
    {
        cell.imageView.image = [UIImage imageNamed:@"My-objects-Camera_Pad.png"];
    }
    else if([cell.textLabel.text isEqualToString:@"LIGHT"])
    {
        cell.imageView.image = [UIImage imageNamed:@"My-objects-Light_Pad.png"];
    }
    else
    {
        cell.imageView.image = [UIImage imageNamed:@"My-objects-Models_Pad.png"];
    }
    return cell;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        [renderViewMan removeNodeFromScene:(int)indexPath.row];
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

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    
    [self objectSelectionCompleted:indexPath.row];
    
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

#pragma mark - Button Actions

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
    isLoggedin=YES;
    if(isLoggedin){
        _loggedInVc = [[LoggedInViewController alloc] initWithNibName:@"LoggedInViewController" bundle:nil];
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_loggedInVc];
        self.popoverController.popoverContentSize = CGSizeMake(305, 525);
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
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:totalFrames-1 inSection:0];
    [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:YES];
}

- (IBAction)firstFrameBtnAction:(id)sender
{
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:0 inSection:0];
    [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:YES];
}

- (IBAction)optionsBtnAction:(id)sender
{		
    NSLog(@"Options Clicked");
}

- (IBAction)moveBtnAction:(id)sender
{
    NSLog(@"Move Button Clicked");
}

- (IBAction)rotateBtnAction:(id)sender
{
    NSLog(@"Rotate Button Clicked");
}

- (IBAction)scaleBtnAction:(id)sender
{
    NSLog(@"Scale Button Clicked");
}

- (IBAction)undoBtnAction:(id)sender
{
    NSLog(@"UndoBtn Clicked");
}

- (IBAction)redoBtnAction:(id)sender
{
    NSLog(@"RedoBtn Clicked");
}




- (void) presentViewControllerInCurrentView:(UIViewController*) vcToPresent
{
    [self presentViewController:vcToPresent animated:YES completion:nil];
}

#pragma mark - Rendering ViewCOntroller Delegates

- (void) renderFrame:(int)frame withType:(int)shaderType andRemoveWatermark:(bool)removeWatermark{
    
}
- (void) setShaderTypeForRendering:(int)shaderType{
    
}
- (NSMutableArray*) exportSGFDsWith:(int)startFrame EndFrame:(int)endFrame{
    
}
- (CGPoint) getCameraResolution{
    
}
- (void) cameraResolutionChanged:(int)resolutinType{
    
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

-(void)pickedImageWithInfo:(NSDictionary*)info;
{
    NSURL *imageURL = [info valueForKey:UIImagePickerControllerReferenceURL];
    NSLog(@"Image Path %@",imageURL);
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

- (void) presentPopOver:(CGRect )arect{
    NSLog(@"Delegate called");
    
    UIAlertController * view=   [UIAlertController
                                 alertControllerWithTitle:nil
                                 message:nil
                                 preferredStyle:UIAlertControllerStyleActionSheet];
    UIAlertAction* deleteButton = [UIAlertAction
                                   actionWithTitle:@"Delete"
                                   style:UIAlertActionStyleDefault
                                   handler:^(UIAlertAction * action)
                                   {
                                       NSLog(@"Delete Selected Node Id: %d",editorScene->selectedNodeId);
                                       [renderViewMan removeNodeFromScene:editorScene->selectedNodeId];
                                       
                                   }];
    UIAlertAction* duplicateButton = [UIAlertAction
                                      actionWithTitle:@"Duplicate"
                                      style:UIAlertActionStyleDefault
                                      handler:^(UIAlertAction * action)
                                      {
                                          
                                      }];
    
    [view addAction:duplicateButton];
    [view addAction:deleteButton];
    UIPopoverPresentationController *popover = view.popoverPresentationController;
    popover.sourceRect = arect;
    popover.sourceView=self.renderView;
    popover.permittedArrowDirections = UIPopoverArrowDirectionAny;
    [self presentViewController:view animated:YES completion:nil];

    
}
-(void) updateAssetListInScenes :(int)nodeType assetId:(int)assetId actionType:(int)action

{
    if(action==100){
        if(nodeType==7){
            [assetsInScenes addObject:@"CAMERA"];
        }
        else if(nodeType==8){
            [assetsInScenes addObject:@"LIGHT"];
        }
        else{
            CacheSystem *cacheSystem;
            cacheSystem = [CacheSystem cacheSystem];
            AssetItem *assetObj = [cacheSystem GetAsset:assetId];
            [assetsInScenes addObject:assetObj.name];
            [self.objectList reloadData];
        }
    }
    else if (action==200){
        [assetsInScenes removeObjectAtIndex:assetId];
        [self.objectList reloadData];
    }
    
}


- (void)objectSelectionCompleted:(int)assetIndex
{
    editorScene->selectMan->selectObject(assetIndex);
   
}

#pragma mark PopUpViewConrollerDelegate

- (void) animationBtnDelegateAction:(int)indexValue
{
    if(indexValue==0){
         [self.popoverController dismissPopoverAnimated:YES];
        animationsliderVC =[[AnimationSelectionSlider alloc] initWithNibName:@"AnimationSelectionSlider" bundle:Nil];
        animationsliderVC.delegate = self;
        [self showOrHideLeftView:YES withView:animationsliderVC.view];
    }
    else if(indexValue==1){
        NSLog(@"Save Animation");
    }
    
}
- (void) importBtnDelegateAction:(int)indexValue{

    if(indexValue==0){
        [self.popoverController dismissPopoverAnimated:YES];
        assetSelectionSlider =[[AssetSelectionSidePanel alloc] initWithNibName:@"AssetSelectionSidePanel" bundle:Nil];
        assetSelectionSlider.assetSelectionDelegate = self;
        [self showOrHideLeftView:YES withView:assetSelectionSlider.view];
    }
    else if(indexValue==1){
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
    else if(indexValue==2){
        [self.popoverController dismissPopoverAnimated:YES];
        textSelectionSlider =[[TextSelectionSidePanel alloc] initWithNibName:@"TextSelectionSidePanel" bundle:Nil];
        textSelectionSlider.textSelectionDelegate = self;
        [self showOrHideLeftView:YES withView:textSelectionSlider.view];

    }
    else if(indexValue==3){
        NSLog(@"Light Button");
    }
    else if(indexValue==4){
        NSLog(@"OBJ Button");
    }
    else if(indexValue==5){
        NSLog(@"ADD BONE Button");
    }

}
- (void) exportBtnDelegateAction:(int)indexValue{
    if(indexValue==0){
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
        
        if ([Utility IsPadDevice]) {
            renderingView.view.superview.backgroundColor = [UIColor clearColor];
            renderingView.view.layer.borderWidth = 2.0f;
            renderingView.view.layer.borderColor = [UIColor grayColor].CGColor;
        }
    
    }
    else if(indexValue==1){
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
            renderingView.view.layer.borderWidth = 2.0f;
            renderingView.view.layer.borderColor = [UIColor grayColor].CGColor;
            
            NSLog(@"Videos Clicked");
        }
        
    }

}
- (void) viewBtnDelegateAction:(int)indexValue{
    if(indexValue==0){
        [self.popoverController dismissPopoverAnimated:YES];
       editorScene->updater->changeCameraView(VIEW_FRONT);
    }
    else if(indexValue==1){
         [self.popoverController dismissPopoverAnimated:YES];
        editorScene->updater->changeCameraView(VIEW_TOP);
    }
    else if(indexValue==2){
         [self.popoverController dismissPopoverAnimated:YES];
        editorScene->updater->changeCameraView(VIEW_LEFT);
    }
    else if(indexValue==3){
         [self.popoverController dismissPopoverAnimated:YES];
        editorScene->updater->changeCameraView(VIEW_BACK);
    }
    else if(indexValue==4){
         [self.popoverController dismissPopoverAnimated:YES];
        editorScene->updater->changeCameraView(VIEW_RIGHT);
    }
    else if(indexValue==5){
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
  
    [self.popoverController dismissPopoverAnimated:YES];
    loginVc = [[LoginViewController alloc] initWithNibName:@"LoginViewController" bundle:nil];
    [loginVc.view setClipsToBounds:YES];
    loginVc.modalPresentationStyle = UIModalPresentationFormSheet;
    [self presentViewController:loginVc animated:YES completion:nil];
    loginVc.view.superview.backgroundColor = [UIColor clearColor];
    NSLog(@"Frame Height :%f",loginVc.view.frame.size.height);
   
    
}
- (void) infoBtnDelegateAction:(int)indexValue{
   

}
- (void)dealloc
{
    renderViewMan.delegate = nil;
    renderViewMan = nil;
}

@end


