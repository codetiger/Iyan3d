//
//  RenderingViewController.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 15/02/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GTLYouTube.h"
#import "VideoData.h"
#import <MobileCoreServices/MobileCoreServices.h>
#import "YouTubeUploadVideo.h"
#import "GTMOAuth2ViewControllerTouch.h"
#import "AppHelper.h"
#import <ImageIO/ImageIO.h>
#import <MobileCoreServices/MobileCoreServices.h>
#import "GAI.h"
#import "CacheSystem.h"
#import "RETrimControl.h"
#import "TextColorPicker.h"
#import "WEPopoverController.h"
#import "PopUpViewController.h"

@protocol RenderingViewControllerDelegate
- (void) freezeEditorRender:(BOOL) freeze;
- (void) renderFrame:(int)frame withType:(int)shaderType isImage:(bool)isImage andRemoveWatermark:(bool)removeWatermark;
- (void) setShaderTypeForRendering:(int)shaderType;
- (NSMutableArray*) getFileNamesToAttach;
- (BOOL) canUploadToCloud;
- (CGPoint) getCameraResolution;
- (void) cameraResolutionChanged:(int)resolutinType;
- (void) clearFolder:(NSString*)dirPath;
- (void) resumeRenderingAnimationScene;
- (void) saveScene;
- (void) changeRenderingBgColor:(Vector4)vertexColor;
- (IBAction)loginBtnAction:(id)sender;
@end

@class GADBannerView;

@interface RenderingViewController : GAITrackedViewController<YouTubeUploadVideoDelegate ,RETrimControlDelegate , NSURLConnectionDelegate,UICollectionViewDataSource,UICollectionViewDelegate,TextColorPickerDelegate>
{
    int renderingStartFrame, renderingEndFrame, renderingFrame , shaderType,finalFrame,publishId;
    bool isCanceled,isAppInBg;
	int renderingExportImage;
    NSThread* thread;
    int resolutionType;
    NSArray *sgfdFiles;
    float cameraResolutionWidth, cameraResolutionHeight;
    UIAlertView *uploadSceneAlert;
    UIAlertView *logoutAlert;
    NSMutableArray *imagesArray, *shaderArray;
    NSMutableDictionary *shaderTypesDict;
    CacheSystem* cache;
    int selectedIndex,tempSelectedIndex;
    BOOL cancelPressed;
    Vector3 bgColor;
    int ScreenWidth,ScreenHeight;
}

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil StartFrame:(int)startFrame EndFrame:(int)endFrame renderOutput:(int)exportType caMresolution:(int)resolution ScreenWidth:(int)screenWidth ScreenHeight:(int)screenHeight;
@property (strong, nonatomic) RETrimControl *trimControl;
@property (strong, nonatomic) NSString *sgbPath;
@property (strong, nonatomic) NSString *projectName;
@property (strong, nonatomic) NSString *videoFilePath;
@property (weak, nonatomic) IBOutlet UILabel *transparentBgLabel;
@property(nonatomic, weak) IBOutlet GADBannerView *bannerView;
@property (weak, nonatomic) IBOutlet UIImageView *renderedImageView;
@property (weak, nonatomic) IBOutlet UIButton *exportButton;
@property (weak, nonatomic) IBOutlet UIButton *cancelButton;
@property (weak, nonatomic) IBOutlet UIButton *nextButton;
@property (weak, nonatomic) IBOutlet UIButton *youtubeButton;
@property (weak, nonatomic) IBOutlet UIButton *rateButtonImage;
@property (weak, nonatomic) IBOutlet UIButton *rateButtonText;
@property (weak, nonatomic) IBOutlet UIButton *startButtonText;
@property (weak, nonatomic) IBOutlet UIProgressView *renderingProgressBar;
@property (weak, nonatomic) IBOutlet UILabel *renderingProgressLabel;
@property (weak, nonatomic) IBOutlet UILabel *renderDesc;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *activityIndicatorView;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *shareActivityIndicator;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *cancelActivityIndicator;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *makeVideoLoading;
@property (weak, nonatomic) IBOutlet UISwitch *transparentBgSwitch;
@property (weak, nonatomic) IBOutlet UISegmentedControl *resolutionSegment;
@property (weak, nonatomic) IBOutlet UILabel *resolutionTypeLabel;
@property (weak, nonatomic) IBOutlet UILabel *renderingStyleLabel;
@property (weak, nonatomic) IBOutlet UICollectionView *shaderStyle;
@property (weak, nonatomic) IBOutlet UILabel *resolutionType1;
@property (weak, nonatomic) IBOutlet UILabel *resolutionType2;
@property (weak, nonatomic) IBOutlet UILabel *resolutionType3;
@property (weak, nonatomic) IBOutlet UICollectionView *renderingTypes;
@property (weak, nonatomic) IBOutlet UILabel *waterMarkLabel;

@property (nonatomic, retain) GTLServiceYouTube *youtubeService;
@property(nonatomic, strong) YouTubeUploadVideo *uploadVideo;
@property(nonatomic, strong) GTMOAuth2ViewControllerTouch *authController;
@property (weak, nonatomic) id <RenderingViewControllerDelegate> delegate;
@property (nonatomic, strong) TextColorPicker *bgColorProp;
@property (nonatomic, strong) WEPopoverController *popoverController;
@property (nonatomic, strong) PopUpViewController *popUpVc;

@property (weak, nonatomic) IBOutlet UISwitch *watermarkSwitch;
@property (weak, nonatomic) IBOutlet UILabel *backgroundColorLable;
@property (weak, nonatomic) IBOutlet UIButton *colorPickerBtn;
@property (weak, nonatomic) IBOutlet UILabel *transparentLable;
@property (weak, nonatomic) IBOutlet UISwitch *transparentBackgroundbtn;
@property (weak, nonatomic) IBOutlet UILabel *creditLable;
@property (weak, nonatomic) IBOutlet UILabel *resolutionType4;
@property (weak, nonatomic) IBOutlet UIView *progressSub;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *checkCreditProgress;


- (IBAction)cameraResolutionChanged:(id)sender;
- (IBAction)transparentBgValueChanged:(id)sender;
- (IBAction)waterMarkValueChanged:(id)sender;
- (IBAction)cancelButtonAction:(id)sender;
- (IBAction)rateButtonAction:(id)sender;
- (IBAction)exportButtonAction:(id)sender;
- (IBAction)youtubeButtonAction:(id)sender;
- (IBAction)startButtonAction:(id)sender;
- (void) shaderPhotoAction:(int)credits;

@end
