	//
	//  AnimationEditorViewControllerPad.h
	//  Iyan3D
	//
	//  Created by Harishankar Narayanan on 06/01/14.
	//  Copyright (c) 2014 Smackall Games. All rights reserved.
	//

#define OPENGL_RENDERER
#undef METAL_RENDERER

#if !(TARGET_IPHONE_SIMULATOR)
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#endif
#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>
#import "CacheSystem.h"
#import "SceneItem.h"
#import "AssetSelectionViewControllerPad.h"
#import "AnimationSelectionViewController.h"
#import "ImageImportViewController.h"
#import "TextImportViewController.h"
#import "RenderingViewController.h"
#import "ScaleViewController.h"
#import "FPPopoverController.h"
#import "HelpViewController.h"
#import "SGEngineCommon.h"
#import "RenderingView.h"
#import "GAI.h"
#import "RenPro.h"

#define OBJFile 6
#define TEXT_FILE 4
#define IMAGE_FILE 5

class SGAnimationScene;
class SGAction;
@interface AnimationEditorViewControllerPad : GAITrackedViewController <UIGestureRecognizerDelegate, UIAlertViewDelegate, UINavigationControllerDelegate,UICollectionViewDataSource, UICollectionViewDelegate,UITableViewDataSource, UITableViewDelegate,AssetSelectionViewControllerPadDelegate,ScalePropertiesViewControllerDelegate,TextImportViewControllerDelegate,HelpViewControllerDelegate, RenderingViewControllerDelegate , ImageImportViewControllerDelgate, AnimationSelectionViewControllerDelegate, PremiumUpgardeVCViewControllerDelegate> {
    CacheSystem* cache;
    CADisplayLink *displayLink;
    NSTimer * displayTimer;
    int     assetAddType , totalFrames , previousAction , lightCount;
    Vector4 textColor;
    int     TopbarViewHeight,frameBarViewHeight;
    NSString *fontFilePath;
    int     touchCountTracker;
	bool    isViewDisplayed,isMetalSupported;
	bool    isViewLoaded;
    bool    isPanned,isTapped;
    float   frameInFloat,screenScale,screenWidth,screenHeight,lightColorRed, lightColorGreen, lightColorBlue,cameraFOV;
    int     cameraResolution,scrollConstant,timerCount,timerI,sliderId,propertyViewWidth,propertyViewHeight;
    BOOL    importPressed;
    SceneItem *currentScene;
	
    
    bool        isFileSaved;
    UIImage *imgData;
    NSString *imgSalt;
    int IndexOfCarousel;
    
    bool    isPlaying;
    NSTimer *playTimer;
    bool isHomePressed,isFileSaving,isAppInBG;
    NSDate *lastTipTime;
    
    Vector3 sceneLightColor;
    float sceneShadow;
    
    //---
    
#if !(TARGET_IPHONE_SIMULATOR)
    CAMetalLayer *_metalLayer;
    id <CAMetalDrawable> _currentDrawable;
    MTLRenderPassDescriptor *_renderPassDescriptor;
    
    // controller
    id <MTLBuffer> _dynamicConstantBuffer;
    
    // renderer
    id <MTLDevice> _device;
    id <MTLCommandQueue> _commandQueue;
    id <MTLLibrary> _defaultLibrary;
    id <MTLBuffer> vertexBuffer;
    id <MTLDepthStencilState> _depthState;
    id <MTLTexture> _depthTex;
    float _rotation;
    id <MTLCommandBuffer> CMDBuffer;
    id <MTLRenderCommandEncoder> RenderCMDBuffer;
    id <CAMetalDrawable> drawable;
#endif
    
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    GLuint _colorRenderBuffer;
    GLuint _depthRenderBuffer;
    GLuint _frameBuffer;
}

@property (strong, atomic) NSLocale *priceLocale;
@property (strong, atomic) NSMutableArray *assetsNamesInScene;
@property (strong, nonatomic) NSMutableArray *undoActions;
@property (weak, nonatomic) IBOutlet UILabel *sceneName;

@property (weak, nonatomic) IBOutlet UIButton *homeButton;
@property (weak, nonatomic) IBOutlet UIButton *projectButton;
@property (weak, nonatomic) IBOutlet UIButton *menuButton;
@property (weak, nonatomic) IBOutlet UIButton *saveButton;
@property (weak, nonatomic) IBOutlet UICollectionView *frameCarouselView;
@property (weak, nonatomic) IBOutlet UIButton *addFrameButton;


