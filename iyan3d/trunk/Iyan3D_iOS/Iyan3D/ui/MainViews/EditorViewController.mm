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
#import <sys/utsname.h>
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
#define IMPORT_PARTICLE 7
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
#define ASSET_PARTICLES 13
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
#define DELETE_BUTTON_OBJECT 1
#define DELETE_BUTTON_OBJECT_ANIMATION 2
#define SGR_WARNING 512

#define CAMERA_PREVIEW_SMALL 0

#define FRAME_COUNT 0
#define FRAME_DURATION 1

#define PREVIEW_LEFTBOTTOM 0
#define PREVIEW_LEFTTOP 1
#define PREVIEW_RIGHTBOTTOM 2
#define PREVIEW_RIGHTTOP 3

#define TOOLBAR_RIGHT 0
#define TOOLBAR_LEFT 1

#define ONE_FRAME 0
#define TWENTY_FOUR_FRAMES 1
#define TWO_FOURTY_FRAMES 2


#define SETTINGS 3
#define CONTACT_US 4

#define MIRROR_OFF 0
#define MIRROR_ON 1
#define MIRROR_DISABLE 2

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
        cachesDir = [paths objectAtIndex:0];
        NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        docDir = [docPaths objectAtIndex:0];
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
    [self updateXYZValuesHide:YES X:0.0 Y:0.0 Z:0.0];
    self.rigAddToSceneBtn.layer.cornerRadius = CORNER_RADIUS;
    self.rigCancelBtn.layer.cornerRadius = CORNER_RADIUS;
    self.publishBtn.layer.cornerRadius = CORNER_RADIUS;
  
//    _addFrameBtn.imageView.contentMode = UIViewContentModeScaleAspectFit;
//    _lastFrameBtn.imageView.contentMode = UIViewContentModeScaleAspectFit;
    
#if !(TARGET_IPHONE_SIMULATOR)
    if (iOSVersion >= 8.0)
        isMetalSupported = (MTLCreateSystemDefaultDevice() != NULL) ? true : false;
#endif
//    CGRect screenRect = [[UIScreen mainScreen] bounds];
//    screenHeight = screenRect.size.height;
    constants::BundlePath = (char*)[[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSASCIIStringEncoding];
    [[UIApplication sharedApplication] setStatusBarHidden:NO];
    if ([Utility IsPadDevice])
        [self.framesCollectionView registerNib:[UINib nibWithNibName:@"FrameCellNew" bundle:nil] forCellWithReuseIdentifier:@"FRAMECELL"];
    else
        [self.framesCollectionView registerNib:[UINib nibWithNibName:@"FrameCellNewPhone" bundle:nil] forCellWithReuseIdentifier:@"FRAMECELL"];
    assetsInScenes = [NSMutableArray array];
    if ([[AppHelper getAppHelper] userDefaultsForKey:@"indicationType"])
        [self.framesCollectionView setTag:[[[AppHelper getAppHelper] userDefaultsForKey:@"indicationType"] longValue]];
    else
        [self.framesCollectionView setTag:FRAME_COUNT];
    
    [self.objectList reloadData];

    [self undoRedoButtonState:DEACTIVATE_BOTH];
    if ([[AppHelper getAppHelper]userDefaultsBoolForKey:@"multiSelectOption"]==YES) {
        self.objectList.allowsMultipleSelection=YES;
    }
    else
    {
         self.objectList.allowsMultipleSelection=NO;
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
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersBG) name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersFG) name:@"applicationDidBecomeActive" object:nil];
    
    ScreenWidth = self.renderView.frame.size.width;
    ScreenHeight = self.renderView.frame.size.height;
    if(!editorScene){
        [self initScene];
        [self changeAllButtonBG];
        [self setupEnableDisableControls];
        [self createDisplayLink];
        
        if([[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]){
            [self toolbarPosition:(int)[[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]];
            NSLog(@"Toolbar position : %@ ",[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]);
        } else {
            [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:0] withKey:@"toolbarPosition"];
            [self toolbarPosition:0];
        }
        [self cameraPreviewPosition];
        [self cameraPreviewSize];
        [self frameCountDisplayMode];
    }
    [self.framesCollectionView reloadData];
    if (editorScene && editorScene->actionMan->actions.size() > 0)
        [self.undoBtn setEnabled:YES];
    
    
    if(editorScene && ShaderManager::lightPosition.size() < 5){
        int lightId = 0;
        for(int i = 0 ; i < editorScene->nodes.size(); i++){
                if(editorScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
                    lightId = MAX(editorScene->nodes[i]->assetId,lightId);
        }
        lightCount += lightId-((lightId != 0) ? 900 : 0);
    }
}

 - (void)viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"applicationDidBecomeActive" object:nil];
}

- (void) appEntersBG
{
    isAppInBG = true;
}

- (void) appEntersFG
{
    isAppInBG = false;
    [[AppHelper getAppHelper] moveFilesFromInboxDirectory:cache];
}

