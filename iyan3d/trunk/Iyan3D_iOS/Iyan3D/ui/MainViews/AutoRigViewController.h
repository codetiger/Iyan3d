//
//  AutoRigViewController.h
//  Iyan3D
//
//  Created by Karthi on 12/06/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#define OPENGL_RENDERER
#undef METAL_RENDERER

#if !(TARGET_IPHONE_SIMULATOR)
    #import <QuartzCore/CAMetalLayer.h>
    #import <Metal/Metal.h>
#endif
#import <UIKit/UIKit.h>
#import "ScaleForAutoRigViewController.h"
#import "PremiumUpgardeVCViewController.h"
#import "HelpViewController.h"
#import "Cachesystem.h"
#import "AssetItem.h"
#import "Constants.h"
#import "scaleViewController.h"
#import "FPPopoverController.h"
#import "ObjFileViewController.h"
#import "RenderingView.h"
#import "GAI.h"

class SGAutoRigScene;

@protocol AutoRigViewControllerDelegate

@end

@interface AutoRigViewController : GAITrackedViewController < UIGestureRecognizerDelegate , AutoRigScaleViewControllerDelegate,ObjFileViewControllerDelegate,HelpViewControllerDelegate,ScalePropertiesViewControllerDelegate,PremiumUpgardeVCViewControllerDelegate>
{
    
    CADisplayLink *displayLinkRig;
    float screenScaleRig;
    NSString *objmainfilenameRig;
    NSString *texturemainFileNameRig;
    CacheSystem *cacheRig;
    int     timerIRig, sliderIdRig;
    int assetIdRig;
    PremiumUpgardeVCViewController *upgradeView;
    bool isViewLoadSetupRig,isViewDisplayedRig,isMetalSupported;
    NSTimer *autoRigAnimationTimerRig;
    int touchCountTrackerRig, frameBarViewHeight,scaleButtonValue;
    bool isPannedRig,isAppInBGRig;
    
    
    //---
#if !(TARGET_IPHONE_SIMULATOR)
    CAMetalLayer *_metalLayerRig;
    id <CAMetalDrawable> _currentDrawableRig;
    MTLRenderPassDescriptor *_renderPassDescriptorRig;
    
    // controller
    id <MTLBuffer> _dynamicConstantBufferRig;
    
