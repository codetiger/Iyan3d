//
//  AutoRigViewController.m
//  Iyan3D
//
//  Created by Karthi on 12/06/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "AutoRigViewController.h"
#import "SGAutoRigScene.h"
#import "AppDelegate.h"
#import "SceneSelectionViewControllerPad.h"
#include "SGEngineCommon.h"
#include "SGEngineOGL.h"
#import <OpenGLES/ES2/glext.h>
#include "SGEngineMTL.h"
#import <simd/simd.h>

#define OBJType 6
#define COMESFROMAUTORIG 7
#define HELP_FROM_AUTORIG 1
#define ADD_OBJ_TO_LIBRARY 1
#define ADD_RIG_MESH_TO_LIBRARY 2
#define SAVE_BUTTON_TAG 1
#define OWN_RIGGING 1
#define HUMAN_RIGGING 2
#define OK_BUTTON_INDEX 1
#define CHOOSE_RIGGING_METHOD 8
#define BONE_COUNT_ALERT 9
#define OBJ_LOAD_ISSUE_ALERT_BUTTON 8
#define CANCEL_BUTTON_INDEX 0
#define NO_TOUCHES 0
#define DATA_LOSS_ALERT 999

@interface AutoRigViewController () {
    MetalHandler *MTLHandler;
    bool checkTapSelection;
    Vector2 tapPosition;
    bool checkCtrlSelection;
    Vector2 touchBeganPosition;
}

@property (strong, nonatomic) EAGLContext *context;
- (BOOL)compileShader:(GLuint *)shader type:(GLenum)type file:(NSString *)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;
@end

SGAutoRigScene *autoRigObject;
SceneManager *_smgrRig;
BOOL nextButtonPressed , isScaling;
@implementation AutoRigViewController

#pragma mark NativeMethods
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cacheRig = [CacheSystem cacheSystem];
        screenScaleRig = [[UIScreen mainScreen] scale];
        isViewLoadSetupRig = isViewDisplayedRig = false;
        constants::BundlePath = constants::BundlePathRig = (char*)[[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSASCIIStringEncoding];
        constants::iOS_Version = iOSVersion;
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* cachesDir = [paths objectAtIndex:0];
        constants::CachesStoragePath = constants::CachesStoragePathRig = (char*)[cachesDir cStringUsingEncoding:NSASCIIStringEncoding];
        checkCtrlSelection = checkTapSelection = isMetalSupported = false;
#if !(TARGET_IPHONE_SIMULATOR)
        if(iOSVersion >= 8.0)
            isMetalSupported = (MTLCreateSystemDefaultDevice() != NULL)?true:false;
#endif
    }
    return self;
}
- (void)viewDidLoad
{
    [super viewDidLoad];
    touchCountTrackerRig = NO_TOUCHES;
    isAppInBGRig = false;
    //[self.loadingforObjView startAnimating];
    [self stateOfViewsDuringLoading];
    [self.nextButton setTag:RIG_MODE_OBJVIEW];
    [self.prevButton setTag:RIG_MODE_OBJVIEW];
    [self.exportRigMesh setTag:ADD_OBJ_TO_LIBRARY];
    self.exportRigMesh.layer.cornerRadius = 5.0;
    self.importObjButton.layer.cornerRadius = 5.0;
    self.displayText.text = [NSString stringWithFormat:@"IMPORT OBJ"];
    self.viewBTScroll.layer.cornerRadius = 0.7f;
    self.moveBTiconView.layer.cornerRadius = 5.7f;
    self.rotateBTiconView.layer.cornerRadius = 5.7f;
    self.scaleBTiconView.layer.cornerRadius = 5.7f;
    self.viewBTScroll.layer.borderWidth = 1.0f;
    self.viewBTScroll.layer.borderColor = [UIColor colorWithRed:67.0f/255.0f green:68.0f/255.0f blue:67.0f/255.0f alpha:1.0f].CGColor;
    [self.moveBTiconView setHidden:YES];
    [self.moveBTView setHidden:YES];
    [self.rotateBTiconView setHidden:YES];
    [self.rotateBTView setHidden:YES];
    [self.scaleBTiconView setHidden:YES];
    [self.scaleBTView setHidden:YES];
    nextButtonPressed = false;
    self.addJointsButton.layer.cornerRadius = 5.0;
    self.addJointsButton.layer.borderWidth = 1.0f;
    self.addJointsButton.layer.borderColor = [UIColor colorWithRed:67.0f/255.0f green:68.0f/255.0f blue:67.0f/255.0f alpha:1.0f].CGColor;
    nextButtonPressed = false;
    
    // Do any additional setup after loading the view from its nib.
}
- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(delayFunction:) name:@"Popovergone" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersBG) name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersFG) name:@"applicationDidBecomeActive" object:nil];
    
    isAppInBGRig = false;
    self.screenName = @"AutoRigView";
    if(!isViewLoadSetupRig){
        isViewLoadSetupRig = true;
        [self initScene];
        [self createDisplayLink];
        [self importObjButtonAction:nil];
    }
    isViewDisplayedRig = true;
    if(autoRigObject->isOBJimported)
    {
        //[self.loadingforObjView stopAnimating];
        [self.loadingforObjView setHidden:YES];
    }
    [self resetViewVisibility];
}
- (void) viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"applicationDidBecomeActive" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"Popovergone" object:nil];

}
- (void) viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    isViewDisplayedRig = false;
}
-(void) stateOfViewsDuringLoading
{
    [self.addJointsButton setHidden:YES];
    [self.backgroundimage setHidden:NO];
    [self.loadingforObjView setHidden:NO];
    [self.exportRigMesh setEnabled:false];
    [self undoRedoButtonState:DEACTIVATE_BOTH];
    [self.nextButton setEnabled:NO];
    [self.prevButton setEnabled:NO];
    
}
- (void)didReceiveMemoryWarning
{
    [self.view endEditing:YES];
    UIAlertView *memoryWarning = [[UIAlertView alloc]initWithTitle:@"Memory Warning" message:@"The App has reached the memory limits. Please save your content." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
    [memoryWarning show];
    [super didReceiveMemoryWarning];    // Dispose of any resources that can be recreated.
}
#pragma mark -
-(void) appEntersBG{
    isAppInBGRig = true;
}
-(void) appEntersFG{
    isAppInBGRig = false;
}
#pragma mark EngineRelated
-(void) initScene
{
    if(isMetalSupported){
        [[AppDelegate getAppDelegate] initEngine:METAL ScreenWidth:SCREENWIDTH ScreenHeight:SCREENHEIGHT ScreenScale:screenScaleRig renderView:_renderViewRig];
        _smgrRig = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        autoRigObject = new SGAutoRigScene(METAL,_smgrRig,SCREENWIDTH * screenScaleRig,SCREENHEIGHT * screenScaleRig);
    }else{
        
        [self setupLayer];
        [self setupContext];
        
        [[AppDelegate getAppDelegate] initEngine:OPENGLES2 ScreenWidth:SCREENWIDTH ScreenHeight:SCREENHEIGHT ScreenScale:screenScaleRig renderView:_renderViewRig];
        _smgrRig = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        autoRigObject = new SGAutoRigScene(OPENGLES2,_smgrRig,SCREENWIDTH * screenScaleRig,SCREENHEIGHT * screenScaleRig);
        [self setupDepthBuffer];
        [self setupRenderBuffer];
        [self setupFrameBuffer];
        _smgrRig->setFrameBufferObjects(_frameBufferRig,_colorRenderBufferRig,_depthRenderBufferRig);
    }
    _smgrRig->ShaderCallBackForNode = &shaderCallBackForNodeRig;
    _smgrRig->isTransparentCallBack = &isTransparentCallBackRig;
    autoRigObject->boneLimitsCallBack = &boneLimitsCallBack;
    autoRigObject->objLoaderCallBack = &objLoaderCallBack;
    autoRigObject->notRespondingCallBack = &notRespondingCallBackAutoRigScene;
    self.mirrorSwitch.on = autoRigObject->getMirrorState();
    [self addGesturesToSceneView];
    [self undoRedoButtonState:DEACTIVATE_BOTH];
}
-(void) addGesturesToSceneView
{
    UIPanGestureRecognizer *panRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGesture:)];
    panRecognizer.delegate = self;
    panRecognizer.minimumNumberOfTouches = 1;
    panRecognizer.maximumNumberOfTouches = 2;
    UITapGestureRecognizer *tapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapGesture:)];
    tapRecognizer.delegate = self;
    if(!isMetalSupported){
        [self.renderViewRig addGestureRecognizer:panRecognizer];
        [self.renderViewRig addGestureRecognizer:tapRecognizer];
    }
    else{
        [self.renderViewRig addGestureRecognizer:panRecognizer];
        [self.renderViewRig addGestureRecognizer:tapRecognizer];
    }
}

