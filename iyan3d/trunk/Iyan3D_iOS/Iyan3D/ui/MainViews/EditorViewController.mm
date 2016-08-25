//
//  EditorViewController.m
//  Iyan3D
//
//  Created by Sankar on 18/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import <Crashlytics/Answers.h>

#import "EditorViewController.h"
#import "FrameCellNew.h"
#import "AppDelegate.h"
#import "SceneSelectionControllerNew.h"
#import "MediaPreviewVC.h"
#import <sys/utsname.h>

#import "SceneImporter.h"

@implementation EditorViewController

#define BASIC_SHAPES_COUNT 6

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
#define IMAGE_EXPORT 8
#define VIDEO_EXPORT 9
#define CHANGE_TEXTURE 7


#define TUTORIAL 0
#define SETTINGS 1
#define CONTACT_US 4
#define FOLLOW_US 3
#define RATE_US 2

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

#define MIRROR_OFF 0
#define MIRROR_ON 1
#define MIRROR_DISABLE 2

BOOL missingAlertShown;

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
    self.screenName = @"EditorView iOS";
    
    [self.backButton setTitle:NSLocalizedString(@"Scenes", nil) forState:UIControlStateNormal];
    [self.myObjectsBtn setTitle:NSLocalizedString(@"My Objects", nil) forState:UIControlStateNormal];
    [self.myObjectsLabel setText:NSLocalizedString(@"My Objects", nil)];
    [self.editobjectBtn setTitle:NSLocalizedString(@"Edit", nil) forState:UIControlStateNormal];

    [self.importBtn setTitle:NSLocalizedString(@"ADD", nil) forState:UIControlStateNormal];
    [self.animationBtn setTitle:NSLocalizedString(@"ANIMATION", nil) forState:UIControlStateNormal];
    [self.optionsBtn setTitle:NSLocalizedString(@"OPTIONS", nil) forState:UIControlStateNormal];
    [self.exportBtn setTitle:NSLocalizedString(@"EXPORT", nil) forState:UIControlStateNormal];

    [self.moveBtn setTitle:NSLocalizedString(@"MOVE", nil) forState:UIControlStateNormal];
    [self.rotateBtn setTitle:NSLocalizedString(@"ROTATE", nil) forState:UIControlStateNormal];
    [self.scaleBtn setTitle:NSLocalizedString(@"SCALE", nil) forState:UIControlStateNormal];

    [self.addJointBtn setTitle:NSLocalizedString(@"ADD JOINT", nil) forState:UIControlStateNormal];
    [self.rigCancelBtn setTitle:NSLocalizedString(@"CANCEL", nil) forState:UIControlStateNormal];
    [self.rigAddToSceneBtn setTitle:NSLocalizedString(@"ADD TO SCENE", nil) forState:UIControlStateNormal];

    [self.autorigMirrorLable setText:NSLocalizedString(@"MIRROR", nil)];

    isSelected=NO;
    followUsVC = nil;
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

    if ([[AppHelper getAppHelper]userDefaultsBoolForKey:@"multiSelectOption"] == YES)
        self.objectList.allowsMultipleSelection = YES;
    else
        self.objectList.allowsMultipleSelection=NO;
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    [self.autoRigLbl setHidden:YES];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSLog(@"Document Path : %@ ",documentsDirectory);
    [_center_progress stopAnimating];
    [_center_progress setHidden:YES];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersBG) name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(openLoggedInView) name:@"renderCompleted" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersFG) name:@"applicationDidBecomeActive" object:nil];
    
    ScreenWidth = self.renderView.frame.size.width;
    ScreenHeight = self.renderView.frame.size.height;
    if(!editorScene) {
        [self initScene];
        [self changeAllButtonBG];
        [self setupEnableDisableControls];
        [self createDisplayLink];
        
        if([[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]){
            [self toolbarPosition:(int)[[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]];
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
    
    
    if(editorScene) {
        int lightId = 0;
        for(int i = 0 ; i < editorScene->nodes.size(); i++) {
            if(editorScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
                lightId = MAX(editorScene->nodes[i]->assetId,lightId);
        }
        lightCount += lightId-((lightId != 0) ? 900 : 0);
    }
    
    UITapGestureRecognizer* topTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:nil];
    UITapGestureRecognizer* rightTap = [[UITapGestureRecognizer alloc] initWithTarget:self action:nil];
    topTap.delegate = self;
    rightTap.delegate = self;
    [self.topView addGestureRecognizer:topTap];
    [self.rightView addGestureRecognizer:rightTap];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"applicationDidBecomeActive" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"renderCompleted" object:nil];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    if([touch.view isDescendantOfView:self.renderView] || [touch.view isDescendantOfView:self.toolTipBtn] || [touch.view isDescendantOfView:self.helpBtn]) {
        return YES;
    }
    [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
    return NO;
}

- (void) openLoggedInView
{
    if(editorScene && !editorScene->freezeRendering)
        [self performSelectorOnMainThread:@selector(loginBtnAction:) withObject:nil waitUntilDone:YES];
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
    float screenScale = [[AppHelper getAppHelper] userDefaultsBoolForKey:@"ScreenScaleDisable"] ? 1.0 : [[UIScreen mainScreen] scale];
    renderViewMan = [[RenderViewManager alloc] init];
    renderViewMan.delegate = self;
    [renderViewMan setupLayer:_renderView];
    
    if (isMetalSupported) {
        [[AppDelegate getAppDelegate] initEngine:METAL ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight ScreenScale:screenScale renderView:_renderView];
        smgr = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        
        editorScene = new SGEditorScene(METAL, smgr, ScreenWidth * screenScale, ScreenHeight * screenScale, 4000);
        [renderViewMan setUpPaths:smgr];
        editorScene->screenScale = screenScale;
    }
    else {
        
        maxUnisKey = [NSString stringWithFormat:@"maxUniforms%@",[[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString *)kCFBundleVersionKey]];
        maxJointsKey = [NSString stringWithFormat:@"maxJoints%@",[[[NSBundle mainBundle] infoDictionary] objectForKey:(NSString *)kCFBundleVersionKey]];
        
        [renderViewMan setupContext];
        [[AppDelegate getAppDelegate] initEngine:OPENGLES2 ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight ScreenScale:screenScale renderView:_renderView];
        smgr = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        [self getMaxUniformsForOpenGL];
        [self getMaximumJointsSize];
        int maxUnis = [[[AppHelper getAppHelper] userDefaultsForKey: maxUnisKey] intValue];
        int maxJoints = [[[AppHelper getAppHelper] userDefaultsForKey: maxJointsKey] intValue];
        editorScene = new SGEditorScene(OPENGLES2, smgr, ScreenWidth*screenScale, ScreenHeight*screenScale, maxUnis, maxJoints);
        NSLog(@" \n Max joints %d ", maxJoints);
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

- (void) getMaxUniformsForOpenGL
{
    ShaderManager::BundlePath = constants::BundlePath;
    ShaderManager::deviceType = (isMetalSupported) ? METAL : OPENGLES2;
    
    if(![[AppHelper getAppHelper] userDefaultsForKey:maxUnisKey]) {
        
        int lowerLimit = 0;
        int upperLimit = 512;
        while ((upperLimit - lowerLimit) != 1) {
            int mid = (lowerLimit + upperLimit) / 2;
            if(ShaderManager::LoadShader(smgr, OPENGLES2, "SHADER_MESH", "mesh.vsh", "common.fsh", ShaderManager::getShaderStringsToReplace(mid), true))
                lowerLimit = mid;
            else
                upperLimit = mid;
            
        }
        printf("\n Max Uniforms %d ", lowerLimit);
        
        smgr->clearMaterials();
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:lowerLimit] withKey:maxUnisKey];
    }
}

- (void) getMaximumJointsSize
{
    ShaderManager::BundlePath = constants::BundlePath;
    ShaderManager::deviceType = (isMetalSupported) ? METAL : OPENGLES2;
    
    if(![[AppHelper getAppHelper] userDefaultsForKey:maxJointsKey]) {
        
        int lowerLimit = 0;
        int upperLimit = 512;
        while ((upperLimit - lowerLimit) != 1) {
            int mid = (lowerLimit + upperLimit) / 2;
            if(ShaderManager::LoadShader(smgr, OPENGLES2, "SHADER_SKIN", "skin.vsh", "common.fsh", ShaderManager::getStringsForRiggedObjects(mid), true))
                lowerLimit = mid;
            else
                upperLimit = mid;
            
        }
        printf("\n Max Joints %d ", lowerLimit);
        
        smgr->clearMaterials();
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:lowerLimit] withKey:maxJointsKey];
    }
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
    BOOL status = (editorScene && editorScene->isRigMode) ? YES : hide;
    
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

- (void)changeAllButtonBG
{
    UIColor *selectedColor = [UIColor colorWithRed:123.0f / 255.0f green:123.0f / 255.0f blue:127.0f / 255.0f alpha:1];
    UIColor *unSelectedColor = [UIColor colorWithRed:66.0f / 255.0f green:66.0f / 255.0f blue:68.0f / 255.0f alpha:1];
    
    if(editorScene->isNodeSelected || editorScene->selectedNodeIds.size() > 0) {
        [_moveBtn setBackgroundColor:(editorScene->controlType==MOVE) ? selectedColor : unSelectedColor];
        [_rotateBtn setBackgroundColor:(editorScene->controlType==ROTATE) ? selectedColor : unSelectedColor];
        [_scaleBtn setBackgroundColor:(editorScene->controlType==SCALE) ? selectedColor : unSelectedColor];
    } else {
        [self.moveBtn setBackgroundColor:unSelectedColor];
        [self.rotateBtn setBackgroundColor:unSelectedColor];
        [self.scaleBtn setBackgroundColor:unSelectedColor];
    }
}

-(void)setupEnableDisableControls
{
    [self sceneMirrorUIPositionChanger];
    bool sceneMirrorState = !(editorScene && !editorScene->isRigMode && editorScene->getSelectedNode() && editorScene->getSelectedNode()->joints.size() == HUMAN_JOINTS_SIZE);
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
        
    } else {
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
            [self.rotateBtn setEnabled:(editorScene->nodes[editorScene->selectedNodeId]->getProperty(LIGHT_TYPE).value.x == (int)DIRECTIONAL_LIGHT)];
            [self.optionsBtn setEnabled:true];
            [self.scaleBtn setEnabled:false];
            [self.moveBtn sendActionsForControlEvents:UIControlEventTouchUpInside];
            return;
            
        } else if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_PARTICLES){
            [self.moveBtn setEnabled:true];
            [self.rotateBtn setEnabled:true];
            [self.optionsBtn setEnabled:true];
            [self.scaleBtn setEnabled:false];
            if(editorScene->controlType == SCALE)
                [self.moveBtn sendActionsForControlEvents:UIControlEventTouchUpInside];
            return;
            
        } else if(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_CAMERA) {
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
        } else {
            [self.moveBtn setEnabled:true];
            [self.rotateBtn setEnabled:true];
            [self.scaleBtn setEnabled:true];
            [self.optionsBtn setEnabled:(editorScene->allNodesRemovable() || editorScene->allNodesClonable())];
        }
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
    [self hideLoadingActivity];
}

- (void) load3DTex:(int)type AssetId:(int)assetId TextureName:(NSString*)textureName TypedText:(NSString*)typedText FontSize:(int)fontSize BevelValue:(float)bevelRadius TextColor:(Vector4)colors FontPath:(NSString*)fontFileName isTempNode:(bool)isTempNode
{
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

- (void) load3dTextOnMainThread:(NSMutableDictionary*)fontDetails
{
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
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
        UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Information", nil) message:NSLocalizedString(@"max_five_lights_per_scene", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
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
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
    [renderViewMan loadNodeInScene:ASSET_ADDITIONAL_LIGHT AssetId:assetId AssetName:[self getwstring:lightName] TextureName:(@"-1") Width:20 Height:50 isTempNode:NO More:nil ActionType:assetAddType VertexColor:Vector4(-1)];
}

- (void) loadNodeInScene:(AssetItem*)assetItem ActionType:(ActionType)actionType
{
    assetAddType = actionType;
    assetItem.textureName = [NSString stringWithFormat:@"%d%@",  assetItem.assetId, @"-cm"];
    [self performSelectorOnMainThread:@selector(loadNode:) withObject:assetItem waitUntilDone:YES];
}

- (void)loadNodeForImage:(NSMutableDictionary*)nsDict
{
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
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
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
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
        ShaderManager::shadowsOff = editorScene->screenScale > 1.0f ? true : false;
        
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

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    if(editorScene) {
        if(editorScene->isRigMode && editorScene->rigMan && editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)(RIG_MODE_PREVIEW)){
            [_addJointBtn setEnabled:(editorScene->rigMan->isSGRJointSelected)?NO:YES];
        }
    }
}