    // renderer
    id <MTLDevice> _deviceRig;
    id <MTLCommandQueue> _commandQueueRig;
    id <MTLLibrary> _defaultLibraryRig;
    id <MTLBuffer> vertexBufferRig;
    id <MTLDepthStencilState> _depthStateRig;
    id <MTLTexture> _depthTexRig;
    float _rotationRig;
    id <MTLCommandBuffer> CMDBufferRig;
    id <MTLRenderCommandEncoder> RenderCMDBufferRig;
    id <CAMetalDrawable> drawableRig;
#endif
    CAEAGLLayer* _eaglLayerRig;
    EAGLContext* _contextRig;
    GLuint _colorRenderBufferRig;
    GLuint _depthRenderBufferRig;
    GLuint _frameBufferRig;
}
@property (weak, nonatomic) IBOutlet RenderingView *renderViewRig;
@property (weak, nonatomic) IBOutlet UIImageView *backgroundimage;
@property (weak, nonatomic) IBOutlet UIButton *backButton;
@property (weak, nonatomic) IBOutlet UIButton *importObjButton;
@property (weak, nonatomic) IBOutlet UIButton *moveButton;
@property (weak, nonatomic) IBOutlet UIButton *rotateButton;
@property (weak, nonatomic) IBOutlet UIButton *scaleButton;
@property (weak, nonatomic) IBOutlet UIButton *exportRigMesh;
@property (weak, nonatomic) IBOutlet UIButton *prevButton;
@property (weak, nonatomic) IBOutlet UIButton *nextButton;
@property (weak, nonatomic) IBOutlet UIButton *helpButton;
@property (weak, nonatomic) IBOutlet UILabel *displayText;
@property (weak, nonatomic) IBOutlet UILabel *hintForOperation;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *loadingforObjView;
@property (weak, nonatomic) IBOutlet UISwitch *mirrorSwitch;
@property (weak, nonatomic) IBOutlet UIView* viewBTiconView;
@property (weak, nonatomic) IBOutlet UIView* viewBTView;
@property (weak, nonatomic) IBOutlet UIView* viewBTScroll;
@property (weak, nonatomic) IBOutlet UIButton* viewBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewtopBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewbottomBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewrightBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewleftBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewfrontBT;
@property (weak, nonatomic) IBOutlet UIButton* ViewbackBT;
@property (weak, nonatomic) IBOutlet UIButton* undoButton;
@property (weak, nonatomic) IBOutlet UIButton* redoButton;
@property (weak, nonatomic) IBOutlet UIView* moveBTiconView;
@property (weak, nonatomic) IBOutlet UIView* moveBTView;
@property (weak, nonatomic) IBOutlet UIView* rotateBTiconView;
@property (weak, nonatomic) IBOutlet UIView* rotateBTView;
@property (weak, nonatomic) IBOutlet UIView* scaleBTiconView;
@property (weak, nonatomic) IBOutlet UIView* scaleBTView;
@property (weak, nonatomic) IBOutlet UILabel* mirrorLabel;
@property (weak, nonatomic) IBOutlet UIButton* addJointsButton;
@property (weak, nonatomic) IBOutlet UIView *addJointView;
@property (weak, nonatomic) IBOutlet UIView *topbarView;
@property (weak, nonatomic) IBOutlet UIButton *fullScreenButton;
@property (weak, nonatomic) IBOutlet UIButton *normalScreenButton;

@property (nonatomic, strong) ScaleForAutoRigViewController *scaleProperties;
@property (nonatomic, strong) HelpViewController *morehelpView;
@property (nonatomic, strong) ScaleViewController *xyzScaleProperties;
@property (nonatomic, strong) FPPopoverController* popOverView;
@property (weak, nonatomic) id <AutoRigViewControllerDelegate> delegate;

- (IBAction)moveButtonAction:(id)sender;
- (IBAction)helpButtonAction:(id)sender;
- (IBAction)prevButtonAction:(id)sender;
- (IBAction)nextButtonAction:(id)sender;
- (IBAction)rotateButtonAction:(id)sender;
- (IBAction)scaleButtonAction:(id)sender;
- (IBAction)backButtonAction:(id)sender;
- (IBAction)switchChanged:(id)sender;
- (IBAction)importObjButtonAction:(id)sender;
- (IBAction)exportRigButtonAction:(id)sender;
- (IBAction)viewButtonAction:(id)sender;
- (IBAction)topviewButtonAction:(id)sender;
- (IBAction)rightviewButtonAction:(id)sender;
- (IBAction)leftviewButtonAction:(id)sender;
- (IBAction)bottomviewButtonAction:(id)sender;
- (IBAction)frontviewButtonAction:(id)sender;
- (IBAction)backviewButtonAction:(id)sender;
- (IBAction)undoButtonAction:(id)sender;
- (IBAction)redoButtonAction:(id)sender;
- (IBAction)addJointsButtonAction:(id)sender;
- (IBAction)fullScreenButtonAction:(id)sender;
- (IBAction)normalScreenButtonAction:(id)sender;

- (void) clearFolder:(NSString*)dirPath;
-(void)resetButtonVisibility;
-(NSString*) getMD5ForNonReadableFile:(NSString*)path;
-(void) storeOBJinCachesDirectory:(NSString*)fileName assetId:(int)assetId;
-(void) storeOBJTextureinCachesDirectory:(NSString*)fileName assetId:(int)assetId;
- (NSData*) convertAndScaleImage:(UIImage*)image size:(int)textureRes;
- (void)placeView:(UIView*)theView fromValue:(CGPoint)fromPoint toValue:(CGPoint)toPoint active:(BOOL)setValue;

@end