-(void) createDisplayLink
{
    displayLinkRig = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateRenderer)];
    [displayLinkRig addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}
#pragma mark -
void notRespondingCallBackAutoRigScene(){
}
void objLoaderCallBack(int status)
{
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    AutoRigViewController *autoRigVC = (AutoRigViewController*)[[appDelegate window] rootViewController];
    if(status == OBJ_CROSSED_MAX_VERTICES_LIMIT){
        [autoRigVC.view endEditing:YES];
        UIAlertView *objLoadMsg = [[UIAlertView alloc]initWithTitle:@"Information" message:@"The obj file is too heavy. Maximum vertices limit is 2.7 Millions." delegate:autoRigVC cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [objLoadMsg show];
    }else if(status == OBJ_NOT_LOADED){
        UIAlertView *objLoadMsg = [[UIAlertView alloc]initWithTitle:@"Information" message:@"Please load any obj file to rig." delegate:autoRigVC cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [objLoadMsg show];
    }else{
        [autoRigVC.view endEditing:YES];
        UIAlertView *objLoadMsg = [[UIAlertView alloc]initWithTitle:@"Information" message:@"Obj file corrupted please try another file." delegate:autoRigVC cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [objLoadMsg show];
    }
}
void boneLimitsCallBack(){
    AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
    AutoRigViewController *autoRigVC = (AutoRigViewController*)[[appDelegate window] rootViewController];
    [autoRigVC boneLimitsAlert];
}
void shaderCallBackForNodeRig(int nodeID,string matName,string callbackFuncName)
{
    if(callbackFuncName.compare("ObjUniforms") == 0){
        autoRigObject->objNodeCallBack(matName);
    }else if(callbackFuncName.compare("jointUniforms") == 0){
        autoRigObject->jointNodeCallBack(nodeID,matName);
    }else if(callbackFuncName.compare("BoneUniforms") == 0){
        autoRigObject->boneNodeCallBack(nodeID,matName);
    }else if(callbackFuncName.compare("setCtrlUniforms") == 0){
        autoRigObject->setControlsUniforms(nodeID,matName);
    }else if(callbackFuncName.compare("envelopeUniforms") == 0){
        autoRigObject->setEnvelopeUniforms(nodeID,matName);
    }else if(callbackFuncName.compare("sgrUniforms") == 0){
        autoRigObject->setSGRUniforms(nodeID,matName);
    }else if(callbackFuncName.compare("setJointSpheresUniforms") == 0){
        autoRigObject->setJointsUniforms(nodeID,matName);
    }else if(callbackFuncName.compare("RotationCircle") == 0)
        autoRigObject->setRotationCircleUniforms(nodeID,matName);
}
bool isTransparentCallBackRig(int nodeId,string callbackFuncName)
{
    if(callbackFuncName.compare("ObjUniforms") == 0)
        return autoRigObject->isOBJTransparent(callbackFuncName);
    else if(callbackFuncName.compare("jointUniforms") == 0)
        return false;
    else if(callbackFuncName.compare("BoneUniforms") == 0)
        return false;
    else if(callbackFuncName.compare("setCtrlUniforms") == 0)
        return autoRigObject->isControlsTransparent(nodeId,callbackFuncName);
    else if(callbackFuncName.compare("envelopeUniforms") == 0)
        return false;
    else if(callbackFuncName.compare("sgrUniforms") == 0)
        return autoRigObject->isSGRTransparent(nodeId,callbackFuncName);
    else if(callbackFuncName.compare("setJointSpheresUniforms") == 0)
        return autoRigObject->isJointTransparent(nodeId,callbackFuncName);
    else if(callbackFuncName.compare("RotationCircle") == 0)
        return false;
    
    
    
    return false;
}
- (void)setupLayer {
    _eaglLayerRig = (CAEAGLLayer*) self.renderViewRig.layer;
    _eaglLayerRig.opaque = YES;
    _eaglLayerRig.contentsScale = screenScaleRig;
}
- (void)setupContext {
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    _context = [[EAGLContext alloc] initWithAPI:api];
    if (!_context)
        Logger::log(INFO,"AtoRigEditor","Failed to initialize OpenGLES 2.0 context");
    if (![EAGLContext setCurrentContext:_context])
        Logger::log(INFO,"AtoRigEditor","Failed to set current OpenGL context");
}
- (void)setupRenderBuffer {
    glGenRenderbuffers(1, &_colorRenderBufferRig);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBufferRig);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayerRig];
}
- (void)setupDepthBuffer {
    glGenRenderbuffers(1, &_depthRenderBufferRig);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBufferRig);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, self.renderViewRig.frame.size.width * screenScaleRig, self.renderViewRig.frame.size.height * screenScaleRig);
}
- (void)setupFrameBuffer {
    glGenFramebuffers(1, &_frameBufferRig);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBufferRig);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBufferRig);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBufferRig);
    
}
-(void) presentRenderBuffer{
    [_context presentRenderbuffer:_colorRenderBufferRig];
}
-(void)updateRenderer
{
    if(!isViewDisplayedRig || isAppInBGRig)
        return;
    @autoreleasepool{
        if(checkCtrlSelection){
            autoRigObject->touchBeganRig(touchBeganPosition);
            checkCtrlSelection = false;
        }else if(checkTapSelection){
            autoRigObject->checkSelection(tapPosition);
            checkTapSelection = false;
        }
        autoRigObject->renderAll();
    }
    if(!isMetalSupported)
        [self presentRenderBuffer];
    
    if(autoRigObject->isRTTCompleted) {
        [self resetButtonVisibility];
        [self undoRedoButtonState:DEACTIVATE_BOTH];
        autoRigObject->isRTTCompleted = false;
    }
}

