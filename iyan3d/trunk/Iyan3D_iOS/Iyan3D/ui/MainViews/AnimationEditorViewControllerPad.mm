//
//  AnimationEditorViewControllerPad.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 06/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//
#import "AnimationEditorViewControllerPad.h"
#import "AppDelegate.h"
#import "FrameCellView.h"
#import "SceneSelectionViewControllerPad.h"
#import "HelpViewController.h"
#import "SGAnimationScene.h"
#import "UIImagePickerController+NonRotating.h"

#import <mach/mach.h>

#include "SGEngineCommon.h"
#include "SGEngineOGL.h"
#import <OpenGLES/ES2/glext.h>

#include "SGEngineMTL.h"
#import <simd/simd.h>

#define KEY_FRAME_INDICATOR 4

#define RENDER_IMAGE 0
#define RENDER_VIDEO 1
#define RENDER_GIF 2

#define ASSET_ANIMATION 4
#define ASSET_RIGGED 1
#define ASSET_BACKGROUNDS 2
#define ASSET_ACCESSORIES 3
#define ASSET_OBJ 6
#define ASSET_CAMERA 7
#define ASSET_LIGHT 8
#define ASSET_IMAGE 9
#define ASSET_TEXT_RIG 10
#define ASSET_ADDITIONAL_LIGHT 900

#define OBJ_VERTEX_COUNT 8000
#define DEFAULT_COLOR_VALUE 1.0f
#define DEFAULT_FOV 72.0
#define DEFAULT_FRAME_FLOAT 0.0
#define DEFAULT_LIGHT_SHADOW 0.2f
#define DEFAULT_CAMERA_RESOLUTION 0
#define CAMERA_NODE_INDEX 0
#define LIGHT_NODE_INDEX 1
#define MESH_NODE_INDEX 2
#define HELP_FROM_ANIMATION 2
#define VIEW_BAR_SHOW 1
#define VIEW_BAR_HIDE 0
#define SAVE_BUTTON_TAG 1
#define DELETE_BUTTON_TAG 2
#define PROJECT_BUTTON_TAG 3
#define OK_BUTTON_INDEX 1
#define CANCEL_BUTTON_INDEX 0
#define DELETE_OBJECT_BUTTON 2
#define DELETE_ANIMATION_BUTTON 1
#define DELETE_CONFORM_OBJECT 4
#define DELETE_CONFORM_ANIMATION 5
#define NO_TOUCHES 0

#define ADD_BUTTON_TAG 99

@interface AnimationEditorViewControllerPad () {
    MetalHandler* MTLHandler;
    bool checkTapSelection;
    Vector2 tapPosition;
    bool checkCtrlSelection;
    Vector2 touchBeganPosition;
}

@property (strong, nonatomic) EAGLContext* context;
- (BOOL)compileShader:(GLuint*)shader type:(GLenum)type file:(NSString*)file;
- (BOOL)linkProgram:(GLuint)prog;
- (BOOL)validateProgram:(GLuint)prog;
@end

SceneManager* _smgr;
SGAnimationScene* animationScene;
BOOL missingAlertShown;

@implementation AnimationEditorViewControllerPad

@synthesize popOverView;

Vector2 touchPosition;

#pragma mark NativeMethods

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil SceneItem:(SceneItem*)scene carouselIndex:(int)index
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        currentScene = scene;
        IndexOfCarousel = index;
        self.undoActions = [NSMutableArray array];
        cache = [CacheSystem cacheSystem];
        isViewDisplayed = false;
        screenScale = [[UIScreen mainScreen] scale];
        lightColorRed = lightColorGreen = lightColorBlue = DEFAULT_COLOR_VALUE;
        isViewLoaded = isFileSaving = isPlaying = false;
        frameInFloat = DEFAULT_FRAME_FLOAT;
        cameraFOV = DEFAULT_FOV;
        cameraResolution = DEFAULT_CAMERA_RESOLUTION;
        touchCountTracker = NO_TOUCHES;
        constants::BundlePath = (char*)[[[NSBundle mainBundle] resourcePath] cStringUsingEncoding:NSASCIIStringEncoding];
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* cachesDir = [paths objectAtIndex:0];
        constants::CachesStoragePath = (char*)[cachesDir cStringUsingEncoding:NSASCIIStringEncoding];
        self.assetsNamesInScene = [NSMutableArray array];
        checkTapSelection = checkCtrlSelection = isMetalSupported = missingAlertShown = isTapped = false;
#if !(TARGET_IPHONE_SIMULATOR)
        if (iOSVersion >= 8.0)
            isMetalSupported = (MTLCreateSystemDefaultDevice() != NULL) ? true : false;
#endif
    }
    return self;
}
- (void)viewDidLoad
{
    [super viewDidLoad];
    [self.tipsView setHidden:YES];
    lastTipTime = [NSDate dateWithTimeInterval:-5.1f sinceDate:[NSDate date]];
    [[AppHelper getAppHelper] parseHelpJson];
    [self setTipsSwitchStatus];
    [self.propertiesBTiconView setHidden:YES];
    [self.deleteBTiconView setHidden:YES];
    [self.moveBTiconView setHidden:YES];
    [self.rotateBTiconView setHidden:YES];
    [self.scaleBTiconView setHidden:YES];
    [self.mirrorLabel setHidden:YES];
    [self.mirrorSwitch setHidden:YES];
    self.selectBTiconView.layer.cornerRadius = 7.0f;
    self.propertiesBTiconView.layer.cornerRadius = 7.0f;
    self.moveBTiconView.layer.cornerRadius = 7.0f;
    self.rotateBTiconView.layer.cornerRadius = 7.0f;
    self.scaleBTiconView.layer.cornerRadius = 7.0f;
    self.deleteBTiconView.layer.cornerRadius = 7.0f;
    [[UIApplication sharedApplication] beginIgnoringInteractionEvents];
    totalFrames = 24;
    lightCount = 1;
    if ([Utility IsPadDevice])
        [self.frameCarouselView registerNib:[UINib nibWithNibName:@"FrameCellView" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    else
        [self.frameCarouselView registerNib:[UINib nibWithNibName:@"FrameCellViewPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    
    isAppInBG = isHomePressed = false;
    importPressed = NO;
    propertyViewWidth = self.cameraPropertyView.frame.size.width;
    propertyViewHeight = self.cameraPropertyView.frame.size.height;
    isFileSaved = true;
    [self.redSlider addTarget:self action:@selector(lightSliderChangeEnded) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];
    [self.greenSlider addTarget:self action:@selector(lightSliderChangeEnded) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];
    [self.blueSlider addTarget:self action:@selector(lightSliderChangeEnded) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];
    [self.shadowSlider addTarget:self action:@selector(lightSliderChangeEnded) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];

    [self.brightnessSlider addTarget:self action:@selector(meshSliderEnds) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];
    [self.specularSlider addTarget:self action:@selector(meshSliderEnds) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];

    [self.fovSlider addTarget:self action:@selector(fovSliderEnds) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];
    if ([Utility IsPadDevice])
        self.tipsView.layer.cornerRadius = 8.0;
    else
        self.tipsView.layer.cornerRadius = 5.0;
    self.tipsView.layer.borderWidth = 1.0f;
    self.tipsView.layer.borderColor = [UIColor colorWithRed:67.0f / 255.0f green:68.0f / 255.0f blue:67.0f / 255.0f alpha:1.0f].CGColor;
    self.cameraPropertyView.layer.cornerRadius = 5.0;
    self.cameraPropertyView.layer.borderWidth = 1.0f;
    self.cameraPropertyView.layer.borderColor = [UIColor colorWithRed:67.0f / 255.0f green:68.0f / 255.0f blue:67.0f / 255.0f alpha:1.0f].CGColor;
    self.objectListTableView.layer.cornerRadius = 5.0;
    self.objectListTableView.layer.borderWidth = 1.0f;
    self.objectListTableView.layer.borderColor = [UIColor colorWithRed:67.0f / 255.0f green:68.0f / 255.0f blue:67.0f / 255.0f alpha:1.0f].CGColor;
    self.lightPropertyView.layer.cornerRadius = 5.0;
    self.lightPropertyView.layer.borderWidth = 1.0f;
    self.lightPropertyView.layer.borderColor = [UIColor colorWithRed:67.0f / 255.0f green:68.0f / 255.0f blue:67.0f / 255.0f alpha:1.0f].CGColor;
    self.meshPropertyView.layer.cornerRadius = 5.0;
    self.meshPropertyView.layer.borderWidth = 1.0f;
    self.meshPropertyView.layer.borderColor = [UIColor colorWithRed:67.0f / 255.0f green:68.0f / 255.0f blue:67.0f / 255.0f alpha:1.0f].CGColor;
    self.viewBTScroll.layer.cornerRadius = 5.0;
    self.viewBTScroll.layer.borderWidth = 1.0f;
    self.viewBTScroll.layer.borderColor = [UIColor colorWithRed:67.0f / 255.0f green:68.0f / 255.0f blue:67.0f / 255.0f alpha:1.0f].CGColor;

    [self setupEnableDisableControls];
    [self.loadingActivity setHidden:NO];
    [self.homeLoadingActivity setHidden:YES];
    [self.objectSelectionView setHidden:YES];
    [self.meshPropertyView setHidden:YES];
    [self.lightPropertyView setHidden:YES];
    [self.cameraPropertyView setHidden:YES];
    [self.loadingActivity startAnimating];

    if ([[AppHelper getAppHelper] userDefaultsForKey:@"indicationType"])
        [self.frameButton setTag:[[[AppHelper getAppHelper] userDefaultsForKey:@"indicationType"] longValue]];
    else
        [self.frameButton setTag:2];

    [self.frameButton setTag:(self.frameButton.tag == 1) ? 2 : 1];
    [self frameChangeButtonAction:nil];

    [self undoRedoButtonState:DEACTIVATE_BOTH];
}
- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"Popovergone" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"applicationDidBecomeActive" object:nil];
    isViewDisplayed = false;
}
- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    self.screenName = @"AnimationEditorView";
    isAppInBG = false;
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(delayCallingFunction:) name:@"Popovergone" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersBG) name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersFG) name:@"applicationDidBecomeActive" object:nil];

    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];

    if (displayLink == nil) {
        [self createDisplayLink];
    }

    if (!isViewLoaded) {
        isViewLoaded = true;
        [self initScene];
        animationScene->setDocumentsDirectory([documentsDirectory UTF8String]);
        if (displayLink == nil)
            [self createDisplayLink];
        [self loadScene];
        [[UIApplication sharedApplication] endIgnoringInteractionEvents];
    }

    animationScene->isExportingImages = false;
    isViewDisplayed = true;
    [self setupEnableDisableControls];
    [self.frameCarouselView reloadData];
    [self.brightnessSlider setTag:2];
    [self.specularSlider setTag:3];
    [self.redSlider setTag:4];
    [self.greenSlider setTag:5];
    [self.blueSlider setTag:6];
    [self.shadowSlider setTag:7];
    [self.fovSlider setTag:8];
    [self.loadingActivity stopAnimating];
    [self.loadingActivity setHidden:YES];
    [self.mirrorSwitch setHidden:YES];
    [self.mirrorLabel setHidden:YES];
    self.mirrorSwitch.on = animationScene->getMirrorState();
    [self HighlightFrame];

    if ([[AppHelper getAppHelper] userDefaultsForKey:currentScene.name]) {
        int cameraResolutionType = [[[AppHelper getAppHelper] userDefaultsForKey:currentScene.name] intValue];
        self.resolutionSegment.selectedSegmentIndex = cameraResolutionType;
        animationScene->cameraResolutionType = cameraResolution = (int)self.resolutionSegment.selectedSegmentIndex;
        animationScene->setCameraProperty(animationScene->cameraFOV, animationScene->cameraResolutionType);
    }

    if (animationScene->actions.size() > 0)
        [self.undoButton setEnabled:YES];
}
#pragma mark IOSNotifiers

#pragma mark - ImageImportViewController delegate

- (void)pickedImageWithInfo:(NSDictionary*)info
{
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
    std::wstring saltedFileName = [self getwstring:fileNameSalt];
    std::string fileNameStr = string(saltedFileName.begin() , saltedFileName.end());
    importPressed = true;
    assetAddType = IMPORT_ASSET_ACTION;
    [self loadNodeInScene:ASSET_IMAGE AssetId:0 AssetName:saltedFileName Width:imgW Height:imgH];
    animationScene->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_ADD, 0);
    [self.assetsNamesInScene addObject:[NSString stringWithFormat:@"IMAGE: %s", fileNameStr.c_str()]];
    [self delayCallingFunction:nil];

    isFileSaved = !(animationScene->changeAction = true);

    [self.loadingActivity stopAnimating];
    [self.loadingActivity setHidden:YES];
}