@property (weak, nonatomic) IBOutlet UIButton *importAssetButton;
@property (weak, nonatomic) IBOutlet UIButton *importTextButton;
@property (weak, nonatomic) IBOutlet UIButton *importImageButton;
@property (weak, nonatomic) IBOutlet UIButton *importAnimationButton;
@property (weak, nonatomic) IBOutlet RenderingView *renderView;

@property (weak, nonatomic) IBOutlet UIButton *objectSelectionButton;
@property (weak, nonatomic) IBOutlet UIButton *objectPropertiesButton;
@property (weak, nonatomic) IBOutlet UIButton *undoButton;
@property (weak, nonatomic) IBOutlet UIButton *redoButton;
@property (weak, nonatomic) IBOutlet UIButton *deleteButton;

@property (nonatomic, strong) FPPopoverController* popOverView;
@property (nonatomic, strong) ScaleViewController *scaleProperties;

@property (nonatomic, strong) UIImagePickerController *imagePicker;
@property (weak, nonatomic) IBOutlet UIView *pointerView;
@property (weak, nonatomic) IBOutlet UIButton *moveButton;
@property (weak, nonatomic) IBOutlet UIButton *rotateButton;
@property (weak, nonatomic) IBOutlet UIButton *scaleButton;
@property (weak, nonatomic) IBOutlet UIButton *helpButton;
@property (weak, nonatomic) IBOutlet UIButton *renButton;
@property (weak, nonatomic) IBOutlet UIButton *movefront;
@property (weak, nonatomic) IBOutlet UIButton *movelast;
@property (weak, nonatomic) IBOutlet UIButton *playButton;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *loadingActivity;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *saveloadingActivity;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *homeLoadingActivity;
@property (weak, nonatomic) IBOutlet UIButton* viewBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewtopBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewbottomBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewrightBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewleftBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewfrontBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewbackView;
@property (weak, nonatomic) IBOutlet UIView* menuViewSlider;
@property (weak, nonatomic) IBOutlet UIView* selectBTiconView;
@property (weak, nonatomic) IBOutlet UIView* selectBTView;
@property (weak, nonatomic) IBOutlet UIView* moveBTiconView;
@property (weak, nonatomic) IBOutlet UIView* moveBTView;
@property (weak, nonatomic) IBOutlet UIView* rotateBTiconView;
@property (weak, nonatomic) IBOutlet UIView* rotateBTView;
@property (weak, nonatomic) IBOutlet UIView* scaleBTiconView;
@property (weak, nonatomic) IBOutlet UIView* scaleBTView;
@property (weak, nonatomic) IBOutlet UIView* propertiesBTiconView;
@property (weak, nonatomic) IBOutlet UIView* propertiesBTView;
@property (weak, nonatomic) IBOutlet UIView* deleteBTiconView;
@property (weak, nonatomic) IBOutlet UIView* deleteBTView;
@property (weak, nonatomic) IBOutlet UIView* viewBTiconView;
@property (weak, nonatomic) IBOutlet UIView* viewBTView;
@property (weak, nonatomic) IBOutlet UIView* viewBTScroll;

@property (weak, nonatomic) IBOutlet UIView* objectSelectionView;
@property (weak, nonatomic) IBOutlet UITableView *objectListTableView;

@property (weak, nonatomic) IBOutlet UISlider *brightnessSlider;
@property (weak, nonatomic) IBOutlet UISlider *specularSlider;
@property (weak, nonatomic) IBOutlet UISwitch *lightingSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *visibleSwitch;
@property (weak, nonatomic) IBOutlet UIView* meshPropertyView;

@property (weak, nonatomic) IBOutlet UILabel *lightSliderLbl;
@property (weak, nonatomic) IBOutlet UISlider *redSlider;
@property (weak, nonatomic) IBOutlet UISlider *greenSlider;
@property (weak, nonatomic) IBOutlet UISlider *blueSlider;
@property (weak, nonatomic) IBOutlet UISlider *shadowSlider;
@property (weak, nonatomic) IBOutlet UILabel *colorDisplayLabel;
@property (weak, nonatomic) IBOutlet UIView* lightPropertyView;

@property (weak, nonatomic) IBOutlet UISlider *fovSlider;
@property (weak, nonatomic) IBOutlet UISegmentedControl *resolutionSegment;
@property (weak, nonatomic) IBOutlet UILabel *fovLabel;
@property (weak, nonatomic) IBOutlet UIView* cameraPropertyView;
@property (weak, nonatomic) IBOutlet UISwitch *mirrorSwitch;
@property (weak, nonatomic) IBOutlet UILabel *frameLabel;
@property (weak, nonatomic) IBOutlet UIImageView *frameImage;
@property (weak, nonatomic) IBOutlet UIButton *frameButton;
@property (weak, nonatomic) IBOutlet UILabel *mirrorLabel;