- (void) resetViewVisibility
{
    //[self changeAllButtonColor];
    if(autoRigObject->isOBJimported)
    {
        //[self.loadingforObjView stopAnimating];
        [self.loadingforObjView setHidden:YES];
        [self.backgroundimage setHidden:YES];
    }
    [self.viewBTScroll setClipsToBounds:YES];
    [self.viewBTView setHidden:YES];
    [self.viewBTiconView setHidden:YES];
    [self.viewBTScroll setHidden:YES];
    [self.viewBT setTag:2];
    [self.rotateButton setTag:2];
    [self.scaleButton setTag:2];
    [self.moveButton setTag:2];
}
-(void)resetButtonVisibility
{
    switch (autoRigObject->sceneMode) {
        case RIG_MODE_OBJVIEW:
            self.hintForOperation.text = [@"Preview the Imported OBJ model Or Save to 'My Library'" uppercaseString];
            [self rigModeOBJView];
            break;
        case RIG_MODE_MOVE_JOINTS:
            if(autoRigObject->skeletonType != SKELETON_OWN)
                self.hintForOperation.text = [@"Place the Joints on the appropriate location of OBJ model" uppercaseString];
            else
                self.hintForOperation.text = [@"Add and Place the Joints on the appropriate location of OBJ model" uppercaseString];
            [self rigModeMoveJoints];
            break;
        case RIG_MODE_EDIT_ENVELOPES:
            self.hintForOperation.text =[@"Select Joint and Scale to Cover Vertices" uppercaseString];
            [self rigModeEditEnvelopes];
            break;
        case RIG_MODE_PREVIEW:
            self.hintForOperation.text = [@"Move and Rotate Joints to verify rigging accuracy" uppercaseString];
            [self rigModePreview];
            break;
        default:
            break;
    }
}
-(void)rigModeOBJView
{
    [self.exportRigMesh setEnabled:true];
    [self.prevButton setEnabled:NO];
    [self.exportRigMesh setTitle: @"ADD OBJ TO MY LIBRARY" forState: UIControlStateNormal];
    [self.exportRigMesh setTag:ADD_OBJ_TO_LIBRARY];
    [self.moveBTiconView setHidden:YES];
    [self.mirrorSwitch setHidden:YES];
    [self.rotateBTiconView setHidden:YES];
    [self.scaleBTiconView setHidden:YES];
    [self.importObjButton setHidden:NO];
    [self.exportRigMesh setHidden:NO];
    [self.mirrorLabel setHidden:YES];
    [self resetViewVisibility];
    self.displayText.text = [NSString stringWithFormat:@"IMPORT OBJ"];
    if(autoRigObject->isOBJimported)
        [self.nextButton setEnabled:YES];
    else
        [self.nextButton setEnabled:NO];
}
-(void)rigModeMoveJoints
{
    [self changeAllButtonColor];
    [self.exportRigMesh setEnabled:false];
    [self.prevButton setEnabled:YES];
    [self.nextButton setEnabled:YES];
    [self.exportRigMesh setHidden:YES];
    [self operationVisible];
    [self scaleButtonVisibility];
    [self resetViewVisibility];
    self.displayText.text = [NSString stringWithFormat:@"ATTACH SKELETON"];
    if(autoRigObject->sceneMode == RIG_MODE_MOVE_JOINTS && autoRigObject->isNodeSelected && autoRigObject->selectedNodeId > 0){
        [self.addJointsButton setHidden:NO];
        if(autoRigObject->skeletonType != SKELETON_OWN) {
            [self.mirrorSwitch setHidden:NO];
            [self.mirrorLabel setHidden:NO];
            self.mirrorSwitch.on = autoRigObject->getMirrorState();
        }
    }
    else{
        autoRigObject->setMirrorState(MIRROR_OFF);
        [self.mirrorSwitch setHidden:YES];
        [self.mirrorLabel setHidden:YES];
        self.mirrorSwitch.on = false;
        [self.addJointsButton setHidden:YES];
    }
}
-(void)rigModeEditEnvelopes
{
    [self changeAllButtonColor];
    [self.addJointsButton setHidden:YES];
    [self.prevButton setEnabled:YES];
    [self.nextButton setEnabled:YES];
    [self.exportRigMesh setHidden:YES];
    [self operationVisible];
    [self scaleButtonVisibility];
    [self.importObjButton setHidden:YES];
    [self resetViewVisibility];
    self.displayText.text = [NSString stringWithFormat:@"ADJUST ENVELOP"];
    if(autoRigObject->selectedNodeId > 0){
        if(autoRigObject->skeletonType != SKELETON_OWN) {
            [self.mirrorSwitch setHidden:NO];
            [self.mirrorLabel setHidden:NO];
            self.mirrorSwitch.on = autoRigObject->getMirrorState();
        }
    }
    else{
        [self.mirrorSwitch setHidden:YES];
        self.mirrorSwitch.on = false;
        [self.mirrorLabel setHidden:YES];
    }
    
}
-(void)rigModePreview
{
    [self changeAllButtonColor];
    [self.exportRigMesh setEnabled:true];
    [self.addJointsButton setHidden:YES];
    [self.prevButton setEnabled:YES];
    [self.exportRigMesh setTitle: @"ADD TO MY LIBRARY" forState: UIControlStateNormal];
    [self.exportRigMesh setTag:ADD_RIG_MESH_TO_LIBRARY];
    [self.nextButton setEnabled:NO];
    [self.exportRigMesh setHidden:NO];
    self.displayText.text = [NSString stringWithFormat:@"PREVIEW"];
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* srcTextureFilePath = [NSString stringWithFormat:@"%@/%@",docDirPath,texturemainFileNameRig];
    //autoRigObject->setSGRMeshTexture([srcTextureFilePath UTF8String]);
    [self operationVisible];
    [self.view setUserInteractionEnabled:YES];
    [self.prevButton setEnabled:YES];
    [self.prevButton setHidden:NO];
    [self.nextButton setHidden:NO];
    //[[UIApplication sharedApplication] endIgnoringInteractionEvents];
    [self scaleButtonVisibility];
    [self resetViewVisibility];
    if(autoRigObject->isJointSelected){
        if(autoRigObject->skeletonType != SKELETON_OWN) {
            [self.mirrorSwitch setHidden:NO];
            [self.mirrorLabel setHidden:NO];
            self.mirrorSwitch.on = autoRigObject->getMirrorState();
        }
    }
    else{
        [self.mirrorSwitch setHidden:YES];
        [self.mirrorLabel setHidden:YES];
        self.mirrorSwitch.on = false;
    }
}
-(void) operationVisible
{
    [self.importObjButton setHidden:YES];
    [self.moveBTiconView setHidden:NO];
    [self.scaleBTiconView setHidden:NO];
    [self.rotateBTiconView setHidden:NO];
}
- (void) returnToMainView {
    if(autoRigObject == NULL)
        return;
    NSString *tempDir = NSTemporaryDirectory();
    [self clearFolder:(NSString*)tempDir];
    isViewDisplayedRig = false;
    isViewLoadSetupRig = false;
    [displayLinkRig invalidate];
    displayLinkRig = nil;
    delete autoRigObject;
    [self removeFromParentViewController];
    if([Utility IsPadDevice]) {
        [self dismissViewControllerAnimated:TRUE completion:Nil];
        SceneSelectionViewControllerPad* sceneSelection = [[SceneSelectionViewControllerPad alloc] initWithNibName:@"SceneSelectionControllerPad" bundle:nil SceneNo:0 isAppFirstTime:false];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:sceneSelection];
    } else {
        [self dismissViewControllerAnimated:TRUE completion:Nil];
        SceneSelectionViewControllerPad* sceneSelection = [[SceneSelectionViewControllerPad alloc]  initWithNibName:@"SceneSelectionControllerPhone" bundle:nil SceneNo:0 isAppFirstTime:false];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:sceneSelection];
    }
}
- (void) chooseARigMode
{
    [self.view endEditing:YES];
    UIAlertView *closeAlert = [[UIAlertView alloc]initWithTitle:@"Select Bone Structure" message:@"You can either start with a complete Human Bone structure or with a single bone?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Single Bone", @"Human Bone Structure", nil];
    [closeAlert setTag:CHOOSE_RIGGING_METHOD];
    [closeAlert show];
}
- (void) boneLimitsAlert
{
    [self.view endEditing:YES];
    UIAlertView *boneLimitMsg = [[UIAlertView alloc]initWithTitle:@"Information" message:@"The maximum bones per object cannot exceed 57." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [boneLimitMsg show];
}
- (void)showPremiumView
{
    if([Utility IsPadDevice]) {
        upgradeView = [[PremiumUpgardeVCViewController alloc] initWithNibName:@"PremiumUpgardeVCViewController" bundle:nil];
        upgradeView.delegate = self;
        upgradeView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:upgradeView animated:YES completion:nil];
        upgradeView.view.superview.backgroundColor = [UIColor clearColor];
        upgradeView.view.layer.borderWidth = 2.0f;
        upgradeView.view.layer.borderColor = [UIColor grayColor].CGColor;
    }else{
        upgradeView = [[PremiumUpgardeVCViewController alloc] initWithNibName:@"PremiumUpgradeViewControllerPhone" bundle:nil];
        upgradeView.delegate = self;
        upgradeView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:upgradeView animated:YES completion:nil];
    }
}

-(void)upgradeButtonPressed
{
    
}