- (void)initScene
{
    float screenScale = [[UIScreen mainScreen] scale];
    renderViewMan = [[RenderViewManager alloc] init];
    renderViewMan.delegate = self;
    [renderViewMan setupLayer:_renderView];

    if (isMetalSupported) {
        [[AppDelegate getAppDelegate] initEngine:METAL ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight ScreenScale:screenScale renderView:_renderView];
        smgr = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        editorScene = new SGEditorScene(METAL, smgr, ScreenWidth * screenScale, ScreenHeight * screenScale);
        [renderViewMan setUpPaths:smgr];
        editorScene->screenScale = screenScale;
    }
    else {
        [renderViewMan setupContext];
        [[AppDelegate getAppDelegate] initEngine:OPENGLES2 ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight ScreenScale:screenScale renderView:_renderView];
        smgr = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        editorScene = new SGEditorScene(OPENGLES2, smgr, ScreenWidth*screenScale, ScreenHeight*screenScale);
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
    [self performSelectorOnMainThread:@selector(loadScene) withObject:nil waitUntilDone:YES];
}

- (bool) isMetalSupportedDevice
{
    return isMetalSupported;
}

- (void) reloadFrames
{
    [_framesCollectionView reloadData];
}

- (void) updateXYZValuesHide:(BOOL)hide X:(float)x Y:(float)y Z:(float)z
{
    BOOL status = hide;
    
    if(x == -999.0 && y == -999.0 & z == -999.0)
        status = YES;
    
    [_xLbl setHidden:status];
    [_yLbl setHidden:status];
    [_zLbl setHidden:status];
    [_xValue setHidden:status];
    [_yValue setHidden:status];
    [_zValue setHidden:status];
    
    _xValue.text = [NSString stringWithFormat:@"%.1f", x];
    _yValue.text = [NSString stringWithFormat:@"%.1f", y];
    _zValue.text = [NSString stringWithFormat:@"%.1f", z];
}

- (void)changeAllButtonBG{
    UIColor *selectedColor = [UIColor colorWithRed:123.0f / 255.0f green:123.0f / 255.0f blue:127.0f / 255.0f alpha:1];
    UIColor *unSelectedColor = [UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1];
    if(editorScene->isNodeSelected || editorScene->selectedNodeIds.size()>0)
    {
        [_moveBtn setBackgroundColor:(editorScene->controlType==MOVE) ? selectedColor : unSelectedColor];
        [_rotateBtn setBackgroundColor:(editorScene->controlType==ROTATE) ? selectedColor : unSelectedColor];
        [_scaleBtn setBackgroundColor:(editorScene->controlType==SCALE) ? selectedColor : unSelectedColor];
    }
    else
    {
        [self.moveBtn setBackgroundColor:unSelectedColor];
        [self.rotateBtn setBackgroundColor:unSelectedColor];
        [self.scaleBtn setBackgroundColor:unSelectedColor];
    }
}

-(void)setupEnableDisableControls{
    [self sceneMirrorUIPositionChanger];
    bool sceneMirrorState = !(editorScene && !editorScene->isRigMode && editorScene->hasNodeSelected() && editorScene->getSelectedNode()->joints.size() == HUMAN_JOINTS_SIZE);
    NSLog(@"Scene Mirror State : %d " , sceneMirrorState);
    [_sceneMirrorLable setHidden:sceneMirrorState];
    [_sceneMirrorSwitch setHidden:sceneMirrorState];
    [_sceneMirrorSwitch setOn:editorScene->getMirrorState() animated:YES];
    
    if (editorScene->isRigMode){
        [self.moveBtn setHidden:true];
        [self.optionsBtn setHidden:true];
        [self.rotateBtn setHidden:true];
        [self.scaleBtn setHidden:true];
        [self.undoBtn setHidden:true];
        [self.redoBtn setHidden:true];
        [self.importBtn setHidden:true];
        [self.exportBtn setHidden:true];
        [self.animationBtn setHidden:true];
        [self.optionsBtn setHidden:true];
        [self.objectList setHidden:true];
        [self.playBtn setHidden:true];
        [self.framesCollectionView setHidden:true];
        [self.lastFrameBtn setHidden:true];
        [self.firstFrameBtn setHidden:true];
        [self.addFrameBtn setHidden:true];
        [self.objTableview setHidden:true];
        [self.backButton setHidden:true];
        [self.rigTitle setHidden:false];
        [self.scaleBtnAutorig setHidden:false];
        [self.moveBtnAutorig setHidden:false];
        [self.rotateBtnAutorig setHidden:false];
        [self.myObjectsBtn setHidden:YES];
        return;
    }
    else
    {
        [self.moveBtn setHidden:false];
        [self.optionsBtn setHidden:false];
        [self.rotateBtn setHidden:false];
        [self.scaleBtn setHidden:false];
        [self.undoBtn setHidden:false];
        [self.redoBtn setHidden:false];
        [self.importBtn setHidden:false];
        [self.exportBtn setHidden:false];
        [self.animationBtn setHidden:false];
        [self.optionsBtn setHidden:false];
        [self.objectList setHidden:false];
        [self.playBtn setHidden:false];
        [self.framesCollectionView setHidden:false];
        [self.lastFrameBtn setHidden:false];
        [self.firstFrameBtn setHidden:false];
        [self.addFrameBtn setHidden:false];
        [self.objTableview setHidden:false];
        [self.backButton setHidden:false];
        [self.rigTitle setHidden:true];
        [self.scaleBtnAutorig setHidden:true];
        [self.moveBtnAutorig setHidden:true];
        [self.rotateBtnAutorig setHidden:true];
        [self.autorigMirrorBtnHolder setHidden:true];
        [self.myObjectsBtn setHidden:NO];
    
    }

    
    if(editorScene->isNodeSelected)
    {
        if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT || editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT){
            [self.moveBtn setEnabled:true];
            [self.rotateBtn setEnabled:false];
            [self.optionsBtn setEnabled:true];
            [self.scaleBtn setEnabled:false];
            [self.moveBtn sendActionsForControlEvents:UIControlEventTouchUpInside];
            return;
        }
        else if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_PARTICLES){
            [self.moveBtn setEnabled:true];
            [self.rotateBtn setEnabled:true];
            [self.optionsBtn setEnabled:true];
            [self.scaleBtn setEnabled:false];
            if(editorScene->controlType == SCALE)
                [self.moveBtn sendActionsForControlEvents:UIControlEventTouchUpInside];
            return;
        }
        if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_CAMERA){
            [self.moveBtn setEnabled:true];
            [self.rotateBtn setEnabled:true];
            [self.optionsBtn setEnabled:true];
            [self.scaleBtn setEnabled:false];
            [self.moveBtn sendActionsForControlEvents:UIControlEventTouchUpInside];
            return;
        }
        [self.moveBtn setEnabled:true];
        [self.optionsBtn setEnabled:true];
        [self.rotateBtn setEnabled:true];
        [self.scaleBtn setEnabled:true];
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
    }
    else{
        [self.optionsBtn setEnabled:true];
        [self.moveBtn setEnabled:false];
        [self.rotateBtn setEnabled:false];
        [self.scaleBtn setEnabled:false];
        [self.importBtn setEnabled:true];
        [self.exportBtn setEnabled:true];
        [self.animationBtn setEnabled:true];
        [self.objectList setUserInteractionEnabled:true];
        [self.playBtn setEnabled:true];
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

- (void) sceneMirrorUIPositionChanger
{
    bool positionState = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
    
        CGRect mirrorLableFrame = _sceneMirrorLable.frame;
    mirrorLableFrame.origin.x =(positionState) ? ScreenWidth-_sceneMirrorLable.frame.size.width : _sceneMirrorLable.frame.size.width;
        _sceneMirrorLable.frame = mirrorLableFrame;
        CGRect mirrorSwitchFrame = _sceneMirrorSwitch.frame;
    mirrorSwitchFrame.origin.x =(positionState) ? ScreenWidth-_sceneMirrorLable.frame.size.width - _sceneMirrorSwitch.frame.size.width : _sceneMirrorLable.frame.size.width - _sceneMirrorSwitch.frame.size.width;
        _sceneMirrorSwitch.frame = mirrorSwitchFrame;
}

- (void) removeTempNodeFromScene
{
    if(editorScene && editorScene->loader)
        editorScene->loader->removeTempNodeIfExists();
}

- (void) load3DTex:(int)type AssetId:(int)assetId TextureName:(NSString*)textureName TypedText:(NSString*)typedText FontSize:(int)fontSize BevelValue:(float)bevelRadius TextColor:(Vector4)colors FontPath:(NSString*)fontFileName isTempNode:(bool)isTempNode{
    NSMutableDictionary *textDetails = [[NSMutableDictionary alloc] init];
    [textDetails setObject:[NSNumber numberWithInt:type] forKey:@"type"];
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
    int type = [[fontDetails objectForKey:@"type"]intValue];
    int assetId = [[fontDetails objectForKey:@"AssetId"]intValue];
    std::wstring assetName = [self getwstring:[fontDetails objectForKey:@"typedText"]];
    bool isTempNode = [[fontDetails objectForKey:@"isTempNode"]boolValue];
    int fontSize = [[fontDetails objectForKey:@"fontSize"]intValue];
    float bevalValue = [[fontDetails objectForKey:@"bevelRadius"]floatValue];
    NSString* textureName = [fontDetails objectForKey:@"textureName"];
    
    [renderViewMan loadNodeInScene:type AssetId:assetId AssetName:assetName TextureName:textureName Width:fontSize Height:bevalValue isTempNode:isTempNode More:fontDetails ActionType:assetAddType VertexColor:Vector4(-1)];
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
        double duration = CMTimeGetSeconds(asset.duration);
        int videoFrames = (int)(duration * 24.0);
        int extraFrames = (videoFrames/24 > 0) ? videoFrames - ((videoFrames/24) * 24) : 24 - videoFrames;
        videoFrames = videoFrames - extraFrames;
        printf(" \n duration %f frames %d ", duration, videoFrames);
        editorScene->totalFrames = (editorScene->totalFrames < videoFrames) ? videoFrames : editorScene->totalFrames;
        [self.framesCollectionView reloadData];
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
    [renderViewMan loadNodeInScene:ASSET_ADDITIONAL_LIGHT AssetId:assetId AssetName:[self getwstring:lightName] TextureName:(@"-1") Width:20 Height:50 isTempNode:NO More:nil ActionType:assetAddType VertexColor:Vector4(-1)];
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
    float imgWidth = [[nsDict objectForKey:@"Width"]floatValue];
    float imgHeight = [[nsDict objectForKey:@"Height"]floatValue];
    int isTempNode = [[nsDict objectForKey:@"isTempNode"]intValue];
    [renderViewMan loadNodeInScene:type AssetId:assetId AssetName:saltedFileName TextureName:(@"") Width:imgWidth Height:imgHeight isTempNode:isTempNode More:nil ActionType:assetAddType VertexColor:Vector4(-1)];
    if (!isTempNode) {
        [self undoRedoButtonState:DEACTIVATE_BOTH];
    }
}

- (void) loadNode:(AssetItem*) asset
{
    NSLog(@"Texture Name : %@ ",asset.textureName);
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
    if(isAppInBG)
        return;
    
    if(editorScene) {
        if (editorScene && renderViewMan.checkCtrlSelection) {
            bool isMultiSelectEnabled=[[AppHelper getAppHelper] userDefaultsBoolForKey:@"multiSelectOption"];
            editorScene->selectMan->checkCtrlSelection(renderViewMan.touchMovePosition[0], isMultiSelectEnabled);
            renderViewMan.checkCtrlSelection = false;
        }
        if (editorScene && renderViewMan.checkTapSelection) {
            if(editorScene->isRigMode){
                editorScene->selectMan->checkSelectionForAutoRig(renderViewMan.tapPosition);
            }
            else{
                bool isMultiSelectEnabled=[[AppHelper getAppHelper] userDefaultsBoolForKey:@"multiSelectOption"];
                editorScene->selectMan->checkSelection(renderViewMan.tapPosition,isMultiSelectEnabled);
                 [self changeAllButtonBG];
                 [self setupEnableDisableControls];
            }
            [self reloadFrames];
            renderViewMan.checkTapSelection = false;
            [self highlightObjectList];
            if(editorScene && editorScene->isRigMode && editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)(RIG_MODE_MOVE_JOINTS)){
                [_addJointBtn setEnabled:(editorScene->rigMan->isSkeletonJointSelected)?YES:NO];
            }
            if(editorScene && editorScene->isRigMode)
                [self autoRigMirrorBtnHandler];
            
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

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    return YES;
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    editorScene->setLightingOn();
    
    if(editorScene->isRigMode && editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)(RIG_MODE_PREVIEW)){
        [_addJointBtn setEnabled:(editorScene->rigMan->isSGRJointSelected)?NO:YES];
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
    cameraResoultionType = editorScene->cameraResolutionType;
    editorScene->updater->setDataForFrame(editorScene->currentFrame);
}

- (NSString*) getSGBPath
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* srcFilePath = [NSString stringWithFormat:@"%@/Projects", documentsDirectory];
    NSString* filePath = [NSString stringWithFormat:@"%@/%@.sgb", srcFilePath, currentScene.sceneFile];
    return filePath;
}

- (bool)loadFromFile
{
    NSString* filePath = [self getSGBPath];
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
    
    [self.framesCollectionView reloadData];
    [self reloadSceneObjects];
    delete SGBFilePath;
    return true;
}

- (void) reloadSceneObjects{
    NSUInteger assetsCount = editorScene->nodes.size();
    [self updateAssetListInScenes];
    [self.objectList reloadData];
}

#pragma mark - Frames Collection View Deligates

- (NSInteger)collectionView:(UICollectionView*)collectionView numberOfItemsInSection:(NSInteger)section
{
    return (editorScene) ? editorScene->totalFrames : 24;
}

- (FrameCellNew*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    FrameCellNew* cell = [self.framesCollectionView dequeueReusableCellWithReuseIdentifier:@"FRAMECELL" forIndexPath:indexPath];
        if (editorScene && editorScene->isKeySetForFrame.find((int)indexPath.row) != editorScene->isKeySetForFrame.end() && (indexPath.row) == editorScene->currentFrame) {
            cell.backgroundColor = [UIColor blackColor];
            cell.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                     green:156.0f / 255.0f
                                                      blue:156.0f / 255.0f
                                                     alpha:1.0f]
            .CGColor;
            cell.layer.borderWidth = 2.0f;
        }
        else if (editorScene && editorScene->isKeySetForFrame.find((int)indexPath.row) != editorScene->isKeySetForFrame.end()) {
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
        if (editorScene && (indexPath.row) == editorScene->currentFrame) {
            cell.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                     green:156.0f / 255.0f
                                                      blue:156.0f / 255.0f
                                                     alpha:1.0f]
            .CGColor;
            cell.layer.borderWidth = 2.0f;
        }
    
    
    if (_framesCollectionView.tag==FRAME_DURATION){
        cell.framesLabel.text = [NSString stringWithFormat:@"%.2fs", ((float)indexPath.row + 1) / 24];
    }
    else {
        cell.framesLabel.text = [NSString stringWithFormat:@"%d", (int)indexPath.row + 1];
    }
    
    cell.framesLabel.adjustsFontSizeToFitWidth = YES;
    cell.framesLabel.minimumFontSize = 3.0;
    return cell;
}