@property (weak, nonatomic) IBOutlet UIView *tipsView;

@property (weak, nonatomic) IBOutlet UILabel *tipsLabel;
@property (weak, nonatomic) IBOutlet UISwitch *tipsSwitch;
@property (weak, nonatomic) IBOutlet UIButton *saveAnimationButton;
@property (weak, nonatomic) IBOutlet UIView *moveBaseView;
@property (weak, nonatomic) IBOutlet UIView *rotateBaseView;
@property (weak, nonatomic) IBOutlet UIView *scaleBaseView;
@property (weak, nonatomic) IBOutlet UIView *deleteBaseView;
@property (weak, nonatomic) IBOutlet UIView *propertyBaseView;
@property (weak, nonatomic) IBOutlet UIView *selectBaseView;
@property (weak, nonatomic) IBOutlet UIButton *fullViewExpand;
@property (weak, nonatomic) IBOutlet UIButton *minViewButton;
@property (weak, nonatomic) IBOutlet UIView *topView;
@property (weak, nonatomic) IBOutlet UIView *frameView;

- (IBAction)tipsSwitchPressed:(id)sender;

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil SceneItem:(SceneItem*)s carouselIndex:(int)index;

- (IBAction)importAssetAction:(id)sender;
- (IBAction)importTextAction:(id)sender;
- (IBAction)importImageAction:(id)sender;
- (IBAction)importAnimationAction:(id)sender;
- (IBAction)importLightAction:(id)sender;

- (IBAction)frameChangeButtonAction:(id)sender;
- (IBAction)viewButtonAction:(id)sender;
- (IBAction)topviewButtonAction:(id)sender;
- (IBAction)rightviewButtonAction:(id)sender;
- (IBAction)leftviewButtonAction:(id)sender;
- (IBAction)bottomviewButtonAction:(id)sender;
- (IBAction)frontviewButtonAction:(id)sender;
- (IBAction)backviewButtonAction:(id)sender;

- (IBAction)objectSelectionAction:(id)sender;
- (IBAction)objectPropertiesAction:(id)sender;
- (IBAction)undoAction:(id)sender;
- (IBAction)redoAction:(id)sender;
- (IBAction)deleteAction:(id)sender;
- (IBAction)delayCallingFunction:(id)sender;

- (IBAction)moveButtonAction:(id)sender;
- (IBAction)rotateButtonAction:(id)sender;
- (IBAction)scaleButtonAction:(id)sender;

- (IBAction)saveButtonAction:(id)sender;
- (IBAction)menuButtonAction:(id)sender;
- (IBAction)homeButtonAction:(id)sender;
- (IBAction)projectButtonAction:(id)sender;

- (IBAction)moveFrontButtonAction:(id)sender;
- (IBAction)moveBackButtonAction:(id)sender;

- (IBAction)addFrameAction:(id)sender;
- (IBAction)playButtonAction:(id)sender;

- (IBAction)helpButtonAction:(id)sender;

- (IBAction)renderImageButtonAction:(id)sender;
- (IBAction)renderVideoButtonAction:(id)sender;
- (IBAction)renderGifButtonAction:(id)sender;

- (IBAction)meshValueChangedAction:(id)sender;
- (IBAction)lightValueChangedAction:(id)sender;
- (IBAction)cameraValueChangedAction:(id)sender;
- (IBAction)mirrorSwitchChanged:(id)sender;
- (IBAction)sliderValueChanged:(id)sender;
- (IBAction)saveAnimationButtonAction:(id)sender;
- (IBAction)fullViewButtonAction:(id)sender;
- (IBAction)minViewButtonAction:(id)sender;

- (void) saveAnimationData;
- (void) missingAlertForCallback;
- (void) clearFolder:(NSString*)dirPath;
- (void) resumeRenderingAnimationScene;
- (void) deleteKey:(NSMutableDictionary*)dirPath;
- (void) placeView:(UIView*)theView fromValue:(CGPoint)fromPoint toValue:(CGPoint)toPoint active:(BOOL)setValue;
- (NSMutableArray*) sortArrInAscendingOrder:(NSMutableArray*) arr;
- (void) sortArrInAscendingOrder:(int)nodeId jointIndex:(int)jointId controllIndex:(int)controllType;
- (void) removeObjectFromMutableArr:(NSMutableArray*)array Object:(int)objValue;
-(bool) checkNodeForKeyInFrame:(int)frameNo nodeIndex:(int)selectedNodeIndex;

@end