-(void)statusForOBJImport:(NSNumber *)object
{
    
}

- (void) premiumUnlocked
{
    [self exportRigButtonAction:nil];
}

-(void)loadingViewStatus:(BOOL)status
{
    
}
-(void)premiumUnlockedCancelled
{
    
}
-(void)addrigFileToCacheDirAndDatabase
{
    NSString *tempDir = NSTemporaryDirectory();
    NSString *sgrFilePath = [NSString stringWithFormat:@"%@r-%@.sgr", tempDir, @"autorig"];
    
    NSDate *currDate = [NSDate date];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc]init];
    [dateFormatter setDateFormat:@"YY-MM-DD HH:mm:ss"];
    NSString *dateString = [dateFormatter stringFromDate:currDate];
    
    AssetItem* objAsset = [[AssetItem alloc] init];
    
    objAsset.assetId = 40000 + [cacheRig getNextAutoRigAssetId];
    objAsset.type = 1;
    objAsset.name = [NSString stringWithFormat:@"autorig%d",[cacheRig getNextAutoRigAssetId]];
    objAsset.iap = 0;
    objAsset.keywords = [NSString stringWithFormat:@" %@ , %@ , %@",objmainfilenameRig,texturemainFileNameRig,@"autorig"];
    objAsset.boneCount = 0;
    objAsset.hash = [self getMD5ForNonReadableFile:sgrFilePath];
    objAsset.modifiedDate = dateString;
    objAsset.price = @"FREE";
    
    [cacheRig UpdateAsset:objAsset];
    [cacheRig AddDownloadedAsset:objAsset];
    
    [self storeRiginCachesDirectory:objAsset.name assetId:objAsset.assetId];
    [self storeRigTextureinCachesDirectory:texturemainFileNameRig assetId:objAsset.assetId];
}
-(void)addobjFileToCacheDirAndDatabase
{
    NSArray* directoryPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectoryPath = [directoryPath objectAtIndex:0];
    
    
    NSDate *currDate = [NSDate date];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc]init];
    [dateFormatter setDateFormat:@"YY-MM-DD HH:mm:ss"];
    NSString *dateString = [dateFormatter stringFromDate:currDate];
    
    //NSIndexPath *textureIndex = [self.textureFileListTable indexPathForSelectedRow];
    
    AssetItem* objAsset = [[AssetItem alloc] init];
    
    objAsset.assetId = 20000 + [cacheRig getNextObjAssetId];
    //OBJ asset ID starts from 20000 to 30000 for that only we using 20000 here....
    objAsset.type = OBJType;
    objAsset.name = [NSString stringWithFormat:@"OBJFile%d",[cacheRig getNextObjAssetId]];
    objAsset.iap = 0;
    objAsset.keywords = [NSString stringWithFormat:@" %@ , %@ , %@",objmainfilenameRig,texturemainFileNameRig,@"obj"];
    objAsset.boneCount = 0;
    NSString *filePath = [NSString stringWithFormat:@"%@/%@", documentsDirectoryPath, objmainfilenameRig];
    objAsset.hash = [self getMD5ForNonReadableFile:filePath];
    objAsset.modifiedDate = dateString;
    
    [cacheRig UpdateAsset:objAsset];
    [cacheRig AddDownloadedAsset:objAsset];
    [self storeOBJinCachesDirectory:objmainfilenameRig assetId:objAsset.assetId];
    [self storeOBJTextureinCachesDirectory:texturemainFileNameRig assetId:objAsset.assetId];
    assetIdRig = objAsset.assetId;
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
-(void) storeOBJinCachesDirectory:(NSString*)desFilename assetId:(int)localAssetId
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* srcFilePath = [NSString stringWithFormat:@"%@/%@",docDirPath,desFilename];
    
    NSString* desFilePath = [NSString stringWithFormat:@"%@/Resources/Objs/%d.obj",docDirPath,localAssetId];
    NSData *objData = [NSData dataWithContentsOfFile:srcFilePath];
    [objData writeToFile:desFilePath atomically:YES];
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
- (void) clearFolder:(NSString*)dirPath
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError * error;
    NSArray * cacheFiles = [fileManager contentsOfDirectoryAtPath:dirPath error:&error];
    
    for(NSString * file in cacheFiles) {
        error = nil;
        NSString * filePath = [dirPath stringByAppendingPathComponent:file];
        [fileManager removeItemAtPath:filePath error:&error];
    }
}

-(void) storeRigTextureinCachesDirectory:(NSString*)fileName assetId:(int)localAssetId
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* srcTextureFilePath = [NSString stringWithFormat:@"%@/%@",docDirPath,fileName];
    
    NSString* desFilePath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d-cm.png",docDirPath,localAssetId];
    NSString* desFilePathForDisplay = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.png",docDirPath,localAssetId];
    
    UIImage *image =[UIImage imageWithContentsOfFile:srcTextureFilePath];
    NSData *imageData = [self convertAndScaleImage:image size:-1];
    NSData *imageDataforDisplay = [self convertAndScaleImage:image size:128];
    // image File size should be exactly 128 for display.
    [imageData writeToFile:desFilePath atomically:YES];
    [imageDataforDisplay writeToFile:desFilePathForDisplay atomically:YES];
}

-(void) storeOBJTextureinCachesDirectory:(NSString*)fileName assetId:(int)localAssetId
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* srcFilePath = [NSString stringWithFormat:@"%@/%@",docDirPath,fileName];
    
    NSString* desFilePath = [NSString stringWithFormat:@"%@/Resources/Objs/%d-cm.png",docDirPath,localAssetId];
    NSString* desFilePathForDisplay = [NSString stringWithFormat:@"%@/Resources/Objs/%d.png",docDirPath,localAssetId];
    
    UIImage *image =[UIImage imageWithContentsOfFile:srcFilePath];
    NSData *imageData = [self convertAndScaleImage:image size:-1];
    NSData *imageDataforDisplay = [self convertAndScaleImage:image size:128];
    // image File size should be exactly 128 for display.
    [imageData writeToFile:desFilePath atomically:YES];
    [imageDataforDisplay writeToFile:desFilePathForDisplay atomically:YES];
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

- (void)placeView:(UIView*)theView fromValue:(CGPoint)fromPoint toValue:(CGPoint)toPoint active:(BOOL)setValue
{
    CABasicAnimation *hover = [CABasicAnimation animationWithKeyPath:@"position"];
    hover.additive = YES; // fromValue and toValue will be relative instead of absolute values
    hover.fromValue = [NSValue valueWithCGPoint:fromPoint];
    [theView setHidden:setValue];
    hover.toValue = [NSValue valueWithCGPoint:toPoint]; // y increases downwards on iOS
    hover.autoreverses = NO; // Animate back to normal afterwards
    hover.duration = 0.2; // The duration for one part of the animation (0.2 up and 0.2 down)
    hover.repeatCount = 1; // The number of times the animation should repeat
    [theView.layer addAnimation:hover forKey:@"myHoverAnimation"];
}

-(IBAction)delayCallingFunction:(id)sender{
    [self resetViewVisibility];
}
-(IBAction)delayFunction:(id)sender{
    [self changeAllButtonColor];
}
-(void) viewDropDown:(UIView*)dropDownView
{
    [dropDownView setHidden:NO];
    float height = dropDownView.frame.size.height;
    dropDownView.frame = CGRectMake(dropDownView.frame.origin.x ,dropDownView.frame.origin.y, dropDownView.frame.size.width , 0);
    
    [UIView animateWithDuration:1.0 delay:0.01 options:UIViewAnimationCurveEaseIn animations:^{
        
        dropDownView.frame = CGRectMake(dropDownView.frame.origin.x,dropDownView.frame.origin.y , dropDownView.frame.size.width , height);
    } completion:^(BOOL finished){
        
    }];
}

-(void) autoRigAnimationEffect
{
    switch (sliderIdRig) {
        case VARIOUS_VIEW:
            [self viewDropDown:self.viewBTScroll];
            break;
        default:
            break;
    }
}