#pragma mark EngineRelated
- (void)initScene
{
    if (isMetalSupported) {
        [[AppDelegate getAppDelegate] initEngine:METAL ScreenWidth:SCREENWIDTH ScreenHeight:SCREENHEIGHT ScreenScale:screenScale renderView:_renderView];
        _smgr = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        animationScene = new SGAnimationScene(METAL, _smgr, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
    }
    else {
        [self setupLayer];
        [self setupContext];

        [[AppDelegate getAppDelegate] initEngine:OPENGLES2 ScreenWidth:SCREENWIDTH ScreenHeight:SCREENHEIGHT ScreenScale:screenScale renderView:_renderView];
        _smgr = (SceneManager*)[[AppDelegate getAppDelegate] getSceneManager];
        animationScene = new SGAnimationScene(OPENGLES2, _smgr, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
        animationScene->screenScale = screenScale;

        [self setupDepthBuffer];
        [self setupRenderBuffer];
        [self setupFrameBuffer];
        _smgr->setFrameBufferObjects(_frameBuffer, _colorRenderBuffer, _depthRenderBuffer);
    }
    _smgr->ShaderCallBackForNode = &shaderCallBackForNode;
    _smgr->isTransparentCallBack = &isTransparentCallBack;
    animationScene->notRespondingCallBack = &notRespondingCallBackAnimationScene;
    animationScene->downloadMissingAssetCallBack = &downloadMissingAssetCallBack;
    animationScene->fileWriteCallBack = &fileWriteCallBack;
    [self addGesturesToSceneView];
}
- (void)createDisplayLink
{
    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateRenderer)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}
- (void)addGesturesToSceneView
{
    UIPanGestureRecognizer* panRecognizer = [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panGesture:)];
    panRecognizer.delegate = self;
    panRecognizer.minimumNumberOfTouches = 1;
    panRecognizer.maximumNumberOfTouches = 2;
    UITapGestureRecognizer* tapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapGesture:)];
    tapRecognizer.delegate = self;
    if (!isMetalSupported) {
        [self.renderView addGestureRecognizer:panRecognizer];
        [self.renderView addGestureRecognizer:tapRecognizer];
    }
    else {
        [self.renderView addGestureRecognizer:panRecognizer];
        [self.renderView addGestureRecognizer:tapRecognizer];
    }
}
- (void)appEntersBG
{
    isAppInBG = true;
}
- (void)appEntersFG
{
    isAppInBG = false;
}
- (IBAction)delayCallingFunction:(id)sender
{
    [self setupEnableDisableControls];
}
- (void)loadScene
{
    [self showTipsViewForAction:SCENE_BEGINS];
    if (![self loadFromFile]) {
        [self addCameraLight];
    }
    animationScene->setDataForFrame(animationScene->currentFrame);
    
    for(int i = 0; i < animationScene->nodes.size(); i++)
    {
        if(animationScene->nodes[i]->getType() == NODE_ADDITIONAL_LIGHT)
            lightCount = (animationScene->nodes[i]->assetId - ASSET_ADDITIONAL_LIGHT) + 1;
    }
    
    self.sceneName.text = [currentScene.name uppercaseString];
}
- (void)showTipsViewForAction:(USER_ACTION)action
{
    NSDate* currentTime = [NSDate date];
    NSTimeInterval interval = [currentTime timeIntervalSinceDate:lastTipTime];

    if ([self advancedHelp] && interval > 5.0) {
        [self hideTipsView];
        [UIView transitionWithView:self.tipsView
                          duration:0.6
                           options:UIViewAnimationOptionTransitionFlipFromTop
                        animations:NULL
                        completion:NULL];
        [self.tipsView setHidden:NO];
        [self.tipsLabel setText:[[AppHelper getAppHelper] getHelpStatementForAction:(action == NO_ACTION) ? previousAction : action]];

        if (action != NO_ACTION)
            previousAction = action;
    }
}

- (void)hideTipsView
{
    [UIView transitionWithView:self.tipsView
                      duration:0.6
                       options:UIViewAnimationOptionTransitionFlipFromBottom
                    animations:NULL
                    completion:NULL];
    [self.tipsView setHidden:YES];
    lastTipTime = [NSDate date];
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
        if (fileSize == 0 || !animationScene->loadSceneData(SGBFilePath)) {
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
    totalFrames = animationScene->totalFrames;
    [self.frameCarouselView reloadData];
    NSUInteger assetsCount = animationScene->nodes.size();
    for (int i = 0; i < assetsCount; i++) {
        NSString* assetName = [self stringWithwstring:animationScene->nodes[i]->name];// [NSString stringWithCString:animationScene->nodes[i]->name.c_str() encoding:[NSString defaultCStringEncoding]];
        if (animationScene->nodes[i]->getType() == NODE_TEXT)
            assetName = [NSString stringWithFormat:@"TEXT: %@", assetName];
        else if (animationScene->nodes[i]->getType() == NODE_IMAGE)
            assetName = [NSString stringWithFormat:@"IMAGE: %@", assetName];
        [self.assetsNamesInScene addObject:assetName];
        //        animationScene->animationScenes[i]->actionId = ++(animationScene->actionObjectsSize);
    }
    delete SGBFilePath;
    return true;
}
- (void)addCameraLight
{
    assetAddType = IMPORT_ASSET_ACTION;
    [self loadNodeInScene:ASSET_CAMERA AssetId:0 AssetName:L"CAMERA"];
    [self.assetsNamesInScene addObject:@"Camera"];
    [self loadNodeInScene:ASSET_LIGHT AssetId:0 AssetName:L"LIGHT"];
    [self.assetsNamesInScene addObject:@"Light"];
}

- (void)resetContext
{
    if (![EAGLContext setCurrentContext:_context])
        Logger::log(INFO, "AnimatinEditor", "Failed to set current OpenGL context");

    if (animationScene) {
        ShaderManager::shadowDensity = sceneShadow;
        animationScene->clearLightProps();
        animationScene->setDataForFrame(animationScene->currentFrame);
        animationScene->updateLightCamera();
    }
}

- (void)loadNodeInScene:(int)assetId
{
    if(assetId > 900 && assetId < 1000) {
        int numberOfLight = assetId - 900;
        [self addLightToScene:[NSString stringWithFormat:@"Light%d",numberOfLight] assetId:assetId];
    } else {
        
        [self resetContext];
        
        AssetItem* assetObject = [cache GetAsset:assetId];
        if (importPressed && assetObject.type != ASSET_ANIMATION) {
            animationScene->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, assetId);
            assetAddType = IMPORT_ASSET_ACTION;
        }
        
        if (assetObject.type != ASSET_ANIMATION)
            [self.assetsNamesInScene addObject:assetObject.name];
        
        [self loadNodeInScene:assetObject.type AssetId:assetId AssetName:[self getwstring:assetObject.name]];
    }
}
- (void)loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name
{
    [self loadNodeInScene:type AssetId:assetId AssetName:name Width:0 Height:0];
}
- (bool)loadNodeInScene:(int)type AssetId:(int)assetId AssetName:(wstring)name Width:(int)imgWidth Height:(int)imgHeight
{
    switch (type) {
    case ASSET_CAMERA: {
        animationScene->loadNode(NODE_CAMERA, assetId, L"CAMERA", 0, 0, assetAddType);
        break;
    }
    case ASSET_LIGHT: {
        animationScene->loadNode(NODE_LIGHT, assetId, L"LIGHT", 0, 0, assetAddType);
        break;
    }
    case ASSET_BACKGROUNDS:
    case ASSET_ACCESSORIES: {
        [self showTipsViewForAction:OBJECT_IMPORTED];
        animationScene->loadNode(NODE_SGM, assetId, name, 0, 0, assetAddType);
        isFileSaved = !(animationScene->changeAction = true);
        break;
    }
    case ASSET_RIGGED: {
        [self showTipsViewForAction:OBJECT_IMPORTED_HUMAN];
        animationScene->loadNode(NODE_RIG, assetId, name, 0, 0, assetAddType);
        isFileSaved = !(animationScene->changeAction = true);
        break;
    }
    case ASSET_OBJ: {
        [self showTipsViewForAction:OBJECT_IMPORTED];
        animationScene->loadNode(NODE_OBJ, assetId, name, 0, 0, assetAddType);
        isFileSaved = !(animationScene->changeAction = true);
        break;
    }
    case ASSET_IMAGE: {
        [self showTipsViewForAction:OBJECT_IMPORTED];
        animationScene->loadNode(NODE_IMAGE, 0, name, imgWidth, imgHeight, assetAddType,Vector4(imgWidth,imgHeight,0,0));
        isFileSaved = !(animationScene->changeAction = true);
        break;
    }
    case ASSET_ANIMATION: {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* documentsDirectory = [paths objectAtIndex:0];
        NSString* filePath = [NSString stringWithFormat:@"%@/%d.sga", documentsDirectory, assetId];
        std::string* sgaFilePath = new std::string([filePath UTF8String]);
        animationScene->animFilePath = "";
        animationScene->applySGAOnNode(sgaFilePath);
        delete sgaFilePath;
        animationScene->storeAddOrRemoveAssetAction(ACTION_APPLY_ANIM, 0);
        [self.frameCarouselView reloadData];
        [self showTipsViewForAction:ANIMATION_APPLIED];
        isFileSaved = !(animationScene->changeAction = true);
        break;
    }
    case ASSET_TEXT_RIG: {
        [self showTipsViewForAction:OBJECT_IMPORTED];
        SGNode* textNode = animationScene->loadNode(NODE_TEXT, 0, name, imgWidth, imgHeight, assetAddType, textColor, [fontFilePath UTF8String]);
        if (textNode == NULL) {
            [self.view endEditing:YES];
            UIAlertView* loadNodeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"The font style you chose does not support the characters you entered." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
            [loadNodeAlert show];
            return false;
        }
        isFileSaved = !(animationScene->changeAction = true);
        break;
    }
    case ASSET_ADDITIONAL_LIGHT: {
        //TODO enum for max lights
        if(ShaderManager::lightPosition.size() < 5) {
            [self.assetsNamesInScene addObject:[self stringWithwstring:name]];
            animationScene->loadNode(NODE_ADDITIONAL_LIGHT, assetId , name, imgWidth , imgHeight , assetAddType , Vector4(1.0));
            isFileSaved = !(animationScene->changeAction = true);
        } else {
            NSLog(@"Max lights 5");
        }
    
        break;
        }
    default: {
        break;
    }
    }
    return true;
}
- (void)applyAnimationToSelectedNode:(NSString*)filePath
{
    isFileSaved = !(animationScene->changeAction = true);
    animationScene->animFilePath = "";
    animationScene->applyAnimations([filePath UTF8String]);
    animationScene->storeAddOrRemoveAssetAction(ACTION_APPLY_ANIM, 0);
}
- (int)getBoneCount
{
    return (int)animationScene->nodes[animationScene->selectedNodeId]->joints.size();
}

void fileWriteCallBack()
{
    [[NSNotificationCenter defaultCenter] postNotificationName:@"FileWriteCompleted" object:nil];
}

void notRespondingCallBackAnimationScene()
{
}
bool downloadMissingAssetCallBack(std::string fileName, NODE_TYPE nodeType)
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];

    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [docPaths objectAtIndex:0];

    switch (nodeType) {
    case NODE_SGM:
    case NODE_RIG: {
        BOOL assetPurchaseStatus = [[CacheSystem cacheSystem] checkDownloadedAsset:stoi(fileName)];
        if (assetPurchaseStatus) {
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
        }
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
void shaderCallBackForNode(int nodeID, string matName, string callbackFuncName)
{
//    if (callbackFuncName.compare("setUniforms") == 0)
//        animationScene->shaderCallBackForNode(nodeID, matName);
//    else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
//        animationScene->setJointsUniforms(nodeID, matName);
//    else if (callbackFuncName.compare("setCtrlUniforms") == 0)
//        animationScene->setControlsUniforms(nodeID, matName);
//    else if (callbackFuncName.compare("RotationCircle") == 0)
//        animationScene->setRotationCircleUniforms(nodeID, matName);
}
bool isTransparentCallBack(int nodeId, string callbackFuncName)
{
//    if (callbackFuncName.compare("setUniforms") == 0)
//        return animationScene->isNodeTransparent(nodeId);
//    else if (callbackFuncName.compare("setJointSpheresUniforms") == 0)
//        return animationScene->isJointTransparent(nodeId, callbackFuncName);
//    else if (callbackFuncName.compare("setCtrlUniforms") == 0)
//        return animationScene->isControlsTransparent(nodeId, callbackFuncName);
//    else if (callbackFuncName.compare("setOBJUniforms") == 0)
//        return false;
//    else if (callbackFuncName.compare("RotationCircle") == 0)
//        return false;
//    return false;
}
void downloadFile(NSString* url, NSString* fileName)
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName])
        return;

    NSData* data = [NSData dataWithContentsOfURL:[NSURL URLWithString:url]];

    if (data)
        [[AppHelper getAppHelper] writeDataToFile:data FileName:fileName];
}

- (void)setupLayer
{
    _eaglLayer = (CAEAGLLayer*)self.renderView.layer;
    _eaglLayer.opaque = YES;
    _eaglLayer.contentsScale = screenScale;
}
- (void)setupContext
{
    EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES2;
    _context = [[EAGLContext alloc] initWithAPI:api];
    if (!_context)
        Logger::log(INFO, "AnimatinEditor", "Failed to initialize OpenGLES 2.0 context");

    if (![EAGLContext setCurrentContext:_context])
        Logger::log(INFO, "AnimatinEditor", "Failed to set current OpenGL context");
}
- (void)setupRenderBuffer
{
    glGenRenderbuffers(1, &_colorRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _colorRenderBuffer);
    [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:_eaglLayer];
}
- (void)setupDepthBuffer
{
    glGenRenderbuffers(1, &_depthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, _depthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24_OES, self.renderView.frame.size.width * screenScale, self.renderView.frame.size.height * screenScale);
}
- (void)setupFrameBuffer
{
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _colorRenderBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthRenderBuffer);
}
- (void)presentRenderBuffer
{
    [_context presentRenderbuffer:_colorRenderBuffer];
}
- (void)updateRenderer
{
    if (!isViewDisplayed || isAppInBG || animationScene->isExportingImages || isHomePressed || isFileSaving)
        return;
    
    @autoreleasepool
    {
        if (checkCtrlSelection) {
            animationScene->checkCtrlSelection(touchBeganPosition);
            checkCtrlSelection = false;
        }
        else if (checkTapSelection) {
            animationScene->checkSelection(tapPosition);
            checkTapSelection = false;
        }
        animationScene->renderAll();
    }
    if (!isMetalSupported)
        [self presentRenderBuffer];

    if (animationScene->isRTTCompleted) {
        [self setupEnableDisableControls];
        if (isTapped)
            [self showAdvancedTipsForGestures];
        animationScene->isRTTCompleted = false;
    }
}