- (void)collectionView:(UICollectionView*)collectionView didSelectItemAtIndexPath:(NSIndexPath*)indexPath
{
    
    [self NormalHighLight];
    editorScene->previousFrame = editorScene->currentFrame;
    editorScene->currentFrame = (int)indexPath.row;
    [self HighlightFrame];
    [self.framesCollectionView reloadData];
    editorScene->actionMan->switchFrame((float)editorScene->currentFrame);
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
    if(editorScene && editorScene->nodes.size() > indexPath.row){
        if(editorScene && editorScene->nodes[indexPath.row]->getType() == NODE_CAMERA)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Camera_Pad"];
        else if(editorScene && editorScene->nodes[indexPath.row]->getType() == NODE_LIGHT)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Light_Pad"];
        else if(editorScene && editorScene->nodes[indexPath.row]->getType() == NODE_ADDITIONAL_LIGHT)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Light_Pad"];
        else if(editorScene && editorScene->nodes[indexPath.row]->getType() == NODE_TEXT_SKIN)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Text_Pad"];
        else if(editorScene && editorScene->nodes[indexPath.row]->getType() == NODE_TEXT)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Text_Pad"];
        else if(editorScene && editorScene->nodes[indexPath.row]->getType() == NODE_IMAGE)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Image_Pad"];
        else if(editorScene && editorScene->nodes[indexPath.row]->getType() == NODE_PARTICLES)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Particles"];
        else if(editorScene && editorScene->nodes[indexPath.row]->getType() == NODE_VIDEO)
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Camera_Pad"];
        else
            cell.imageView.image = [UIImage imageNamed:@"My-objects-Models_Pad.png"];
    }
    return cell;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        editorScene->selectMan->unselectObjects();
        [renderViewMan removeNodeFromScene:(int)indexPath.row IsUndoOrRedo:NO];
        [self reloadFrames];
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

- (void) removeAllSubViewAndMemory
{
    if(animationsliderVC != nil){
        [animationsliderVC cancelBtnFunction:nil];
        [animationsliderVC removeFromParentViewController];
    }
    if(assetSelectionSlider != nil){
        [assetSelectionSlider cancelButtonAction:nil];
        [assetSelectionSlider removeFromParentViewController];
    }
    if(textSelectionSlider != nil){
        [textSelectionSlider cancelBtnAction:nil];
        [textSelectionSlider removeFromParentViewController];
    }
    if(objVc != nil){
        [objVc cancelBtnAction:nil];
        [objVc removeFromParentViewController];
    }
    if(importImageViewVC != nil){
        [importImageViewVC cancelBtnAction:nil];
        [importImageViewVC removeFromParentViewController];
    }
}

- (IBAction)backToScenes:(id)sender {
   
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
    [self performSelectorOnMainThread:@selector(removeAllSubViewAndMemory) withObject:nil waitUntilDone:YES];
    if(editorScene->isPlaying){
        [self stopPlaying];
        return;
    }
    if(editorScene)
        editorScene->isPreviewMode = false;
    [self performSelectorOnMainThread:@selector(saveAnimationData) withObject:nil waitUntilDone:YES];
    [self performSelectorOnMainThread:@selector(saveThumbnail) withObject:nil waitUntilDone:YES];
    [self loadSceneSelectionView];
}

- (IBAction)playButtonAction:(id)sender {
    [self playAnimation];
}


- (IBAction)moveLastAction:(id)sender {
    if((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode) != RIG_MODE_PREVIEW){
        if(editorScene->rigMan->sceneMode == RIG_MODE_OBJVIEW){
            
            if(editorScene->rigMan->rigNodeType == NODE_RIG) {
                editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode+1));
            } else {
                UIAlertView *closeAlert = [[UIAlertView alloc]initWithTitle:@"Select Bone Structure" message:@"You can either start with a complete Human Bone structure or with a single bone?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Single Bone", @"Human Bone Structure", nil];
                [closeAlert setTag:CHOOSE_RIGGING_METHOD];
                [closeAlert show];
            }
            
        } else if(editorScene->rigMan->sceneMode + 1 == RIG_MODE_PREVIEW){
            //[[UIApplication sharedApplication] beginIgnoringInteractionEvents];
            [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
            NSString *tempDir = NSTemporaryDirectory();
            NSString *sgrFilePath = [NSString stringWithFormat:@"%@r-%@.sgr", tempDir, @"autorig"];
            string path = (char*)[sgrFilePath cStringUsingEncoding:NSUTF8StringEncoding];
            [self exportSgr:sgrFilePath];
            [_rigAddToSceneBtn setHidden:NO];
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode+1));
            [_addJointBtn setEnabled:NO];
        }
        else
        {
            [_rigAddToSceneBtn setHidden:YES];
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode+1));
        }
    }
    
    [self autoRigMirrorBtnHandler];
    [self performSelectorInBackground:@selector(hideLoadingActivity) withObject:nil];
}

- (void) showLoadingActivity
{
    [_center_progress setHidden:NO];
    [_center_progress startAnimating];
}

- (void) hideLoadingActivity
{
    [_center_progress stopAnimating];
    [_center_progress setHidden:YES];
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
    [_addJointBtn setEnabled:NO];
    [self autoRigMirrorBtnHandler];
}

- (IBAction)rigCancelAction:(id)sender
{
    editorScene->rigMan->deallocAutoRig(NO);
    editorScene->enterOrExitAutoRigMode(false);
    [self setupEnableDisableControls];
    [self autoRigViewButtonHandler:YES];
    selectedNodeId = -1;
    [self autoRigMirrorBtnHandler];
}

- (IBAction)rigAddToSceneAction:(id)sender
{
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
    Vector3 vertexColor = Vector3(-1.0);
    if(editorScene->rigMan->nodeToRig->props.perVertexColor){
        vertexColor = editorScene->rigMan->nodeToRig->props.vertexColor;
    }
    
    [self addrigFileToCacheDirAndDatabase:[NSString stringWithFormat:@"%s%@",editorScene->nodes[selectedNodeId]->textureName.c_str(),@".png"] VertexColor:vertexColor];
    if(editorScene->rigMan->deallocAutoRig(YES)){
        editorScene->enterOrExitAutoRigMode(false);
        [self setupEnableDisableControls];
    }
    [self autoRigViewButtonHandler:YES];
    [self updateAssetListInScenes];
    selectedNodeId = -1;
    [self autoRigMirrorBtnHandler];
    [self performSelectorInBackground:@selector(hideLoadingActivity) withObject:nil];
}

- (IBAction)addJoinAction:(id)sender {
    if(editorScene->rigMan->isSkeletonJointSelected)
        editorScene->rigMan->addNewJoint();
}

- (IBAction)publishBtnAction:(id)sender {
    [animationsliderVC publishBtnaction:sender];
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
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT){
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
    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"])
    {
        if ([Utility IsPadDevice])
        {
            _loggedInVc = [[LoggedInViewController alloc] initWithNibName:@"LoggedInViewController" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_loggedInVc];
            self.popoverController.popoverContentSize = CGSizeMake(305, 495);
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
            self.popoverController.popoverContentSize = CGSizeMake(228.00, 208.0);
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
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT){
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
    BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"importBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popoverController.popoverContentSize = ([self iPhone6Plus]) ? CGSizeMake(205.0, 320) : CGSizeMake(205.0, 320);
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
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->totalFrames-1 inSection:0];
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
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT){
        _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"myObjectsBtn"];
        [_popUpVc.view setClipsToBounds:YES];
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        self.popoverController.popoverContentSize = CGSizeMake(205.0, ((editorScene->nodes.size() * 39) < 360)  ? editorScene->nodes.size()*39 : 360);
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
        self.popoverController.popoverContentSize = CGSizeMake(205.0, ((editorScene->nodes.size() * 39) < 360)  ? editorScene->nodes.size()*39 : 360);
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
    
    NSLog(@"\nPopup Size %f %f " , _popUpVc.view.frame.size.width,_popUpVc.view.frame.size.height);
}

- (IBAction)autorigMirrorSwitchAction:(id)sender {
    if(editorScene && editorScene->isRigMode)
        editorScene->rigMan->switchMirrorState();
}

- (IBAction)sceneMirrorAction:(id)sender {
    editorScene->switchMirrorState();
}