-(void)viewExpand:(BOOL)isHidden
{
    [self.fullScreenButton setHidden:isHidden];
    int normalButtonPlace =0,scrollViewButton=0,normalmoveButtonPlace=0,scrollmoveViewButton = 0, importObjButtonPlace= 0, importobjFullScreenplace =0 ;
    if([Utility IsPadDevice]){
        normalmoveButtonPlace = 23;
        scrollmoveViewButton = 62;
        normalButtonPlace = 77;
        scrollViewButton = 116;
        importObjButtonPlace = 125;
        importobjFullScreenplace = 72;
    }else{
        normalmoveButtonPlace = 15;
        scrollmoveViewButton = 40;
        normalButtonPlace = 47;
        scrollViewButton = 74;
        importObjButtonPlace = 81;
        importobjFullScreenplace = 48;
    }
    if(isHidden){
        frameBarViewHeight = self.topbarView.frame.size.height;
        [self hiddenview:self.topbarView];
        self.importObjButton.frame = CGRectMake(self.importObjButton.frame.origin.x,importobjFullScreenplace , self.importObjButton.frame.size.width, self.importObjButton.frame.size.height);
        //self.tipsView.frame=CGRectMake(self.tipsView.frame.origin.x, normalmoveButtonPlace, self.tipsView.frame.size.width, self.tipsView.frame.size.height);
        self.viewBTView.frame=CGRectMake(self.viewBTView.frame.origin.x, normalmoveButtonPlace, self.viewBTView.frame.size.width, self.viewBTView.frame.size.height);
        self.addJointsButton.frame=CGRectMake(self.addJointsButton.frame.origin.x, normalmoveButtonPlace, self.addJointsButton.frame.size.width, self.addJointsButton.frame.size.height);
        self.viewBT.frame=CGRectMake(self.viewBT.frame.origin.x, normalmoveButtonPlace, self.viewBT.frame.size.width, self.viewBT.frame.size.height);
        self.viewBTiconView.frame=CGRectMake(self.viewBTiconView.frame.origin.x, normalmoveButtonPlace, self.viewBTiconView.frame.size.width, self.viewBTiconView.frame.size.height);
        self.viewBTScroll.frame=CGRectMake(self.viewBTScroll.frame.origin.x, scrollmoveViewButton, self.viewBTScroll.frame.size.width, self.viewBTScroll.frame.size.height);
        self.mirrorSwitch.frame=CGRectMake(self.mirrorSwitch.frame.origin.x, normalmoveButtonPlace, self.mirrorSwitch.frame.size.width, self.mirrorSwitch.frame.size.height);
        self.mirrorLabel.frame=CGRectMake(self.mirrorLabel.frame.origin.x, normalmoveButtonPlace, self.mirrorLabel.frame.size.width, self.mirrorLabel.frame.size.height);
    }else{
        [self performSelectorOnMainThread:@selector(backtoNormal) withObject:nil waitUntilDone:YES];
        //self.tipsView.frame=CGRectMake(self.tipsView.frame.origin.x,normalButtonPlace, self.tipsView.frame.size.width, self.tipsView.frame.size.height);
        self.importObjButton.frame = CGRectMake(self.importObjButton.frame.origin.x,importObjButtonPlace , self.importObjButton.frame.size.width, self.importObjButton.frame.size.height);
        self.viewBTView.frame=CGRectMake(self.viewBTView.frame.origin.x, normalButtonPlace, self.viewBTView.frame.size.width, self.viewBTView.frame.size.height);
        self.addJointsButton.frame=CGRectMake(self.addJointsButton.frame.origin.x, normalButtonPlace, self.addJointsButton.frame.size.width, self.addJointsButton.frame.size.height);
        self.viewBT.frame=CGRectMake(self.viewBT.frame.origin.x, normalButtonPlace, self.viewBT.frame.size.width, self.viewBT.frame.size.height);
        self.viewBTiconView.frame=CGRectMake(self.viewBTiconView.frame.origin.x, normalButtonPlace, self.viewBTiconView.frame.size.width, self.viewBTiconView.frame.size.height);
        self.viewBTScroll.frame=CGRectMake(self.viewBTScroll.frame.origin.x, scrollViewButton, self.viewBTScroll.frame.size.width, self.viewBTScroll.frame.size.height);
        self.mirrorSwitch.frame=CGRectMake(self.mirrorSwitch.frame.origin.x, normalButtonPlace, self.mirrorSwitch.frame.size.width, self.mirrorSwitch.frame.size.height);
        self.mirrorLabel.frame=CGRectMake(self.mirrorLabel.frame.origin.x, normalButtonPlace, self.mirrorLabel.frame.size.width, self.mirrorLabel.frame.size.height);
    }
}

- (void)backtoNormal
{
    [self backtoPosition:self.topbarView view:frameBarViewHeight];
}

- (void)backtoPosition:(UIView*)dropDownView view:(int)height
{
    dropDownView.frame = CGRectMake(dropDownView.frame.origin.x ,dropDownView.frame.origin.y, dropDownView.frame.size.width , 0);
    
    [UIView animateWithDuration:0.5 delay:0.0 options:UIViewAnimationCurveEaseIn animations:^{
        
        dropDownView.frame = CGRectMake(dropDownView.frame.origin.x,dropDownView.frame.origin.y , dropDownView.frame.size.width , height);
    } completion:^(BOOL finished){
        
    }];
}

- (void)hiddenview:(UIView*)dropDownView
{
    dropDownView.frame = CGRectMake(dropDownView.frame.origin.x ,dropDownView.frame.origin.y, dropDownView.frame.size.width , dropDownView.frame.size.height);
    
    [UIView animateWithDuration:0.5 delay:0.0 options:UIViewAnimationCurveEaseIn animations:^{
        
        dropDownView.frame = CGRectMake(dropDownView.frame.origin.x,dropDownView.frame.origin.y , dropDownView.frame.size.width , 0);
    } completion:^(BOOL finished){
        
    }];
}


#pragma button action

- (IBAction)fullScreenButtonAction:(id)sender
{
    [self viewExpand:YES];
}

- (IBAction)normalScreenButtonAction:(id)sender
{
    [self viewExpand:NO];
}

- (IBAction)prevButtonAction:(id)sender
{
    if(nextButtonPressed)
    {
    } else {
        if([self.prevButton tag]==0)
        {
            if(autoRigObject->isOBJimported == 0)
                [self importObjButtonAction:nil];
        }
        else{
            if((AUTORIG_SCENE_MODE)(autoRigObject->sceneMode-1) == RIG_MODE_OBJVIEW) {
                [self.view endEditing:YES];
                UIAlertView *dataLossAlert = [[UIAlertView alloc]initWithTitle:@"Warning" message:@"Are you sure you want to go back to previous mode? By pressing Yes all your changes will be discarded." delegate:self cancelButtonTitle:@"NO" otherButtonTitles:@"Yes", nil];
                [dataLossAlert setTag:DATA_LOSS_ALERT];
                [dataLossAlert show];
            } else {
                [self moveToPreviousMode];
            }
        }
    }
}
- (IBAction)addJointsButtonAction:(id)sender
{
    autoRigObject->addNewJoint();
    [self undoRedoButtonState:DEACTIVATE_BOTH];
}
- (IBAction)undoButtonAction:(id)sender
{
    autoRigObject->undo();
    
    if(autoRigObject->currentAction <=0) {
        if(autoRigObject->actions.size() > 0)
            [self undoRedoButtonState:DEACTIVATE_UNDO];
        else
            [self undoRedoButtonState:DEACTIVATE_BOTH];
    }
    [self undoRedoButtonState:DEACTIVATE_BOTH];
    [self resetButtonVisibility];
}
- (IBAction)redoButtonAction:(id)sender
{
    autoRigObject->redo();
    
    if(autoRigObject->currentAction == autoRigObject->actions.size()) {
        if(autoRigObject->currentAction <= 0)
            [self undoRedoButtonState:DEACTIVATE_BOTH];
        else
            [self undoRedoButtonState:DEACTIVATE_REDO];
    }
    [self undoRedoButtonState:DEACTIVATE_BOTH];
    [self resetButtonVisibility];
}