- (void)showAdvancedTipsForGestures
{
    if (!isTapped)
        return;
    if (!animationScene->isNodeSelected) {
        [self showTipsViewForAction:OBJECT_IMPORTED];
        return;
    }
    isTapped = false;
    if (animationScene->selectedNodeId == NODE_CAMERA)
        [self showTipsViewForAction:OBJECT_SELECTED_CAMERA];
    else if (animationScene->selectedNodeId == NODE_LIGHT)
        [self showTipsViewForAction:OBJECT_SELECTED_LIGHT];
    else if (animationScene->selectedNodeId != NOT_SELECTED && animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_RIG) {
        if (animationScene->isJointSelected)
            [self showTipsViewForAction:JOINT_SELECTED];
        else
            [self showTipsViewForAction:OBJECT_SELECTED_HUMAN];
    }
    else
        [self showTipsViewForAction:OBJECT_SELECTED];
}

#pragma mark -

#pragma mark Gestures
- (BOOL)gestureRecognizer:(UIGestureRecognizer*)gestureRecognizer shouldRecognizeSimultaneouslyWithGestureRecognizer:(UIGestureRecognizer*)otherGestureRecognizer
{
    isPanned = false;
    return TRUE;
}

//– touchesBegan:withEvent:
//– touchesMoved:withEvent:
//– touchesEnded:withEvent:
//– touchesCancelled:withEvent:

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    animationScene->setLightingOff();
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    animationScene->setLightingOn();
}

- (void)panGesture:(UIPanGestureRecognizer*)rec
{
    isPanned = true;
    vector<Vector2> p(2);
    CGPoint velocity = (!isMetalSupported) ? [rec velocityInView:self.renderView] : [rec velocityInView:self.renderView];
    NSUInteger touchCount = rec.numberOfTouches;
    for (int i = 0; i < touchCount; i++) {
        if (!isMetalSupported) {
            p[i].x = [rec locationOfTouch:i inView:self.renderView].x;
            p[i].y = [rec locationOfTouch:i inView:self.renderView].y;
        }
        else {
            p[i].x = [rec locationOfTouch:i inView:self.renderView].x;
            p[i].y = [rec locationOfTouch:i inView:self.renderView].y;
        }
    }
    switch (rec.state) {
    case UIGestureRecognizerStateBegan: {
        touchCountTracker = (int)touchCount;
        animationScene->touchBegan(p[0] * screenScale);
        switch (touchCount) {
        case 1: {
            checkCtrlSelection = true;
            touchBeganPosition = p[0] * screenScale;
            animationScene->swipeProgress(-velocity.x / 50.0, -velocity.y / 50.0);
            animationScene->updateLightCamera();
            break;
        }
        case 2: {
            animationScene->panBegan(p[0] * screenScale, p[1] * screenScale);
            animationScene->updateLightCamera();
            break;
        }
        default:
            break;
        }
        break;
    }
    case UIGestureRecognizerStateChanged: {
        if (touchCount != touchCountTracker) {
            touchCountTracker = (int)touchCount;
            animationScene->panBegan(p[0] * screenScale, p[1] * screenScale);
        }
        switch (touchCount) {
        case 1: {
            animationScene->touchMove(p[0] * screenScale, p[1] * screenScale, SCREENWIDTH * screenScale, SCREENHEIGHT * screenScale);
            animationScene->swipeProgress(-velocity.x / 50.0, -velocity.y / 50.0);
            animationScene->updateLightCamera();
            break;
        }
        case 2: {
            animationScene->panProgress(p[0] * screenScale, p[1] * screenScale);
            animationScene->updateLightCamera();
            break;
        }
        default:
            break;
        }
        break;
    }
    default: {
        if (animationScene->isControlSelected && animationScene->selectedNodeId != NOT_SELECTED && animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_RIG) {
            if (animationScene->isJointSelected)
                [self showTipsViewForAction:JOINT_MOVED];
            else
                [self showTipsViewForAction:OBJECT_RIG_MOVED];
        }
        else if (animationScene->isControlSelected)
            [self showTipsViewForAction:OBJECT_MOVED];

        animationScene->touchEnd(p[0] * screenScale);
        [_frameCarouselView reloadData];
        break;
    }
    }
    animationScene->updateControlsOrientaion();
    if (animationScene->actions.size() > 0 && animationScene->currentAction > 0 && !isPlaying) {
        [self.undoButton setEnabled:YES];
        isFileSaved = !(animationScene->changeAction);
    }
}
- (void)lightSliderChangeEnded
{
    animationScene->storeLightPropertyChangeAction(self.redSlider.value, self.greenSlider.value, self.blueSlider.value, self.shadowSlider.value);
    isFileSaved = !(animationScene->changeAction = true);
}
- (IBAction)sliderValueChanged:(id)sender
{
}

- (void)tapGesture:(UITapGestureRecognizer*)rec
{
    isTapped = true;
    [self changeAllButtonBG];
    if (!isPlaying && !isPanned) {
        if ([self.helpButton tag] == 1) {
            [self updateHelpImageView];
        }
        CGPoint position;
        position = [rec locationInView:self.renderView];
        checkTapSelection = true;
        tapPosition = Vector2(position.x, position.y) * screenScale;
        animationScene->isRTTCompleted = true;
        [_frameCarouselView reloadData];
        [self setupEnableDisableControls];

        if (animationScene->actions.size() > 0 && animationScene->currentAction > 0)
            [self.undoButton setEnabled:YES];
    }
    animationScene->setLightingOn();
}

#pragma mark -

#pragma mark ButtonActions

- (IBAction)fullViewButtonAction:(id)sender
{
    [self viewExpand:YES];
}

- (IBAction)minViewButtonAction:(id)sender
{
    [self viewExpand:NO];
}

- (void)viewExpand:(BOOL)isHidden
{
    [self.fullViewExpand setHidden:isHidden];
    int normalButtonPlace = 0, scrollViewButton = 0, normalmoveButtonPlace = 0, scrollmoveViewButton = 0;
    if ([Utility IsPadDevice]) {
        normalmoveButtonPlace = 23;
        scrollmoveViewButton = 62;
        normalButtonPlace = 122;
        scrollViewButton = 160;
    }
    else {
        normalmoveButtonPlace = 13;
        scrollmoveViewButton = 40;
        normalButtonPlace = 82;
        scrollViewButton = 109;
    }
    if (isHidden) {
        frameBarViewHeight = self.frameView.frame.size.height;
        [self hiddenview:self.frameView];
        self.tipsView.frame = CGRectMake(self.tipsView.frame.origin.x, normalmoveButtonPlace, self.tipsView.frame.size.width, self.tipsView.frame.size.height);
        self.viewBTView.frame = CGRectMake(self.viewBTView.frame.origin.x, normalmoveButtonPlace, self.viewBTView.frame.size.width, self.viewBTView.frame.size.height);
        self.viewBT.frame = CGRectMake(self.viewBT.frame.origin.x, normalmoveButtonPlace, self.viewBT.frame.size.width, self.viewBT.frame.size.height);
        self.viewBTiconView.frame = CGRectMake(self.viewBT.frame.origin.x, normalmoveButtonPlace, self.viewBT.frame.size.width, self.viewBT.frame.size.height);
        self.viewBTScroll.frame = CGRectMake(self.viewBTScroll.frame.origin.x, scrollmoveViewButton, self.viewBTScroll.frame.size.width, self.viewBTScroll.frame.size.height);
        self.mirrorSwitch.frame = CGRectMake(self.mirrorSwitch.frame.origin.x, normalmoveButtonPlace, self.mirrorSwitch.frame.size.width, self.mirrorSwitch.frame.size.height);
        self.mirrorLabel.frame = CGRectMake(self.mirrorLabel.frame.origin.x, normalmoveButtonPlace, self.mirrorLabel.frame.size.width, self.mirrorLabel.frame.size.height);
    }
    else {
        [self performSelectorOnMainThread:@selector(backtoNormal) withObject:nil waitUntilDone:YES];
        self.tipsView.frame = CGRectMake(self.tipsView.frame.origin.x, normalButtonPlace, self.tipsView.frame.size.width, self.tipsView.frame.size.height);
        self.viewBTView.frame = CGRectMake(self.viewBTView.frame.origin.x, normalButtonPlace, self.viewBTView.frame.size.width, self.viewBTView.frame.size.height);
        self.viewBT.frame = CGRectMake(self.viewBT.frame.origin.x, normalButtonPlace, self.viewBT.frame.size.width, self.viewBT.frame.size.height);
        self.viewBTiconView.frame = CGRectMake(self.viewBT.frame.origin.x, normalButtonPlace, self.viewBT.frame.size.width, self.viewBT.frame.size.height);
        self.viewBTScroll.frame = CGRectMake(self.viewBTScroll.frame.origin.x, scrollViewButton, self.viewBTScroll.frame.size.width, self.viewBTScroll.frame.size.height);
        self.mirrorSwitch.frame = CGRectMake(self.mirrorSwitch.frame.origin.x, normalButtonPlace, self.mirrorSwitch.frame.size.width, self.mirrorSwitch.frame.size.height);
        self.mirrorLabel.frame = CGRectMake(self.mirrorLabel.frame.origin.x, normalButtonPlace, self.mirrorLabel.frame.size.width, self.mirrorLabel.frame.size.height);
    }
}
- (void)backtoNormal
{
    [self backtoPosition:self.frameView view:frameBarViewHeight];
}
- (void)backtoPosition:(UIView*)dropDownView view:(int)height
{
    dropDownView.frame = CGRectMake(dropDownView.frame.origin.x, dropDownView.frame.origin.y, dropDownView.frame.size.width, 0);

    [UIView animateWithDuration:0.5
        delay:0.0
        options:UIViewAnimationCurveEaseIn
        animations:^{

            dropDownView.frame = CGRectMake(dropDownView.frame.origin.x, dropDownView.frame.origin.y, dropDownView.frame.size.width, height);
        }
        completion:^(BOOL finished){

        }];
}
- (void)hiddenview:(UIView*)dropDownView
{
    //    [self.topView setHidden:YES];
    //    [self.frameView setHidden:YES];
    //float height = dropDownView.frame.size.height;
    dropDownView.frame = CGRectMake(dropDownView.frame.origin.x, dropDownView.frame.origin.y, dropDownView.frame.size.width, dropDownView.frame.size.height);

    [UIView animateWithDuration:0.5
        delay:0.0
        options:UIViewAnimationCurveEaseIn
        animations:^{

            dropDownView.frame = CGRectMake(dropDownView.frame.origin.x, dropDownView.frame.origin.y, dropDownView.frame.size.width, 0);
        }
        completion:^(BOOL finished){

        }];
}
- (IBAction)saveAnimationButtonAction:(id)sender
{
    if (animationScene->selectedNodeId <= 1 || (!(animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_RIG) && !(animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_TEXT)) || animationScene->isJointSelected) {
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

- (void)saveUserAnimation:(NSString*)assetName
{
    int assetId = ANIMATIONS_ID + [cache getNextAnimationAssetId];
    int type = animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_TEXT ? 1 : 0;

    bool status = animationScene->storeAnimations(assetId);

    if (status) {
        AnimationItem* animItem = [[AnimationItem alloc] init];
        animItem.assetId = assetId;
        animItem.assetName = assetName;
        animItem.published = 0;
        animItem.rating = 0;
        animItem.boneCount = (int)animationScene->nodes[animationScene->selectedNodeId]->joints.size();
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

- (IBAction)frameChangeButtonAction:(id)sender
{
    NSString* extension;
    if ([Utility IsPadDevice])
        extension = @"";
    else
        extension = @"-phone";

    [self.frameButton setTag:(self.frameButton.tag == 1) ? 2 : 1];
    self.frameLabel.text = (self.frameButton.tag == 1) ? @"FRAMES" : @"DURATIONS";
    NSString* indicationImageName = (self.frameButton.tag == 1) ? @"frames" : @"clock";
    [self.frameImage setImage:[UIImage imageNamed:[NSString stringWithFormat:@"%@%@", indicationImageName, extension]]];
    [self.frameCarouselView reloadData];

    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithLong:self.frameButton.tag] withKey:@"indicationType"];
}
- (IBAction)renderImageButtonAction:(id)sender
{
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];

    animationScene->setLightingOn();
    RenderingViewController* renderingView;
    float cameraResolutionWidth, cameraResolutionHeight;
    if (cameraResolution == 0) {
        cameraResolutionWidth = 1920.0f;
        cameraResolutionHeight = 1080.0f;
    }
    else if (cameraResolution == 1) {
        cameraResolutionWidth = 1280.0f;
        cameraResolutionHeight = 720.0f;
    }
    else {
        cameraResolutionWidth = 720.0f;
        cameraResolutionHeight = 480.0f;
    }
    [self setupEnableDisableControls];
    if ([Utility IsPadDevice])
        renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewController" bundle:nil StartFrame:animationScene->currentFrame EndFrame:animationScene->currentFrame renderOutput:RENDER_IMAGE caMresolution:animationScene->cameraResolutionType];
    else
        renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewControllerPhone" bundle:nil StartFrame:animationScene->currentFrame EndFrame:animationScene->currentFrame renderOutput:RENDER_IMAGE caMresolution:animationScene->cameraResolutionType];
    
    renderingView.projectName = _sceneName.text;
    renderingView.delegate = self;
    renderingView.modalPresentationStyle = UIModalPresentationFormSheet;
    [self presentViewController:renderingView animated:YES completion:nil];
    if ([Utility IsPadDevice]) {
        renderingView.view.superview.backgroundColor = [UIColor clearColor];
        renderingView.view.layer.borderWidth = 2.0f;
        renderingView.view.layer.borderColor = [UIColor grayColor].CGColor;
    }
}
- (IBAction)renderVideoButtonAction:(id)sender
{
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    
    animationScene->setLightingOn();
    RenderingViewController* renderingView;
    float cameraResolutionWidth, cameraResolutionHeight;
    if (cameraResolution == 0) {
        cameraResolutionWidth = 1920.0f;
        cameraResolutionHeight = 1080.0f;
    }
    else if (cameraResolution == 1) {
        cameraResolutionWidth = 1280.0f;
        cameraResolutionHeight = 720.0f;
    }
    else {
        cameraResolutionWidth = 720.0f;
        cameraResolutionHeight = 480.0f;
    }
    [self setupEnableDisableControls];
    if ([Utility IsPadDevice])
        renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewController" bundle:nil StartFrame:0 EndFrame:animationScene->totalFrames renderOutput:RENDER_VIDEO caMresolution:animationScene->cameraResolutionType];
    else
        renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewControllerPhone" bundle:nil StartFrame:0 EndFrame:animationScene->totalFrames renderOutput:RENDER_VIDEO caMresolution:animationScene->cameraResolutionType];
    renderingView.projectName = _sceneName.text;
    renderingView.delegate = self;
    renderingView.modalPresentationStyle = UIModalPresentationFormSheet;
    [self presentViewController:renderingView animated:YES completion:nil];

    if ([Utility IsPadDevice]) {
        renderingView.view.superview.backgroundColor = [UIColor clearColor];
        renderingView.view.layer.borderWidth = 2.0f;
        renderingView.view.layer.borderColor = [UIColor grayColor].CGColor;
    }
}
- (IBAction)renderGifButtonAction:(id)sender
{
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    
    animationScene->setLightingOn();
    RenderingViewController* renderingView;
    float cameraResolutionWidth, cameraResolutionHeight;
    if (cameraResolution == 0) {
        cameraResolutionWidth = 1920.0f;
        cameraResolutionHeight = 1080.0f;
    }
    else if (cameraResolution == 1) {
        cameraResolutionWidth = 1280.0f;
        cameraResolutionHeight = 720.0f;
    }
    else {
        cameraResolutionWidth = 720.0f;
        cameraResolutionHeight = 480.0f;
    }
    [self setupEnableDisableControls];
    if ([Utility IsPadDevice])
        renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewController" bundle:nil StartFrame:0 EndFrame:animationScene->totalFrames renderOutput:RENDER_GIF caMresolution:animationScene->cameraResolutionType];
    else
        renderingView = [[RenderingViewController alloc] initWithNibName:@"RenderingViewControllerPhone" bundle:nil StartFrame:0 EndFrame:animationScene->totalFrames renderOutput:RENDER_GIF caMresolution:animationScene->cameraResolutionType];
    renderingView.projectName = _sceneName.text;
    renderingView.delegate = self;
    renderingView.modalPresentationStyle = UIModalPresentationFormSheet;
    [self presentViewController:renderingView animated:YES completion:nil];

    if ([Utility IsPadDevice]) {
        renderingView.view.superview.backgroundColor = [UIColor clearColor];
        renderingView.view.layer.borderWidth = 2.0f;
        renderingView.view.layer.borderColor = [UIColor grayColor].CGColor;
    }
}
- (IBAction)importImageAction:(id)sender
{
    animationScene->setLightingOn();
    [self.loadingActivity setHidden:NO];
    [self.loadingActivity startAnimating];
    [self setupEnableDisableControls];
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    if (self.imagePicker == nil) {
        self.imagePicker = [[UIImagePickerController alloc] init];
        self.imagePicker.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
        [self.imagePicker setNavigationBarHidden:YES];
        [self.imagePicker setToolbarHidden:YES];
    }

    if ([Utility IsPadDevice]) {
        ImageImportViewController* imageImportView = [[ImageImportViewController alloc] initWithNibName:@"ImageImportViewPad" bundle:nil];
        imageImportView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:imageImportView animated:YES completion:nil];
        imageImportView.view.superview.backgroundColor = [UIColor clearColor];
        imageImportView.view.layer.borderWidth = 2.0f;
        imageImportView.view.layer.borderColor = [UIColor grayColor].CGColor;
        [imageImportView.imagesView addSubview:self.imagePicker.view];
        self.imagePicker.delegate = imageImportView;
        [self.imagePicker.view setFrame:CGRectMake(0, 0, imageImportView.imagesView.frame.size.width, imageImportView.imagesView.frame.size.height)];
        imageImportView.delegate = self;
    }
    else {
        ImageImportViewController* imageImportView = [[ImageImportViewController alloc] initWithNibName:@"ImageImportViewPhone" bundle:nil];
        imageImportView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:imageImportView animated:YES completion:nil];
        imageImportView.view.superview.backgroundColor = [UIColor clearColor];
        imageImportView.view.layer.borderWidth = 2.0f;
        imageImportView.view.layer.borderColor = [UIColor grayColor].CGColor;
        [imageImportView.imagesView addSubview:self.imagePicker.view];
        self.imagePicker.delegate = imageImportView;
        [self.imagePicker.view setFrame:CGRectMake(0, 0, imageImportView.imagesView.frame.size.width, imageImportView.imagesView.frame.size.height)];
        imageImportView.delegate = self;
    }
}
- (IBAction)importAssetAction:(id)sender
{
    [self storeShaderProps];
    if(animationScene)
        animationScene->clearLightProps();
    
    importPressed = YES;
    assetAddType = IMPORT_ASSET_ACTION;
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];

    [displayLink invalidate];
    displayLink = nil;

    if ([Utility IsPadDevice]) {
        AssetSelectionViewControllerPad* assetSelectionView = [[AssetSelectionViewControllerPad alloc] initWithNibName:@"AssetSelectionViewControllerPad" bundle:nil];
        assetSelectionView.priceLocale = self.priceLocale;
        assetSelectionView.delegate = self;
        [self presentViewController:assetSelectionView animated:YES completion:nil];
    }
    else {
        AssetSelectionViewControllerPad* assetSelectionView = [[AssetSelectionViewControllerPad alloc] initWithNibName:@"AssetSelectionViewControllerPhone" bundle:nil];
        assetSelectionView.priceLocale = self.priceLocale;
        assetSelectionView.delegate = self;
        [self presentViewController:assetSelectionView animated:YES completion:nil];
    }
}