- (void)loadScene
{
    if (![self loadFromFile]) {
        if(renderViewMan){
            [renderViewMan addCameraLight];
        }
    }
    cameraResolutionType = editorScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x;
    editorScene->updater->setCameraProperty(editorScene->nodes[NODE_CAMERA]->getProperty(FOV).value.x, editorScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x);
    editorScene->updater->setDataForFrame(editorScene->currentFrame);
    
    if(![[AppHelper getAppHelper] userDefaultsForKey:@"addbtnpressed"]) {
        [self setTipArrowDirections];
        [[AppHelper getAppHelper] showTipForView:self.importBtn InMainView:self.view];
    }
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
            UIAlertView* fileCorruptAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"File Corrupted", nil) message:NSLocalizedString(@"File_Corrupted_message", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil, nil];
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

- (void) reloadSceneObjects
{
    [self updateAssetListInScenes];
    [self.objectList reloadData];
}

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
        [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
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

-(void)tableView:(UITableView *)tableView deSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    
    [self.objectList deselectRowAtIndexPath:indexPath animated:YES];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

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

- (IBAction)backToScenes:(id)sender
{
    
    if(![[AppHelper getAppHelper] userDefaultsForKey:@"backPressed"]) {
        [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
        [self setTipArrowDirections];
        [[AppHelper getAppHelper] showTipForView:self.backButton InMainView:self.view];
        [[AppHelper getAppHelper] showTipForView:self.exportBtn InMainView:self.view];
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithBool:YES] withKey:@"backPressed"];
    } else {
        [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
        [self exitScene];
    }
}

- (void) exitScene
{
    [self addFabricEvent:@"BackToScenes" WithAttribute:-1];
    if(editorScene)
        editorScene->freezeRendering = true;
    if(editorScene && editorScene->isPlaying) {
        [self stopPlaying];
        editorScene->freezeRendering = false;
        return;
    }
    
    if(editorScene)
        editorScene->isPreviewMode = false;
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
    [self performSelectorOnMainThread:@selector(removeAllSubViewAndMemory) withObject:nil waitUntilDone:YES];
    [self performSelectorOnMainThread:@selector(saveAndExit) withObject:nil waitUntilDone:YES];
}

- (void) saveAndExit
{
    [self saveAnimationData];
    NSString* thumbPath = [self saveThumbnail];
    [renderViewMan createi3dFileWithThumb:thumbPath];
    [self loadSceneSelectionView];
    
}

- (IBAction)playButtonAction:(id)sender
{
    [self showLoadingActivity];
    [self performSelectorInBackground:@selector(syncPhysicsAndPlay) withObject:nil];
}

- (void) syncPhysicsAndPlay
{
    if(editorScene && editorScene->currentFrame == 0)
        [self syncSceneWithPhysicsWorld];
    
    [self performSelectorOnMainThread:@selector(playAnimation) withObject:nil waitUntilDone:NO];
    [self hideLoadingActivity];
}

- (IBAction)toolTipAction:(id)sender
{
    
    [self setTipArrowDirections];
    [[AppHelper getAppHelper] toggleHelp:self Enable:YES];
}

- (void) setTipArrowDirections
{
    NSInteger toolBarPos = [[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue];
    
    self.myObjectsBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.importBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.animationBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.optionsBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.exportBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.rotateBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.playBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.objectList.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.undoBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"" : @"1";
    self.redoBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"";
    self.addJointBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.autorigMirrorBtnHolder.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.rotateBtnAutorig.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"3" : @"1";
    self.undoBtn.accessibilityHint = NSLocalizedString((toolBarPos == TOOLBAR_LEFT) ? @"" : @"Undo / Redo your actions.", nil);
    self.redoBtn.accessibilityHint = NSLocalizedString((toolBarPos == TOOLBAR_LEFT) ? @"Undo / Redo your actions." : @"", nil);
   
}

- (IBAction)moveLastAction:(id)sender
{
    if(sender != nil)
        [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
    
    if((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode) != RIG_MODE_PREVIEW) {
        if(editorScene->rigMan->sceneMode == RIG_MODE_OBJVIEW) {
            
            if(editorScene->rigMan->rigNodeType == NODE_RIG) {
                [self performSelectorOnMainThread:@selector(switchAutoRigSceneMode:) withObject:[NSNumber numberWithInt:1] waitUntilDone:YES];
            } else {
                UIAlertView *closeAlert = [[UIAlertView alloc]initWithTitle:NSLocalizedString(@"Select Bone Structure", nil) message:NSLocalizedString(@"start_single_bone_or_human_message", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"Single Bone", nil), NSLocalizedString(@"Human Bone Structure", nil), nil];
                [closeAlert setTag:CHOOSE_RIGGING_METHOD];
                [closeAlert show];
            }
            
        } else if(editorScene->rigMan->sceneMode + 1 == RIG_MODE_PREVIEW) {
            [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
            NSString *tempDir = NSTemporaryDirectory();
            NSString *sgrFilePath = [NSString stringWithFormat:@"%@r-%@.sgr", tempDir, @"autorig"];
            string path = (char*)[sgrFilePath cStringUsingEncoding:NSUTF8StringEncoding];
            [self exportSgr:sgrFilePath];
            [_rigAddToSceneBtn setHidden:NO];
            [self performSelectorOnMainThread:@selector(switchAutoRigSceneMode:) withObject:[NSNumber numberWithInt:1] waitUntilDone:YES];
            [_addJointBtn setEnabled:NO];
        } else {
            [_rigAddToSceneBtn setHidden:YES];
            [self performSelectorOnMainThread:@selector(switchAutoRigSceneMode:) withObject:[NSNumber numberWithInt:1] waitUntilDone:YES];
        }
    }
    
    [self autoRigMirrorBtnHandler];
    [self performSelectorInBackground:@selector(hideLoadingActivity) withObject:nil];
}

- (void) switchAutoRigSceneMode:(NSNumber*)number
{
    if(editorScene && editorScene->rigMan)
        editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode+[number intValue]));
}

- (void) showLoadingActivity
{
    if(_center_progress != nil) {
        [_center_progress setHidden:NO];
        [_center_progress startAnimating];
    }
}

- (void) setUserInteractionStatus:(BOOL) status
{
    [self.view setUserInteractionEnabled:status];
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

- (IBAction)moveFirstAction:(id)sender
{
    
    [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
    if((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode) != RIG_MODE_OBJVIEW){
        if((AUTORIG_SCENE_MODE)(editorScene->rigMan->sceneMode-1) == RIG_MODE_OBJVIEW) {
            [self.view endEditing:YES];
            UIAlertView *dataLossAlert = [[UIAlertView alloc]initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"rigging_back_previous_mode", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"NO", nil) otherButtonTitles:NSLocalizedString(@"Yes", nil), nil];
            [dataLossAlert setTag:DATA_LOSS_ALERT];
            [dataLossAlert show];
        } else {
            [self performSelectorOnMainThread:@selector(switchAutoRigSceneMode:) withObject:[NSNumber numberWithInt:-1] waitUntilDone:YES];
        }
    }
    [_rigAddToSceneBtn setHidden:YES];
    [_addJointBtn setEnabled:NO];
    [self autoRigMirrorBtnHandler];
}

- (IBAction)rigCancelAction:(id)sender
{
    [self performSelectorOnMainThread:@selector(deallocateAutoRig) withObject:nil waitUntilDone:YES];
}

- (void) deallocateAutoRig
{
    [self performSelectorOnMainThread:@selector(deallocateAutoRigOnMainThread:) withObject:[NSNumber numberWithBool:NO] waitUntilDone:YES];
}

- (IBAction)rigAddToSceneAction:(id)sender
{
    [Answers logCustomEventWithName:@"AutoRig-Completion" customAttributes:nil];
    
    Vector3 vertexColor = Vector3(-1.0);
    if(editorScene->rigMan->nodeToRig->getProperty(IS_VERTEX_COLOR).value.x){
        Vector4 vColor = editorScene->rigMan->nodeToRig->getProperty(VERTEX_COLOR).value;
        vertexColor = Vector3(vColor.x, vColor.y, vColor.z);
    }
    
    [self addrigFileToCacheDirAndDatabase:[NSString stringWithFormat:@"%s%@",editorScene->nodes[selectedNodeId]->getProperty(TEXTURE).fileName.c_str(),@".png"] VertexColor:vertexColor];
    
    [self performSelectorOnMainThread:@selector(deallocateAutoRigOnMainThread:) withObject:[NSNumber numberWithBool:YES] waitUntilDone:YES];
}

- (void) deallocateAutoRigOnMainThread:(NSNumber*) object
{
    [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
    if(editorScene->rigMan->deallocAutoRig([object boolValue])) {
        editorScene->enterOrExitAutoRigMode(false);
        [self setupEnableDisableControls];
    }
    [self autoRigViewButtonHandler:YES];
    selectedNodeId = -1;
    [self autoRigMirrorBtnHandler];
    [self highlightObjectList];
    [self performSelectorInBackground:@selector(hideLoadingActivity) withObject:nil];
    [self reloadSceneObjects];
}

- (IBAction)addJoinAction:(id)sender
{
    if(editorScene->rigMan->isSkeletonJointSelected)
        editorScene->rigMan->addNewJoint();
}

- (IBAction)publishBtnAction:(id)sender
{
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
    self.popoverController.popoverContentSize = CGSizeMake(180.0, 39*3);
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
        self.popoverController.popoverContentSize = CGSizeMake(204.0, 39*2);
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
        self.popoverController.popoverContentSize = CGSizeMake(204.0, 39*2);
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

- (IBAction)loginBtnAction:(id)sender
{
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
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT) {
        _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"animationBtn"];
        [_popUpVc.view setClipsToBounds:YES];
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
        self.popoverController.popoverContentSize = CGSizeMake(204.0, 39*2);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        self.popoverController.delegate =self;
        self.popUpVc.delegate=self;
        CGRect rect = _animationBtn.frame;
        rect = [self.view convertRect:rect fromView:_animationBtn.superview];
        [self.popoverController presentPopoverFromRect:rect inView:self.view permittedArrowDirections:UIPopoverArrowDirectionLeft animated:YES];

    } else {
        _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"animationBtn"];
        [_popUpVc.view setClipsToBounds:YES];
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
        self.popoverController.popoverContentSize = CGSizeMake(204.0, 39*2);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        self.popoverController.delegate =self;
        self.popUpVc.delegate=self;
        CGRect rect = _animationBtn.frame;
        rect = [self.view convertRect:rect fromView:_animationBtn.superview];
        [self.popoverController presentPopoverFromRect:rect inView:self.view permittedArrowDirections:UIPopoverArrowDirectionRight animated:YES];
    }
}

- (IBAction)importBtnAction:(id)sender
{
    if(![[AppHelper getAppHelper] userDefaultsForKey:@"addbtnpressed"])
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithBool:YES] withKey:@"addbtnpressed"];
    BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"importBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popoverController.popoverContentSize = ([self iPhone6Plus]) ? CGSizeMake(205.0, 39*8) : CGSizeMake(205.0, 39*8);
    self.popoverController.delegate =self;
    self.popUpVc.delegate=self;
    CGRect rect = _importBtn.frame;
    rect = [self.view convertRect:rect fromView:_importBtn.superview];
    [self.popoverController presentPopoverFromRect:rect inView:self.view permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight animated:YES];
}