- (void) undoRedoButtonState:(int)state
{
    switch (state) {
        case DEACTIVATE_UNDO:
            if([self.undoButton isEnabled])
                [self.undoButton setEnabled:NO];
            if(![self.redoButton isEnabled])
                [self.redoButton setEnabled:YES];
            break;
        case DEACTIVATE_REDO:
            if([self.redoButton isEnabled])
                [self.redoButton setEnabled:NO];
            if(![self.undoButton isEnabled])
                [self.undoButton setEnabled:YES];
            break;
        case DEACTIVATE_BOTH:
            if([self.undoButton isEnabled])
                [self.undoButton setEnabled:NO];
            if([self.redoButton isEnabled])
                [self.redoButton setEnabled:NO];
            break;
        default:
            if(![self.undoButton isEnabled])
                [self.undoButton setEnabled:YES];
            if(![self.redoButton isEnabled])
                [self.redoButton setEnabled:YES];
            break;
    }
    
    if(autoRigObject) {
        if(autoRigObject->actions.size() > 0 && autoRigObject->currentAction > 0)
            [self.undoButton setEnabled:YES];
        if(autoRigObject->actions.size() > 0 && autoRigObject->currentAction < (autoRigObject->actions.size()))
            [self.redoButton setEnabled:YES];
    }
}

- (IBAction)switchChanged:(id)sender
{
    self.mirrorSwitch.on = autoRigObject->switchMirrorState();
}

-(void)moveToPreviousMode
{
    autoRigObject->switchSceneMode((AUTORIG_SCENE_MODE)(autoRigObject->sceneMode-1));
    [self.prevButton setTag:[self.prevButton tag]-1];
    [self.nextButton setTag:[self.nextButton tag]-1];
    [self resetButtonVisibility];
    [self undoRedoButtonState:DEACTIVATE_BOTH];
}

- (void)loadingforObjViewActive
{
    [self.loadingforObjView setHidden:NO];
    [self.view setUserInteractionEnabled:NO];
    [self.prevButton setEnabled:NO];
    [self.prevButton setHidden:YES];
    [self.nextButton setHidden:YES];
}
- (IBAction)nextButtonAction:(id)sender
{
    [self.nextButton setTag:[self.nextButton tag]+1];
    [self.prevButton setTag:[self.prevButton tag]+1];
    
    if(autoRigObject->sceneMode + 1 == RIG_MODE_PREVIEW){
        //[[UIApplication sharedApplication] beginIgnoringInteractionEvents];
        autoRigObject->setLighting(false);
        nextButtonPressed = true;
        [self performSelectorInBackground:@selector(loadingforObjViewActive) withObject:nil];
        NSString *tempDir = NSTemporaryDirectory();
        NSString *sgrFilePath = [NSString stringWithFormat:@"%@r-%@.sgr", tempDir, @"autorig"];
        string path = (char*)[sgrFilePath cStringUsingEncoding:NSUTF8StringEncoding];
        autoRigObject->exportSGR(path);
    }
    
    if(autoRigObject->sceneMode == RIG_MODE_OBJVIEW){
        [self chooseARigMode];
    }else{
        autoRigObject->switchSceneMode((AUTORIG_SCENE_MODE)(autoRigObject->sceneMode+1));
    }
    [self resetButtonVisibility];
    
    [self undoRedoButtonState:DEACTIVATE_BOTH];
    nextButtonPressed = false;
}
- (IBAction)backButtonAction:(id)sender
{
    [self.view endEditing:YES];
    UIAlertView *closeAlert = [[UIAlertView alloc]initWithTitle:@"Exit" message:@"Do you want to discard the current changes and Exit?" delegate:self cancelButtonTitle:@"NO" otherButtonTitles:@"Yes", nil];
    closeAlert.delegate = self;
    [closeAlert setTag:SAVE_BUTTON_TAG];
    [closeAlert show];
}
- (IBAction)helpButtonAction:(id)sender
{
    [self resetViewVisibility];
    if([Utility IsPadDevice]) {
        HelpViewController* morehelpView = [[HelpViewController alloc] initWithNibName:@"HelpViewController" bundle:nil CalledFrom:HELP_FROM_AUTORIG];
        morehelpView.delegate = self;
        morehelpView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:morehelpView animated:YES completion:nil];
        CGRect rect = CGRectInset(morehelpView.view.frame, -80, -0);
        morehelpView.view.superview.backgroundColor = [UIColor clearColor];
        morehelpView.view.frame = rect;
    }
    else{
        HelpViewController* morehelpView = [[HelpViewController alloc] initWithNibName:@"HelpViewControllerPhone" bundle:nil CalledFrom:HELP_FROM_AUTORIG];
        morehelpView.delegate = self;
        morehelpView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:morehelpView animated:YES completion:nil];
    }
}