- (void)storeShaderProps
{
    sceneShadow = ShaderManager::shadowDensity;
    sceneLightColor = ShaderManager::lightColor[0];
}

- (IBAction)importAnimationAction:(id)sender
{
    [self storeShaderProps];
    if(animationScene)
        animationScene->clearLightProps();
    
    if (animationScene->selectedNodeId <= 1 || (!(animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_RIG) && !(animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_TEXT)) || animationScene->isJointSelected) {
        [self.view endEditing:YES];
        UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Please select a text or character to apply animation." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [closeAlert show];
    }
    else {
        ANIMATION_TYPE animType = (animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_TEXT) ? TEXT_ANIMATION : RIG_ANIMATION;

        if ([Utility IsPadDevice]) {
            AnimationSelectionViewController* animationSelectionView = [[AnimationSelectionViewController alloc] initWithNibName:@"AnimationSelectionViewControllerPad" bundle:nil withType:animType];
            animationSelectionView.delegate = self;
            animationSelectionView.currentAssetName = [self stringWithwstring:animationScene->nodes[animationScene->selectedNodeId]->name]; //[NSString stringWithFormat:@"%s", (animationScene->nodes[animationScene->selectedNodeId]->name).c_str()];
            if (animType == TEXT_ANIMATION)
                animationSelectionView.currentTextNode = animationScene->nodes[animationScene->selectedNodeId];
            else
                animationSelectionView.currentTextNode = NULL;

            [self presentViewController:animationSelectionView animated:YES completion:nil];
        }
        else {
            AnimationSelectionViewController* animationSelectionView = [[AnimationSelectionViewController alloc] initWithNibName:@"AnimationSelectionViewControllerPhone" bundle:nil withType:animType];
            animationSelectionView.delegate = self;
            animationSelectionView.currentAssetName = [self stringWithwstring:animationScene->nodes[animationScene->selectedNodeId]->name]; // [NSString stringWithFormat:@"%s", (animationScene->nodes[animationScene->selectedNodeId]->name).c_str()];
            if (animType == TEXT_ANIMATION)
                animationSelectionView.currentTextNode = animationScene->nodes[animationScene->selectedNodeId];
            else
                animationSelectionView.currentTextNode = NULL;

            [self presentViewController:animationSelectionView animated:YES completion:nil];
        }
        [displayLink invalidate];
        displayLink = nil;
    }
}

- (IBAction)importLightAction:(id)sender {
    [self showUpgradeView];
}

- (void) importLightIntoScene
{
    [self setupEnableDisableControls];
    if(ShaderManager::lightPosition.size() < 5) {
        assetAddType = IMPORT_ASSET_ACTION;
        animationScene->storeAddOrRemoveAssetAction(ACTION_NODE_ADDED, ASSET_ADDITIONAL_LIGHT + lightCount , "Light"+ to_string(lightCount));
        [self addLightToScene:[NSString stringWithFormat:@"Light%d",lightCount] assetId:ASSET_ADDITIONAL_LIGHT + lightCount ];
        lightCount++;
        [self undoRedoButtonState:DEACTIVATE_BOTH];
    } else {
        UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Scene cannot contain more than five lights." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [closeAlert show];
    }
}

-(void) addLightToScene:(NSString*)lightName assetId:(int)assetId
{
    [self loadNodeInScene:ASSET_ADDITIONAL_LIGHT AssetId:assetId AssetName:[self getwstring:lightName] Width:20 Height:50];
    textColor = Vector4(1.0,1.0,1.0,1.0);
}