- (IBAction)infoBtnAction:(id)sender
{
    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"infoBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(200.0, 39*5);
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
    self.popoverController.popoverContentSize = CGSizeMake(182.0, 39*6);
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
    if(toPath.row < [self collectionView:self.framesCollectionView numberOfItemsInSection:0])
        [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    else
        return;
    editorScene->previousFrame = editorScene->currentFrame;
    editorScene->currentFrame = editorScene->totalFrames - 1;
    editorScene->actionMan->switchFrame(editorScene->currentFrame);
    [self HighlightFrame];
}

- (IBAction)firstFrameBtnAction:(id)sender
{
    [self NormalHighLight];
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:0 inSection:0];
    if(toPath.row < [self collectionView:self.framesCollectionView numberOfItemsInSection:0])
        [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    else
        return;
    editorScene->previousFrame = editorScene->currentFrame;
    editorScene->currentFrame = 0;
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
    [self switchToFrame:editorScene->currentFrame];
    [self HighlightFrame];
}

- (void) switchToFrame:(int)frame
{
    editorScene->actionMan->switchFrame(editorScene->currentFrame);
    [self performSelectorInBackground:@selector(hideLoadingActivity) withObject:nil];
}

- (IBAction)myObjectsBtnAction:(id)sender
{
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

- (IBAction)autorigMirrorSwitchAction:(id)sender
{
    if(editorScene && editorScene->isRigMode)
        editorScene->rigMan->switchMirrorState();
}

- (IBAction)sceneMirrorAction:(id)sender
{
    editorScene->switchMirrorState();
}

- (IBAction)optionsBtnAction:(id)sender
{
    
    if((!editorScene->isNodeSelected || (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_UNDEFINED)) && editorScene->selectedNodeIds.size() <= 0) {
        
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);

        CommonProps *commonProps = [[CommonProps alloc] initWithNibName:@"CommonProps" bundle:nil WithProps:editorScene->shaderMGR->sceneProps];
        commonProps.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:commonProps];
        self.popoverController.popoverContentSize = CGSizeMake(464 , 280);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [commonProps.view setClipsToBounds:YES];
        CGRect rect = _optionsBtn.frame;
        rect = [self.view convertRect:rect fromView:_optionsBtn.superview];
        [self.popoverController presentPopoverFromRect:rect
                                                inView:self.view
                              permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight
                                              animated:NO];

        return;
    }
    
    if(editorScene->selectedNodeIds.size() > 0) {
        if(editorScene->allNodesRemovable() || editorScene->allNodesClonable()){
            CGRect rect = _optionsBtn.frame;
            rect = [self.view convertRect:rect fromView:_optionsBtn.superview];
            [self presentPopOver:rect];
        }
    } else if(editorScene->isNodeSelected && (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_LIGHT || editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_ADDITIONAL_LIGHT))
    {
        Quaternion lightProp;
        Vector3 lightColor = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(editorScene->currentFrame, editorScene->nodes[editorScene->selectedNodeId]->scaleKeys);
        float shad = ShaderManager::shadowDensity;
        lightProp = Quaternion(lightColor.x, lightColor.y, lightColor.z, shad);
        float dist = ((editorScene->getSelectedNode()->getProperty(SPECIFIC_FLOAT).value.x/300.0)-0.001);
        
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
        /*
        _lightProp = [[LightProperties alloc] initWithNibName:([Utility IsPadDevice]) ? @"LightProperties"  : @"LightPropertiesPhone" bundle:nil LightColor:lightProp NodeType:editorScene->getSelectedNode()->getType() Distance:dist LightType:editorScene->getSelectedNode()->getProperty(LIGHT_TYPE).value.x];
        _lightProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_lightProp];
        int height = ([Utility IsPadDevice] && editorScene->getSelectedNode()->getType() == NODE_LIGHT) ? 300 : 322;
        if(![Utility IsPadDevice])
            height = (editorScene->getSelectedNode()->getType() == NODE_LIGHT) ? 236 : 241;
        int width = ([Utility IsPadDevice] ? 388 : 320);
        self.popoverController.popoverContentSize = CGSizeMake(width, height);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [_lightProp.view setClipsToBounds:YES];
        CGRect rect = _optionsBtn.frame;
        rect = [self.view convertRect:rect fromView:_optionsBtn.superview];
        [self.popoverController presentPopoverFromRect:rect
                                                inView:self.view
                              permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight
                                              animated:NO];
         */
        
        CommonProps *commonProps = [[CommonProps alloc] initWithNibName:@"CommonProps" bundle:nil WithProps:editorScene->nodes[editorScene->selectedNodeId]->getAllProperties()];
        commonProps.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:commonProps];
        self.popoverController.popoverContentSize = CGSizeMake(464 , 280);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [commonProps.view setClipsToBounds:YES];
        CGRect rect = _optionsBtn.frame;
        rect = [self.view convertRect:rect fromView:_optionsBtn.superview];
        [self.popoverController presentPopoverFromRect:rect
                                                inView:self.view
                              permittedArrowDirections:(status) ? UIPopoverArrowDirectionLeft : UIPopoverArrowDirectionRight
                                              animated:NO];

    }
    else if(editorScene->isNodeSelected && (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_CAMERA))
    {
        /*
        float fovValue = editorScene->cameraFOV;
        NSInteger resolutionType = editorScene->cameraResolutionType;

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
         */
        
        
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);

        CommonProps *commonProps = [[CommonProps alloc] initWithNibName:@"CommonProps" bundle:nil WithProps:editorScene->nodes[editorScene->selectedNodeId]->getAllProperties()];
        commonProps.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:commonProps];
        self.popoverController.popoverContentSize = CGSizeMake(464 , 280);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [commonProps.view setClipsToBounds:YES];
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
        
        BOOL status = ([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT);
//        int state = (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG && editorScene->nodes[editorScene->selectedNodeId]->joints.size() == HUMAN_JOINTS_SIZE) ? editorScene->getMirrorState() : MIRROR_DISABLE;
//        int smoothTex = (editorScene->nodes[editorScene->selectedNodeId]->props.perVertexColor) ? -1 : (int)editorScene->nodes[editorScene->selectedNodeId]->smoothTexture;
//        _meshProp = [[MeshProperties alloc] initWithNibName:(isVideoOrImageOrParticle) ? @"LightAndVideoProperties" : @"MeshProperties" bundle:nil WithProps:editorScene->nodes[editorScene->selectedNodeId] MirrorState:state AndSmoothTexture:smoothTex];
//        _meshProp.delegate = self;
        
        if(editorScene->nodes[editorScene->selectedNodeId]->IsPropertyExists(PHYSICS_KIND))
            printf("\n Physics Type %f ", editorScene->nodes[editorScene->selectedNodeId]->getProperty(PHYSICS_KIND).value.x);
        
        CommonProps *commonProps = [[CommonProps alloc] initWithNibName:@"CommonProps" bundle:nil WithProps:editorScene->nodes[editorScene->selectedNodeId]->getAllProperties()];
        commonProps.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:commonProps];
        self.popoverController.popoverContentSize = (isVideoOrImageOrParticle) ? CGSizeMake(183 , 115) : CGSizeMake(464 , 280);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [commonProps.view setClipsToBounds:YES];
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
        
        if(status){
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
                [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
                [renderViewMan removeNodeFromScene:nodeIndex IsUndoOrRedo:YES];
                [self reloadFrames];
            }
            break;
        }
        case ADD_INSTANCE_BACK: {
            SGAction &recentAction = editorScene->actionMan->actions[editorScene->actionMan->currentAction - 1];
            
            int actionId = recentAction.actionSpecificIntegers[1];
            int nodeIndex = -1;
            for (int i = 0; i < editorScene->nodes.size(); i++) {
                if(actionId == editorScene->nodes[i]->actionId)
                    nodeIndex = i;
            }
            
            if(nodeIndex != NOT_EXISTS) {
                editorScene->loader->createInstance(editorScene->nodes[nodeIndex], editorScene->nodes[nodeIndex]->getType(), UNDO_ACTION);
            }
            break;
        }
        case DELETE_MULTI_ASSET:
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
            if(assetId >= 900 && assetId < 1000) {
                int numberOfLight = assetId - 900;
                [self addLightToScene:[NSString stringWithFormat:@"Light%d",numberOfLight] assetId:assetId];
            }
            else if(assetId >= 10000 && assetId < 20000){
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
            if(toPath.row < [self collectionView:self.framesCollectionView numberOfItemsInSection:0])
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
            [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
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
    else if(returnValue == ACTION_MULTI_NODE_DELETED_BEFORE || returnValue == (int)ADD_MULTI_ASSET_BACK){
        SGAction &recentAction = editorScene->actionMan->actions[editorScene->actionMan->currentAction-1];
        [self redoMultiAssetDeleted:recentAction.objectIndex];
    }
    else if (returnValue == ADD_INSTANCE_BACK) {
        SGAction &recentAction = editorScene->actionMan->actions[editorScene->actionMan->currentAction];
        
        int actionId = recentAction.actionSpecificIntegers[1];
        int nodeIndex = -1;
        for (int i = 0; i < editorScene->nodes.size(); i++) {
            if(actionId == editorScene->nodes[i]->actionId)
                nodeIndex = i;
        }
        
        if(nodeIndex != NOT_EXISTS) {
            editorScene->loader->createInstance(editorScene->nodes[nodeIndex], editorScene->nodes[nodeIndex]->getType(), REDO_ACTION);
        }
        
    }  else {
        if (returnValue != DEACTIVATE_UNDO && returnValue != DEACTIVATE_REDO && returnValue != DEACTIVATE_BOTH) {
            //importPressed = NO;
            assetAddType = REDO_ACTION;
            int assetId = returnValue;
            if(assetId > 900 && assetId < 1000) {
                int numberOfLight = assetId - 900;
                [self addLightToScene:[NSString stringWithFormat:NSLocalizedString(@"Light %d", nil), numberOfLight] assetId:assetId];
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
    if(toPath.row > 0 && (int)toPath.row < editorScene->totalFrames && toPath.row < [self collectionView:self.framesCollectionView numberOfItemsInSection:0])
        [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    [self HighlightFrame];
    [self updateAssetListInScenes];
    [self.framesCollectionView reloadData];
}

- (void) playAnimation
{
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

- (void) playTimerTarget
{
    if (editorScene->currentFrame + 1 < editorScene->totalFrames) {
        [self NormalHighLight];
        editorScene->currentFrame++;
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->currentFrame inSection:0];
        if(toPath.row < [self collectionView:self.framesCollectionView numberOfItemsInSection:0])
            [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
        [self HighlightFrame];
        editorScene->setLightingOff();
        [self performSelectorOnMainThread:@selector(switchFrame) withObject:nil waitUntilDone:NO];
    }
    else if (editorScene->currentFrame + 1 >= editorScene->totalFrames) {
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    }
    else if (editorScene->currentFrame == editorScene->totalFrames) {
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    }
    
}

- (void) switchFrame
{
    if(editorScene)
        editorScene->actionMan->switchFrame((float)editorScene->currentFrame);
}

- (void) stopPlaying
{
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
            if([Utility IsPadDevice]) {
                if(path.row < [self tableView:self.objectList numberOfRowsInSection:0])
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
                if([Utility IsPadDevice]) {
                    if(path.row < [self tableView:self.objectList numberOfRowsInSection:0])
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
        for(int i = 0; i < editorScene->selectedNodeIds.size(); i++){
            NSIndexPath *path = [NSIndexPath indexPathForRow:i inSection:0];
            [self.objectList deselectRowAtIndexPath:path animated:YES];
        }
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

- (void) saveScene
{
    if(editorScene) {
        editorScene->updatePhysics(editorScene->totalFrames-1);
    }
    [self performSelectorOnMainThread:@selector(saveAnimationData) withObject:nil waitUntilDone:YES];
}

- (NSMutableArray*) getFileNamesToAttach:(bool) forBackUp
{
    return [renderViewMan getFileNamesFromScene:forBackUp];
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
    int camResolnType = editorScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x;
    float resWidth = RESOLUTION[camResolnType][0];
    float resHeight = RESOLUTION[camResolnType][1];
    
    return CGPointMake(resWidth, resHeight);
}

- (void) changeLightProps:(Vector4)lightColor Distance:(float)distance LightType:(int)lType isStoredProperty:(BOOL)isStored
{
    
    if(editorScene)
        editorScene->shadowsOff = false;
    
    if(!isStored)
        editorScene->actionMan->changeLightProperty(lightColor.x, lightColor.y, lightColor.z, lightColor.w, distance, lType, true);
    else if(isStored)
        editorScene->actionMan->storeLightPropertyChangeAction(lightColor.x, lightColor.y, lightColor.z, lightColor.w, distance, lType);
    
    if(isStored) {
        [self performSelectorOnMainThread:@selector(updateLightInMainThread:) withObject:[NSNumber numberWithInt:lType] waitUntilDone:NO];
    }
}

- (void) updateLightInMainThread:(NSNumber*) object
{
    editorScene->updateDirectionLine();
    [self setupEnableDisableControls];
}

- (void) setLightDirection
{
    [_popoverController dismissPopoverAnimated:YES];
    editorScene->updateDirectionLine();
    editorScene->controlType = ROTATE;
    editorScene->updater->updateControlsOrientaion();
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
    delete filePathStr;
}

- (void) removeTempAnimation
{
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

- (void)scalePropertyChanged:(float)XValue YValue:(float)YValue ZValue:(float)ZValue
{
    if(!editorScene->isRigMode && (editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size())  && editorScene->selectedNodeIds.size() <= 0)
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
    if((editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size()) && editorScene->selectedNodeIds.size() <= 0)
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

- (void) showOrHideRightView:(BOOL)showView
{
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
    imgSalt = [[info objectForKey:UIImagePickerControllerReferenceURL] absoluteString];
    
    UIImage* uiImage = [info objectForKey:UIImagePickerControllerOriginalImage];
    float imgW = uiImage.size.width;
    float imgH = uiImage.size.height;
    NSData* data = [self convertAndScaleImage:uiImage size:-1];
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cachesDirectory = [paths objectAtIndex:0];
    NSString* fileNameSalt = [Utility getMD5ForString:imgSalt];
    NSString* saveFileName = [NSString stringWithFormat:@"%@/%@.png", cachesDirectory, fileNameSalt];
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

- (void) setShaderTypeForRendering:(int)shaderType
{
    editorScene->updater->resetMaterialTypes((shaderType == 0) ? false : true);
}

- (void) renderFrame:(int)frame withType:(int)shaderType isImage:(bool)isImage andRemoveWatermark:(bool)removeWatermark
{
    editorScene->renHelper->isExportingImages = true;
    editorScene->updatePhysics(frame);
    editorScene->updater->setDataForFrame(frame);
    NSString* tempDir = NSTemporaryDirectory();
    NSString* imageFilePath = [NSString stringWithFormat:@"%@/r-%d.png", tempDir, frame];
    editorScene->renHelper->renderAndSaveImage((char*)[imageFilePath cStringUsingEncoding:NSUTF8StringEncoding], shaderType, false, removeWatermark,(isImage) ? -1 : frame, renderBgColor);
}

- (void) freezeEditorRender:(BOOL) freeze
{
    editorScene->freezeRendering = freeze;
}

- (void) presentPopOver:(CGRect )arect
{
    
    UIAlertController * view=   [UIAlertController
                                 alertControllerWithTitle:nil
                                 message:nil
                                 preferredStyle:UIAlertControllerStyleActionSheet];
    
    
    UIAlertAction* deleteButton = [UIAlertAction
                                   actionWithTitle:NSLocalizedString(@"Delete", nil)
                                   style:UIAlertActionStyleDefault
                                   handler:^(UIAlertAction * action)
                                   {
                                       [self showOrHideProgress:SHOW_PROGRESS];
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
                                       [self showOrHideProgress:HIDE_PROGRESS];
                                   }];
    UIAlertAction* cloneButton = [UIAlertAction
                                  actionWithTitle:NSLocalizedString(@"Clone", nil)
                                  style:UIAlertActionStyleDefault
                                  handler:^(UIAlertAction * action)
                                  {
                                      [self showOrHideProgress:SHOW_PROGRESS];
                                      if(editorScene->selectedNodeIds.size() > 0){
                                          assetAddType = IMPORT_ASSET_ACTION;
                                          [self createDuplicateAssets];
                                          [self undoRedoButtonState:DEACTIVATE_BOTH];
                                      }
                                      
                                      [self updateAssetListInScenes];
                                      [self showOrHideProgress:HIDE_PROGRESS];
                                  }];
    
    [view addAction:deleteButton];
    if(editorScene && editorScene->allNodesClonable())
        [view addAction:cloneButton];
    UIPopoverPresentationController *popover = view.popoverPresentationController;
    popover.sourceRect = arect;
    popover.sourceView=self.renderView;
    popover.permittedArrowDirections = UIPopoverArrowDirectionAny;
    [self presentViewController:view animated:YES completion:nil];
}

-(void)showOptions :(CGRect)longPressposition
{
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
        Quaternion lightProp;
        Vector3 lightColor = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(editorScene->currentFrame, editorScene->nodes[editorScene->selectedNodeId]->scaleKeys);
        float shad = ShaderManager::shadowDensity;
        lightProp = Quaternion(lightColor.x, lightColor.y, lightColor.z, shad);
        float dist = ((editorScene->getSelectedNode()->getProperty(SPECIFIC_FLOAT).value.x/300.0)-0.001);
        

        _lightProp = [[LightProperties alloc] initWithNibName:([Utility IsPadDevice]) ? @"LightProperties"  : @"LightPropertiesPhone" bundle:nil LightColor:lightProp NodeType:editorScene->getSelectedNode()->getType() Distance:dist LightType:editorScene->getSelectedNode()->getProperty(LIGHT_TYPE).value.x];
        _lightProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_lightProp];
        int height = ([Utility IsPadDevice] && editorScene->getSelectedNode()->getType() == NODE_LIGHT) ? 305 : 335;
        if(![Utility IsPadDevice])
            height = (editorScene->getSelectedNode()->getType() == NODE_LIGHT) ? 236 : 241;
        int width = ([Utility IsPadDevice] ? 388 : 320);
        self.popoverController.popoverContentSize = CGSizeMake(width,  height);
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
        
        float fovValue = editorScene->nodes[NODE_CAMERA]->getProperty(FOV).value.x;
        NSInteger resolutionType = editorScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x;
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
        int state = (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG && editorScene->nodes[editorScene->selectedNodeId]->joints.size() == HUMAN_JOINTS_SIZE) ? editorScene->getMirrorState() : MIRROR_DISABLE;
        int smoothTex = (editorScene->nodes[editorScene->selectedNodeId]->getProperty(IS_VERTEX_COLOR).value.x) ? -1 : (int)editorScene->nodes[editorScene->selectedNodeId]->smoothTexture;
        _meshProp = [[MeshProperties alloc] initWithNibName:(isVideoOrImageOrParticle) ? @"LightAndVideoProperties" : @"MeshProperties" bundle:nil WithProps:editorScene->nodes[editorScene->selectedNodeId] MirrorState:state AndSmoothTexture:smoothTex];
        _meshProp.delegate = self;
        self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_meshProp];
        self.popoverController.popoverContentSize =(isVideoOrImageOrParticle) ? CGSizeMake(183 , 115) : CGSizeMake(464 , 280);
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [_meshProp.view setClipsToBounds:YES];
        [self.popoverController presentPopoverFromRect:longPressposition
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionAny
                                              animated:NO];
    }
}

- (void) createDuplicateAssetsForAnimation
{
    
    int selectedAssetId  = NOT_EXISTS;
    int selectedNode = NOT_EXISTS;
    NODE_TYPE selectedNodeType = NODE_UNDEFINED;
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        selectedAssetId = editorScene->nodes[editorScene->selectedNodeId]->assetId;
        selectedNodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
        selectedNode = editorScene->selectedNodeId;
    }
    
    assetAddType = IMPORT_ASSET_ACTION;
    if((selectedNodeType == NODE_RIG || selectedNodeType ==  NODE_SGM || selectedNodeType ==  NODE_OBJ) && selectedAssetId != NOT_EXISTS)
    {
        AssetItem *assetItem = [cache GetAsset:selectedAssetId];
        assetItem.textureName = [NSString stringWithCString:editorScene->nodes[selectedNode]->getProperty(TEXTURE).fileName.c_str()
                                                   encoding:[NSString defaultCStringEncoding]];
        assetItem.isTempAsset = YES;
        [self performSelectorOnMainThread:@selector(loadNode:) withObject:assetItem waitUntilDone:YES];
        editorScene->animMan->copyKeysOfNode(selectedNode, (int)editorScene->nodes.size()-1);
        editorScene->animMan->copyPropsOfNode(selectedNode, (int)editorScene->nodes.size()-1, true);
    }
    else if((selectedNodeType == NODE_TEXT_SKIN || selectedNodeType == NODE_TEXT) && selectedAssetId != NOT_EXISTS){
        NSString *typedText = [self stringWithwstring:editorScene->nodes[editorScene->selectedNodeId]->name];
        NSString *fontName = [NSString stringWithCString:editorScene->nodes[editorScene->selectedNodeId]->optionalFilePath.c_str()
                                                encoding:[NSString defaultCStringEncoding]];
        Vector4 color = editorScene->nodes[editorScene->selectedNodeId]->getProperty(VERTEX_COLOR).value;
        color.w = 0.0;
        float bevalValue = editorScene->nodes[editorScene->selectedNodeId]->getProperty(SPECIFIC_FLOAT).value.x;
        int fontSize = editorScene->nodes[editorScene->selectedNodeId]->getProperty(FONT_SIZE).value.x;
        
        [self load3DTex:(selectedNodeType == NODE_TEXT) ? ASSET_TEXT : ASSET_TEXT_RIG AssetId:0 TextureName:[NSString stringWithCString:editorScene->nodes[selectedNode]->getProperty(TEXTURE).fileName.c_str()
                                                                                                                               encoding:[NSString defaultCStringEncoding]] TypedText:typedText FontSize:fontSize BevelValue:bevalValue TextColor:color FontPath:fontName isTempNode:NO];
    }
}

- (void) createDuplicateAssets
{
    
    int selectedAssetId  = NOT_EXISTS;
    int selectedNode = NOT_EXISTS;
    NODE_TYPE selectedNodeType = NODE_UNDEFINED;
    
    if(editorScene && editorScene->selectedNodeIds.size() > 0) {
        editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_MULTI_NODE_ADDED, 0);
        vector< int > addedNodeIds;
        vector< int > fromNodeIds(editorScene->selectedNodeIds);
        
        for(int i = 0; i < fromNodeIds.size(); i++) {
            selectedAssetId = editorScene->nodes[fromNodeIds[i]]->assetId;
            selectedNodeType = editorScene->nodes[fromNodeIds[i]]->getType();
            selectedNode = fromNodeIds[i];
            [self cloneSelectedAssetWithId:selectedAssetId NodeType:selectedNodeType AndSelNodeId:selectedNode];
            addedNodeIds.push_back(editorScene->nodes.size() - 1);
        }
        
        editorScene->selectMan->unselectObjects();
        for(int i = 0; i < addedNodeIds.size(); i++) {
            editorScene->selectMan->selectObject(addedNodeIds[i], true);
        }
        editorScene->updater->updateControlsOrientaion();
        editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_MULTI_NODE_ADDED, 0);
        
    } else if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        selectedAssetId = editorScene->nodes[editorScene->selectedNodeId]->assetId;
        selectedNodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
        selectedNode = editorScene->selectedNodeId;
        
        [self cloneSelectedAssetWithId:selectedAssetId NodeType:selectedNodeType AndSelNodeId:selectedNode];
        editorScene->selectMan->selectObject(editorScene->nodes.size()-1 , false);
        editorScene->updater->setDataForFrame(editorScene->currentFrame);
    }
}

- (void) cloneSelectedAssetWithId:(int) selectedAssetId NodeType:(int) selectedNodeType AndSelNodeId:(int)selectedNodeIndex
{
    SGNode* sgNode = NULL;
    
    if(selectedNodeIndex != NOT_EXISTS)
        sgNode = editorScene->nodes[selectedNodeIndex];
    
    if (selectedNodeType ==  NODE_SGM || selectedNodeType ==  NODE_OBJ) {
        
        if(sgNode->node->original && sgNode->node->original->instancedNodes.size() >= 8000) {
            UIAlertView * cloneAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Information", nil) message:NSLocalizedString(@"copy_limit_exceed_message", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil];
            [cloneAlert show];
            return;
        }
        editorScene->loader->createInstance(sgNode, sgNode->getType(), assetAddType);
        
        if(assetAddType != UNDO_ACTION && assetAddType != REDO_ACTION)
            editorScene->actionMan->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, sgNode->assetId);
        [self updateAssetListInScenes];
        
        editorScene->animMan->copyPropsOfNode(selectedNodeIndex, (int)editorScene->nodes.size()-1);
        
        
    } else if((selectedNodeType == NODE_RIG || selectedNodeType == NODE_PARTICLES) && selectedAssetId != NOT_EXISTS)
    {
        assetAddType = IMPORT_ASSET_ACTION;
        AssetItem *assetItem = [cache GetAsset:selectedAssetId];
        assetItem.textureName = [NSString stringWithCString:sgNode->getProperty(TEXTURE).fileName.c_str()
                                                   encoding:[NSString defaultCStringEncoding]];
        [self loadCloneNodeWithType:selectedNodeType WithObject:assetItem nodeId:selectedNodeIndex];
        
    }
    else if((selectedNodeType == NODE_TEXT_SKIN || selectedNodeType == NODE_TEXT) && selectedAssetId != NOT_EXISTS){
        
        NSString *typedText = [self stringWithwstring:sgNode->name];
        NSString *fontName = [NSString stringWithCString:sgNode->optionalFilePath.c_str()
                                                encoding:[NSString defaultCStringEncoding]];
        Vector4 color = sgNode->getProperty(VERTEX_COLOR).value;
        color.w = 0.0;
        
        float bevalValue = sgNode->getProperty(SPECIFIC_FLOAT).value.x;
        int fontSize = sgNode->getProperty(FONT_SIZE).value.x;
        [self load3DTex:(selectedNodeType == NODE_TEXT) ? ASSET_TEXT : ASSET_TEXT_RIG AssetId:0 TextureName:[NSString stringWithCString:sgNode->getProperty(TEXTURE).fileName.c_str()
                                                                                                                               encoding:[NSString defaultCStringEncoding]] TypedText:typedText FontSize:fontSize BevelValue:bevalValue TextColor:color FontPath:fontName isTempNode:NO];
        editorScene->animMan->copyPropsOfNode(selectedNodeIndex, (int)editorScene->nodes.size()-1);
        
    } else if ((selectedNodeType == NODE_IMAGE || selectedNodeType == NODE_VIDEO) && sgNode) {
        
        int assetType = (selectedNodeType == NODE_IMAGE) ? ASSET_IMAGE : ASSET_VIDEO;
        NSMutableDictionary *imageDetails = [[NSMutableDictionary alloc] init];
        [imageDetails setObject:[NSNumber numberWithInt:assetType] forKey:@"type"];
        [imageDetails setObject:[NSNumber numberWithInt:0] forKey:@"AssetId"];
        [imageDetails setObject:[NSString stringWithFormat:@"%s", sgNode->getProperty(TEXTURE).fileName.c_str()] forKey:@"AssetName"];
        [imageDetails setObject:[NSNumber numberWithFloat:sgNode->getProperty(VERTEX_COLOR).value.x] forKey:@"Width"];
        [imageDetails setObject:[NSNumber numberWithFloat:sgNode->getProperty(VERTEX_COLOR).value.y] forKey:@"Height"];
        [imageDetails setObject:[NSNumber numberWithBool:NO] forKey:@"isTempNode"];
        
        assetAddType = IMPORT_ASSET_ACTION;
        [self performSelectorOnMainThread:@selector(loadNodeForImage:) withObject:imageDetails waitUntilDone:YES];
        
        editorScene->animMan->copyPropsOfNode(selectedNodeIndex, (int)editorScene->nodes.size()-1);
    }
}

- (void) loadCloneNodeWithType:(int) selectedNodeType WithObject:(id) object nodeId:(int) selectedNode
{
    
    if(selectedNodeType == NODE_RIG || selectedNodeType ==  NODE_SGM || selectedNodeType ==  NODE_OBJ || selectedNodeType == NODE_PARTICLES) {
        [self loadNode:(AssetItem *)object];
    }
    editorScene->animMan->copyPropsOfNode(selectedNode, (int)editorScene->nodes.size()-1);
}

- (void) changeTextureForAsset
{
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
    NSInteger toolBarPos = [[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue];
    objVc.view.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
    objVc.importBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
    objVc.colorWheelBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
    objVc.addBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
    
}

-(void) updateAssetListInScenes
{
    [assetsInScenes removeAllObjects];
    
    if(editorScene && editorScene->nodes.size() == 3 && ![[AppHelper getAppHelper] userDefaultsForKey:@"objectImported"]) {
        [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
        [self setTipArrowDirections];
        [[AppHelper getAppHelper] showTipForView:self.animationBtn InMainView:self.view];
        [[AppHelper getAppHelper] showTipForView:self.optionsBtn InMainView:self.view];
        [[AppHelper getAppHelper] showTipForView:self.rotateBtn InMainView:self.view];
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithBool:YES] withKey:@"objectImported"];
    }
    
    for(int i = 0; i < editorScene->nodes.size(); i++){
        NSString* name = [self stringWithwstring:editorScene->nodes[i]->name];
        if(name == nil)
            name = @"";
        
        if(editorScene->nodes[i]->getType() == NODE_CAMERA) {
            [assetsInScenes addObject:NSLocalizedString(@"CAMERA", nil)];
        } else if(editorScene->nodes[i]->getType() == NODE_LIGHT) {
            [assetsInScenes addObject:NSLocalizedString(@"LIGHT", nil)];
        } else if(editorScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT) {
            [assetsInScenes addObject:name];
        } else if(editorScene->nodes[i]->getType() == NODE_IMAGE) {
            [assetsInScenes addObject:[NSString stringWithFormat:NSLocalizedString(@"Image : %@", nil), name]];
        } else if(editorScene->nodes[i]->getType() == NODE_TEXT_SKIN) {
            [assetsInScenes addObject:[NSString stringWithFormat:NSLocalizedString(@"Text : %@", nil), name]];
        } else if(editorScene->nodes[i]->getType() == NODE_TEXT) {
            [assetsInScenes addObject:[NSString stringWithFormat:NSLocalizedString(@"Text : %@", nil), name]];
        } else if(editorScene->nodes[i]->getType() == NODE_VIDEO) {
            [assetsInScenes addObject:[NSString stringWithFormat:NSLocalizedString(@"Video : %@", nil), name]];
        } else {
            [assetsInScenes addObject:name];
        }
    }
    
    int totalObjs = (editorScene) ? (int)editorScene->nodes.size() : 0;
    self.numberOfItems.text = [NSString stringWithFormat:@"%d", totalObjs];
    [self.objectList reloadData];
}

- (void)objectSelectionCompleted:(int)assetIndex
{
    if(!editorScene)
        return;
    
    bool isMultiSelectenabled= [[AppHelper getAppHelper] userDefaultsBoolForKey:@"multiSelectOption"];
    editorScene->selectMan->selectObject(assetIndex,isMultiSelectenabled);
    [self setupEnableDisableControls];
    
}

- (void) loadAnimationSelectionSliderWithType:(ANIMATION_TYPE) animType
{
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
    NSInteger toolBarPos = [[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue];
    animationsliderVC.view.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
    animationsliderVC.categoryBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
    animationsliderVC.addBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
    
}

- (void) animationBtnDelegateAction:(int)indexValue
{
    if(indexValue==APPLY_ANIMATION) {
        [self.popoverController dismissPopoverAnimated:YES];
        if (editorScene->selectedNodeId <= 1 || (!(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG) && !(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT_SKIN)) || editorScene->isJointSelected) {
            [self.view endEditing:YES];
            UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Information", nil) message:NSLocalizedString(@"apply_animation_error", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
            [closeAlert show];
        }
        else{
            ANIMATION_TYPE animType = (editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT_SKIN) ? TEXT_ANIMATION : RIG_ANIMATION;
            [self loadAnimationSelectionSliderWithType:animType];
        }
    }
    else if(indexValue==SAVE_ANIMATION){
        [self.popoverController dismissPopoverAnimated:YES];
        if (editorScene->selectedNodeId <= 1 || (!(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_RIG) && !(editorScene->nodes[editorScene->selectedNodeId]->getType() == NODE_TEXT_SKIN)) || editorScene->isJointSelected) {
            [self.view endEditing:YES];
            UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Information", nil) message:NSLocalizedString(@"apply_animation_error", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
            [closeAlert show];
        }
        else {
            
            [self.view endEditing:YES];
            UIAlertView* addAnimAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Confirmation", nil) message:NSLocalizedString(@"Save your Animation as template", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"Ok", nil), nil];
            [addAnimAlert setAlertViewStyle:UIAlertViewStylePlainTextInput];
            [[addAnimAlert textFieldAtIndex:0] setPlaceholder:NSLocalizedString(@"Animation Name", nil)];
            [[addAnimAlert textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
            [addAnimAlert setTag:ADD_BUTTON_TAG];
            [addAnimAlert show];
            [[addAnimAlert textFieldAtIndex:0] becomeFirstResponder];
        }
    }
}

- (void) importBtnDelegateAction:(int)indexValue
{
    
    [self addFabricEvent:@"ImportAction" WithAttribute:indexValue];
    switch (indexValue) {
        case IMPORT_PARTICLE:
        case IMPORT_MODELS: {
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
            NSInteger toolBarPos = [[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue];
            assetSelectionSlider.modelCategory.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            assetSelectionSlider.view.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            
            break;
        }   case IMPORT_VIDEO:
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
        case IMPORT_OBJFILE: {
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
            NSInteger toolBarPos = [[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue];
            objVc.view.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            objVc.importBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            objVc.colorWheelBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            objVc.addBtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            break;
        }
        case IMPORT_ADDBONE:{
            [self.popoverController dismissPopoverAnimated:YES];
            NODE_TYPE selectedNodeType = NODE_UNDEFINED;
            if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
                selectedNodeType = editorScene->nodes[editorScene->selectedNodeId]->getType();
                
                if(selectedNodeType != NODE_SGM && selectedNodeType != NODE_TEXT && selectedNodeType != NODE_RIG) {
                    UIAlertView* error = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Alert", nil) message:NSLocalizedString(@"Bones cannot be added to this model.", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
                    [error show];
                } else if(selectedNodeType == NODE_RIG && !editorScene->canEditRigBones(editorScene->nodes[editorScene->selectedNodeId])) {
                    UIAlertView* warning = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Alert", nil) message:NSLocalizedString(@"model_cannot_add_bone", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"NO", nil) otherButtonTitles:NSLocalizedString(@"YES", nil), nil];
                    [warning setTag:SGR_WARNING];
                    [warning show];
                    
                } else {
                    [self performSelectorOnMainThread:@selector(beginRigging) withObject:nil waitUntilDone:YES];
                }
            }
            else
            {
                UIAlertView* error = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Alert", nil) message:NSLocalizedString(@"Please Select any Node to add Bone", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
                [error show];
            }
            break;
        }
        case IMPORT_TEXT:{
            if([Utility IsPadDevice])
            {
                [self.popoverController dismissPopoverAnimated:YES];
                textSelectionSlider = [[TextSelectionSidePanel alloc] initWithNibName:@"TextSelectionSidePanel" bundle:Nil];
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
            NSInteger toolBarPos = [[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue];
            textSelectionSlider.inputText.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            textSelectionSlider.colorWheelbtn.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            textSelectionSlider.view.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            textSelectionSlider.bevelSlider.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            textSelectionSlider.addToScene.accessibilityIdentifier = (toolBarPos == TOOLBAR_LEFT) ? @"1" : @"3";
            textSelectionSlider.dummyRight.accessibilityHint = NSLocalizedString((toolBarPos == TOOLBAR_LEFT) ? @"" : @"Toggle between store fonts and the fonts you have imported.", nil);
            textSelectionSlider.dummyLeft.accessibilityHint = NSLocalizedString((toolBarPos == TOOLBAR_LEFT) ? @"Toggle between store fonts and the fonts you have imported." : @"", nil);
            break;
        }
        default:
            break;
    }
}

- (void) addFabricEvent:(NSString*)eventName WithAttribute:(int)indexValue
{
    NSMutableDictionary * attributes = [[NSMutableDictionary alloc] init];
    
    switch (indexValue) {
        case IMPORT_PARTICLE:
            [attributes setObject:@"Particles" forKey:@"Import"];
            break;
        case IMPORT_MODELS:
            [attributes setObject:@"Models" forKey:@"Import"];
            break;
        case IMPORT_IMAGES:
            [attributes setObject:@"Images" forKey:@"Import"];
            break;
        case IMPORT_VIDEO:
            [attributes setObject:@"Videos" forKey:@"Import"];
            break;
        case IMPORT_TEXT:
            [attributes setObject:@"Text" forKey:@"Import"];
            break;
        case IMPORT_ADDBONE:
            [attributes setObject:@"AddBone" forKey:@"Import"];
            break;
        case IMPORT_OBJFILE:
            [attributes setObject:@"OBJ" forKey:@"Import"];
            break;
        case IMPORT_LIGHT:
            [attributes setObject:@"Light" forKey:@"Import"];
            break;
        case IMAGE_EXPORT:
            [attributes setObject:@"Image" forKey:@"Export"];
            break;
        case VIDEO_EXPORT:
            [attributes setObject:@"Video" forKey:@"Export"];
            break;
        default:
            break;
    }
    [Answers logCustomEventWithName:eventName customAttributes:attributes];
}

- (void) beginRigging
{
    
    [self.autoRigLbl setHidden:NO];
    [self showOrHideProgress:SHOW_PROGRESS];
    [self.addJointBtn setHidden:NO];
    [self.addJointBtn setEnabled:NO];
    [self.moveLast setHidden:NO];
    [self.moveFirst setHidden:NO];
    [self.rigScreenLabel setHidden:NO];
    selectedNodeId = editorScene->riggingNodeId = editorScene->selectedNodeId;
    editorScene->enterOrExitAutoRigMode(true);
    editorScene->rigMan->sgmForRig(editorScene->nodes[selectedNodeId]);
    editorScene->rigMan->switchSceneMode((AUTORIG_SCENE_MODE)(RIG_MODE_OBJVIEW));
    [self setupEnableDisableControls];
    [_rigCancelBtn setHidden:NO];
    [self autoRigMirrorBtnHandler];
    [self moveLastAction:nil];
    [self showOrHideProgress:HIDE_PROGRESS];
    
    if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"AutoRigTip"]) {
        [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
        [[AppHelper getAppHelper] toggleHelp:self Enable:YES];
        [[AppHelper getAppHelper] saveBoolUserDefaults:YES withKey:@"AutoRigTip"];
    }
}

- (void) exportBtnDelegateAction:(int)indexValue
{
    if(editorScene){
        editorScene->selectMan->unselectObjects();
        [self setupEnableDisableControls];
    }
    
    [self addFabricEvent:@"ExportAction" WithAttribute:IMPORT_PARTICLE+indexValue+1];
    cameraResolutionType = editorScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x;

    if(indexValue==EXPORT_IMAGE) {
        renderBgColor = Vector4(0.1,0.1,0.1,1.0);
        if([Utility IsPadDevice]) {
            [self.popoverController dismissPopoverAnimated:YES];
            RenderingViewController* renderingView;
            
            
            renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewController" bundle:nil StartFrame:editorScene->currentFrame EndFrame:editorScene->totalFrames renderOutput:RENDER_IMAGE caMresolution:cameraResolutionType ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
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
                             @"RenderingViewControllerPhone" bundle:nil StartFrame:editorScene->currentFrame EndFrame:editorScene->totalFrames renderOutput:RENDER_IMAGE caMresolution:cameraResolutionType ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
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
            renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewController" bundle:nil StartFrame:0 EndFrame:editorScene->totalFrames renderOutput:RENDER_VIDEO caMresolution:cameraResolutionType ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
        else
            renderingView = [[RenderingViewController alloc]initWithNibName:(ScreenHeight>320) ? @"RenderingViewControllerPhone@2x" :
                             @"RenderingViewControllerPhone" bundle:nil StartFrame:0 EndFrame:editorScene->totalFrames renderOutput:RENDER_VIDEO caMresolution:cameraResolutionType ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
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

- (void) viewBtnDelegateAction:(int)indexValue
{
    CAMERA_VIEW_MODE cameraView = (indexValue == FRONT_VIEW) ? VIEW_FRONT : (indexValue == TOP_VIEW) ? (CAMERA_VIEW_MODE)VIEW_TOP : (indexValue == LEFT_VIEW) ? VIEW_LEFT : (indexValue == BACK_VIEW) ? VIEW_BACK : (indexValue == RIGHT_VIEW) ? VIEW_RIGHT :VIEW_BOTTOM;
    [self.popoverController dismissPopoverAnimated:YES];
    editorScene->updater->changeCameraView(cameraView);
}

- (void) addFrameBtnDelegateAction:(int)indexValue
{
    int addCount = (indexValue == ONE_FRAME) ? 1 : (indexValue == TWENTY_FOUR_FRAMES) ? 24 : 240;
    [self.popoverController dismissPopoverAnimated:YES];
    editorScene->totalFrames += addCount;
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:editorScene->totalFrames-1 inSection:0];
    [self.framesCollectionView reloadData];
    if(toPath.row < [self collectionView:self.framesCollectionView numberOfItemsInSection:0])
        [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
}

- (void) loginBtnAction
{
    [self.popoverController dismissPopoverAnimated:YES];
    loginVc = [[LoginViewController alloc] initWithNibName:([Utility IsPadDevice]) ? @"LoginViewController" : @"LoginViewControllerPhone"bundle:nil];
    [loginVc.view setClipsToBounds:YES];
    loginVc.modalPresentationStyle = UIModalPresentationFormSheet;
    [self presentViewController:loginVc animated:YES completion:nil];
    loginVc.view.superview.backgroundColor = [UIColor clearColor];
}

- (void) infoBtnDelegateAction:(int)indexValue
{
    
    if(indexValue == TUTORIAL){
        [self.popoverController dismissPopoverAnimated:YES];
        [self presentOnBoard];
    }
    else if(indexValue==SETTINGS){
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
        if ([MFMailComposeViewController canSendMail]) {
            MFMailComposeViewController *picker = [[MFMailComposeViewController alloc] init];
            picker.mailComposeDelegate = self;
            NSArray *usersTo = [NSArray arrayWithObject: @"iyan3d@smackall.com"];
            [picker setSubject:[NSString stringWithFormat:@"Feedback on Iyan 3d app (%@ , iOS Version: %.01f)", [self deviceName],iOSVersion]];
            [picker setToRecipients:usersTo];
            [self presentModalViewController:picker animated:YES];
        }else {
            [self.view endEditing:YES];
            UIAlertView *alert=[[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Alert", nil) message:NSLocalizedString(@"No_Email_account_configured", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
            [alert show];
            return;
        }
    } else if (indexValue == RATE_US) {
        [self.popoverController dismissPopoverAnimated:YES];
        NSString *templateReviewURLiOS7 = @"https://itunes.apple.com/app/id640516535?mt=8";
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:templateReviewURLiOS7]];
    } else if (indexValue == FOLLOW_US) {
        [self.popoverController dismissPopoverAnimated:YES];
        if([Utility IsPadDevice]) {
            if(followUsVC == nil)
                followUsVC = [[FollowUsVC alloc] initWithNibName:@"FollowUsVC" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:followUsVC];
            self.popoverController.popoverContentSize = CGSizeMake(425.0 , 325.0);
        } else {
            if(followUsVC == nil)
                followUsVC = [[FollowUsVC alloc] initWithNibName:@"FollowUsVCPhone" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:followUsVC];
            self.popoverController.popoverContentSize = CGSizeMake(312.0 , 213.0);
        }
        
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [followUsVC.view setClipsToBounds:YES];
        self.popUpVc.delegate=self;
        self.popoverController.delegate =self;
        [self.popoverController presentPopoverFromRect:_infoBtn.frame
                                                inView:self.view
                              permittedArrowDirections:UIPopoverArrowDirectionUp
                                              animated:YES];
    }
}

- (void) presentOnBoard
{
    NSString *nibName = @"OnBoardVC";
    if([self iPhone6Plus])
        nibName = @"OnBoardVCPhone@2x";
    else if(![Utility IsPadDevice])
        nibName = @"OnBoardVCPhone";
    
    OnBoardVC* ovc = [[OnBoardVC alloc] initWithNibName:nibName bundle:nil];
    if([Utility IsPadDevice])
        ovc.modalPresentationStyle = UIModalPresentationFormSheet;
    ovc.delegate = self;
    [self presentViewController:ovc animated:YES completion:nil];
}

- (void) closingOnBoard
{
}

- (void)mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error
{
    switch (result)
    {
        case MFMailComposeResultCancelled:
            [self dismissViewControllerAnimated:YES completion:nil];
            break;
        case MFMailComposeResultSaved:
            [self dismissViewControllerAnimated:YES completion:nil];
            break;
        case MFMailComposeResultSent:
            [self dismissViewControllerAnimated:YES completion:nil];
            break;
        case MFMailComposeResultFailed:
            NSLog(@"Mail sent failure: %@", [error localizedDescription]);
            [self dismissViewControllerAnimated:YES completion:nil];
            break;
        default:
            [self dismissViewControllerAnimated:YES completion:nil];
            break;
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
        deleteAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Delete", nil) message:@"" delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"Delete Animation in this Frame", nil), nil];
        [deleteAlert setTag:DELETE_BUTTON_OBJECT_ANIMATION];
    } else {
        [self.view endEditing:YES];
        deleteAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Delete", nil) message:@"" delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"Delete Animation in this Frame", nil), NSLocalizedString(@"Delete Object", nil), nil];
        [deleteAlert setTag:DELETE_BUTTON_OBJECT];
    }
    
    [deleteAlert show];
}

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

- (void) myObjectsBtnDelegateAction:(int)indexValue
{
    [self objectSelectionCompleted:indexValue];
}

- (NSString*) saveThumbnail
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* imageFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, currentScene.sceneFile];
    editorScene->saveThumbnail((char*)[imageFilePath cStringUsingEncoding:NSUTF8StringEncoding]);
    return imageFilePath;
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
    
    [self loadAnimationSelectionSliderWithType:(ANIMATION_TYPE)type];
    animationsliderVC.tableType = 7;
    [animationsliderVC openMyAnimations];
}

bool downloadMissingAssetCallBack(std::string fileName, NODE_TYPE nodeType, bool hasTexture, std::string textureName)
{
    NSLog(@"File Name %s Node Type %d", fileName.c_str(), nodeType);
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [docPaths objectAtIndex:0];
    
    switch (nodeType) {
        case NODE_PARTICLES: {
            NSString* assetIdStr = [NSString stringWithCString:fileName.c_str() encoding:[NSString defaultCStringEncoding]];
            if ([assetIdStr intValue] >= 50000 && ([assetIdStr intValue] < 60000)) {
                NSString* particlePath = [NSString stringWithFormat:@"%@%d.json", cacheDirectory, [assetIdStr intValue]];
                if (![[NSFileManager defaultManager] fileExistsAtPath:particlePath]) {
                    NSString* filePath = [NSString stringWithFormat:@"%@/%d.json", cacheDirectory, [assetIdStr intValue]];
                    NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/particles/%d.json", [assetIdStr intValue]];
                    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath])
                        downloadFile(url, filePath);
                    NSString* meshfilePath = [NSString stringWithFormat:@"%@/%d.sgm", cacheDirectory, [assetIdStr intValue]];
                    NSString* meshurl = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/mesh/%d.sgm", [assetIdStr intValue]];
                    if (![[NSFileManager defaultManager] fileExistsAtPath:meshfilePath])
                        downloadFile(meshurl, meshfilePath);
                    NSString* texfilePath = [NSString stringWithFormat:@"%@/%d-cm.png", cacheDirectory, [assetIdStr intValue]];
                    NSString* texurl = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/meshtexture/%d.png", [assetIdStr intValue]];
                    if (![[NSFileManager defaultManager] fileExistsAtPath:texfilePath])
                        downloadFile(texurl, texfilePath);
                    
                    if (![[NSFileManager defaultManager] fileExistsAtPath:texfilePath]) {
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
        UIAlertView* deleteAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Info", nil) message:NSLocalizedString(@"There is no animation in this frame.", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil];
        [deleteAlert setTag:UNDEFINED_OBJECT];
        [deleteAlert show];
    }
    else
        [self.framesCollectionView reloadData];
}

- (void)cameraPropertyChanged:(float)fov Resolution:(NSInteger)resolution
{
    NSLog(@"Fov value : %f Resolution type: %ld",fov,(long)resolution);
    cameraResolutionType = editorScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x;
    
    if (cameraResolutionType != resolution) {
        
        editorScene->actionMan->changeCameraProperty(fov, (int)resolution, true);
    } //switch action
    else
        editorScene->actionMan->changeCameraProperty(fov, (int)resolution, false); //slider action
}

#pragma mark CommonPropsDelegate

- (void) changedPropertyAtIndex:(PROP_INDEX) index WithValue:(Vector4) value AndStatus:(BOOL) status
{
    
    SGNode* selectedNode= NULL;
    
    if(editorScene->selectedNodeId >= 0 || editorScene->selectedNodeId < editorScene->nodes.size())
        selectedNode = editorScene->nodes[editorScene->selectedNodeId];
    
    switch (index) {
            
        case VISIBILITY:
        case LIGHTING:
        case REFRACTION:
        case REFLECTION: {
            float prevRefraction = selectedNode->getProperty(REFRACTION).value.x;
            float prevReflection = selectedNode->getProperty(REFLECTION).value.x;
            bool prevLighting = selectedNode->getProperty(LIGHTING).value.x;
            bool prevVisibility = selectedNode->getProperty(VISIBILITY).value.x;
            
            editorScene->actionMan->changeMeshProperty((index == REFRACTION) ? value.x : prevRefraction, (index == REFLECTION) ? value.x : prevReflection, (index == LIGHTING) ? value.x : prevLighting, (index == VISIBILITY) ? value.x : prevVisibility, status);
            break;
        }
        case TEXTURE_SMOOTH: {
            if(editorScene && editorScene->selectedNodeId != NOT_SELECTED && editorScene->nodes[editorScene->selectedNodeId]) {
                editorScene->nodes[editorScene->selectedNodeId]->smoothTexture = status;
                editorScene->nodes[editorScene->selectedNodeId]->getProperty(TEXTURE_SMOOTH).value.x = value.x;
                if(smgr->device == OPENGLES2)
                    [self performSelectorOnMainThread:@selector(reloadTexture) withObject:nil waitUntilDone:NO];
            }
            break;
        }
        case TEXTURE: {
            [self changeTextureForAsset];
            break;
        }
        case TEXTURE_SCALE: {
            editorScene->selectMan->unHightlightSelectedNode();
            editorScene->actionMan->changeUVScale(editorScene->selectedNodeId, value.x);
            break;
        }
        case DELETE: {
            if(editorScene->selectedNodeIds.size() > 0){
                editorScene->loader->removeSelectedObjects();
                [self undoRedoButtonState:DEACTIVATE_BOTH];
            }
            else{
                [self deleteObjectOrAnimation];
                [self undoRedoButtonState:DEACTIVATE_BOTH];
            }
            [self updateAssetListInScenes];
            break;
        }
        case CLONE: {
            assetAddType = IMPORT_ASSET_ACTION;
            [self createDuplicateAssets];
            [self updateAssetListInScenes];
            break;
        }
        case VERTEX_COLOR: {
            if(selectedNode->getType() == NODE_LIGHT || selectedNode->getType() == NODE_ADDITIONAL_LIGHT) {
                [self changeLightPropertyAtIndex:index WithValue:value AndStatus:status];
            }
            break;
        }
        
        case SPECIFIC_FLOAT: {
            if(selectedNode->getType() == NODE_LIGHT || selectedNode->getType() == NODE_ADDITIONAL_LIGHT) {
                [self changeLightPropertyAtIndex:index WithValue:value AndStatus:status];
            }
            break;
        }
        case SHADOW_DARKNESS:
        case LIGHT_DIRECTIONAL:
        case LIGHT_POINT: {
            [self changeLightPropertyAtIndex:index WithValue:value AndStatus:status];
            break;
        }
        case FORCE_DIRECTION: {
            [self setDirection];
            break;
        }
        case FOV:
        case CAM_RESOLUTION: {
            
            float cameraFOV = (index == FOV) ? value.x : editorScene->nodes[editorScene->selectedNodeId]->getProperty(FOV).value.x;
            int resolnType = (index == CAM_RESOLUTION) ? value.x : editorScene->nodes[editorScene->selectedNodeId]->getProperty(CAM_RESOLUTION).value.x;
            
            editorScene->actionMan->changeCameraProperty(cameraFOV , resolnType, status);
            break;
        }
        case AMBIENT_LIGHT: {
            editorScene->shaderMGR->getProperty(AMBIENT_LIGHT).value.x = value.x;
            break;
        }
        default:
            break;
    }
}

- (void) changeLightPropertyAtIndex:(PROP_INDEX) pIndex WithValue:(Vector4) value AndStatus:(BOOL) status
{
    if(!editorScene || editorScene->selectedNodeId == NOT_SELECTED)
        return;
    
    SGNode* selectedNode = editorScene->nodes[editorScene->selectedNodeId];
    Vector3 scale = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(editorScene->currentFrame, selectedNode->scaleKeys);
    Vector4 lightProps = (pIndex == VERTEX_COLOR) ? value : Vector4(scale, ShaderManager::shadowDensity);
    float dist = (pIndex == SPECIFIC_FLOAT) ? value.x : selectedNode->getProperty(SPECIFIC_FLOAT).value.x;
    int lightType = (selectedNode->getProperty(pIndex).groupName == "LIGHT TYPE") ? pIndex - 23 : selectedNode->getProperty(LIGHT_TYPE).value.x;
    lightProps.w = (pIndex == SHADOW_DARKNESS) ? value.x : ShaderManager::shadowDensity;
    /* Number 22 because Point light's value is 0 in previous version , now it is 22 */
    if(!status)
        editorScene->actionMan->changeLightProperty(lightProps.x, lightProps.y, lightProps.z, lightProps.w, dist, lightType, true);
    else if(status)
        editorScene->actionMan->storeLightPropertyChangeAction(lightProps.x, lightProps.y, lightProps.z, lightProps.w, dist, lightType);
    
    if(status) {
        [self performSelectorOnMainThread:@selector(updateLightInMainThread:) withObject:[NSNumber numberWithInt:lightType] waitUntilDone:NO];
    }

}

- (void) applyPhysicsProps:(Property) property
{
    if(!editorScene || editorScene->selectedNodeId == NOT_SELECTED)
        return;
    
    SGNode* selectedNode = editorScene->nodes[editorScene->selectedNodeId];
    if(selectedNode->getType() == NODE_SGM || selectedNode->getType() == NODE_OBJ || selectedNode->getType() == NODE_TEXT || selectedNode->getType() == NODE_RIG || selectedNode->getType() == NODE_TEXT_SKIN) {
        std::map<PROP_INDEX, Property> currentPhysics = selectedNode->getProperty(HAS_PHYSICS).subProps;
        std::map<PROP_INDEX, Property> newPhysics = property.subProps;
        
        float prevRefraction = selectedNode->getProperty(REFRACTION).value.x;
        float prevReflection = selectedNode->getProperty(REFLECTION).value.x;
        bool prevLighting = selectedNode->getProperty(LIGHTING).value.x;
        bool prevVisibility = selectedNode->getProperty(VISIBILITY).value.x;
        
        if(currentPhysics[FORCE_MAGNITUDE].value.x != newPhysics[FORCE_MAGNITUDE].value.x) {
            [self velocityChanged:newPhysics[FORCE_MAGNITUDE].value.x];
            [self meshPropertyChanged:prevRefraction Reflection:prevReflection Lighting:prevLighting Visible:prevVisibility storeInAction:YES];
        }
        if(currentPhysics[PHYSICS_KIND].value.x != newPhysics[PHYSICS_KIND].value.x) {
            
            [self meshPropertyChanged:prevRefraction Reflection:prevReflection Lighting:prevLighting Visible:prevVisibility storeInAction:NO];
            int pType = (newPhysics[PHYSICS_KIND].value.x);
            if(pType == PHYSICS_NONE)
                [self setPhysics:false];
            else {
                [self setPhysics:true];
                [self setPhysicsType:pType];
            }
            
            [self meshPropertyChanged:prevRefraction Reflection:prevReflection Lighting:prevLighting Visible:prevVisibility storeInAction:YES];
        }
        
        [self hideLoadingActivity];
        [self setUserInteractionStatus:YES];
    }
}

#pragma mark Meshproperties Delegate

- (void)meshPropertyChanged:(float)refraction Reflection:(float)reflection Lighting:(BOOL)light Visible:(BOOL)visible storeInAction:(BOOL)status
{
    if(editorScene->selectedNodeId < 0 || editorScene->selectedNodeId > editorScene->nodes.size())
        return;
    
    if (editorScene->nodes[editorScene->selectedNodeId]->getProperty(LIGHTING).value.x != light || editorScene->nodes[editorScene->selectedNodeId]->getProperty(VISIBILITY).value.x != visible || status) { //switch action
        editorScene->actionMan->changeMeshProperty(refraction, reflection, light, visible, true);
    }
    else { //slider action
        editorScene->actionMan->changeMeshProperty(refraction, reflection, light, visible, false);
    }
}

- (void) setTextureSmoothStatus:(BOOL) status
{
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED && editorScene->nodes[editorScene->selectedNodeId]) {
        editorScene->nodes[editorScene->selectedNodeId]->smoothTexture = status;
        if(smgr->device == OPENGLES2)
            [self performSelectorOnMainThread:@selector(reloadTexture) withObject:nil waitUntilDone:NO];
    }
}

- (void) reloadTexture
{
    editorScene->changeTexture(editorScene->nodes[editorScene->selectedNodeId]->getProperty(TEXTURE).fileName, Vector3(1.0), false, false);
}

- (void) switchMirror
{
    editorScene->switchMirrorState();
}

- (void) setPhysics:(bool)status
{
    [self performSelectorOnMainThread:@selector(enablePhysics:) withObject:[NSNumber numberWithBool:status] waitUntilDone:NO];
}

- (void) enablePhysics:(NSNumber*)object
{
    bool status = [object boolValue];
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        editorScene->nodes[editorScene->selectedNodeId]->getProperty(HAS_PHYSICS).value = Vector4(status, 0, 0, 0);
        if(!status) {
            [self showLoadingActivity];
            [self syncSceneWithPhysicsWorld];
        }
    }
}

- (void) setPhysicsType:(int)type;
{
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        editorScene->setPropsOfObject(editorScene->nodes[editorScene->selectedNodeId], type);
        [self showLoadingActivity];
        [self syncSceneWithPhysicsWorld];
    }
    
}

- (void) velocityChanged:(double)vel
{
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        editorScene->nodes[editorScene->selectedNodeId]->getProperty(FORCE_MAGNITUDE).value = Vector4(vel, 0, 0, true);
        [self showLoadingActivity];
        [self syncSceneWithPhysicsWorld];
    }
}

- (void) setDirection
{
    [_popoverController dismissPopoverAnimated:YES];
    if(editorScene && editorScene->selectedNodeId != NOT_SELECTED) {
        editorScene->enableDirectionIndicator();
        editorScene->updater->updateControlsOrientaion();
    }
}

- (void) syncSceneWithPhysicsWorld
{
    if(editorScene)
        editorScene->syncSceneWithPhysicsWorld();
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

-(void)cloneDelegateAction
{
    assetAddType = IMPORT_ASSET_ACTION;
    [self createDuplicateAssets];
    [self updateAssetListInScenes];
    [self.popoverController dismissPopoverAnimated:YES];
}

-(void)changeSkinDelgate
{
    [self changeTextureForAsset];
    [self.popoverController dismissPopoverAnimated:YES];
}

- (void) loadSceneSelectionView
{
    
    if([Utility IsPadDevice]) {
        SceneSelectionControllerNew* sceneSelectionView = [[SceneSelectionControllerNew alloc] initWithNibName:@"SceneSelectionControllerNew" bundle:nil IsFirstTimeOpen:NO];
        sceneSelectionView.fromLoadingView = false;
        sceneSelectionView.isAppFirstTime = false;
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:sceneSelectionView];
    } else {
        SceneSelectionControllerNew* sceneSelectionView = [[SceneSelectionControllerNew alloc] initWithNibName:([self iPhone6Plus]) ?  @"SceneSelectionControllerNewPhone@2x" : @"SceneSelectionControllerNewPhone" bundle:nil IsFirstTimeOpen:NO];
        sceneSelectionView.fromLoadingView = false;
        sceneSelectionView.isAppFirstTime = false;
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

-(void)googleSigninDelegate
{
    isLoggedin= true;
}

-(void)dismisspopover
{
    [self.popoverController dismissPopoverAnimated:YES];
}

#pragma mark LoggedinViewController Delegate

-(void)dismissView:(UIViewController*) VC
{
    [VC dismissViewControllerAnimated:YES completion:nil];
    [self.popoverController dismissPopoverAnimated:YES];
    
}

-(void)dismissView:(UIViewController*) VC WithProperty:(Property) physicsProp
{
    [VC dismissViewControllerAnimated:YES completion:nil];
    [self.popoverController dismissPopoverAnimated:YES];
    [self applyPhysicsProps:physicsProp];
}

- (void) showPreview:(NSString*) outputPath
{
    [self performSelectorOnMainThread:@selector(showPreviewInMainThread:) withObject:outputPath waitUntilDone:NO];
}

- (void) showPreviewInMainThread:(NSString*) outputPath
{
    int mediaType = [[outputPath pathExtension] isEqualToString:@"png"] ? 0 : 1;
    
    if([Utility IsPadDevice]) {
        MediaPreviewVC *medPreview = [[MediaPreviewVC alloc] initWithNibName:@"MediaPreviewVC" bundle:nil mediaType:mediaType medPath:outputPath];
        [self dismissView:_loggedInVc];
            medPreview.modalPresentationStyle = UIModalPresentationFullScreen;
            [self presentViewControllerInCurrentView:medPreview];
    } else {
        MediaPreviewVC *medPreview = [[MediaPreviewVC alloc] initWithNibName:[[AppHelper getAppHelper] iPhone6Plus] ? @"MediaPreviewVCPhone@2x" : @"MediaPreviewVCPhone" bundle:nil mediaType:mediaType medPath:outputPath];
        [self dismissView:_loggedInVc];
        medPreview.modalPresentationStyle = UIModalPresentationFullScreen;
        [self presentViewControllerInCurrentView:medPreview];
    }
}

-(void) showOrHideProgress:(BOOL) value
{
    
    if(value == SHOW_PROGRESS){
        [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
    }
    else if(value == HIDE_PROGRESS){
        [self performSelectorInBackground:@selector(hideLoadingActivity) withObject:nil];
    }
}

- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if (alertView.tag == SGR_WARNING) {
        if(buttonIndex == OK_BUTTON_INDEX) {
            [self performSelectorOnMainThread:@selector(beginRigging) withObject:nil waitUntilDone:NO];
        }
    }
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
            [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];
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
                UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"Animation name cannot be empty.", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil];
                [errorAlert show];
            }
            else {
                [self.view endEditing:YES];
                if ([name rangeOfCharacterFromSet:set].location != NSNotFound) {
                    UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"Animation_Name_special", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil];
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
            [self performSelectorOnMainThread:@selector(switchAutoRigSceneMode:) withObject:[NSNumber numberWithInt:1] waitUntilDone:YES];
        }else if(buttonIndex == OWN_RIGGING) {
            editorScene->rigMan->skeletonType = SKELETON_OWN;
            [self performSelectorOnMainThread:@selector(switchAutoRigSceneMode:) withObject:[NSNumber numberWithInt:1] waitUntilDone:YES];
        }
        [self autoRigMirrorBtnHandler];
    } else if (alertView.tag == DATA_LOSS_ALERT) {
        if(buttonIndex == CANCEL_BUTTON_INDEX) {
            
        } else if(buttonIndex == OK_BUTTON_INDEX) {
            [self performSelectorOnMainThread:@selector(switchAutoRigSceneMode:) withObject:[NSNumber numberWithInt:-1] waitUntilDone:YES];
            [self moveLastAction:nil];
        }
    }
    
    if(editorScene && editorScene->isRigMode)
        [self autoRigMirrorBtnHandler];
}

-(void)frameCountDisplayMode
{
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

-(void)cameraPreviewPosition
{
    cameraResolutionType = editorScene->nodes[NODE_CAMERA]->getProperty(CAM_RESOLUTION).value.x;
    
    int selctedIndex = [[[AppHelper getAppHelper] userDefaultsForKey:@"cameraPreviewPosition"]intValue];
    float camPrevRatio = RESOLUTION[cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
    if(selctedIndex==PREVIEW_LEFTBOTTOM){
        camPrevRatio = RESOLUTION[cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
        if(editorScene->camPreviewScale==1.0){
            editorScene->camPreviewOrigin.x=0;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=-editorScene->camPreviewOrigin.x + RESOLUTION[cameraResolutionType][0] / camPrevRatio;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[cameraResolutionType][1] / camPrevRatio;
            
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[cameraResolutionType][1] / camPrevRatio;
            
        }
    }
    if(selctedIndex==PREVIEW_LEFTTOP){
        camPrevRatio = RESOLUTION[cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
        if(editorScene->camPreviewScale==1.0){
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height*editorScene->screenScale;
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=0.0000;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height*editorScene->screenScale;
        }
    }
    
    if(selctedIndex==PREVIEW_RIGHTBOTTOM){
        if(editorScene->camPreviewScale==1.0){
            camPrevRatio = RESOLUTION[cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=self.renderView.frame.size.width*editorScene->screenScale-self.rightView.frame.size.width*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=-editorScene->camPreviewOrigin.x + RESOLUTION[cameraResolutionType][0] / camPrevRatio;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[cameraResolutionType][1] / camPrevRatio;
            
            
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewOrigin.x=self.renderView.frame.size.width*editorScene->screenScale-self.rightView.frame.size.width*editorScene->screenScale;
            editorScene->camPreviewOrigin.y=self.renderView.layer.bounds.size.height*editorScene->screenScale-camPrevRatio;
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[cameraResolutionType][1] / camPrevRatio;
            NSLog(@"Camera preview orgin.x : %f",editorScene->camPreviewOrigin.x);
            
        }
    }
    if(selctedIndex==PREVIEW_RIGHTTOP){
        
        if(editorScene->camPreviewScale==1.0){
            camPrevRatio = RESOLUTION[cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[cameraResolutionType][1] / camPrevRatio;
            editorScene->camPreviewOrigin.x=self.view.frame.size.width*editorScene->screenScale-self.rightView.frame.size.width*editorScene->screenScale;
            editorScene->camPreviewOrigin.y=self.topView.frame.size.height*editorScene->screenScale;
        }
        if(editorScene->camPreviewScale==2.0){
            camPrevRatio = RESOLUTION[cameraResolutionType][1] / ((SceneHelper::screenHeight) * CAM_PREV_PERCENT * editorScene->camPreviewScale);
            editorScene->camPreviewEnd.x=editorScene->camPreviewOrigin.x + RESOLUTION[cameraResolutionType][0] / camPrevRatio;;
            editorScene->camPreviewEnd.y=editorScene->camPreviewOrigin.y + RESOLUTION[cameraResolutionType][1] / camPrevRatio;
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

-(void)multiSelectUpdate:(BOOL)value
{
    if(!value)
        editorScene->selectMan->unselectObjects();
    self.objectList.allowsMultipleSelection=value;
    [[AppHelper getAppHelper] saveBoolUserDefaults:value withKey:@"multiSelectOption"];
    NSLog(@"%d",[[AppHelper getAppHelper] userDefaultsBoolForKey:@"multiSelectOption"]);
}

- (void)setupEnableDisableControlsPlayAnimation
{
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

- (void) autoRigMirrorBtnHandler
{
    bool status = YES;
    if(editorScene && editorScene->isRigMode && (editorScene->rigMan->sceneMode == RIG_MODE_MOVE_JOINTS || editorScene->rigMan->sceneMode == RIG_MODE_EDIT_ENVELOPES)){
        status = (editorScene->rigMan->isNodeSelected && editorScene->rigMan->skeletonType == SKELETON_HUMAN) ? NO : YES;
    }
    [self.autoRigLbl setHidden:(!editorScene->isRigMode)];
    [_autorigMirrorBtnHolder setHidden:status];
    [_autorigMirrorLable setHidden:status];
    [_autoRigMirrorSwitch setHidden:status];
    if(editorScene && editorScene->isRigMode){
        switch (editorScene->rigMan->sceneMode) {
            case RIG_MODE_OBJVIEW:
            case RIG_MODE_MOVE_JOINTS:
                [self.autoRigLbl setText:NSLocalizedString(@"Step_1_rigging_message", nil)];
                _rigScreenLabel.text = NSLocalizedString(@"ATTACH SKELETON", nil);
                break;
            case RIG_MODE_EDIT_ENVELOPES:
                [self.autoRigLbl setText:NSLocalizedString(@"Step_2_rigging_message", nil)];
                _rigScreenLabel.text = NSLocalizedString(@"ADJUST ENVELOP", nil);
                break;
            case RIG_MODE_PREVIEW:{
                [self.autoRigLbl setText:NSLocalizedString(@"Step_3_rigging_message", nil)];
                _rigScreenLabel.text = NSLocalizedString(@"PREVIEW", nil);
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

- (void) importObjWithIndexPath:(int) indexPath TextureName:(NSString*) textureFileName MeshColor:(Vector3) color HasTexture:(BOOL) hasTexture IsTempNode:(BOOL) isTempNode
{
    
    [self performSelectorInBackground:@selector(showLoadingActivity) withObject:nil];

    NSString *displayName, *finalMeshName, *finalTextureName;
    int finalNodeType;
    
    NSString *docDirPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSArray *dirFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirPath error:nil];
    NSArray *filesList = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", [NSArray arrayWithObjects:@"obj", @"fbx", @"dae", @"3ds", nil]]];

    NSArray *basicShapes = [NSArray arrayWithObjects:@"Cone", @"Cube", @"Cylinder", @"Plane", @"Sphere", @"Torus", nil];
    NSArray *basicShapesId = [NSArray arrayWithObjects:@"60001.sgm", @"60002.sgm", @"60003.sgm", @"60004.sgm", @"60005.sgm", @"60006.sgm", nil];
    NSString *sourcePath;

    if(indexPath < 6) {
        
        displayName = [basicShapes objectAtIndex:indexPath];
        finalMeshName = [basicShapesId objectAtIndex:indexPath];
        finalNodeType = (int)NODE_SGM;
        sourcePath = [docDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"Resources/Sgm/%@", finalMeshName]];
        
    } else {
        
        finalMeshName = [filesList objectAtIndex:indexPath - BASIC_SHAPES_COUNT];
        displayName = finalMeshName;
        finalNodeType = (int)NODE_OBJ;
        sourcePath = [docDirPath stringByAppendingPathComponent:finalMeshName];
    }
    
    finalTextureName = @"White-Texture";
    
    if(hasTexture) {
        finalTextureName = textureFileName;
        
        NSString* sourceTexPath = [NSString stringWithFormat:@"%@/%@.png", docDirPath, textureFileName];
        NSString* destTexPath = [NSString stringWithFormat:@"%@/Resources/Textures/%@.png", docDirPath, textureFileName];
        
        NSFileManager* fm = [[NSFileManager alloc] init];
        if([fm fileExistsAtPath:sourceTexPath]) {
            if([fm fileExistsAtPath:destTexPath])
                [fm removeItemAtPath:destTexPath error:nil];

            UIImage *image = [UIImage imageWithContentsOfFile:sourceTexPath];
            NSData *imageData = [self convertAndScaleImage:image size:-1];
            [imageData writeToFile:destTexPath atomically:YES];
            
        } else {
            if(![fm fileExistsAtPath:destTexPath])
                finalTextureName = @"White-Texture";
        }
    }
    
    NSMutableDictionary* dict = [[NSMutableDictionary alloc] init];
    [dict setObject:displayName forKey:@"name"];
    [dict setObject:finalMeshName forKey:@"meshName"];
    [dict setObject:finalTextureName forKey:@"textureName"];
    [dict setObject:[NSNumber numberWithInt:finalNodeType] forKey:@"nodeType"];
    [dict setObject:[NSNumber numberWithBool:isTempNode] forKey:@"isTempNode"];
    [dict setObject:[NSNumber numberWithBool:hasTexture] forKey:@"hasTexture"];
    [dict setObject:[NSNumber numberWithFloat:color.x] forKey:@"x"];
    [dict setObject:[NSNumber numberWithFloat:color.y] forKey:@"y"];
    [dict setObject:[NSNumber numberWithFloat:color.z] forKey:@"z"];
    
    [self performSelectorOnMainThread:@selector(loadObjOrSGM:) withObject:dict waitUntilDone:YES];
    [self performSelectorInBackground:@selector(hideLoadingActivity) withObject:nil];
}

-(int)addSgmFileToCacheDirAndDatabase:(NSString*)fileName
{
    NSArray* directoryPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectoryPath = [directoryPath objectAtIndex:0];
    NSDate *currDate = [NSDate date];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"YY-MM-DD HH:mm:ss"];
    NSString *dateString = [dateFormatter stringFromDate:currDate];
    
    AssetItem* sgmAsset = [[AssetItem alloc] init];
    sgmAsset.assetId = 20000 + [cache getNextObjAssetId];
    sgmAsset.type = NODE_SGM;
    sgmAsset.name = [NSString stringWithFormat:@"%@%d", fileName, [cache getNextObjAssetId]];
    sgmAsset.iap = 0;
    sgmAsset.keywords = [NSString stringWithFormat:@" %@", fileName];
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
    [self storeRigTextureinTexturesDirectory:texturemainFileNameRig assetId:objAsset.assetId];
    assetAddType = IMPORT_ASSET_ACTION;
    objAsset.isTempAsset = NO;
    objAsset.textureName = (vertexColor == -1) ? [NSString stringWithFormat:@"%d%@",objAsset.assetId,@"-cm"] : @"-1";
    [self performSelectorOnMainThread:@selector(loadNode:) withObject:objAsset waitUntilDone:YES];
    editorScene->animMan->copyKeysOfNode(selectedNodeId, editorScene->nodes.size()-1);
    editorScene->animMan->copyPropsOfNode(selectedNodeId, editorScene->nodes.size()-1, true);
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

-(void) storeRigTextureinTexturesDirectory:(NSString*)fileName assetId:(int)localAssetId
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
    
    NSString* desFilePath = [NSString stringWithFormat:@"%@/Resources/Textures/%d-cm.png",docDirPath,localAssetId];
    NSString* desFilePathForDisplay = [NSString stringWithFormat:@"%@/Resources/Textures/%d.png",docDirPath,localAssetId];
    
    UIImage *image =[UIImage imageWithContentsOfFile:srcTextureFilePath];
    NSData *imageData = [self convertAndScaleImage:image size:-1];
    NSData *imageDataforDisplay = [self convertAndScaleImage:image size:128];
    // image File size should be exactly 128 for display.
    [imageData writeToFile:desFilePath atomically:YES];
    [imageDataforDisplay writeToFile:desFilePathForDisplay atomically:YES];
}

- (NSData*) convertAndScaleImage:(UIImage*)image size:(int)textureRes
{
    float nWidth = textureRes, nHeight = textureRes;
    
    if(textureRes == -1) {
        float imgW = image.size.width;
        float imgH = image.size.height;
        
        nWidth = 2;
        while(nWidth < imgW && nWidth <= 1024)
            nWidth *= 2;
        
        nHeight = 2;
        while(nHeight < imgH && nHeight <= 1024)
            nHeight *= 2;
    }
    
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(nWidth, nHeight), NO, 1.0);
    [image drawInRect:CGRectMake(0, 0, nWidth, nHeight)];
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
    NSString * name = [dict objectForKey:@"name"];
    NSString * meshName = [dict objectForKey:@"meshName"];
    NSString * textureName = [dict objectForKey:@"textureName"];
    int nodeType = [[dict objectForKey:@"nodeType"] intValue];
    bool isTempNode = [[dict objectForKey:@"isTempNode"] boolValue];
    bool hasTexture = [[dict objectForKey:@"hasTexture"] boolValue];
    float x = [[dict objectForKey:@"x"] floatValue];
    float y = [[dict objectForKey:@"y"] floatValue];
    float z = [[dict objectForKey:@"z"] floatValue];
    
    Vector3 mColor = Vector3(x, y, z);
    
    string meshPath = "";
    
    if(nodeType == NODE_SGM) {
        meshPath = FileHelper::getDocumentsDirectory() + "Resources/Sgm/" + [meshName UTF8String];
    } else {
        meshPath = FileHelper::getDocumentsDirectory() + [meshName UTF8String];
    }
   
    editorScene->loader->removeTempNodeIfExists();
    
    SceneImporter *loader = new SceneImporter();
    loader->importNodesFromFile(editorScene, [name UTF8String], meshPath, [textureName UTF8String], !hasTexture, mColor, isTempNode);
    if(!isTempNode)
        [self reloadSceneObjects];
}

- (void) legacyLoadObjOrSGM:(NSMutableDictionary*)dict
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    Vector4 color = Vector4([[dict objectForKey:@"x"]floatValue], [[dict objectForKey:@"y"]floatValue], [[dict objectForKey:@"z"]floatValue], 1.0);
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
                if(i != editorScene->nodes.size()-1 && editorScene->nodes[i]->getProperty(VISIBILITY).value.x == true){
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


- (void) scalePropertyChangedInRigView:(float)scaleValue
{
    if((editorScene->rigMan->sceneMode == (AUTORIG_SCENE_MODE)(RIG_MODE_EDIT_ENVELOPES)) &&  editorScene->rigMan->isSkeletonJointSelected){
        editorScene->rigMan->changeEnvelopeScale(Vector3(scaleValue), false);
    }
}

- (void) scaleValueForAction:(float)scaleValue
{
    
}

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
    delete texture;
}

- (void) removeTempTextureAndVertex
{
    editorScene->removeTempTextureAndVertex(selectedNodeId);
}

- (void) changeRenderingBgColor:(Vector4)vertexColor
{
    renderBgColor = vertexColor;
}

- (void)cameraResolutionChanged:(int)changedResolutionType
{
    cameraResolutionType = changedResolutionType;
    [self cameraPropertyChanged:editorScene->nodes[NODE_CAMERA]->getProperty(FOV).value.x Resolution:cameraResolutionType];
}

-(BOOL)iPhone6Plus
{
    if (([UIScreen mainScreen].scale > 2.0))
        return YES;
    
    return NO;
}

-(NODE_TYPE) getNodeType:(int)nodeId
{
    if(nodeId < editorScene->nodes.size())
        return editorScene->nodes[nodeId]->getType();

    return NODE_TYPE(NODE_UNDEFINED);
}

- (void)deallocSubViews
{
    if(animationsliderVC != nil)
        animationsliderVC = nil;

    if(assetSelectionSlider != nil)
        assetSelectionSlider = nil;

    if(textSelectionSlider != nil)
        textSelectionSlider = nil;

    if(objVc != nil)
        objVc = nil;

    if(importImageViewVC != nil)
        importImageViewVC = nil;
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
    followUsVC = nil;
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