- (IBAction)exportRigButtonAction:(id)sender
{
    if([self.exportRigMesh tag] == 2)
    {
        [self addrigFileToCacheDirAndDatabase];
        [self.view endEditing:YES];
        UIAlertView *infoAlert = [[UIAlertView alloc]initWithTitle:@"Success" message:@"Rigged Mesh Added to 'My Library'" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [infoAlert show];
    }
    else{
        if(autoRigObject->isOBJimported){
            [self addobjFileToCacheDirAndDatabase];
            [self.view endEditing:YES];
            UIAlertView *infoAlert = [[UIAlertView alloc]initWithTitle:@"Success" message:@"OBJ Mesh Added to the 'My Library'" delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [infoAlert show];
        }
    }
}

- (IBAction)importObjButtonAction:(id)sender
{
    [self.loadingforObjView setHidden:NO];
    //[self.loadingforObjView startAnimating];
    if([Utility IsPadDevice]){
        ObjFileViewController* objfile = [[ObjFileViewController alloc] initWithNibName:@"ObjFileViewController" bundle:nil callerId:COMESFROMAUTORIG objImported:false];
        objfile.delegate = self;
        [self presentViewController:objfile animated:YES completion:nil];
    }else{
        ObjFileViewController* objfile = [[ObjFileViewController alloc] initWithNibName:@"ObjFileViewControllerPhone" bundle:nil callerId:COMESFROMAUTORIG objImported:false];
        objfile.delegate = self;
        [self presentViewController:objfile animated:YES completion:nil];
    }
}
- (IBAction)moveButtonAction:(id)sender
{
    [self resetViewVisibility];
    autoRigObject->controlType = MOVE;
    autoRigObject->updateControlsOrientaion();
    [self changeAllButtonColor];
}
- (IBAction)rotateButtonAction:(id)sender
{
    [self resetViewVisibility];
    autoRigObject->controlType = ROTATE;
    autoRigObject->updateControlsOrientaion();
    [self changeAllButtonColor];
}
- (IBAction)scaleButtonAction:(id)sender
{
    [self changeAllButtonColor];
    [self resetViewVisibility];
    autoRigObject->setControlsVisibility(false);
    self.moveBTiconView.backgroundColor = [UIColor colorWithRed:50.0f/255.0f green:50.0f/255.0f blue:52.0f/255.0f alpha:1.0f];
    self.rotateBTiconView.backgroundColor = [UIColor colorWithRed:50.0f/255.0f green:50.0f/255.0f blue:52.0f/255.0f alpha:1.0f];
    self.scaleBTiconView.backgroundColor = [UIColor blackColor];
    if(autoRigObject->sceneMode == RIG_MODE_EDIT_ENVELOPES && autoRigObject->selectedNodeId > 0) {
        float scale = autoRigObject->getSelectedJointScale();
        
        if (self.scaleProperties == nil) {
            self.scaleProperties = [[ScaleForAutoRigViewController alloc] initWithNibName:@"ScaleForAutoRigViewController" bundle:nil updateScale:scale];
            self.scaleProperties.delegate = self;
        }
        [self.scaleProperties updateScale:scale];
        
        self.scaleProperties.scaleValue.text = [NSString stringWithFormat:@"%0.2f", scale];
        if(self.popOverView && [self.popOverView isBeingPresented])
            [self.popOverView dismissPopoverAnimated:YES];
        
        self.popOverView = [[FPPopoverController alloc] initWithViewController:self.scaleProperties];
        self.popOverView.border = NO;
        self.popOverView.tint = FPPopoverWhiteTint;
        self.popOverView.contentSize = CGSizeMake(390.0, 100.0);
        [self.popOverView presentPopoverFromPoint:self.scaleBTiconView.frame.origin];
    }
    else if(autoRigObject->isNodeSelected){
        Vector3 scales = autoRigObject->getSelectedNodeScale();
        if (self.xyzScaleProperties == nil) {
            self.xyzScaleProperties = [[ScaleViewController alloc] initWithNibName:@"ScaleViewController" bundle:nil updateXValue:scales.x updateYValue:scales.y updateZValue:scales.z];
            self.xyzScaleProperties.delegate = self;
        }
        [self.xyzScaleProperties updateScale:scales.x:scales.y:scales.z];
        if(self.popOverView && [self.popOverView isBeingPresented])
            [self.popOverView dismissPopoverAnimated:YES];
        
        self.popOverView = [[FPPopoverController alloc] initWithViewController:self.xyzScaleProperties];
        self.popOverView.border = NO;
        self.popOverView.tint = FPPopoverWhiteTint;
        self.popOverView.contentSize = CGSizeMake(320.0, 180.0);
        [self.popOverView presentPopoverFromPoint:self.scaleBTiconView.frame.origin];
    }
    [NSTimer scheduledTimerWithTimeInterval:.09 target:self selector:@selector(delayCallingFunction:) userInfo:nil repeats:NO];
}
- (IBAction)viewButtonAction:(id)sender
{
    [autoRigAnimationTimerRig invalidate];
    autoRigAnimationTimerRig = nil;
    if([self.viewBT tag]==2){
        [self resetButtonVisibility];
        [self.viewBTiconView setHidden:NO];
        [self placeView:self.viewBTView fromValue:CGPointMake(95.0, 0.0) toValue:CGPointMake(0.0, 0.0) active:NO];
        [self.viewBT setTag:1];
        timerIRig = 0;
        sliderIdRig=VARIOUS_VIEW;
        [self autoRigAnimationEffect];
    }
    else if ([self.viewBT tag]==1){
        [self placeView:self.viewBTView fromValue:CGPointMake(0.0, -95.0) toValue:CGPointMake(0.0, 0.0) active:YES];
        [self.viewBTiconView setHidden:YES];
        [self.viewBT setTag:2];
        [self.viewBTScroll setHidden:YES];
    }
}
- (IBAction)topviewButtonAction:(id)sender
{
    autoRigObject->changeCameraView(VIEW_TOP);
}
- (IBAction)rightviewButtonAction:(id)sender
{
    autoRigObject->changeCameraView(VIEW_RIGHT);
}
- (IBAction)leftviewButtonAction:(id)sender
{
    autoRigObject->changeCameraView(VIEW_LEFT);
}
- (IBAction)bottomviewButtonAction:(id)sender
{
    autoRigObject->changeCameraView(VIEW_BOTTOM);
}
- (IBAction)frontviewButtonAction:(id)sender
{
    autoRigObject->changeCameraView(VIEW_FRONT);
}
- (IBAction)backviewButtonAction:(id)sender
{
    autoRigObject->changeCameraView(VIEW_BACK);
}
#pragma mark -
#pragma mark Delegates
- (void) ObjFileSelected:(NSString *)srcObjPath string2:(NSString *)srcTexturePath string3:(NSString *)fileName string4:(NSString *)textureFile
{
    [self checkImageForSquareSize:srcTexturePath];
    std::string  objPath = [srcObjPath UTF8String];
    std::string  texturePath = [srcTexturePath UTF8String];
    objmainfilenameRig = fileName;
    texturemainFileNameRig = textureFile;
    autoRigObject->addObjToScene(objPath, texturePath);
    if(autoRigObject->isOBJimported == false)
        [self returnToMainView];
    [self resetButtonVisibility];
}
-(void) checkImageForSquareSize:(NSString*)imagePath
{
    UIImage *textureImage = [UIImage imageWithContentsOfFile:imagePath];
    int width = textureImage.size.width;
    int height = textureImage.size.height;
    
    if (![self isPowerOfTwo:width] || ![self isPowerOfTwo:height]) {
        float bigSide = (width >= height) ? width : height;
        float target = 0;
        
        if(bigSide <= 128)
            target = 128;
        else if(bigSide <= 256)
            target = 256;
        else if(bigSide <= 512)
            target = 512;
        else if(bigSide <= 1024)
            target = 1024;
        else
            target = 2048;
        
        UIGraphicsBeginImageContextWithOptions(CGSizeMake(target, target), NO, 1.0);
        [textureImage drawInRect:CGRectMake(0, 0, target, target)];
        UIImage* nImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        NSData* data = UIImagePNGRepresentation(nImage);
        [data writeToFile:imagePath atomically:YES];
    }
}

-(BOOL) isPowerOfTwo:(int) number
{
    if (number == 0)
        return NO;
    while (number != 1){
        if (number%2 != 0)
            return NO;
        number = number/2;
    }
    return YES;
}

- (void) scaleValueForAction:(float)scaleValue
{
    autoRigObject->changeEnvelopeScale(Vector3(scaleValue),true);
    [self undoRedoButtonState:DEACTIVATE_BOTH];
    [self.loadingforObjView setHidden:YES];
    [self performSelectorOnMainThread:@selector(showOrHideLoadingView:) withObject:[NSNumber numberWithBool:YES] waitUntilDone:YES];
    isScaling = false;
}
-(void) showOrHideLoadingView:(NSNumber*)status
{
    [self.loadingforObjView setHidden:[status boolValue]];
}
- (void) scaleValueForAction:(float)XValue YValue:(float)YValue ZValue:(float)ZValue
{
    autoRigObject->changeNodeScale(Vector3(XValue,YValue,ZValue),true);
    [self undoRedoButtonState:DEACTIVATE_BOTH];
}
- (void) scalePropertyChanged:(float)XValue YValue:(float)YValue ZValue:(float)ZValue
{
    autoRigObject->changeNodeScale(Vector3(XValue,YValue,ZValue),false);
    [self undoRedoButtonState:DEACTIVATE_BOTH];
}
- (void) scalePropertyChangedInRigView:(float)scaleValue
{
    if(!isScaling) {
        [self performSelectorOnMainThread:@selector(showOrHideLoadingView:) withObject:[NSNumber numberWithBool:NO] waitUntilDone:YES];
        isScaling = true;
    }
    autoRigObject->changeEnvelopeScale(Vector3(scaleValue),false);
    [self undoRedoButtonState:DEACTIVATE_BOTH];
}
- (void) CancelSelected:(BOOL)isCancelled
{
    if(autoRigObject->isOBJimported == false)
        [self returnToMainView];
}
#pragma mark -

#pragma alertview delegate
- (void) alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if(alertView.tag == SAVE_BUTTON_TAG) {
        if(buttonIndex == CANCEL_BUTTON_INDEX) {
            
        } else if(buttonIndex == OK_BUTTON_INDEX || buttonIndex == OBJ_LOAD_ISSUE_ALERT_BUTTON) {
            [self returnToMainView];
        }
        alertView.delegate = nil;
    }
    if (alertView.tag == CHOOSE_RIGGING_METHOD) {
        if(buttonIndex == HUMAN_RIGGING) {
            [self rigModeMoveJoints];
            autoRigObject->skeletonType = SKELETON_HUMAN;
            autoRigObject->switchSceneMode((AUTORIG_SCENE_MODE)(autoRigObject->sceneMode+1));
        }else if(buttonIndex == OWN_RIGGING) {
            autoRigObject->skeletonType = SKELETON_OWN;
            autoRigObject->rigBoneCount = 3;
            autoRigObject->switchSceneMode((AUTORIG_SCENE_MODE)(RIG_MODE_MOVE_JOINTS));
        }
        [self resetButtonVisibility];
    }
    if (alertView.tag == DATA_LOSS_ALERT) {
        if(buttonIndex == CANCEL_BUTTON_INDEX) {
            
        } else if(buttonIndex == OK_BUTTON_INDEX || buttonIndex == OBJ_LOAD_ISSUE_ALERT_BUTTON) {
            [self moveToPreviousMode];
        }
    }
}
#pragma mark Gestures
//- (void) pinchGesture:(UIPinchGestureRecognizer *)rec
//{
//    vector<Vector2> p(2);
//    NSUInteger touchCount = (int) rec.numberOfTouches;
//    if(touchCount != 2)
//        return;
//    for(int i=0; i<touchCount ;i++){
//        p[i].X = [rec locationOfTouch:i inView:self.riggingView].x;
//        p[i].Y = [rec locationOfTouch:i inView:self.riggingView].y;
//    }
//
//    switch(rec.state){
//        case UIGestureRecognizerStateBegan:{
//            autoRigObject->panBegan(p[0]*screenScale,p[1]*screenScale);
//            break;
//        }
//
//        case UIGestureRecognizerStateChanged:{
//            autoRigObject->panProgress(p[0]*screenScale,p[1]*screenScale);
//            break;
//        }
//
//        case UIGestureRecognizerStateCancelled:
//        case UIGestureRecognizerStateEnded:{
//            autoRigObject->panEnded();
//            break;
//        }
//        default:
//            break;
//    }
//    autoRigObject->updateControlsOrientaion();
//}

- (BOOL) gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer *)otherGestureRecognizer
{
    isPannedRig = false;
    return TRUE;
}