- (IBAction)menuButtonAction:(id)sender
{
    if (isPlaying) {
        [self.importAnimationButton setEnabled:NO];
        [self.saveAnimationButton setEnabled:NO];
    }
    NSString* extension;
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
        extension = @"-phone";
    else
        extension = @"";
    if ([self.menuButton tag] == 2) {
        self.projectButton.layer.cornerRadius = 5.0;
        [self setupEnableDisableControls];
        [self.menuButton setImage:[UIImage imageNamed:[NSString stringWithFormat:@"MenuReverse%@.png", extension]] forState:UIControlStateNormal];
        [self placeView:self.menuViewSlider fromValue:CGPointMake(0.0, 308.0) toValue:CGPointMake(0.0, 0.0) active:NO];
        [self.menuButton setTag:1];
    }
    else if ([self.menuButton tag] == 1) {
        [self.menuButton setImage:[UIImage imageNamed:[NSString stringWithFormat:@"Menu%@.png", extension]] forState:UIControlStateNormal];
        [self.menuViewSlider setHidden:YES];
        [self.menuButton setTag:2];
    }
}
- (IBAction)playButtonAction:(id)sender
{
    isPlaying = !isPlaying;
    animationScene->isPlaying = isPlaying;

    if (animationScene->selectedNodeId != NOT_SELECTED) {
        animationScene->unselectObject(animationScene->selectedNodeId);
        [self.frameCarouselView reloadData];
    }
    if (isPlaying) {
        //[self setInterfaceVisibility:YES];
        [self setupEnableDisableControls];
        [self playButtonEnableDisable:NO];
        [self.playButton setImage:[UIImage imageNamed:@"stop-img.png"] forState:UIControlStateNormal];
        [self.playButton setTitle:@"  STOP" forState:UIControlStateNormal];
        playTimer = [NSTimer scheduledTimerWithTimeInterval:(1.0f / 24.0f) target:self selector:@selector(playTimerTarget) userInfo:nil repeats:YES];
        [self showTipsViewForAction:SCENE_PLAYING];
    }
    else {
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
        [self showTipsViewForAction:FRAMES_HELP];
    }
}
- (IBAction)moveFrontButtonAction:(id)sender
{
    scrollConstant = SCROLL_FIRST_FRAME;
    timerCount = 0;
    [self scrolltoEnd];
}
- (IBAction)moveBackButtonAction:(id)sender
{
    scrollConstant = SCROLL_LAST_FRAME;
    timerCount = 0;
    [self scrolltoEnd];
}
- (IBAction)addFrameAction:(id)sender
{
    animationScene->totalFrames++;
    [self.frameCarouselView reloadData];
    animationScene->currentFrame = animationScene->totalFrames - 1;
    animationScene->switchFrame(animationScene->currentFrame);
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:animationScene->currentFrame inSection:0];
    [self.frameCarouselView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    [self.frameCarouselView reloadData];
    isFileSaved = false;
}
- (IBAction)importTextAction:(id)sender
{
    [self storeShaderProps];
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    if ([Utility IsPadDevice]) {
        TextImportViewController* textView = [[TextImportViewController alloc] initWithNibName:@"TextImportViewController" bundle:nil];
        textView.delegate = self;
        [self presentViewController:textView animated:YES completion:nil];
    }
    else {
        TextImportViewController* textView = [[TextImportViewController alloc] initWithNibName:@"TextimportViewControllerPhone" bundle:nil];
        textView.delegate = self;
        [self presentViewController:textView animated:YES completion:nil];
    }
    [displayLink invalidate];
    displayLink = nil;
}
- (IBAction)viewButtonAction:(id)sender
{
    [self changeAllButtonBG];
    if ([self.viewBT tag] == 2) {
        [self setupEnableDisableControls];
        [self.viewBTiconView setHidden:NO];
        [self placeView:self.viewBTView fromValue:CGPointMake(55.0, 0.0) toValue:CGPointMake(0.0, 0.0) active:NO];
        [self.viewBT setTag:1];
        timerI = 0;
        sliderId = VARIOUS_VIEW;
        [self animationEffect];
        [self showTipsViewForAction:CAM_PERSPECTIVE];
    }
    else if ([self.viewBT tag] == 1) {
        [self placeView:self.viewBTView fromValue:CGPointMake(0.0, -55.0) toValue:CGPointMake(0.0, 0.0) active:YES];
        [self.viewBTiconView setHidden:YES];
        [self.viewBT setTag:2];
        [self.viewBTScroll setHidden:YES];
        [self showTipsViewForAction:OBJECT_IMPORTED];
    }
}
- (IBAction)topviewButtonAction:(id)sender
{
    animationScene->changeCameraView(VIEW_TOP);
    animationScene->updateLightCamera();
}
- (IBAction)rightviewButtonAction:(id)sender
{
    animationScene->changeCameraView(VIEW_RIGHT);
    animationScene->updateLightCamera();
}
- (IBAction)leftviewButtonAction:(id)sender
{
    animationScene->changeCameraView(VIEW_LEFT);
    animationScene->updateLightCamera();
}
- (IBAction)bottomviewButtonAction:(id)sender
{
    animationScene->changeCameraView(VIEW_BOTTOM);
    animationScene->updateLightCamera();
}
- (IBAction)frontviewButtonAction:(id)sender
{
    animationScene->changeCameraView(VIEW_FRONT);
    animationScene->updateLightCamera();
}
- (IBAction)backviewButtonAction:(id)sender
{
    animationScene->changeCameraView(VIEW_BACK);
    animationScene->updateLightCamera();
}
- (IBAction)objectSelectionAction:(id)sender
{
    [self changeAllButtonBG];
    if ([self.objectSelectionButton tag] == 2) {
        [self setupEnableDisableControls];
        self.selectBTiconView.backgroundColor = [UIColor blackColor];
        [self placeView:self.selectBTView fromValue:CGPointMake(-95.0, 0.0) toValue:CGPointMake(0.0, 0.0) active:NO];
        [self.objectListTableView reloadData];
        [self.objectSelectionView addSubview:self.objectListTableView];
        self.objectSelectionView.backgroundColor = [UIColor clearColor];
        timerI = 0;
        sliderId = OBJECT_SELECTION;
        [self.objectSelectionButton setTag:1];
        [self animationEffect];
        [self showTipsViewForAction:SELECT_OBJECT];
    }
    else {
        [self placeView:self.selectBTView fromValue:CGPointMake(0.0, 0.0) toValue:CGPointMake(95.0, 0.0) active:YES];
        [self.objectSelectionView setHidden:YES];
        [self.objectSelectionButton setTag:2];
        [self showTipsViewForAction:OBJECT_IMPORTED];
    }
}
- (IBAction)moveButtonAction:(id)sender
{
    if ([self.moveButton tag] == 2) {
        [self setupEnableDisableControls];
        [self placeView:self.moveBTView fromValue:CGPointMake(-95.0, 0.0) toValue:CGPointMake(0.0, 0.0) active:NO];
        [self.moveButton setTag:1];
    }
    else {
        [self placeView:self.moveBTView fromValue:CGPointMake(0.0, 0.0) toValue:CGPointMake(95.0, 0.0) active:YES];
        [self.moveButton setTag:2];
    }
    animationScene->controlType = MOVE;
    [self changeAllButtonBG];
    self.moveBTiconView.backgroundColor = [UIColor blackColor];
    animationScene->updateControlsOrientaion();
}
- (IBAction)rotateButtonAction:(id)sender
{
    if ([self.rotateButton tag] == 2) {
        [self setupEnableDisableControls];
        [self placeView:self.rotateBTView fromValue:CGPointMake(-95.0, 0.0) toValue:CGPointMake(0.0, 0.0) active:NO];
        [self.rotateButton setTag:1];
    }
    else {
        [self placeView:self.rotateBTView fromValue:CGPointMake(0.0, 0.0) toValue:CGPointMake(95.0, 0.0) active:YES];
        [self.rotateButton setTag:2];
    }
    animationScene->controlType = ROTATE;
    [self changeAllButtonBG];
    self.rotateBTiconView.backgroundColor = [UIColor blackColor];
    animationScene->updateControlsOrientaion();
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    //isFileSaved = false;
}
- (IBAction)scaleButtonAction:(id)sender
{
    [self changeAllButtonBG];
    if ([self.scaleButton tag] == 2) {
        [self setupEnableDisableControls];
        self.scaleBTiconView.backgroundColor = [UIColor blackColor];
        [self placeView:self.scaleBTView fromValue:CGPointMake(-95.0, 0.0) toValue:CGPointMake(0.0, 0.0) active:NO];
        [self.scaleButton setTag:1];
    }
    else {
        [self placeView:self.scaleBTView fromValue:CGPointMake(0.0, 0.0) toValue:CGPointMake(95.0, 0.0) active:YES];
        [self.scaleButton setTag:2];
    }
    animationScene->setControlsVisibility(false);
    Vector3 currentScale = animationScene->getSelectedNodeScale();
    if (self.scaleProperties == nil) {
        self.scaleProperties = [[ScaleViewController alloc] initWithNibName:@"ScaleViewController" bundle:nil updateXValue:currentScale.x updateYValue:currentScale.y updateZValue:currentScale.z];

        self.scaleProperties.delegate = self;
    }
    [self.scaleProperties updateScale:currentScale.x:currentScale.y:currentScale.z];

    if (self.popOverView && [self.popOverView isBeingPresented]) {
        [self showTipsViewForAction:OBJECT_IMPORTED];
        [self.popOverView dismissPopoverAnimated:YES];
    }
    else
        [self showTipsViewForAction:SCALE_ACTION];

    self.popOverView = [[FPPopoverController alloc] initWithViewController:self.scaleProperties];
    self.popOverView.border = NO;
    self.popOverView.tint = FPPopoverWhiteTint;
    self.popOverView.contentSize = CGSizeMake(320.0, 180.0);
    [self.popOverView presentPopoverFromPoint:self.scaleBTiconView.frame.origin];
}
- (IBAction)objectPropertiesAction:(id)sender
{
    [self changeAllButtonBG];
    if ([self.objectPropertiesButton tag] == 2) {
        [self setupEnableDisableControls];
        self.propertiesBTiconView.backgroundColor = [UIColor blackColor];
        [self placeView:self.propertiesBTView fromValue:CGPointMake(-95.0, 0.0) toValue:CGPointMake(0.0, 0.0) active:NO];
        [self.objectPropertiesButton setTag:1];
        NODE_TYPE selectedNodeType = animationScene->nodes[animationScene->selectedNodeId]->getType();
        switch (selectedNodeType) {
        case NODE_CAMERA: {
            printf("Camera FOV %f ",animationScene->cameraFOV);
            self.fovSlider.value = animationScene->cameraFOV;
            self.fovLabel.text = [NSString stringWithFormat:@"%0.2f", animationScene->cameraFOV];
            self.resolutionSegment.selectedSegmentIndex = animationScene->cameraResolutionType;
            timerI = 0;
            sliderId = CAMERA_PROPERTY;
            [self animationEffect];
            [self showTipsViewForAction:CAMERA_PROPS];
            break;
        }
            case NODE_LIGHT:
            case NODE_ADDITIONAL_LIGHT:{
                Quaternion lightProps = KeyHelper::getKeyInterpolationForFrame<int, SGRotationKey, Quaternion>(animationScene->currentFrame,animationScene->nodes[animationScene->selectedNodeId]->rotationKeys,true);
                Vector3 mainLight = KeyHelper::getKeyInterpolationForFrame<int, SGScaleKey, Vector3>(animationScene->currentFrame, animationScene->nodes[animationScene->selectedNodeId]->scaleKeys);
                
                self.redSlider.value = (selectedNodeType == NODE_LIGHT) ? mainLight.x:lightProps.x;//ShaderManager::lightColor[0].x;
                self.greenSlider.value = (selectedNodeType == NODE_LIGHT) ? mainLight.y:lightProps.y;
                self.blueSlider.value = (selectedNodeType == NODE_LIGHT) ? mainLight.z:lightProps.z;
                if(selectedNodeType == NODE_LIGHT) {
                    self.shadowSlider.value = ShaderManager::shadowDensity;
                    self.lightSliderLbl.text = @"SHADOW DARKNESS";
                } else {
                    self.shadowSlider.value = ((lightProps.w/300.0)-0.001);
                    self.lightSliderLbl.text = @"DISTANCE";
                }
                
                self.colorDisplayLabel.textColor = [UIColor colorWithRed:self.redSlider.value green:self.greenSlider.value blue:self.blueSlider.value alpha:1.0];
                timerI = 0;
                sliderId = LIGHT_PROPERTY;
                [self animationEffect];
                [self showTipsViewForAction:LIGHT_PROPS];
                break;
        }
        case NODE_UNDEFINED: {
            break;
        }
        default: {
            self.brightnessSlider.value = animationScene->nodes[animationScene->selectedNodeId]->props.brightness;
            if (animationScene->selectedNodeId != NOT_SELECTED && animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_IMAGE)
                [self.specularSlider setEnabled:NO];
            else
                [self.specularSlider setEnabled:YES];
            self.specularSlider.value = animationScene->nodes[animationScene->selectedNodeId]->props.shininess;
            self.lightingSwitch.on = animationScene->nodes[animationScene->selectedNodeId]->props.isLighting;
            self.visibleSwitch.on = animationScene->nodes[animationScene->selectedNodeId]->props.isVisible;
            timerI = 0;
            sliderId = MESH_PROPERTY;
            [self animationEffect];
            [self showTipsViewForAction:MESH_PROPS];
            break;
        }
        }
    }
    else if ([self.objectPropertiesButton tag] == 1) {
        [self.cameraPropertyView setHidden:YES];
        [self.meshPropertyView setHidden:YES];
        [self.lightPropertyView setHidden:YES];
        [self.objectPropertiesButton setTag:2];
        [self showTipsViewForAction:OBJECT_IMPORTED];
    }
}
- (IBAction)deleteAction:(id)sender
{
    [self changeAllButtonBG];
    [self setupEnableDisableControls];
    self.deleteBTiconView.backgroundColor = [UIColor blackColor];
    if ([self.deleteButton tag] == 2) {
        [self placeView:self.deleteBTView fromValue:CGPointMake(-95.0, 0.0) toValue:CGPointMake(0.0, 0.0) active:NO];
        [self.deleteButton setTag:1];
    }
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];

    UIAlertView* deleteAlert = nil;
    if (animationScene->selectedNodeId <= NODE_LIGHT) {
        [self.view endEditing:YES];
        deleteAlert = [[UIAlertView alloc] initWithTitle:@"Delete" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Delete Animation in this Frame", nil];
        [self showTipsViewForAction:CAM_LIGHT_DELETE];
    }
    else {
        [self.view endEditing:YES];
        deleteAlert = [[UIAlertView alloc] initWithTitle:@"Delete" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Delete Animation in this Frame", @"Delete Object", nil];
        [self showTipsViewForAction:DELETE_PRESSED];
    }

    [deleteAlert setTag:DELETE_BUTTON_TAG];
    [deleteAlert show];
}
- (IBAction)helpButtonAction:(id)sender
{
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    if ([Utility IsPadDevice]) {
        HelpViewController* morehelpView = [[HelpViewController alloc] initWithNibName:@"HelpViewController" bundle:nil CalledFrom:HELP_FROM_ANIMATION];
        morehelpView.delegate = self;
        morehelpView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:morehelpView animated:YES completion:nil];
        CGRect rect = CGRectInset(morehelpView.view.frame, -80, -0);
        morehelpView.view.superview.backgroundColor = [UIColor clearColor];
        morehelpView.view.frame = rect;
    }
    else {
        HelpViewController* morehelpView = [[HelpViewController alloc] initWithNibName:@"HelpViewControllerPhone" bundle:nil CalledFrom:HELP_FROM_ANIMATION];
        morehelpView.delegate = self;
        morehelpView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:morehelpView animated:YES completion:nil];
    }
}
- (void)placeView:(UIView*)theView fromValue:(CGPoint)fromPoint toValue:(CGPoint)toPoint active:(BOOL)setValue
{
    CABasicAnimation* hover = [CABasicAnimation animationWithKeyPath:@"position"];
    hover.additive = YES; // fromValue and toValue will be relative instead of absolute values
    hover.fromValue = [NSValue valueWithCGPoint:fromPoint];
    [theView setHidden:setValue];
    hover.toValue = [NSValue valueWithCGPoint:toPoint]; // y increases downwards on iOS
    hover.autoreverses = NO; // Animate back to normal afterwards
    hover.duration = 0.2; // The duration for one part of the animation (0.2 up and 0.2 down)
    hover.repeatCount = 1; // The number of times the animation should repeat
    [theView.layer addAnimation:hover forKey:@"myHoverAnimation"];
}
- (IBAction)saveButtonAction:(id)sender
{
    if (isPlaying)
        [self stopPlaying];
    if (!isFileSaving) {
        [self.view setUserInteractionEnabled:NO];
        [self.saveButton setEnabled:NO];
        [self.saveloadingActivity setHidden:NO];
        [self.saveloadingActivity startAnimating];
        [self.loadingActivity setHidden:NO];
        [self.loadingActivity startAnimating];
        [self performSelectorInBackground:@selector(saveAnimationData) withObject:nil];
        isFileSaving = true;
    }
}
- (void)playButtonEnableDisable:(bool)isHidden
{
    [self.menuButton setEnabled:isHidden];
    [self.undoButton setEnabled:isHidden];
    [self.redoButton setEnabled:isHidden];
    [self.objectSelectionButton setEnabled:isHidden];
    [self.viewBT setEnabled:isHidden];
}
- (void)updateHelpImageView
{
    [self.helpButton setTag:0];
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
- (void)resumeRenderingAnimationScene
{
    animationScene->isExportingImages = false;
    animationScene->setDataForFrame(animationScene->currentFrame);
}
- (void)saveAnimationData
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* filePath = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, currentScene.sceneFile];
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath])
        [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
    std::string* outputFilePath = new std::string([filePath UTF8String]);
    animationScene->saveSceneData(outputFilePath);
    delete outputFilePath;
    
    [self performSelectorOnMainThread:@selector(saveThumbnail) withObject:nil waitUntilDone:YES];

    isFileSaving = false;
    isFileSaved = !(animationScene->changeAction = false);

    [self.saveButton setEnabled:YES];
    [self.saveloadingActivity stopAnimating];
    [self.saveloadingActivity setHidden:YES];
    [self.loadingActivity stopAnimating];
    [self.loadingActivity setHidden:YES];
    if (isHomePressed) {
        [self.homeLoadingActivity stopAnimating];
        [self.homeLoadingActivity setHidden:YES];
        [self gotoSceneselectionView];
    }
    [self.view setUserInteractionEnabled:YES];
}
- (void)saveThumbnail
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* imageFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, currentScene.sceneFile];
    
    animationScene->saveThumbnail((char*)[imageFilePath cStringUsingEncoding:NSUTF8StringEncoding]);

    animationScene->reloadKeyFrameMap();
    [self.frameCarouselView reloadData];
}
- (IBAction)homeButtonAction:(id)sender
{
    [self.homeButton setEnabled:NO];
    [self.homeLoadingActivity setHidden:NO];
    [self.homeLoadingActivity startAnimating];
    [self.loadingActivity setHidden:NO];
    [self.loadingActivity startAnimating];
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    if (animationScene->changeAction) {
        if (isFileSaved) {
            [self gotoSceneselectionView];
        }
        else {
            [self.view endEditing:YES];
            UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Save" message:@"Do you want to save the changes made to this scene?" delegate:self cancelButtonTitle:@"NO" otherButtonTitles:@"Yes", nil];
            [closeAlert setTag:PROJECT_BUTTON_TAG];
            closeAlert.delegate = self;
            [closeAlert show];
        }
    }
    else {
        [self performSelectorInBackground:@selector(HomeButtonActionInBGThread) withObject:nil];
    }
}
- (void)HomeButtonActionInBGThread
{
    isHomePressed = true;
    [self performSelectorOnMainThread:@selector(gotoSceneselectionView) withObject:nil waitUntilDone:NO];
}
- (IBAction)projectButtonAction:(id)sender
{
    if (isPlaying)
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    if (animationScene->changeAction) {
        if (isFileSaved) {
            [self gotoSceneselectionView];
        }
        else if (animationScene->actions.size() > 0) {
            [self.view endEditing:YES];
            UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Save" message:@"Do you want to save the changes made to this scene?" delegate:self cancelButtonTitle:@"NO" otherButtonTitles:@"Yes", nil];
            [closeAlert setTag:PROJECT_BUTTON_TAG];
            closeAlert.delegate = self;
            [closeAlert show];
        }
    }
    else {
        [self gotoSceneselectionView];
    }
}
- (IBAction)mirrorSwitchChanged:(id)sender
{
    self.mirrorSwitch.on = animationScene->switchMirrorState();
    [self showTipsViewForAction:FRAMES_HELP];
}