- (IBAction)optionsBtnAction:(id)sender
{
    
    if(!editorScene->isNodeSelected || (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_UNDEFINED))
    {
        /*
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
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
         */
        return;
    }
    if(editorScene->isNodeSelected && (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT || editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT))
    {
        Quaternion lightProps;
        if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT)
            lightProps = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(editorScene->currentFrame,editorScene->nodes[editorScene->selectedNodeId]->rotationKeys,true);
        if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT){
            Vector3 mainLight = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(editorScene->currentFrame, editorScene->nodes[editorScene->selectedNodeId]->scaleKeys);
            lightProps = Quaternion(mainLight.x,mainLight.y,mainLight.z,ShaderManager::shadowDensity);
        }
        lightProps.w = ShaderManager::shadowDensity;
        float distance = ((editorScene->getSelectedNode()->props.nodeSpecificFloat/300.0)-0.001);
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
        _lightProp = [[LightProperties alloc] initWithNibName:([Utility IsPadDevice]) ? @"LightProperties"  : @"LightPropertiesPhone" bundle:nil LightColor:lightProps LightType:editorScene->getSelectedNode()->getType() Distance:distance];
        _lightProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_lightProp];
        self.popoverController.popoverContentSize = CGSizeMake(270, ([Utility IsPadDevice]) ? 335 : 265);
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
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
        _camProp = [[CameraSettings alloc] initWithNibName:@"CameraSettings" bundle:nil FOVvalue:fovValue ResolutionType:resolutionType];
        _camProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_camProp];
        self.popoverController.popoverContentSize = CGSizeMake(300 , 230);
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
        bool isVideoOrImageOrParticle = false;
        NODE_TYPE nodeType = NODE_UNDEFINED;
        if(editorScene && editorScene->hasNodeSelected()){
            nodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
            if(nodeType == NODE_IMAGE || nodeType == NODE_VIDEO || nodeType == NODE_PARTICLES)
                isVideoOrImageOrParticle = true;
        }
        float refractionValue = editorScene->nodes[editorScene->selectedNodeId]->props.refraction;
        float reflectionValue = editorScene->nodes[editorScene->selectedNodeId]->props.reflection;
        bool isLightningValue = editorScene->nodes[editorScene->selectedNodeId]->props.isLighting;
        bool isVisibleValue = editorScene->nodes[editorScene->selectedNodeId]->props.isVisible;
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
        int state = (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG && editorScene->nodes[editorScene->selectedNodeId]->joints.size() == HUMAN_JOINTS_SIZE) ? editorScene->getMirrorState() : MIRROR_DISABLE;
        _meshProp = [[MeshProperties alloc] initWithNibName:(isVideoOrImageOrParticle) ? @"LightAndVideoProperties" : @"MeshProperties" bundle:nil RefractionValue:refractionValue ReflectionValue:reflectionValue LightningValue:isLightningValue Visibility:isVisibleValue MirrorState:state LightState:(nodeType == NODE_PARTICLES) ? NO : YES];
        _meshProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_meshProp];
        self.popoverController.popoverContentSize = (isVideoOrImageOrParticle) ? CGSizeMake(183 , 115) : CGSizeMake(407 , 203);
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
    if(!editorScene->isRigMode || (editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_MOVE_JOINTS))){
        bool status = false;
        if((editorScene->selectedNodeIds.size() > 0) && (editorScene->allObjectsScalable())){
            status = true;
        }
        else if(!(editorScene->selectedNodeIds.size() > 0) && (editorScene->isRigMode ||(editorScene->hasNodeSelected() && (editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_CAMERA && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_LIGHT && editorScene->nodes[editorScene->selectedNodeId]->getType() != NODE_ADDITIONAL_LIGHT)))){
            status = true;
        }
        else
            status = false;
        
        if(status && editorScene->selectedNodeIds.size() > 0){
            editorScene->controlType = SCALE;
            editorScene->updater->updateControlsOrientaion();
            editorScene->renHelper->setControlsVisibility(false);
        }
        else if(status){
            editorScene->controlType = SCALE;
            editorScene->updater->updateControlsOrientaion();
            editorScene->renHelper->setControlsVisibility(false);
            Vector3 currentScale = (editorScene->isRigMode) ? editorScene->rigMan->getSelectedNodeScale() : editorScene->getSelectedNodeScale();
            _scaleProps = [[ScaleViewController alloc] initWithNibName:@"ScaleViewController" bundle:nil updateXValue:currentScale.x updateYValue:currentScale.y updateZValue:currentScale.z];
            _scaleProps.delegate = self;
            BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_scaleProps];
            self.popoverController.popoverContentSize = CGSizeMake(270, 177);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [_popUpVc.view setClipsToBounds:YES];
            CGRect rect = ((UIButton*)sender).frame;
            rect = [self.view convertRect:rect fromView:((UIButton*)sender).superview];
            [self.popoverController presentPopoverFromRect:rect
                                                    inView:self.view
                                  permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight
                                                  animated:NO];
        }
        [self changeAllButtonBG];
    }
    else if(editorScene->isRigMode && (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_EDIT_ENVELOPES) && editorScene->rigMan->isSkeletonJointSelected){
        float scale = editorScene->rigMan->getSelectedJointScale();
        scaleAutoRig=[[ScaleForAutoRigViewController alloc] initWithNibName:@"ScaleForAutoRigViewController" bundle:nil updateScale:scale];
        scaleAutoRig.delegate = self;
        self.popoverController = [[WEPopoverController alloc]initWithContentViewController:scaleAutoRig];
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        self.popoverController.popoverContentSize = CGSizeMake(390, 100.0);
        self.popoverController.delegate =self;
        CGRect rect = ((UIButton*)sender).frame;
        rect = [self.view convertRect:rect fromView:((UIButton*)sender).superview];
        [self.popoverController presentPopoverFromRect:rect
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionRight
                                              animated:YES];
    }
}

- (void) changeSkeletonScale:(Vector3)scale
{
    editorScene->rigMan->changeNodeScale(scale);
}