- (void) panGesture:(UIPanGestureRecognizer *)rec
{
    vector<Vector2> p(2);
    CGPoint velocity = [rec velocityInView:self.renderViewRig];
    if(!isMetalSupported)
        velocity = [rec velocityInView:self.renderViewRig];
    NSUInteger touchCount = rec.numberOfTouches;
    for(int i=0; i<touchCount ;i++){
        if(!isMetalSupported){
            p[i].x = [rec locationOfTouch:i inView:self.renderViewRig].x;
            p[i].y = [rec locationOfTouch:i inView:self.renderViewRig].y;
        }else{
            p[i].x = [rec locationOfTouch:i inView:self.renderViewRig].x;
            p[i].y = [rec locationOfTouch:i inView:self.renderViewRig].y;
        }
    }
    isPannedRig = true;
    switch(rec.state){
        case UIGestureRecognizerStateBegan:{
            touchCountTrackerRig = (int)touchCount;
            switch(touchCount){
                case 1:{
                    checkCtrlSelection = true;
                    touchBeganPosition = p[0] * screenScaleRig;
                    autoRigObject->swipeProgress(-velocity.x/50.0 , -velocity.y/50.0);
                    break;
                }
                case 2:{
                    autoRigObject->panBegan(p[0]*screenScaleRig,p[1]*screenScaleRig);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case UIGestureRecognizerStateChanged:{
            if(touchCount != touchCountTrackerRig){
                touchCountTrackerRig = (int)touchCount;
                autoRigObject->panBegan(p[0]*screenScaleRig,p[1]*screenScaleRig);
            }
            switch(touchCount){
                case 1:{
                    autoRigObject->touchMoveRig(p[0] * screenScaleRig,p[1] * screenScaleRig,SCREENWIDTH * screenScaleRig,SCREENHEIGHT * screenScaleRig);
                    autoRigObject->swipeProgress(-velocity.x/50.0 , -velocity.y/50.0);
                    break;
                }
                case 2:{
                    autoRigObject->panProgress(p[0]*screenScaleRig,p[1]*screenScaleRig);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default: {
            autoRigObject->touchEndRig(p[0] * screenScaleRig);
            break;
        }
    }
    autoRigObject->updateControlsOrientaion();
    [self undoRedoButtonState:DEACTIVATE_BOTH];
}
- (void) tapGesture:(UITapGestureRecognizer *)rec
{
    [self changeAllButtonColor];
    if(!isPannedRig){
        [self resetViewVisibility];
        Vector2 touchPos;
        if(!isMetalSupported){
            touchPos.x = [rec locationInView:self.renderViewRig].x;
            touchPos.y = [rec locationInView:self.renderViewRig].y;
        }else{
            touchPos.x = [rec locationInView:self.renderViewRig].x;
            touchPos.y = [rec locationInView:self.renderViewRig].y;
        }
        checkTapSelection = true;
        tapPosition = touchPos * screenScaleRig;
        [self scaleButtonVisibility];
        [self resetButtonVisibility];
        autoRigObject->isRTTCompleted = true;
        [self undoRedoButtonState:DEACTIVATE_BOTH];
    }
}
#pragma mark -

-(void)changeAllButtonColor
{
    self.moveBTiconView.backgroundColor = [UIColor colorWithRed:50.0f/255.0f green:50.0f/255.0f blue:52.0f/255.0f alpha:1.0f];
    self.rotateBTiconView.backgroundColor = [UIColor colorWithRed:50.0f/255.0f green:50.0f/255.0f blue:52.0f/255.0f alpha:1.0f];
    self.scaleBTiconView.backgroundColor = [UIColor colorWithRed:50.0f/255.0f green:50.0f/255.0f blue:52.0f/255.0f alpha:1.0f];
    if(autoRigObject->controlType == MOVE)
        self.moveBTiconView.backgroundColor = [UIColor blackColor];
    else if(autoRigObject->controlType == ROTATE)
        self.rotateBTiconView.backgroundColor = [UIColor blackColor];
    
}

-(void)scaleButtonVisibility
{
    switch (autoRigObject->sceneMode) {
        case RIG_MODE_MOVE_JOINTS:
            if(autoRigObject->selectedNodeId > 0)
                [self.scaleBTiconView setHidden:true];
            else
                [self.scaleBTiconView setHidden:false];
            [self.moveBTiconView setHidden:false];
            [self.rotateBTiconView setHidden:false];
            if(autoRigObject->controlType == MOVE)
                self.moveBTiconView.backgroundColor = [UIColor blackColor];
            break;
        case RIG_MODE_EDIT_ENVELOPES:
            if(autoRigObject->selectedNodeId > 0)
                [self.scaleBTiconView setHidden:false];
            else
                [self.scaleBTiconView setHidden:true];
            [self.moveBTiconView setHidden:true];
            [self.rotateBTiconView setHidden:true];
            break;
        case RIG_MODE_PREVIEW:
            if(autoRigObject->isNodeSelected){
                if(autoRigObject->isJointSelected)
                    [self.scaleBTiconView setHidden:true];
                else
                    [self.scaleBTiconView setHidden:false];
                [self.moveBTiconView setHidden:false];
                [self.rotateBTiconView setHidden:false];
            }
            else{
                [self.scaleBTiconView setHidden:true];
                [self.moveBTiconView setHidden:true];
                [self.rotateBTiconView setHidden:true];
            }
            break;
        default:
            break;
    }
}
- (void) dealloc
{
    _moveBTiconView = nil;
    _moveBTView = nil;
    _rotateBTiconView = nil;
    _rotateBTView = nil;
    _scaleBTiconView = nil;
    _scaleBTView = nil;
    _scaleProperties = nil;
    _morehelpView = nil;
    _xyzScaleProperties = nil;
    _popOverView = nil;
    autoRigObject = nil;
    _renderViewRig = nil;
    _viewBTiconView = nil;
    _viewBTView = nil;
    screenScaleRig = 0;
    _viewBTScroll = nil;
    cacheRig = nil;
    objmainfilenameRig = nil;
    texturemainFileNameRig = nil;
    [autoRigAnimationTimerRig invalidate];
    autoRigAnimationTimerRig = nil;
}
@end