- (IBAction)undoAction:(id)sender
{
    [self setupEnableDisableControls];
    //    [self NormalHighLight];
    int returnValue2 = NOT_SELECTED;
    ACTION_TYPE actionType = (ACTION_TYPE)animationScene->undo(returnValue2);
    [self undoRedoButtonState:actionType];

    switch (actionType) {
    case DO_NOTHING: {
        break;
    }
    case DELETE_ASSET: {
        int nodeIndex = returnValue2;
        if (nodeIndex < self.assetsNamesInScene.count) {
            [self.assetsNamesInScene removeObjectAtIndex:nodeIndex];
            animationScene->removeObject(nodeIndex);
        }
        break;
    }
    case ADD_TEXT_IMAGE_BACK: {
        std::wstring name = animationScene->nodes[animationScene->nodes.size() - 1]->name;
        [self.assetsNamesInScene addObject:[self stringWithwstring:name]];
        break;
    }
    case ADD_ASSET_BACK: {
        int assetId = returnValue2;
        importPressed = NO;
        assetAddType = UNDO_ACTION;
        [self loadNodeInScene:assetId];
        break;
    }
    case SWITCH_FRAME: {
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:animationScene->currentFrame inSection:0];
        [self.frameCarouselView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
        [self HighlightFrame];
        [self.frameCarouselView reloadData];
        break;
    }
    case RELOAD_FRAMES: {
        [self.frameCarouselView reloadData];
        break;
    }
    case SWITCH_MIRROR:
        self.mirrorSwitch.on = animationScene->getMirrorState();
        break;
    default: {
        break;
    }
    }

    if (animationScene->actions.size() <= 0 || animationScene->currentAction <= 0)
        [self undoRedoButtonState:DEACTIVATE_UNDO];

    [self showTipsViewForAction:UNDO_PRESSED];
}

- (IBAction)renButtonAction:(id)sender
{
    if([Utility IsPadDevice ]){
        RenPro* renView = [[RenPro alloc] initWithNibName:@"RenPro" bundle:nil];
        self.popOverView = [[FPPopoverController alloc] initWithViewController:renView];
        self.popOverView.border = NO;
        self.popOverView.tint = FPPopoverWhiteTint;
        self.popOverView.contentSize = CGSizeMake(500.0, 400.0);
        [self.popOverView presentPopoverFromPoint:self.renButton.frame.origin];
        NSLog(@"This is it: %@", @"Render Button Is pressed");
    }
    else{
    RenPro* renView = [[RenPro alloc] initWithNibName:@"RenPro" bundle:nil];
    self.popOverView = [[FPPopoverController alloc] initWithViewController:renView];
    self.popOverView.border = NO;
    self.popOverView.tint = FPPopoverWhiteTint;
    self.popOverView.contentSize = CGSizeMake(400.0, 300.0);
    [self.popOverView presentPopoverFromPoint:self.renButton.frame.origin];
    NSLog(@"This is it: %@", @"Render Button Is pressed");
    }
}

- (IBAction)redoAction:(id)sender
{
    [self setupEnableDisableControls];
    [self NormalHighLight];

    int returnValue = animationScene->redo();
    [self undoRedoButtonState:returnValue];

    if (returnValue == DO_NOTHING) {
        //DO NOTHING
    }
    else if (returnValue == DELETE_ASSET) {
        if (animationScene->selectedNodeId < self.assetsNamesInScene.count) {
            [self.assetsNamesInScene removeObjectAtIndex:animationScene->selectedNodeId];
            animationScene->removeObject(animationScene->selectedNodeId);
        }
    }
    else if (returnValue == ADD_TEXT_IMAGE_BACK) {
        std::wstring name = animationScene->nodes[animationScene->nodes.size() - 1]->name;
        [self.assetsNamesInScene addObject:[self stringWithwstring:name]];
    }
    else if (returnValue == SWITCH_MIRROR) {
        self.mirrorSwitch.on = animationScene->getMirrorState();
    }
    else {
        if (returnValue != DEACTIVATE_UNDO && returnValue != DEACTIVATE_REDO && returnValue != DEACTIVATE_BOTH) {
            importPressed = NO;
            assetAddType = REDO_ACTION;
            [self loadNodeInScene:returnValue];
        }
    }

    if (animationScene->actions.size() <= 0 || animationScene->currentAction >= animationScene->actions.size())
        [self undoRedoButtonState:DEACTIVATE_REDO];

    NSIndexPath* toPath = [NSIndexPath indexPathForItem:animationScene->currentFrame inSection:0];
    [self.frameCarouselView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    [self HighlightFrame];
    [self.frameCarouselView reloadData];
    [self showTipsViewForAction:REDO_PRESSED];
}

- (void)undoRedoButtonState:(int)state
{
    switch (state) {
    case DEACTIVATE_UNDO:
        if ([self.undoButton isEnabled])
            [self.undoButton setEnabled:NO];
        if (![self.redoButton isEnabled])
            [self.redoButton setEnabled:YES];
        break;
    case DEACTIVATE_REDO:
        if ([self.redoButton isEnabled])
            [self.redoButton setEnabled:NO];
        if (![self.undoButton isEnabled])
            [self.undoButton setEnabled:YES];
        break;
    case DEACTIVATE_BOTH:
        if ([self.undoButton isEnabled])
            [self.undoButton setEnabled:NO];
        if ([self.redoButton isEnabled])
            [self.redoButton setEnabled:NO];
        break;
    default:
        if (![self.undoButton isEnabled])
            [self.undoButton setEnabled:YES];
        if (![self.redoButton isEnabled])
            [self.redoButton setEnabled:YES];
        break;
    }

    if (animationScene) {
        if (animationScene->actions.size() > 0 && animationScene->currentAction > 0)
            [self.undoButton setEnabled:YES];
        if (animationScene->actions.size() > 0 && animationScene->currentAction < (animationScene->actions.size()))
            [self.redoButton setEnabled:YES];
    }
}
#pragma mark -
#pragma AlertView
- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if (alertView.tag == PROJECT_BUTTON_TAG) {
        if (buttonIndex == OK_BUTTON_INDEX) {
            [self.loadingActivity setHidden:NO];
            [self.loadingActivity startAnimating];
            isFileSaving = true;
            [self saveAnimationData];
        }
    }
    alertView.delegate = nil;
}
- (void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (alertView.tag == DELETE_BUTTON_TAG) {
        if (buttonIndex == DELETE_OBJECT_BUTTON) {
            [self.view endEditing:YES];
            UIAlertView* deleteAlert = [[UIAlertView alloc] initWithTitle:@"Delete Object" message:@"Do you want to delete the object?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
            [deleteAlert setTag:DELETE_CONFORM_OBJECT];
            [deleteAlert show];
        }
        else if (buttonIndex == DELETE_ANIMATION_BUTTON) {
            [self.view endEditing:YES];
            UIAlertView* deleteAlert = [[UIAlertView alloc] initWithTitle:@"Delete Animation" message:@"Do you want to delete the Animation?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
            [deleteAlert setTag:DELETE_CONFORM_ANIMATION];
            [deleteAlert show];
        }
        else {
            [self showTipsViewForAction:OBJECT_IMPORTED];
        }
        [_frameCarouselView reloadData];
        if ([self.deleteButton tag] == 1) {
            [self placeView:self.deleteBTView fromValue:CGPointMake(0.0, 0.0) toValue:CGPointMake(95.0, 0.0) active:YES];
            [self.deleteButton setTag:2];
        }
        [self changeAllButtonBG];
    }
    else if (alertView.tag == DELETE_CONFORM_OBJECT) {
        if (buttonIndex == CANCEL_BUTTON_INDEX) {
            [self showTipsViewForAction:OBJECT_IMPORTED];
        }
        else if (buttonIndex == OK_BUTTON_INDEX) {
            if (animationScene->selectedNodeId != NOT_SELECTED) {
                if (animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_TEXT || animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_IMAGE)
                    animationScene->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_DELETE, 0);
                else
                    animationScene->storeAddOrRemoveAssetAction(ACTION_NODE_DELETED, 0);
                [self.assetsNamesInScene removeObjectAtIndex:animationScene->selectedNodeId];
                animationScene->removeObject(animationScene->selectedNodeId);
                isFileSaved = !(animationScene->changeAction = true);
                [self undoRedoButtonState:DEACTIVATE_BOTH];
            }
            [self setupEnableDisableControls];
            [_frameCarouselView reloadData];
            [self showTipsViewForAction:SCENE_BEGINS];
        }
        [self changeAllButtonBG];
    }
    else if (alertView.tag == SAVE_BUTTON_TAG) {
        isHomePressed = false;
        if (buttonIndex == CANCEL_BUTTON_INDEX) {
            [self gotoSceneselectionView];
        }
        else if (buttonIndex == OK_BUTTON_INDEX) {
            [self.loadingActivity setHidden:NO];
            [self.loadingActivity startAnimating];
            [self.view setUserInteractionEnabled:NO];
        }
    }
    else if (alertView.tag == PROJECT_BUTTON_TAG) {
        isHomePressed = true;
        if (buttonIndex == OK_BUTTON_INDEX) {
            /*
            [self.loadingActivity setHidden:NO];
            [self.loadingActivity startAnimating];
            isFileSaving = true;
            [self performSelectorInBackground:@selector(saveAnimationData) withObject:nil];
             */
            [self.view setUserInteractionEnabled:NO];
        }
        else if (buttonIndex == CANCEL_BUTTON_INDEX) {
            [self gotoSceneselectionView];
            alertView.delegate = nil;
        }

        //alertView.delegate = nil;
    }
    else if (alertView.tag == DELETE_CONFORM_ANIMATION) {
        if (buttonIndex == CANCEL_BUTTON_INDEX) {
        }
        else if (buttonIndex == OK_BUTTON_INDEX) {
            bool isKeySetForNode = animationScene->removeAnimationForSelectedNodeAtFrame(animationScene->currentFrame);
            if (!isKeySetForNode) {
                [self.view endEditing:YES];
                UIAlertView* deleteAlert = [[UIAlertView alloc] initWithTitle:@"Info" message:@"There is no animation in this frame." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil, nil];
                [deleteAlert setTag:DELETE_CONFORM_ANIMATION];
                [deleteAlert show];
            }
            else {
                isFileSaved = !(animationScene->changeAction = true);
                [self undoRedoButtonState:DEACTIVATE_BOTH];
                [self.frameCarouselView reloadData];
            }
        }
        [self changeAllButtonBG];
    }
    else if (alertView.tag == ADD_BUTTON_TAG) {
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
    [self showTipsViewForAction:OBJECT_IMPORTED];
}
#pragma mark -
#pragma mark TableView
- (NSInteger)tableView:(UITableView*)tableView numberOfRowsInSection:(NSInteger)section
{
    return [self.assetsNamesInScene count];
}
- (UITableViewCell*)tableView:(UITableView*)tableView cellForRowAtIndexPath:(NSIndexPath*)indexPath
{
    static NSString* cellIdentifier = @"Cell";
    UITableViewCell* cell = [tableView dequeueReusableCellWithIdentifier:cellIdentifier];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:cellIdentifier];
    }
    cell.backgroundColor = [UIColor colorWithRed:22.0f / 255.0f
                                           green:23.0f / 255.0f
                                            blue:23.0f / 255.0f
                                           alpha:1.0f];
    cell.textColor = [UIColor whiteColor];
    cell.textLabel.text = [self.assetsNamesInScene objectAtIndex:indexPath.row];
    return cell;
}
- (void)tableView:(UITableView*)tableView didSelectRowAtIndexPath:(NSIndexPath*)indexPath
{
    [self changeAllButtonBG];
    [self objectSelectionCompleted:(int)indexPath.row];
    [_frameCarouselView reloadData];
    isTapped = true;
    [self showAdvancedTipsForGestures];
}
#pragma mark -