- (IBAction)undoBtnAction:(id)sender
{
    int returnValue2 = NOT_SELECTED;
    
    ACTION_TYPE actionType = (ACTION_TYPE)editorScene->undo(returnValue2);
    [self undoRedoButtonState:actionType];
        switch (actionType) {
        case DO_NOTHING: {
            break;
        }
        case DELETE_ASSET: {
            int nodeIndex = returnValue2;
            if (nodeIndex < assetsInScenes.count) {
                [renderViewMan removeNodeFromScene:nodeIndex IsUndoOrRedo:YES];
                [self reloadFrames];
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
            if(assetId >= 900 && assetId <= 1000) {
                int numberOfLight = assetId - 900;
                [self addLightToScene:[NSString stringWithFormat:@"Light%d",numberOfLight] assetId:assetId];
            }
            else if(assetId >= 10000 && assetId <= 20000){
                AssetItem* assetObject = [cache GetAsset:assetId];
                assetObject.isTempAsset = NO;
                assetObject.type = ASSET_PARTICLES;
                [self loadNodeInScene:assetObject ActionType:assetAddType];
            }
            else {
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
            [self reloadFrames];
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
    [self updateAssetListInScenes];
    [self.framesCollectionView reloadData];
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

}

- (void) stopPlaying{
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
    [self setupEnableDisableControls];
    editorScene->actionMan->switchFrame(editorScene->currentFrame);
}

-(void) myAnimation:(BOOL)showorHide
{
   [self.publishBtn setHidden:showorHide];
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
                if(_popUpVc)
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
                if(_popUpVc != nil)
                    [_popUpVc updateSelection:path ScrollPosition:editorScene->selectedNodeId];
            }
        }
        if(editorScene->selectedNodeId==-1)
        {
            if(isSelected)
            {
                if([Utility IsPadDevice]){
                    [self.objectList deselectRowAtIndexPath:path animated:YES];
                }
                else{
                    if(_popUpVc)
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
                if(_popUpVc)
                    [_popUpVc updateSelection:path ScrollPosition:editorScene->selectedNodeIds[i]];
            }
        }
        
    }
}

- (void)popoverControllerDidDismissPopover:(UIPopoverController *)popoverController
{
    _popUpVc = nil;
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

- (NSMutableArray*) getFileNamesToAttach
{
    NSMutableArray *fileNamesToZip = [[NSMutableArray alloc] init];
    vector<string> textureFileNames = editorScene->getUserFileNames();
    
    for(int i = 0; i < textureFileNames.size(); i++) {
        NSLog(@"\n User File Name: %s ", textureFileNames[i].c_str());
        [fileNamesToZip addObject:[NSString stringWithCString:textureFileNames[i].c_str() encoding:NSUTF8StringEncoding]];
    }
    
    return fileNamesToZip;
}

- (BOOL) canUploadToCloud
{
    if(editorScene) {
        for(int i = 0; i < editorScene->nodes.size(); i++) {
            if(editorScene->nodes[i]->getType() == NODE_VIDEO || editorScene->nodes[i]->getType() == NODE_PARTICLES) {
                return false;
                break;
            }
        }
        return true;
    }
    return false;
}


- (CGPoint) getCameraResolution
{
    CGPoint resolution = CGPointMake(0.0, 0.0);
    if(!editorScene)
        return resolution;
    
    float resWidth = RESOLUTION[editorScene->cameraResolutionType][0];
    float resHeight = RESOLUTION[editorScene->cameraResolutionType][1];
    
    return CGPointMake(resWidth, resHeight);
}


- (void) changeLightProps:(Quaternion)lightProps Distance:(float)distance isStoredProperty:(BOOL)isStored{
    
    if(!isStored)
        editorScene->actionMan->changeLightProperty(lightProps.x, lightProps.y, lightProps.z, lightProps.w,distance,true);
    else if(isStored)
        editorScene->actionMan->storeLightPropertyChangeAction(lightProps.x, lightProps.y, lightProps.z, lightProps.w,distance);
}

- (void)applyAnimationToSelectedNode:(NSString*)filePath SelectedNodeId:(int)originalId SelectedFrame:(int)selectedFrame
{
    editorScene->animMan->copyKeysOfNode(originalId, (int)editorScene->nodes.size()-1);
    editorScene->nodes[originalId]->node->setVisible(false);
    editorScene->currentFrame = selectedFrame;
    editorScene->actionMan->switchFrame(selectedFrame);
    std::string *filePathStr = new std::string([filePath UTF8String]);
    editorScene->animMan->applyAnimations(*filePathStr, editorScene->selectedNodeId);
    [_framesCollectionView reloadData];
    [self playAnimation];
}

- (void) removeTempAnimation{
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
    if(!editorScene->isRigMode && (editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size()))
        return;
    if(editorScene->isRigMode && editorScene->rigMan->isNodeSelected)
        [self changeSkeletonScale:Vector3(XValue,YValue,ZValue)];
    else{
        editorScene->actionMan->changeObjectScale(Vector3(XValue, YValue, ZValue), false);
        [_framesCollectionView reloadData];
    }
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
    BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
    
    if(showView){
        subViewToAdd.frame = CGRectMake(0, 0, _leftView.frame.size.width, _leftView.frame.size.height);
        [self.leftView addSubview:subViewToAdd];
    }
    
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
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT){
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
    [videoDetail setObject:[NSString stringWithFormat:@"%@.MOV",fileName] forKey:@"AssetName"];
    [videoDetail setObject:[NSNumber numberWithFloat:resolution.x] forKey:@"Width"];
    [videoDetail setObject:[NSNumber numberWithFloat:resolution.y] forKey:@"Height"];
    [videoDetail setObject:[NSNumber numberWithBool:isTemp] forKey:@"isTempNode"];
    assetAddType = IMPORT_ASSET_ACTION;
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

- (void) renderFrame:(int)frame withType:(int)shaderType isImage:(bool)isImage andRemoveWatermark:(bool)removeWatermark
{
    editorScene->renHelper->isExportingImages = true;
    editorScene->updater->setDataForFrame(frame);
    NSString* tempDir = NSTemporaryDirectory();
    NSString* imageFilePath = [NSString stringWithFormat:@"%@/r-%d.png", tempDir, frame];
    editorScene->renHelper->renderAndSaveImage((char*)[imageFilePath cStringUsingEncoding:NSUTF8StringEncoding], shaderType, false, removeWatermark,(isImage) ? -1 : frame, renderBgColor);
}

- (void) freezeEditorRender:(BOOL) freeze
{
    editorScene->freezeRendering = freeze;
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
                                           if(editorScene->selectedNodeIds.size() > 0){
                                               editorScene->loader->removeSelectedObjects();
                                               [self undoRedoButtonState:DEACTIVATE_BOTH];
                                           }
                                           else{
                                               [self deleteObjectOrAnimation];
                                               [self undoRedoButtonState:DEACTIVATE_BOTH];
                                               [self reloadFrames];
                                           }
                                           [self updateAssetListInScenes];
                                           
                                       }];
        [view addAction:deleteButton];
    UIPopoverPresentationController *popover = view.popoverPresentationController;
    popover.sourceRect = arect;
    popover.sourceView=self.renderView;
    popover.permittedArrowDirections = UIPopoverArrowDirectionAny;
    [self presentViewController:view animated:YES completion:nil];
}

-(void)showOptions :(CGRect)longPressposition{
    /*
    if(!editorScene->isNodeSelected || (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_UNDEFINED))
    {
//        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1);
        _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"optionsBtn"];
        [_popUpVc.view setClipsToBounds:YES];
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        self.popoverController.popoverContentSize = CGSizeMake(205.0, 42);
        self.popoverController.delegate =self;
        self.popUpVc.delegate=self;
        [self.popoverController presentPopoverFromRect:longPressposition
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionAny
                                              animated:YES];
        return;
    }
    */
    
    if(!editorScene || editorScene->selectedNodeId == NOT_SELECTED)
        return;
    
    if(editorScene->isNodeSelected && (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT || editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT))
    {
        Quaternion lightProps;
        if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT)
            lightProps = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(editorScene->currentFrame,editorScene->nodes[editorScene->selectedNodeId]->rotationKeys,true);
        if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT){
            Vector3 mainLight = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(editorScene->currentFrame, editorScene->nodes[editorScene->selectedNodeId]->scaleKeys);
            lightProps = Quaternion(mainLight.x,mainLight.y,mainLight.z,ShaderManager::shadowDensity);
        }
        lightProps.w = ShaderManager::shadowDensity;
//        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==1);
        float distance = ((editorScene->getSelectedNode()->props.nodeSpecificFloat/300.0)-0.001);

        _lightProp = [[LightProperties alloc] initWithNibName:([Utility IsPadDevice]) ? @"LightProperties"  : @"LightPropertiesPhone" bundle:nil LightColor:lightProps LightType:editorScene->getSelectedNode()->getType() Distance:distance];
        _lightProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_lightProp];
        self.popoverController.popoverContentSize = CGSizeMake(270,  ([Utility IsPadDevice]) ? 335 : 265);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [_lightProp.view setClipsToBounds:YES];

        [self.popoverController presentPopoverFromRect:longPressposition
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionAny
                                              animated:NO];
    }
    else if(editorScene->isNodeSelected && (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_CAMERA))
    {
        float fovValue = editorScene->cameraFOV;
        NSInteger resolutionType = editorScene->cameraResolutionType;
        _camProp = [[CameraSettings alloc] initWithNibName:@"CameraSettings" bundle:nil FOVvalue:fovValue ResolutionType:resolutionType];
        _camProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_camProp];
        self.popoverController.popoverContentSize = CGSizeMake(300 , 230);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [_camProp.view setClipsToBounds:YES];
        [self.popoverController presentPopoverFromRect:longPressposition
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionAny
                                              animated:NO];
        
    }
    else if(editorScene && editorScene->isNodeSelected && (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_PARTICLES))
    {
        CGRect rect = _optionsBtn.frame;
        rect = [self.view convertRect:rect fromView:_optionsBtn.superview];
        [self presentPopOver:rect];
    }
    else if(editorScene && editorScene->hasNodeSelected()){
        bool isVideoOrImageOrParticle = false;
        NODE_TYPE nodeType = NODE_UNDEFINED;
        if(editorScene && editorScene->hasNodeSelected()){
            nodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
            if(nodeType == NODE_IMAGE || nodeType == NODE_VIDEO)
                isVideoOrImageOrParticle = true;
        }
        float refractionValue = editorScene->nodes[editorScene->selectedNodeId]->props.refraction;
        float reflectionValue = editorScene->nodes[editorScene->selectedNodeId]->props.reflection;
        bool isLightningValue = editorScene->nodes[editorScene->selectedNodeId]->props.isLighting;
        bool isVisibleValue = editorScene->nodes[editorScene->selectedNodeId]->props.isVisible;
        int state = (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG && editorScene->nodes[editorScene->selectedNodeId]->joints.size() == HUMAN_JOINTS_SIZE) ? editorScene->getMirrorState() : MIRROR_DISABLE;
        _meshProp = [[MeshProperties alloc] initWithNibName:(isVideoOrImageOrParticle) ? @"LightAndVideoProperties" : @"MeshProperties" bundle:nil RefractionValue:refractionValue ReflectionValue:reflectionValue LightningValue:isLightningValue Visibility:isVisibleValue MirrorState:state LightState:(nodeType == NODE_PARTICLES) ? NO : YES];
        _meshProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_meshProp];
        self.popoverController.popoverContentSize =(isVideoOrImageOrParticle) ? CGSizeMake(183 , 115) : CGSizeMake(407 , 203);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [_meshProp.view setClipsToBounds:YES];
        [self.popoverController presentPopoverFromRect:longPressposition
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionAny
                                              animated:NO];
    }
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
        assetItem.textureName = [NSString stringWithCString:editorScene->nodes[selectedNode]->textureName.c_str()
                                                   encoding:[NSString defaultCStringEncoding]];
        assetItem.isTempAsset = YES;
        [self performSelectorOnMainThread:@selector(loadNode:) withObject:assetItem waitUntilDone:YES];
        editorScene->animMan->copyKeysOfNode(selectedNode, (int)editorScene->nodes.size()-1);
        editorScene->animMan->copyPropsOfNode(selectedNode, (int)editorScene->nodes.size()-1);
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
    
    if((selectedNodeType == NODE_RIG || selectedNodeType ==  NODE_SGM || selectedNodeType ==  NODE_OBJ || selectedNodeType == NODE_PARTICLES) && selectedAssetId != NOT_EXISTS)
    {
        assetAddType = IMPORT_ASSET_ACTION;
        AssetItem *assetItem = [cache GetAsset:selectedAssetId];
        assetItem.textureName = [NSString stringWithCString:editorScene->nodes[selectedNode]->textureName.c_str()
                                                   encoding:[NSString defaultCStringEncoding]];
        [self performSelectorOnMainThread:@selector(loadNode:) withObject:assetItem waitUntilDone:YES];
        editorScene->animMan->copyPropsOfNode(selectedNode, (int)editorScene->nodes.size()-1);
    }
    else if((selectedNodeType == NODE_IMAGE || selectedNodeType == NODE_VIDEO) && selectedAssetId != NOT_EXISTS){
        NSString* fileName = [self stringWithwstring:editorScene->nodes[editorScene->selectedNodeId]->name];
        float imgW = editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.x;
        float imgH = editorScene->nodes[editorScene->selectedNodeId]->props.vertexColor.y;
        NSMutableDictionary *imageDetails = [[NSMutableDictionary alloc] init];
        [imageDetails setObject:[NSNumber numberWithInt:(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_IMAGE) ? ASSET_IMAGE : ASSET_VIDEO] forKey:@"type"];
        [imageDetails setObject:[NSNumber numberWithInt:0] forKey:@"AssetId"];
        [imageDetails setObject:fileName forKey:@"AssetName"];
        [imageDetails setObject:[NSNumber numberWithFloat:imgW] forKey:@"Width"];
        [imageDetails setObject:[NSNumber numberWithFloat:imgH] forKey:@"Height"];
        [imageDetails setObject:[NSNumber numberWithBool:NO] forKey:@"isTempNode"];
        assetAddType = IMPORT_ASSET_ACTION;
        [self performSelectorOnMainThread:@selector(loadNodeForImage:) withObject:imageDetails waitUntilDone:YES];
        editorScene->animMan->copyPropsOfNode(selectedNode, (int)editorScene->nodes.size()-1);
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
        editorScene->animMan->copyPropsOfNode(selectedNode, (int)editorScene->nodes.size()-1);
    }
}

- (void) changeTextureForAsset{
    selectedNodeId = editorScene->selectedNodeId;
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
        if(name == nil)
            name = @"";
        
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
        else if(editorScene->nodes[i]->getType() == NODE_TEXT){
            [assetsInScenes addObject:[NSString stringWithFormat:@"Text : %@",name]];
        }
        else if(editorScene->nodes[i]->getType() == NODE_VIDEO){
            [assetsInScenes addObject:[NSString stringWithFormat:@"Video : %@",name]];
        }
        else{
            [assetsInScenes addObject:name];
        }
    }    
    [self.objectList reloadData];
}