#pragma mark CollectionView
- (NSInteger)collectionView:(UICollectionView*)collectionView numberOfItemsInSection:(NSInteger)section
{
    if (isViewDisplayed)
        return animationScene->totalFrames;
    else
        return totalFrames;
}
- (FrameCellView*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    FrameCellView* cell = [self.frameCarouselView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
    if (isViewDisplayed) {
        if (animationScene->isKeySetForFrame.find((int)indexPath.row) != animationScene->isKeySetForFrame.end() && (indexPath.row) == animationScene->currentFrame) {
            cell.backgroundColor = [UIColor blackColor];
            cell.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                     green:156.0f / 255.0f
                                                      blue:156.0f / 255.0f
                                                     alpha:1.0f]
                                         .CGColor;
            cell.layer.borderWidth = 2.0f;
        }
        else if (animationScene->isKeySetForFrame.find((int)indexPath.row) != animationScene->isKeySetForFrame.end()) {
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
        if ((indexPath.row) == animationScene->currentFrame) {
            cell.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                     green:156.0f / 255.0f
                                                      blue:156.0f / 255.0f
                                                     alpha:1.0f]
                                         .CGColor;
            cell.layer.borderWidth = 2.0f;
        }
    }
    if ([self.frameButton tag] == 2)
        cell.framesLabel.text = [NSString stringWithFormat:@"%.2fs", ((float)indexPath.row + 1) / 24];
    else
        cell.framesLabel.text = [NSString stringWithFormat:@"%d", (int)indexPath.row + 1];
    cell.framesLabel.adjustsFontSizeToFitWidth = YES;
    cell.framesLabel.minimumFontSize = 3.0;
    return cell;
}
- (void)collectionView:(UICollectionView*)collectionView didSelectItemAtIndexPath:(NSIndexPath*)indexPath
{
    [self NormalHighLight];
    animationScene->previousFrame = animationScene->currentFrame;
    animationScene->currentFrame = (int)indexPath.row;
    [self HighlightFrame];
    [self.frameCarouselView reloadData];
    animationScene->switchFrame((float)animationScene->currentFrame);
    [self showTipsViewForAction:FRAMES_HELP];
    [self undoRedoButtonState:DEACTIVATE_BOTH];
}
- (void)HighlightFrame
{
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:animationScene->currentFrame inSection:0];
    UICollectionViewCell* todatasetCell = [self.frameCarouselView cellForItemAtIndexPath:toPath];
    todatasetCell.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                      green:156.0f / 255.0f
                                                       blue:156.0f / 255.0f
                                                      alpha:1.0f]
                                          .CGColor;
    todatasetCell.layer.borderWidth = 2.0f;
}
- (void)NormalHighLight
{
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:animationScene->currentFrame inSection:0];
    UICollectionViewCell* todatasetCell = [self.frameCarouselView cellForItemAtIndexPath:toPath];
    todatasetCell.layer.borderColor = [UIColor colorWithRed:61.0f / 255.0f green:62.0f / 255.0f blue:63.0f / 255.0f alpha:1.0f].CGColor;
    todatasetCell.layer.borderWidth = 2.0f;
    //todatasetCell.backgroundColor = [UIColor colorWithRed:61.0f/255.0f green:62.0f/255.0f blue:63.0f/255.0f alpha:1.0f];
}
#pragma mark -

- (void)changeAllButtonBG
{
    [self.selectBTiconView setBackgroundColor:[UIColor colorWithRed:55.0f / 255.0f green:55.0f / 255.0f blue:58.0f / 255.0f alpha:1]];
    [self.propertiesBTiconView setBackgroundColor:[UIColor colorWithRed:55.0f / 255.0f green:55.0f / 255.0f blue:58.0f / 255.0f alpha:1]];
    [self.moveBTiconView setBackgroundColor:[UIColor colorWithRed:55.0f / 255.0f green:55.0f / 255.0f blue:58.0f / 255.0f alpha:1]];
    [self.rotateBTiconView setBackgroundColor:[UIColor colorWithRed:55.0f / 255.0f green:55.0f / 255.0f blue:58.0f / 255.0f alpha:1]];
    [self.scaleBTiconView setBackgroundColor:[UIColor colorWithRed:55.0f / 255.0f green:55.0f / 255.0f blue:58.0f / 255.0f alpha:1]];
    [self.deleteBTiconView setBackgroundColor:[UIColor colorWithRed:55.0f / 255.0f green:55.0f / 255.0f blue:58.0f / 255.0f alpha:1]];
    if (animationScene->controlType == MOVE)
        [self.moveBTiconView setBackgroundColor:[UIColor blackColor]];
    else if (animationScene->controlType == ROTATE)
        [self.rotateBTiconView setBackgroundColor:[UIColor blackColor]];
}
- (void)scrolltoEnd
{
    animationScene->previousFrame = animationScene->currentFrame;
    timerCount++;
    [self NormalHighLight];
    if (scrollConstant == SCROLL_LAST_FRAME)
        animationScene->currentFrame = animationScene->totalFrames - 1;
    else if (scrollConstant == SCROLL_FIRST_FRAME)
        animationScene->currentFrame = 0;
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:animationScene->currentFrame inSection:0];
    [self.frameCarouselView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    animationScene->switchFrame(animationScene->currentFrame);
    if (timerCount >= 4) {
        [self performSelectorOnMainThread:@selector(stopScrolling) withObject:nil waitUntilDone:YES];
    }
}
- (void)animationEffect
{
    switch (sliderId) {
    case OBJECT_SELECTION:
        [self viewDropDown:self.objectSelectionView];
        break;
    case CAMERA_PROPERTY:
        [self viewDropDown:self.cameraPropertyView];
        break;
    case MESH_PROPERTY:
        [self viewDropDown:self.meshPropertyView];
        break;
    case LIGHT_PROPERTY:
        [self viewDropDown:self.lightPropertyView];
        break;
    case VARIOUS_VIEW:
        [self viewDropDown:self.viewBTScroll];
        break;
    default:
        break;
    }
}
- (void)viewDropDown:(UIView*)dropDownView
{
    [dropDownView setHidden:NO];
    float height = dropDownView.frame.size.height;
    dropDownView.frame = CGRectMake(dropDownView.frame.origin.x, dropDownView.frame.origin.y, dropDownView.frame.size.width, 0);

    [UIView animateWithDuration:1.0
        delay:0.01
        options:UIViewAnimationCurveEaseIn
        animations:^{

            dropDownView.frame = CGRectMake(dropDownView.frame.origin.x, dropDownView.frame.origin.y, dropDownView.frame.size.width, height);
        }
        completion:^(BOOL finished){

        }];
}
- (void)stopScrolling
{
}
- (void)playTimerTarget
{
    if (animationScene->currentFrame + 1 < animationScene->totalFrames) {
        [self NormalHighLight];
        animationScene->currentFrame++;
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:animationScene->currentFrame inSection:0];
        [self.frameCarouselView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
        [self HighlightFrame];
        animationScene->setLightingOff();
        animationScene->switchFrame((float)animationScene->currentFrame);
    }
    else if (animationScene->currentFrame + 1 >= animationScene->totalFrames) {
        [self showTipsViewForAction:FRAMES_HELP];
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    }
    else if (animationScene->currentFrame == animationScene->totalFrames) {
        [self performSelectorOnMainThread:@selector(stopPlaying) withObject:nil waitUntilDone:YES];
    }
    animationScene->setDataForFrame(animationScene->currentFrame);
}
- (void)stopPlaying
{
    //[self setInterfaceVisibility:NO];
    animationScene->setLightingOn();
    [self playButtonEnableDisable:YES];
    [self.playButton setImage:[UIImage imageNamed:@"play-video.png"] forState:UIControlStateNormal];
    [self.playButton setTitle:@"   PLAY" forState:UIControlStateNormal];
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:animationScene->currentFrame inSection:0];
    UICollectionViewCell* todatasetCell = [self.frameCarouselView cellForItemAtIndexPath:toPath];
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
        animationScene->isPlaying = false;
    }
    animationScene->switchFrame(animationScene->currentFrame);
}

- (void)gotoSceneselectionView
{
    if (isFileSaving) {
        [self.homeLoadingActivity stopAnimating];
        [self.homeLoadingActivity setHidden:YES];
        [self.homeButton setEnabled:YES];
        [self.view setUserInteractionEnabled:YES];
        return;
    }

    [self.view setUserInteractionEnabled:NO];
    AppDelegate* appDelegate = (AppDelegate*)[[UIApplication sharedApplication] delegate];
    isHomePressed = true;
    isViewDisplayed = false;
    if ([Utility IsPadDevice]) {
        [self dismissViewControllerAnimated:TRUE completion:Nil];
        SceneSelectionViewControllerPad* sceneSelection = [[SceneSelectionViewControllerPad alloc] initWithNibName:@"SceneSelectionControllerPad" bundle:nil SceneNo:IndexOfCarousel isAppFirstTime:false];
        [appDelegate.window setRootViewController:sceneSelection];
    }
    else {
        [self dismissViewControllerAnimated:TRUE completion:Nil];
        SceneSelectionViewControllerPad* sceneSelection = [[SceneSelectionViewControllerPad alloc] initWithNibName:@"SceneSelectionControllerPhone" bundle:nil SceneNo:IndexOfCarousel isAppFirstTime:false];
        [appDelegate.window setRootViewController:sceneSelection];
    }
    [self performSelectorOnMainThread:@selector(removeSGEngine) withObject:nil waitUntilDone:YES];
    [self removeFromParentViewController];
}
- (void)setupEnableDisableControls
{
    [self loadingViewStatus:NO];
    [self.saveloadingActivity setHidden:YES];
    [self.viewBTScroll setClipsToBounds:YES];
    [self.cameraPropertyView setClipsToBounds:YES];
    [self.lightPropertyView setClipsToBounds:YES];
    [self.meshPropertyView setClipsToBounds:YES];
    [self.frameView setClipsToBounds:YES];
    [self.objectSelectionView setClipsToBounds:YES];
    [self.topView setClipsToBounds:YES];
    [self.objectSelectionView setHidden:YES];
    [self.meshPropertyView setHidden:YES];
    [self.lightPropertyView setHidden:YES];
    [self.cameraPropertyView setHidden:YES];
    [self.viewBTView setHidden:YES];
    [self.viewBTiconView setHidden:YES];
    [self.viewBTScroll setHidden:YES];
    [self.menuViewSlider setHidden:YES];
    [self.menuButton setTag:2];
    [self.rotateButton setTag:2];
    [self.scaleButton setTag:2];
    [self.moveButton setTag:2];
    [self.viewBT setTag:2];
    [self.deleteButton setTag:2];
    [self.objectSelectionButton setTag:2];
    [self.objectPropertiesButton setTag:2];
    [self.moveButton setHidden:NO];
    NSString* extension;
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
        extension = @"-phone";
    else
        extension = @"";
    [self.menuButton setImage:[UIImage imageNamed:[NSString stringWithFormat:@"Menu%@.png", extension]] forState:UIControlStateNormal];
    if (isViewDisplayed) {
        [self HighlightFrame];
        if (animationScene->selectedNodeId <= NODE_LIGHT || animationScene->getSelectedNodeType() == NODE_ADDITIONAL_LIGHT) {
            [self deleteButtonEnable];
            [self scaleButtonDisable];
            [self rotateButtonDisable];
            self.mirrorSwitch.on = false;
            [self.mirrorSwitch setHidden:YES];
            [self.mirrorLabel setHidden:YES];
        }
        else {

            if (animationScene->isJointSelected) {
                [self deleteButtonDisable];
                if (animationScene->selectedNodeId != NOT_SELECTED && animationScene->nodes[animationScene->selectedNodeId]->getType() == NODE_TEXT)
                    [self scaleButtonEnable];
                else
                    [self scaleButtonDisable];

                self.mirrorSwitch.on = animationScene->getMirrorState();
                if (animationScene->nodes[animationScene->selectedNodeId]->isMirrorEnabled) {
                    [self.mirrorSwitch setHidden:NO];
                    [self.mirrorLabel setHidden:NO];
                }
                else {
                    //[self.mirrorSwitch setOn:NO];
                    if (self.mirrorSwitch.on)
                        self.mirrorSwitch.on = animationScene->switchMirrorState();
                    [self.mirrorSwitch setHidden:YES];
                    [self.mirrorLabel setHidden:YES];
                }
            }
            else {
                [self deleteButtonEnable];
                [self scaleButtonEnable];
                if (self.mirrorSwitch.on)
                    self.mirrorSwitch.on = animationScene->switchMirrorState();
                [self.mirrorSwitch setHidden:YES];
                [self.mirrorLabel setHidden:YES];
            }
            [self rotateButtonEnable];
        }
        if (animationScene->selectedNodeId == NOT_SELECTED) {
            [self changeAllButtonBG];
            [self deleteButtonDisable];
            [self.moveBTiconView setHidden:YES];
            [self rotateButtonDisable];
            [self objectPropertiesDisable];
        }
        else {
            [self changeAllButtonBG];
            if (animationScene->controlType == MOVE) {
                [self.moveBTiconView setBackgroundColor:[UIColor blackColor]];
            }
            else if (animationScene->controlType == ROTATE) {
                [self.rotateBTiconView setBackgroundColor:[UIColor blackColor]];
            }
            [self.moveBTiconView setHidden:NO];
            if (animationScene->isJointSelected)
                [self objectPropertiesDisable];
            else
                [self objectPropertiesEnable];
        }
        if (animationScene->selectedNodeId == 0) {
            [self rotateButtonEnable];
        }
    }
    [self.frameCarouselView reloadData];
}
- (void)objectPropertiesEnable
{
    [self.propertiesBTiconView setHidden:NO];
}
- (void)objectPropertiesDisable
{
    [self.propertiesBTiconView setHidden:YES];
}
- (void)rotateButtonEnable
{
    [self.rotateBTiconView setHidden:NO];
}
- (void)rotateButtonDisable
{
    [self.rotateBTiconView setHidden:YES];
}
- (void)deleteButtonEnable
{
    [self.deleteBTiconView setHidden:NO];
}
- (void)deleteButtonDisable
{
    [self.deleteBTiconView setHidden:YES];
}
- (void)scaleButtonEnable
{
    [self.scaleBTiconView setHidden:NO];
}
- (void)scaleButtonDisable
{
    [self.scaleBTiconView setHidden:YES];
}
- (void)removeSGEngine
{
    if (displayLink) {
        [displayLink invalidate];
        displayLink = nil;
    }
    if (animationScene) {
        delete animationScene;
        animationScene = NULL;
    }
}
#pragma delegate functions

- (IBAction)tipsSwitchPressed:(id)sender
{
    [[AppHelper getAppHelper] saveBoolUserDefaults:!([self.tipsSwitch isOn])withKey:@"AdvanceHelpDisabled"];
    [self advancedHelp] ? [self showTipsViewForAction:NO_ACTION] : [self hideTipsView];
}
- (BOOL)advancedHelp
{
    return !([[AppHelper getAppHelper] userDefaultsBoolForKey:@"AdvanceHelpDisabled"]);
}
- (void)setTipsSwitchStatus
{
    [self.tipsSwitch setOn:[self advancedHelp]];
    [self advancedHelp] ? [self showTipsViewForAction:NO_ACTION] : [self hideTipsView];
}
- (void)scalePropertyChanged:(float)XValue YValue:(float)YValue ZValue:(float)ZValue
{
    if(animationScene->selectedNodeId < 0 || animationScene->selectedNodeId > animationScene->nodes.size())
        return;

    animationScene->changeObjectScale(Vector3(XValue, YValue, ZValue), false);
    isFileSaved = !(animationScene->changeAction = true);
    [_frameCarouselView reloadData];
}
- (void)scaleValueForAction:(float)XValue YValue:(float)YValue ZValue:(float)ZValue
{
    if(animationScene->selectedNodeId < 0 || animationScene->selectedNodeId > animationScene->nodes.size())
        return;
    
    animationScene->changeObjectScale(Vector3(XValue, YValue, ZValue), true);
    //[self showTipsViewForAction:OBJECT_MOVED];
    isFileSaved = !(animationScene->changeAction = true);
    [_frameCarouselView reloadData];
    [self undoRedoButtonState:DEACTIVATE_BOTH];
}
- (void)objectSelectionCompleted:(int)assetIndex
{
    [self.objectSelectionView setHidden:YES];
    animationScene->selectObject(assetIndex);
    [self setupEnableDisableControls];
}
- (void)meshPropertyChanged:(float)brightness Specular:(float)specular Lighting:(BOOL)light Visible:(BOOL)visible
{
    if(animationScene->selectedNodeId < 0 || animationScene->selectedNodeId > animationScene->nodes.size())
        return;
    
    if (animationScene->nodes[animationScene->selectedNodeId]->props.isLighting != light || animationScene->nodes[animationScene->selectedNodeId]->props.isVisible != visible) { //switch action
        animationScene->changeMeshProperty(brightness, specular, light, visible, true);
    }
    else { //slider action
        animationScene->changeMeshProperty(brightness, specular, light, visible, false);
    }
    isFileSaved = !(animationScene->changeAction = true);
}
- (void)meshSliderEnds
{
    if(animationScene->selectedNodeId < 0 || animationScene->selectedNodeId > animationScene->nodes.size())
        return;

    animationScene->changeMeshProperty(self.brightnessSlider.value, self.specularSlider.value, self.lightingSwitch.isOn, self.visibleSwitch.isOn, true);
    isFileSaved = !(animationScene->changeAction = true);
}
- (IBAction)lightValueChangedAction:(id)sender
{
    self.colorDisplayLabel.textColor = [UIColor colorWithRed:self.redSlider.value green:self.greenSlider.value blue:self.blueSlider.value alpha:1.0];
    [self lightPropertyChanged:self.redSlider.value Green:self.greenSlider.value Blue:self.blueSlider.value Shadow:self.shadowSlider.value];
}
- (IBAction)meshValueChangedAction:(id)sender
{
    [self meshPropertyChanged:self.brightnessSlider.value Specular:self.specularSlider.value Lighting:self.lightingSwitch.isOn Visible:self.visibleSwitch.isOn];
}

- (IBAction)cameraValueChangedAction:(id)sender
{
    cameraResolution = (int)self.resolutionSegment.selectedSegmentIndex;
    NSNumber* cameraResolutionNumber = [NSNumber numberWithInt:(int)self.resolutionSegment.selectedSegmentIndex];
    [[AppHelper getAppHelper] saveToUserDefaults:cameraResolutionNumber withKey:currentScene.name];
    self.fovLabel.text = [NSString stringWithFormat:@"%0.2f", self.fovSlider.value];
    [self cameraPropertyChanged:self.fovSlider.value Resolution:(int)self.resolutionSegment.selectedSegmentIndex];
}
- (void)lightPropertyChanged:(float)red Green:(float)green Blue:(float)blue Shadow:(float)shadow
{
    if(animationScene->selectedNodeId < 0 || animationScene->selectedNodeId > animationScene->nodes.size())
        return;

    animationScene->changeLightProperty(red, green, blue, shadow, true);
    isFileSaved = !(animationScene->changeAction = true);
}
- (void)cameraPropertyChanged:(float)fov Resolution:(int)resolution
{
    printf("Camera Prop changed %f ",fov);
    if (animationScene->cameraResolutionType != resolution) {
        [self showTipsViewForAction:CAMERA_PROPS];
        animationScene->changeCameraProperty(fov, resolution, true);
    } //switch action
    else
        animationScene->changeCameraProperty(fov, resolution, false); //slider action
    isFileSaved = !(animationScene->changeAction = true);
}
- (void)fovSliderEnds
{
    animationScene->changeCameraProperty(self.fovSlider.value, (int)self.resolutionSegment.selectedSegmentIndex, true);
    [self showTipsViewForAction:CAMERA_PROPS];
    isFileSaved = !(animationScene->changeAction = true);
}
- (void)cameraResolutionChanged:(int)changedResolutionType
{
    self.resolutionSegment.selectedSegmentIndex = changedResolutionType;
    self.fovSlider.value = animationScene->cameraFOV;
    [self cameraValueChangedAction:nil];
}

- (void) setShaderTypeForRendering:(int)shaderType
{
    animationScene->resetMaterialTypes((shaderType == 0) ? false : true);
}

- (void)renderFrame:(int)frame withType:(int)shaderType andRemoveWatermark:(bool)removeWatermark
{
    if (isAppInBG)
        return;
    animationScene->isExportingImages = true;
    animationScene->setDataForFrame(frame);
    NSString* tempDir = NSTemporaryDirectory();
    NSString* imageFilePath = [NSString stringWithFormat:@"%@/r-%d.png", tempDir, frame];
    animationScene->renderAndSaveImage((char*)[imageFilePath cStringUsingEncoding:NSUTF8StringEncoding], shaderType, false, removeWatermark);
}

- (NSMutableArray*) exportSGFDsWith:(int)startFrame EndFrame:(int)endFrame
{
    NSMutableArray *fileNamesToZip = [[NSMutableArray alloc] init];
    vector<string> textureFileNames = animationScene->generateSGFDFiles(startFrame, endFrame);
    
    for(int i = 0; i < textureFileNames.size(); i++)
        [fileNamesToZip addObject:[NSString stringWithCString:textureFileNames[i].c_str() encoding:NSUTF8StringEncoding]];
    
    return fileNamesToZip;
}

- (CGPoint) getCameraResolution
{
    
    float width = RESOLUTION[animationScene->cameraResolutionType][0];
    float height = RESOLUTION[animationScene->cameraResolutionType][1];
    
    return CGPointMake(width, height);
}

- (void)loadingViewStatus:(BOOL)status
{
    if (status) {
        [self.loadingActivity setHidden:NO];
        [self.loadingActivity startAnimating];
    }
    else {
        [self.loadingActivity setHidden:YES];
        [self.loadingActivity stopAnimating];
    }
}

- (void)textAddCompleted:(NSDictionary*)textDetails
{
    [self resetContext];
    assetAddType = IMPORT_ASSET_ACTION;
    importPressed = true;
    textColor = Vector4([[textDetails objectForKey:@"red"] floatValue], [[textDetails objectForKey:@"green"] floatValue], [[textDetails objectForKey:@"blue"] floatValue], [[textDetails objectForKey:@"alpha"] floatValue]);

    fontFilePath = [textDetails objectForKey:@"font"];

    bool status = [self loadNodeInScene:ASSET_TEXT_RIG AssetId:0 AssetName:[self getwstring:[textDetails objectForKey:@"text"] ] Width:[[textDetails objectForKey:@"size"] intValue] Height:[[textDetails objectForKey:@"bevel"] intValue]];
    if (!status)
        return;

    animationScene->storeAddOrRemoveAssetAction(ACTION_TEXT_IMAGE_ADD, 0, [fontFilePath UTF8String]);
    [self.assetsNamesInScene addObject:[NSString stringWithFormat:@"TEXT: %@", [textDetails objectForKey:@"text"]]];

    if (animationScene->actions.size() > 0) {
        if (animationScene->currentAction > 0)
            [self.undoButton setEnabled:YES];
        if (animationScene->currentAction < animationScene->actions.size())
            [self.redoButton setEnabled:YES];
    }
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

#pragma end

#pragma Premium Upgrade functions

-(void)showUpgradeView
{
    if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]){
        PremiumUpgardeVCViewController* upgradeView;
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
    else
        [self premiumUnlocked];
}

-(void)upgradeButtonPressed
{
    
}

-(void)premiumUnlocked
{
    [self importLightIntoScene];
}

-(void)premiumUnlockedCancelled
{
    
}

-(void)statusForOBJImport:(NSNumber *)object
{
    
}


#pragma end

- (void)dealloc
{
    if (!isMetalSupported) {
        if ([EAGLContext currentContext] == _context)
            [EAGLContext setCurrentContext:nil];
    }
    cache = nil;
    animationScene = nil;
    displayLink = nil;
    currentScene = nil;
    imgData = nil;
    imgSalt = nil;
    _imagePicker = nil;
    popOverView = nil;
    _scaleProperties = nil;
    _assetsNamesInScene = nil;
    _cameraPropertyView = nil;
    _lightPropertyView = nil;
    _meshPropertyView = nil;
    self.undoActions = nil;
    _loadingActivity = nil;
    _menuViewSlider = nil;
    _selectBTiconView = nil;
    _selectBTView = nil;
    _moveBTiconView = nil;
    _moveBTView = nil;
    _rotateBTiconView = nil;
    _rotateBTView = nil;
    _scaleBTiconView = nil;
    _scaleBTView = nil;
    _propertiesBTiconView = nil;
    _propertiesBTView = nil;
    _deleteBTiconView = nil;
    _deleteBTView = nil;
    _viewBTiconView = nil;
    _viewBTView = nil;
    _viewBTScroll = nil;
    _objectSelectionView = nil;
    _objectListTableView = nil;
    _meshPropertyView = nil;
    _colorDisplayLabel = nil;
    _lightPropertyView = nil;
    _fovLabel = nil;
    _cameraPropertyView = nil;
    self.frameCarouselView = nil;
    self.renderView = nil;
    self.priceLocale = nil;
    _imagePicker = nil;
    _renderView = nil;
}

@end