- (void)objectSelectionCompleted:(int)assetIndex
{
    bool isMultiSelectenabled= [[AppHelper getAppHelper] userDefaultsBoolForKey:@"multiSelectOption"];
    editorScene->selectMan->selectObject(assetIndex,isMultiSelectenabled);
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
                animationsliderVC =[[AnimationSelectionSlider alloc] initWithNibName:@"AnimationSelectionSlider" bundle:Nil withType:animType  EditorScene:editorScene FirstTime:YES ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
                animationsliderVC.delegate = self;
                [self showOrHideLeftView:YES withView:animationsliderVC.view];
            }
            else{
                animationsliderVC =[[AnimationSelectionSlider alloc] initWithNibName:@"AnimationSelectionSliderPhone" bundle:Nil withType:animType EditorScene:editorScene FirstTime:YES ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
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
        case IMPORT_PARTICLE:
        case IMPORT_MODELS:
            if([Utility IsPadDevice]){
                [self.popoverController dismissPopoverAnimated:YES];
                assetSelectionSlider =[[AssetSelectionSidePanel alloc] initWithNibName:@"AssetSelectionSidePanel" bundle:Nil Type:(indexValue == IMPORT_MODELS) ? IMPORT_MODELS : ASSET_PARTICLES ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
                assetSelectionSlider.assetSelectionDelegate = self;
                [self showOrHideLeftView:YES withView:assetSelectionSlider.view];
            }
            else{
                [self.popoverController dismissPopoverAnimated:YES];
                assetSelectionSlider =[[AssetSelectionSidePanel alloc] initWithNibName:@"AssetSelectionSidePanelPhone" bundle:Nil Type:(indexValue == IMPORT_MODELS) ? IMPORT_MODELS : ASSET_PARTICLES ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
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
            [self.popoverController dismissPopoverAnimated:YES];
            NODE_TYPE selectedNodeType = NODE_UNDEFINED;
            if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
                selectedNodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
                
                if(selectedNodeType != NODE_SGM && selectedNodeType != NODE_TEXT && selectedNodeType != NODE_RIG) {
                    UIAlertView* error = [[UIAlertView alloc] initWithTitle:@"Alert" message:@"Bones cannot be added to this model." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
                    [error show];
                } else if(selectedNodeType == NODE_RIG && !editorScene->canEditRigBones(editorScene->nodes[editorScene->selectedNodeId])) {
                    UIAlertView* warning = [[UIAlertView alloc] initWithTitle:@"Alert" message:@"This model was rigged using the previous version. Some of the envelope information might be missing. Do you want to continue?" delegate:self cancelButtonTitle:@"NO" otherButtonTitles:@"YES",nil];
                    [warning setTag:SGR_WARNING];
                    [warning show];
                    
                } else {
                    [self beginRigging];
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

- (void) beginRigging
{
    [self.addJointBtn setHidden:NO];
    [self.addJointBtn setEnabled:NO];
    [self.moveLast setHidden:NO];
    [self.moveFirst setHidden:NO];
    [self.rigScreenLabel setHidden:NO];
    selectedNodeId = editorScene->riggingNodeId = editorScene->selectedNodeId;
    editorScene->enterOrExitAutoRigMode(true);
    editorScene->rigMan->sgmForRig(editorScene->nodes[selectedNodeId]);
    editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(RIG_MODE_OBJVIEW));
    editorScene->rigMan->boneLimitsCallBack = &boneLimitsCallBack;
    [self setupEnableDisableControls];
    [_rigCancelBtn setHidden:NO];
    [self autoRigMirrorBtnHandler];
    [self moveLastAction:nil];
}

- (void) exportBtnDelegateAction:(int)indexValue {
    if(editorScene){
        editorScene->selectMan->unselectObjects();
        [self setupEnableDisableControls];
    }
    if(indexValue==EXPORT_IMAGE) {
        renderBgColor = Vector4(0.1,0.1,0.1,1.0);
        if([Utility IsPadDevice]) {
            [self.popoverController dismissPopoverAnimated:YES];
            RenderingViewController* renderingView;
            renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewController" bundle:nil StartFrame:editorScene->currentFrame EndFrame:editorScene->totalFrames renderOutput:RENDER_IMAGE caMresolution:editorScene->cameraResolutionType ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
            renderingView.delegate = self;
            renderingView.projectName = currentScene.name;
            renderingView.sgbPath = [self getSGBPath];
            renderingView.modalPresentationStyle = UIModalPresentationFormSheet;
            [self showOrHideRightView:YES];
            dispatch_async(dispatch_get_main_queue(), ^ {
                [self presentViewControllerInCurrentView:renderingView];
            });
            renderingView.view.superview.backgroundColor = [UIColor clearColor];
        } else {
            [self.popoverController dismissPopoverAnimated:YES];
            RenderingViewController* renderingView;
            
            renderingView = [[RenderingViewController alloc]initWithNibName:([self iPhone6Plus]) ? @"RenderingViewControllerPhone@2x" :
                             @"RenderingViewControllerPhone" bundle:nil StartFrame:editorScene->currentFrame EndFrame:editorScene->totalFrames renderOutput:RENDER_IMAGE caMresolution:editorScene->cameraResolutionType ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
            renderingView.delegate = self;
            BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
            renderingView.projectName = currentScene.name;
            renderingView.sgbPath = [self getSGBPath];
            if([Utility IsPadDevice] || ScreenHeight < 400)
                renderingView.modalPresentationStyle = UIModalPresentationFormSheet;
            CATransition* transition1 = [CATransition animation];
            transition1.duration = 0.5;
            transition1.type = kCATransitionPush;
            transition1.subtype = (status) ? kCATransitionFromRight : kCATransitionFromLeft;;
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
            renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewController" bundle:nil StartFrame:0 EndFrame:editorScene->totalFrames renderOutput:RENDER_VIDEO caMresolution:editorScene->cameraResolutionType ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
        else
            renderingView = [[RenderingViewController alloc]initWithNibName:(ScreenHeight>320) ? @"RenderingViewControllerPhone@2x" :
                             @"RenderingViewControllerPhone" bundle:nil StartFrame:0 EndFrame:editorScene->totalFrames renderOutput:RENDER_VIDEO caMresolution:editorScene->cameraResolutionType ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
        renderingView.delegate = self;
        renderingView.projectName = currentScene.name;
        renderingView.sgbPath = [self getSGBPath];
        if([Utility IsPadDevice] || ScreenHeight < 400)
            renderingView.modalPresentationStyle = UIModalPresentationFormSheet;
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
        CATransition* transition1 = [CATransition animation];
        transition1.duration = 0.5;
        transition1.type = kCATransitionPush;
        transition1.subtype = (status) ? kCATransitionFromRight : kCATransitionFromLeft;
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
    CAMERA_VIEW_MODE cameraView = (indexValue == FRONT_VIEW) ? VIEW_FRONT : (indexValue == TOP_VIEW) ? (CAMERA_VIEW_MODE)VIEW_TOP : (indexValue == LEFT_VIEW) ? VIEW_LEFT : (indexValue == BACK_VIEW) ? VIEW_BACK : (indexValue == RIGHT_VIEW) ? VIEW_RIGHT :VIEW_BOTTOM;
        [self.popoverController dismissPopoverAnimated:YES];
        editorScene->updater->changeCameraView(cameraView);
}

- (void) addFrameBtnDelegateAction:(int)indexValue{
    int addCount = (indexValue == ONE_FRAME) ? 1 : (indexValue == TWENTY_FOUR_FRAMES) ? 24 : 240;
        [self.popoverController dismissPopoverAnimated:YES];
        editorScene->totalFrames += addCount;
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->totalFrames-1 inSection:0];
        [self.framesCollectionView reloadData];
        [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
}

- (void) loginBtnAction{
        [self.popoverController dismissPopoverAnimated:YES];
        loginVc = [[LoginViewController alloc] initWithNibName:([Utility IsPadDevice]) ? @"LoginViewController" : @"LoginViewControllerPhone"bundle:nil];
        [loginVc.view setClipsToBounds:YES];
        loginVc.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:loginVc animated:YES completion:nil];
        loginVc.view.superview.backgroundColor = [UIColor clearColor];
}

- (void) infoBtnDelegateAction:(int)indexValue{
    
    if(indexValue==SETTINGS){
        [self.popoverController dismissPopoverAnimated:YES];
        settingsVc = [[SettingsViewController alloc]initWithNibName:([Utility IsPadDevice]) ? @"SettingsViewController" :
                      ([self iPhone6Plus]) ? @"SettingsViewControllerPhone2x" :
                      @"SettingsViewControllerPhone" bundle:nil];
        [settingsVc.view setClipsToBounds:YES];
        settingsVc.delegate=self;
        if([Utility IsPadDevice] || ScreenHeight < 400)
            settingsVc.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:settingsVc animated:YES completion:nil];
        settingsVc.view.superview.backgroundColor = [UIColor clearColor];
    }
    else if(indexValue==CONTACT_US){
        [self.popoverController dismissPopoverAnimated:YES];
        NSString *currentDeviceName;
        if(deviceNames != nil && [deviceNames objectForKey:[self deviceName]])
            currentDeviceName = [deviceNames objectForKey:[self deviceName]];
        else
            currentDeviceName = @"Unknown Device";
        if ([MFMailComposeViewController canSendMail]) {
            MFMailComposeViewController *picker = [[MFMailComposeViewController alloc] init];
            picker.mailComposeDelegate = self;
            NSArray *usersTo = [NSArray arrayWithObject: @"iyan3d@smackall.com"];
            [picker setSubject:[NSString stringWithFormat:@"Feedback on Iyan 3d app (%@  , iOS Version: %.01f)",[deviceNames objectForKey:[self deviceName]],iOSVersion]];
            [picker setToRecipients:usersTo];
            [self presentModalViewController:picker animated:YES];
        }else {
            [self.view endEditing:YES];
            UIAlertView *alert=[[UIAlertView alloc] initWithTitle:@"Alert" message:@"Email account not configured." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [alert show];
            return;
        }
    }
}




-(void) optionBtnDelegate:(int)indexValue
{
    [self.popoverController dismissPopoverAnimated:YES];
}

-(NSString*) deviceName
{
    struct utsname systemInfo;
    uname(&systemInfo);
    
    return [NSString stringWithCString:systemInfo.machine
                              encoding:NSUTF8StringEncoding];
}

- (void) deleteObjectOrAnimation
{
    [self.popoverController dismissPopoverAnimated:YES];
    UIAlertView* deleteAlert = nil;
    if (editorScene->selectedNodeId <= NODE_LIGHT) {
        [self.view endEditing:YES];
        deleteAlert = [[UIAlertView alloc] initWithTitle:@"Delete" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Delete Animation in this Frame", nil];
            [deleteAlert setTag:DELETE_BUTTON_OBJECT_ANIMATION];
    }
    else {
        [self.view endEditing:YES];
        deleteAlert = [[UIAlertView alloc] initWithTitle:@"Delete" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Delete Animation in this Frame", @"Delete Object", nil];
            [deleteAlert setTag:DELETE_BUTTON_OBJECT];
    }

    [deleteAlert show];
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
bool downloadMissingAssetCallBack(std::string fileName, NODE_TYPE nodeType, bool hasTexture)
{
    
    NSLog(@"File Name %s Node Type %d",fileName.c_str(),nodeType);
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [docPaths objectAtIndex:0];
    
    switch (nodeType) {
        case NODE_PARTICLES: {
            NSString* name = [NSString stringWithCString:fileName.c_str() encoding:[NSString defaultCStringEncoding]];
            NSString* file = [name stringByDeletingPathExtension];
            if ([file intValue] >= 50000 && ([file intValue] < 60000)) {
                NSString* particlePath = [NSString stringWithFormat:@"%@%d.json", cacheDirectory, [file intValue]];
                if (![[NSFileManager defaultManager] fileExistsAtPath:particlePath]) {
                    NSString* filePath = [NSString stringWithFormat:@"%@/%d.json", cacheDirectory, [file intValue]];
                    NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/particles/%d.json", [file intValue]];
                    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
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
                    return false;
                }
                else
                    return true;
            }

            break;
        }
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

- (void) removeAnimationFromCurrentFrame
{
    bool isKeySetForNode = editorScene->animMan->removeAnimationForSelectedNodeAtFrame(editorScene->currentFrame);
    if (!isKeySetForNode) {
        [self.view endEditing:YES];
        UIAlertView* deleteAlert = [[UIAlertView alloc] initWithTitle:@"Info" message:@"There is no animation in this frame." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil, nil];
        [deleteAlert setTag:UNDEFINED_OBJECT];
        [deleteAlert show];
    }
    else
        [self.framesCollectionView reloadData];
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

- (void)meshPropertyChanged:(float)refraction Reflection:(float)reflection Lighting:(BOOL)light Visible:(BOOL)visible FaceNormal:(BOOL)isHaveFaceNormal
{
    if(editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size())
        return;
    
    if (editorScene->nodes[editorScene->selectedNodeId]->props.isLighting != light || editorScene->nodes[editorScene->selectedNodeId]->props.isVisible != visible) { //switch action
        editorScene->actionMan->changeMeshProperty(refraction, reflection, light, visible, true);
    }
    else { //slider action
        editorScene->actionMan->changeMeshProperty(refraction, reflection, light, visible, false);
    }
}

- (void) switchMirror
{
    editorScene->switchMirrorState();
}

-(void) deleteDelegateAction
{
    if(editorScene->selectedNodeIds.size() > 0){
        editorScene->loader->removeSelectedObjects();
        [self undoRedoButtonState:DEACTIVATE_BOTH];
    }
    else{
        [self deleteObjectOrAnimation];
        [self undoRedoButtonState:DEACTIVATE_BOTH];
    }
    [self.popoverController dismissPopoverAnimated:YES];
    [self updateAssetListInScenes];
}

-(void)cloneDelegateAction{
    [self createDuplicateAssets];
    [self updateAssetListInScenes];
    [self.popoverController dismissPopoverAnimated:YES];
}

-(void)changeSkinDelgate{
    [self changeTextureForAsset];
    [self.popoverController dismissPopoverAnimated:YES];
}

#pragma Switch to SceneSelection
- (void) loadSceneSelectionView
{
    SceneSelectionControllerNew* sceneSelectionView = [[SceneSelectionControllerNew alloc] initWithNibName:([Utility IsPadDevice]) ? @"SceneSelectionControllerNew" : @"SceneSelectionControllerNewPhone" bundle:nil IsFirstTimeOpen:NO];
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    [appDelegate.window setRootViewController:sceneSelectionView];
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
    [_loggedInVc dismissViewControllerAnimated:YES completion:nil];
    [self.popoverController dismissPopoverAnimated:YES];
    
}

#pragma Show Or Hide Progress

-(void) showOrHideProgress:(BOOL) value{
    
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
    if(alertView.tag == DELETE_BUTTON_OBJECT){
        if(buttonIndex == 1){
            [self removeAnimationFromCurrentFrame];
            [self reloadFrames];
        }
        else if(buttonIndex == 2){
            [renderViewMan removeNodeFromScene:editorScene->selectedNodeId IsUndoOrRedo:NO];
            [self reloadFrames];
        }
    } else if(alertView.tag == DELETE_BUTTON_OBJECT_ANIMATION){
        [self removeAnimationFromCurrentFrame];
        [self reloadFrames];
    } else if (alertView.tag == ADD_BUTTON_TAG) {
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
    } else if (alertView.tag == CHOOSE_RIGGING_METHOD) {
        if(buttonIndex == HUMAN_RIGGING) {
            editorScene->rigMan->skeletonType = SKELETON_HUMAN;
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode+1));
        }else if(buttonIndex == OWN_RIGGING) {
            editorScene->rigMan->skeletonType = SKELETON_OWN;
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode+1));
        }
       [self autoRigMirrorBtnHandler];
    } else if (alertView.tag == DATA_LOSS_ALERT) {
        if(buttonIndex == CANCEL_BUTTON_INDEX) {
            
        } else if(buttonIndex == OK_BUTTON_INDEX) {
            editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode-1));
            [self moveLastAction:nil];
        }
    } else if (alertView.tag == SGR_WARNING) {
        if(buttonIndex == OK_BUTTON_INDEX) {
            [self performSelectorOnMainThread:@selector(beginRigging) withObject:nil waitUntilDone:NO];
        }
    }
    if(editorScene && editorScene->isRigMode)
        [self autoRigMirrorBtnHandler];
}


#pragma mark SettingsViewControllerDelegate

-(void)frameCountDisplayMode{
    [self.framesCollectionView setTag:[[[AppHelper getAppHelper] userDefaultsForKey:@"indicationType"]intValue]];
    [self.framesCollectionView reloadData];
}

-(void)cameraPreviewSize
{
    int selectedIndex = [[[AppHelper getAppHelper] userDefaultsForKey:@"cameraPreviewSize"]intValue];
    if(selectedIndex==CAMERA_PREVIEW_SMALL)
    {
        editorScene->camPreviewScale=1.0;
    }
    else
    {
        editorScene->camPreviewScale=2.0;
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithFloat:editorScene->camPreviewScale] withKey:@"cameraPreviewSize"];
    }
}

-(void)cameraPreviewPosition{
    int selctedIndex = [[[AppHelper getAppHelper] userDefaultsForKey:@"cameraPreviewPosition"]intValue];
    NSLog(@"Camera Position : %d " , selctedIndex);
    float camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
    if(selctedIndex==PREVIEW_LEFTBOTTOM){
        camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
        if(editorScene->camPreviewScale==1.0){
            editorScene->camPreviewOrigin.x=0;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=-editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            
        }
    }
    if(selctedIndex==PREVIEW_LEFTTOP){
        camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
        if(editorScene->camPreviewScale==1.0){
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height*editorScene->screenScale;
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height*editorScene->screenScale;
        }
    }
    
    if(selctedIndex==PREVIEW_RIGHTBOTTOM){
        if(editorScene->camPreviewScale==1.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=self.renderView.frame.size.width*editorScene->screenScale-self.rightView.frame.size.width*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=-editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            
            
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=self.renderView.frame.size.width*editorScene->screenScale-self.rightView.frame.size.width*editorScene->screenScale;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            NSLog(@"Camera preview orgin.x : %f",editorScene->camPreviewOrigin.x);
            
        }
    }
    if(selctedIndex==PREVIEW_RIGHTTOP){
        
        if(editorScene->camPreviewScale==1.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=self.view.frame.size.width*editorScene->screenScale-self.rightView.frame.size.width*editorScene->screenScale;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height*editorScene->screenScale;
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[editorScene->cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[editorScene->cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[editorScene->cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=self.view.frame.size.width*editorScene->screenScale-self.rightView.frame.size.width*editorScene->screenScale;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height*editorScene->screenScale;
        }
    }
    
}
-(void)toolbarPosition:(int)selctedIndex
{
    CGRect frame = self.rightView.frame;
    frame.origin.x = (selctedIndex==TOOLBAR_LEFT) ? 0 : self.view.frame.size.width-self.rightView.frame.size.width;
    //frame.origin.y = self.topView.frame.size.height; // new y coordinate
    self.rightView.frame = frame;
    CGRect frame1 = self.leftView.frame;
    frame1.origin.x = ((selctedIndex==TOOLBAR_LEFT) ? self.view.frame.size.width-self.leftView.frame.size.width : 0);
    //frame1.origin.y = (self.topView.frame.size.height); // new y coordinate
    self.leftView.frame = frame1;

    if(editorScene) {
        editorScene->topLeft = Vector2((selctedIndex==TOOLBAR_RIGHT) ? 0.0 : self.rightView.frame.size.width, self.topView.frame.size.height) * editorScene->screenScale;
        editorScene->topRight = Vector2((selctedIndex==TOOLBAR_RIGHT) ? self.view.frame.size.width-self.rightView.frame.size.width : self.view.frame.size.width, self.topView.frame.size.height) * editorScene->screenScale;
        editorScene->bottomLeft = Vector2((selctedIndex==TOOLBAR_RIGHT) ? 0.0 : self.rightView.frame.size.width, self.view.frame.size.height) * editorScene->screenScale;
        editorScene->bottomRight = Vector2((selctedIndex==TOOLBAR_RIGHT) ? self.view.frame.size.width-self.rightView.frame.size.width : self.view.frame.size.width, self.view.frame.size.height) * editorScene->screenScale;
        
        editorScene->renHelper->movePreviewToCorner();
    }
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:selctedIndex] withKey:@"toolbarPosition"];
    [self setupEnableDisableControls];
}

-(void)multiSelectUpdate:(BOOL)value{
        if(!value)
            editorScene->selectMan->unselectObjects();
        self.objectList.allowsMultipleSelection=value;
         [[AppHelper getAppHelper] saveBoolUserDefaults:value withKey:@"multiSelectOption"];
    NSLog(@"%d",[[AppHelper getAppHelper] userDefaultsBoolForKey:@"multiSelectOption"]);
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
- (void) autoRigMirrorBtnHandler
{
    bool status = YES;
    if(editorScene && editorScene->isRigMode && (editorScene->rigMan->sceneMode == RIG_MODE_MOVE_JOINTS || editorScene->rigMan->sceneMode == RIG_MODE_EDIT_ENVELOPES)){
        status = (editorScene->rigMan->isNodeSelected && editorScene->rigMan->skeletonType == SKELETON_HUMAN) ? NO : YES;
    }
    [_autorigMirrorBtnHolder setHidden:status];
    [_autorigMirrorLable setHidden:status];
    [_autoRigMirrorSwitch setHidden:status];    
    if(editorScene && editorScene->isRigMode){
        switch (editorScene->rigMan->sceneMode) {
            case RIG_MODE_OBJVIEW:
                _rigScreenLabel.text = @"ATTACH SKELETON";
                break;
            case RIG_MODE_MOVE_JOINTS:
                _rigScreenLabel.text = @"ATTACH SKELETON";
                break;
            case RIG_MODE_EDIT_ENVELOPES:
                _rigScreenLabel.text = @"ADJUST ENVELOP";
                break;
            case RIG_MODE_PREVIEW:{
                _rigScreenLabel.text = @"PREVIEW";
                break;
            }
            default:
                break;
        }
    }
    
    if((editorScene && editorScene->isRigMode) && (((editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_MOVE_JOINTS)) || (editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_PREVIEW))){
        [_moveBtnAutorig setEnabled:YES];
        [_rotateBtnAutorig setEnabled:YES];
        [_scaleBtnAutorig setEnabled:YES];
    }
    else{
        [_moveBtnAutorig setEnabled:NO];
        [_rotateBtnAutorig setEnabled:NO];
        [_scaleBtnAutorig setEnabled:(editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)RIG_MODE_EDIT_ENVELOPES) ? YES : NO ];
    }
}

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
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
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
    
    NSString* texFrom = [NSString stringWithFormat:@"%@/%@.png",docDirPath,(!isHaveTexture)?@"Resources/Textures/White-texture":textureFileName];    
    std::string *texture = new std::string([textureFileName UTF8String]);
    if ([[NSFileManager defaultManager] fileExistsAtPath:texFrom]){
        NSString* desFilePath;
        if(isTempNode)
            desFilePath = [NSString stringWithFormat:@"%@/Resources/Textures/%@.png",docDirPath,@"temp"];
        else
            desFilePath = [NSString stringWithFormat:@"%@/Resources/Textures/%d-cm.png",docDirPath,assetIdReturn];
        [fm removeItemAtPath:texTo error:nil];
        UIImage *image =[UIImage imageWithContentsOfFile:texFrom];
        NSData *imageData = [self convertAndScaleImage:image size:-1];
        [imageData writeToFile:desFilePath atomically:YES];
        *texture = *new std::string([(isTempNode)?@"temp" : textureFileName UTF8String]);
    }
    
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
        textureName = @"temp";
    else
        textureName = [NSString stringWithFormat:@"%d%@",assetIdReturn,@"-cm"];
    [dict setObject:[NSString stringWithFormat:@"%@",(isHaveTexture) ? textureName : @"-1"] forKey:@"textureName"];
    [dict setObject:[NSNumber numberWithBool:isTempNode] forKey:@"isTempNode"];
    [dict setObject:[NSNumber numberWithBool:isHaveTexture] forKey:@"isHaveTexture"];
    [self performSelectorOnMainThread:@selector(loadObjOrSGM:) withObject:dict waitUntilDone:YES];
    [self performSelectorInBackground:@selector(hideLoadingActivity) withObject:nil];
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

-(int)addrigFileToCacheDirAndDatabase:(NSString*)texturemainFileNameRig VertexColor:(Vector3)vertexColor
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
    objAsset.textureName = (vertexColor == -1) ? [NSString stringWithFormat:@"%d%@",objAsset.assetId,@"-cm"] : @"-1";
    [self performSelectorOnMainThread:@selector(loadNode:) withObject:objAsset waitUntilDone:YES];
    editorScene->animMan->copyKeysOfNode(selectedNodeId, editorScene->nodes.size()-1);
    editorScene->animMan->copyPropsOfNode(selectedNodeId, editorScene->nodes.size()-1);
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
               srcTextureFilePath = [NSString stringWithFormat:@"%@/Resources/Textures/%@",docDirPath,fileName];
                if (![[NSFileManager defaultManager] fileExistsAtPath:srcTextureFilePath])
                    srcTextureFilePath = [NSString stringWithFormat:@"%@/Resources/Rigs/%@",docDirPath,fileName];
                    if (![[NSFileManager defaultManager] fileExistsAtPath:srcTextureFilePath])
                        srcTextureFilePath = [NSString stringWithFormat:@"%@/Resources/Textures/White-texture.png",docDirPath];
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
            selectedNodeId = (int)editorScene->nodes.size()-1;
            editorScene->selectMan->selectObject((int)editorScene->nodes.size()-1,false);
            [self changeTexture:@"0-cm" VertexColor:Vector3(1.0) IsTemp:YES]; // for White Texture Thumbnail
            editorScene->selectMan->unselectObject((int)editorScene->nodes.size()-1);
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
            editorScene->renHelper->renderAndSaveImage((char*)[imageFilePath cStringUsingEncoding:NSUTF8StringEncoding], 0, false, YES, editorScene->currentFrame, renderBgColor);
            
            for(int i = 0; i < [nodes count]; i++){
                editorScene->nodes[[[nodes objectAtIndex:i]intValue]]->node->setVisible(true);
            }
            [nodes removeAllObjects];
            nodes = nil;
        }
        if(!isHaveTexture){
            editorScene->selectMan->selectObject(editorScene->nodes.size()-1,false);
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
    editorScene->selectMan->selectObject(selectedNodeId,false);
    if(!(editorScene->selectedNodeIds.size() > 0) && editorScene->hasNodeSelected()){
        editorScene->changeTexture(*texture, Vector3(color),isTemp,NO);
        editorScene->selectMan->unselectObject(selectedNodeId);
        editorScene->setLightingOn();
    }
    [self showOrHideProgress:HIDE_PROGRESS];
}

- (void) removeTempTextureAndVertex
{
    editorScene->removeTempTextureAndVertex(selectedNodeId);
}

#pragma Rendering Delegates

- (void) changeRenderingBgColor:(Vector4)vertexColor
{
    renderBgColor = vertexColor;
}

- (void)cameraResolutionChanged:(int)changedResolutionType
{
    cameraResoultionType = changedResolutionType;
    [self cameraPropertyChanged:editorScene->cameraFOV Resolution:cameraResoultionType];
}

-(BOOL)iPhone6Plus{
    if (([UIScreen mainScreen].scale > 2.0)) return YES;
    return NO;
}

#pragma PopOver Delegates

-(NODE_TYPE) getNodeType:(int)nodeId
{
    if(nodeId < editorScene->nodes.size()){
        return editorScene->nodes[nodeId]->getType();
    }
    return NODE_TYPE(NODE_UNDEFINED);
}

- (void)deallocSubViews
{
    if(animationsliderVC != nil){
        animationsliderVC = nil;
    }
    if(assetSelectionSlider != nil){
        assetSelectionSlider = nil;
    }
    if(textSelectionSlider != nil){
        textSelectionSlider = nil;
    }
    if(objVc != nil){
        objVc = nil;
    }
    if(importImageViewVC != nil){
        importImageViewVC = nil;
    }
}

- (void)dealloc
{
    if(smgr)
        delete smgr;
    [assetsInScenes removeAllObjects];
    assetsInScenes = nil;
    importImageViewVC.delegate = nil;
    importImageViewVC = nil;
    animationsliderVC.delegate = nil;
    animationsliderVC = nil;
    textSelectionSlider.textSelectionDelegate = nil;
    textSelectionSlider = nil;
    assetSelectionSlider.assetSelectionDelegate = nil;
    assetSelectionSlider = nil;
    loginVc.delegare = nil;
    loginVc = nil;
    lightProperties.delegate = nil;
    lightProperties = nil;
    currentScene = nil;
    cache = nil;
    [playTimer invalidate];
    playTimer = nil;
    settingsVc.delegate = nil;
    settingsVc = nil;
    renderViewMan.delegate = nil;
    renderViewMan = nil;
    objVc.objSlideDelegate = nil;
    objVc = nil;
    scaleAutoRig.delegate = nil;
    scaleAutoRig = nil;
    deviceNames = nil;
    self.imagePicker.delegate = nil;
    self.imagePicker = nil;
    self.popoverController.delegate = nil;
    self.popoverController = nil;
    self.popUpVc.delegate = nil;
    self.popUpVc = nil;
    _loggedInVc.delegare = nil;
    _loggedInVc = nil;
    _lightProp.delegate = nil;
    _lightProp = nil;
    _scaleProps.delegate = nil;
    _scaleProps = nil;
    _camProp.delegate = nil;
    _camProp = nil;
    _meshProp.delegate = nil;
    _meshProp = nil;
}

@end


