//
//  RenderingViewController.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 15/02/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.

#import "RenderingViewController.h"
#import "AVFoundation/AVAssetWriterInput.h"
#import "AVFoundation/AVAssetWriter.h"
#import "AVFoundation/AVFoundation.h"
#import "QuartzCore/QuartzCore.h"
#import "Utility.h"
#import "ZipArchive.h"
#import <ImageIO/ImageIO.h>
#import <MobileCoreServices/MobileCoreServices.h>

#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>

#define RENDER_IMAGE 0
#define RENDER_VIDEO 1
#define RENDER_GIF 2
#define FULL_HD_GIF_RANGE 100
#define HD_GIF_RANGE 200
#define DVD_GIF_RANGE 300

#define THOUSAND_EIGHTY_P 0
#define SEVEN_HUNDRED_TWENTY_P 1
#define FOUR_HUNDRED_EIGHTY_P 2
#define THREE_HUNDRED_SIXTY_P 3
#define TWO_HUNDRED_FOURTY_P 4

#define UPLOAD_ALERT_VIEW 3
#define YOUTUBE_BUTTON_TAG 2
#define LOGOUT_BUTTON_INDEX 1
#define UPLOAD_BUTTON_INDEX 2
#define CANCEL_BUTTON_INDEX 0
#define OK_BUTTON_INDEX 1

#define COMPLETION_ALERT 7

#define DONE 1
#define START 0

#define SIGNIN_ALERT_VIEW 4
#define CREDITS_VIEW 5

@implementation RenderingViewController

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil StartFrame:(int)startFrame EndFrame:(int)endFrame renderOutput:(int)exportType caMresolution:(int)resolution ScreenWidth:(int)screenWidth ScreenHeight:(int)screenHeight {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cache                = [CacheSystem cacheSystem];
        renderingStartFrame  = startFrame;
        renderingEndFrame    = endFrame;
        renderingFrame       = renderingStartFrame;
        isCanceled           = false;
        renderingExportImage = exportType;
        resolutionType       = resolution;
        bgColor              = Vector3(0.1, 0.1, 0.1);
        ScreenWidth          = screenWidth;
        ScreenHeight         = screenHeight;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.screenName = @"RenderingView iOS";

    cancelPressed = resAlertShown               = NO;
    selectedIndex                               = 0;
    isAppInBg                                   = false;
    self.resolutionSegment.selectedSegmentIndex = resolutionType;
    self.nextButton.layer.cornerRadius          = 5.0;
    self.cancelButton.layer.cornerRadius        = 5.0;
    [self.activityIndicatorView setHidden:true];
    [self.shareActivityIndicator setHidden:true];
    [self.cancelActivityIndicator setHidden:true];
    [self.watermarkSwitch setOn:YES];
    [self beginViewHideAndShow:true];
    shaderTypesDict = [[NSMutableDictionary alloc] init];

    [self.delegate freezeEditorRender:YES];

    if ([Utility IsPadDevice])
        [self.renderingTypes registerNib:[UINib nibWithNibName:@"ShaderCell" bundle:nil] forCellWithReuseIdentifier:@"TYPE"];
    else
        [self.renderingTypes registerNib:[UINib nibWithNibName:@"ShaderCellPhone" bundle:nil] forCellWithReuseIdentifier:@"TYPE"];

    if (renderingExportImage == RENDER_IMAGE) {
        [_progressSub setHidden:YES];
        [self.limitFramesLbl setHidden:YES];
        self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d", renderingStartFrame + 1];
        [self.rateButtonImage setEnabled:YES];
        [self.rateButtonText setEnabled:YES];
    } else if (renderingExportImage == RENDER_VIDEO) {
        self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d/%d", 1, (int)(_trimControl.rightValue - _trimControl.leftValue)];
        [self.rateButtonImage setEnabled:NO];
        [self.rateButtonText setEnabled:NO];
    }
    self.renderingProgressBar.progress = ((float)(renderingFrame - renderingStartFrame)) / (1 + renderingEndFrame - renderingStartFrame);
    [self.makeVideoLoading setHidden:YES];
    [self.exportButton setHidden:true];
    if (renderingExportImage != RENDER_IMAGE) {
    }
    _nextButton.tag = START;
    [_checkCreditProgress setHidden:YES];

    self.nextButton.accessibilityHint       = NSLocalizedString(@"tap_to_start", nil);
    self.nextButton.accessibilityIdentifier = @"2";

    [self.renderTitleLabel setText:NSLocalizedString(@"RENDERING PROGRESS", nil)];
    [self.resolutionTypeLabel setText:NSLocalizedString(@"RESOLUTION", nil)];
    [self.backgroundColorLable setText:NSLocalizedString(@"BACKGROUND COLOR", nil)];
    [self.waterMarkLabel setText:NSLocalizedString(@"WATERMARK", nil)];
    [self.renderingStyleLabel setText:NSLocalizedString(@"RENDERING STYLE", nil)];

    [self.renderDesc setText:NSLocalizedString(@"Render Normal Shader in Your Device", nil)];
    [self.limitFramesLbl setText:NSLocalizedString(@"LIMIT FRAMES", nil)];
    [self.creditLable setText:NSLocalizedString(@"Credits", nil)];
}

- (void)trimControl:(RETrimControl*)trimControl didChangeLeftValue:(CGFloat)leftValue rightValue:(CGFloat)rightValue {
}

- (UIEdgeInsets)collectionView:(UICollectionView*)collectionView layout:(UICollectionViewLayout*)collectionViewLayout insetForSectionAtIndex:(NSInteger)section {
    if ([Utility IsPadDevice])
        return UIEdgeInsetsMake(20, 20, 20, 20);
    else {
        if (ScreenWidth == 667)
            return UIEdgeInsetsMake(12, 170, 25, 70);
        else if (ScreenWidth <= 640)
            return UIEdgeInsetsMake(12, 90, 25, 75);
        else
            return UIEdgeInsetsMake(20, 50, 25, 75);
    }

    // top, left, bottom, right
}

- (void)beginViewHideAndShow:(BOOL)isHidden {
    [self.renderingProgressLabel setHidden:isHidden];
    [self.renderingProgressBar setHidden:isHidden];
    if (isHidden == false) {
        isHidden = true;
        [self.renderingStyleLabel setHidden:isHidden];
        [self.resolutionSegment setHidden:isHidden];
        [self.resolutionType1 setHidden:isHidden];
        [self.resolutionType2 setHidden:isHidden];
        [self.resolutionType3 setHidden:isHidden];
        [self.resolutionType4 setHidden:isHidden];
        [self.resolutionTypeLabel setHidden:isHidden];
        [self.startButtonText setHidden:isHidden];
        [self.waterMarkLabel setHidden:isHidden];
        [self.watermarkSwitch setHidden:isHidden];
        [self.shaderStyle setHidden:isHidden];
        [self.renderDesc setHidden:isHidden];
        [self.backgroundColorLable setHidden:isHidden];
        [self.colorPickerBtn setHidden:isHidden];
        [self.transparentLable setHidden:isHidden];
        [self.transparentBackgroundbtn setHidden:isHidden];
        [self.backgroundColorLable setHidden:isHidden];
        [self.transparentBackgroundbtn setHidden:isHidden];
        [self.colorPickerBtn setHidden:isHidden];
        [self.transparentBgLabel setHidden:isHidden];
    }
}

- (void)appEntersBG {
    isAppInBg = true;
}

- (void)appEntersFG {
    isAppInBg = false;
}

- (void)uploadFilesToRender {
    NSLog(@"Written");
}

- (IBAction)startButtonAction:(id)sender {
    if (_nextButton.tag == DONE) {
        [self.limitFramesLbl setHidden:YES];
        [_progressSub setHidden:YES];
        [self cancelButtonAction:nil];
    } else {
        self.nextButton.accessibilityHint = @"";

        [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
        if (![[AppHelper getAppHelper] userDefaultsBoolForKey:@"ExportTipsShown"]) {
            [[AppHelper getAppHelper] toggleHelp:self Enable:YES];
            [[AppHelper getAppHelper] saveBoolUserDefaults:YES withKey:@"ExportTipsShown"];
        }

        [self.limitFramesLbl setHidden:YES];
        [_progressSub setHidden:YES];

        if (renderingExportImage != RENDER_IMAGE) {
            renderingFrame = _trimControl.leftValue;
            [self.trimControl setHidden:YES];
        }
        [self renderBeginFunction:0];
    }
}

- (void)verifiedUsage {
    if (renderingExportImage == RENDER_VIDEO) {
        [self performSelectorOnMainThread:@selector(doMakeVideoUIUpdate) withObject:nil waitUntilDone:YES];

        [self MakeVideo];
        [self.makeVideoLoading stopAnimating];
        [self.makeVideoLoading setHidden:YES];
        [self.rateButtonImage setEnabled:YES];
        [self.rateButtonText setEnabled:YES];
    }

    [self.exportButton setHidden:YES];
    [self.exportButton setEnabled:NO];
    [self.renderingProgressLabel setHidden:YES];
    [self.renderingProgressBar setHidden:YES];
    [self performSelectorOnMainThread:@selector(renderFinishAction:) withObject:[NSNumber numberWithInt:renderingExportImage] waitUntilDone:NO];
}

- (NSMutableArray*)getFileteredFilePathsFrom:(NSMutableArray*)filePaths {
    NSMutableArray* filtFilePaths = [[NSMutableArray alloc] init];
    NSString*       docDirPath    = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString*       cacheDirPath  = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString*       objDirPath    = [docDirPath stringByAppendingPathComponent:@"Resources/Objs"];
    NSString*       rigDirPath    = [docDirPath stringByAppendingPathComponent:@"Resources/Rigs"];
    NSString*       texDirPath    = [docDirPath stringByAppendingPathComponent:@"Resources/Textures"];
    NSString*       sgmDirPath    = [docDirPath stringByAppendingPathComponent:@"Resources/Sgm"];

    NSFileManager* fm = [NSFileManager defaultManager];

    for (int i = 0; i < [filePaths count]; i++) {
        if ([[filePaths objectAtIndex:i] isEqualToString:@""])
            continue;
        NSString* filePath1 = [NSString stringWithFormat:@"%@/%@", cacheDirPath, [filePaths objectAtIndex:i]];
        NSString* filePath2 = [NSString stringWithFormat:@"%@/%@", objDirPath, [filePaths objectAtIndex:i]];
        NSString* filePath3 = [NSString stringWithFormat:@"%@/%@", rigDirPath, [filePaths objectAtIndex:i]];
        NSString* filePath4 = [NSString stringWithFormat:@"%@/%@", texDirPath, [filePaths objectAtIndex:i]];
        NSString* filePath5 = [NSString stringWithFormat:@"%@/%@", sgmDirPath, [filePaths objectAtIndex:i]];
        NSString* filePath6 = [NSString stringWithFormat:@"%@/%@", docDirPath, [filePaths objectAtIndex:i]];
        NSString* filePath7 = [NSString stringWithFormat:@"%s/%@", constants::BundlePath.c_str(), [filePaths objectAtIndex:i]];

        if ([fm fileExistsAtPath:filePath1]) {
            if (![filtFilePaths containsObject:filePath1])
                [filtFilePaths addObject:filePath1];
        } else if ([fm fileExistsAtPath:filePath2]) {
            if (![filtFilePaths containsObject:filePath2])
                [filtFilePaths addObject:filePath2];
        } else if ([fm fileExistsAtPath:filePath3]) {
            if (![filtFilePaths containsObject:filePath3])
                [filtFilePaths addObject:filePath3];
        } else if ([fm fileExistsAtPath:filePath4]) {
            if (![filtFilePaths containsObject:filePath4])
                [filtFilePaths addObject:filePath4];
        } else if ([fm fileExistsAtPath:filePath5]) {
            if (![filtFilePaths containsObject:filePath5])
                [filtFilePaths addObject:filePath5];
        } else if ([fm fileExistsAtPath:filePath6]) {
            if (![filtFilePaths containsObject:filePath6])
                [filtFilePaths addObject:filePath6];
        } else if ([fm fileExistsAtPath:filePath7]) {
            if (![filtFilePaths containsObject:filePath7])
                [filtFilePaths addObject:filePath7];
        }
    }
    return filtFilePaths;
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    if (renderingExportImage != RENDER_IMAGE) {
        _trimControl          = [[RETrimControl alloc] initWithFrame:CGRectMake(_progressSub.frame.origin.x, _progressSub.frame.origin.y, _progressSub.frame.size.width, _progressSub.frame.size.height)];
        _trimControl.length   = renderingEndFrame; // 200 seconds
        _trimControl.delegate = self;
        _trimControl.center   = _progressSub.center;
        [self.view addSubview:_trimControl];
    }

    if (![[AppHelper getAppHelper] userDefaultsBoolForKey:@"ExportTipsShown"]) {
        [[AppHelper getAppHelper] toggleHelp:self Enable:YES];
    }

    [self removeSGFDFilesIfAny];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersBG) name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersFG) name:@"applicationDidBecomeActive" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(uploadFilesToRender) name:@"FileWriteCompleted" object:nil];

    isAppInBg = false;

    UITapGestureRecognizer* tapGest = [[UITapGestureRecognizer alloc] initWithTarget:self action:nil];
    tapGest.delegate                = self;
    [self.view addGestureRecognizer:tapGest];
}

- (void)viewWillDisappear:(BOOL)animated {
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"applicationDidBecomeActive" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"FileWriteCompleted" object:nil];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer*)gestureRecognizer shouldReceiveTouch:(UITouch*)touch {
    if ([touch.view isDescendantOfView:self.helpBtn]) {
        return YES;
    }

    [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
    return NO;
}

- (void)removeSGFDFilesIfAny {
    NSFileManager*   manager       = [NSFileManager defaultManager];
    NSArray*         docPaths      = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSMutableString* docDirectory  = [docPaths objectAtIndex:0];
    NSArray*         dirFiles1     = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirectory error:nil];
    NSArray*         filesToDelete = [dirFiles1 filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.sgfd'"]];
    for (int i = 0; i < [filesToDelete count]; i++)
        [manager removeItemAtPath:[docDirectory stringByAppendingPathComponent:filesToDelete[i]] error:nil];
}

- (void)renderingProgress:(NSNumber*)credits {
    finalFrame = (int)_trimControl.rightValue - (int)_trimControl.leftValue;

    int loopEnd = (renderingExportImage == RENDER_IMAGE) ? renderingStartFrame : (int)_trimControl.rightValue;

    if (isAppInBg)
        return;
    while (renderingFrame <= loopEnd && !isCanceled) {
        if (!isAppInBg) {
            dispatch_block_t work_to_do = ^{
                [self doRenderingOnMainThread];
            };

            if ([NSThread isMainThread])
                work_to_do();
            else
                dispatch_sync(dispatch_get_main_queue(), work_to_do);

            if (!isAppInBg) {
                if (renderingExportImage == RENDER_IMAGE || renderingExportImage == RENDER_VIDEO) {
                    renderingFrame++;
                } else {
                    renderingFrame = renderingFrame + 3;
                }
            }
        }
        [NSThread sleepForTimeInterval:0.1];
    }
    if (isCanceled)
        return;

    if (renderingExportImage == RENDER_VIDEO) {
        [self.makeVideoLoading setHidden:NO];
        [self.makeVideoLoading startAnimating];
    }

    [self verifiedUsage];
}

- (void)renderFinishAction:(NSNumber*)object {
    NSString* tempDir = NSTemporaryDirectory();
    if (renderingExportImage == RENDER_VIDEO) {
        if (self.videoFilePath == nil || [self.videoFilePath isEqualToString:@""])
            return;
        NSString* filePath = [NSString stringWithFormat:@"%@/myMovie.mov", tempDir];
        outputFilePath     = filePath;
        UISaveVideoAtPathToSavedPhotosAlbum(filePath, self, @selector(video:didFinishSavingWithError:contextInfo:), nil);
        //        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Video successfully saved in your gallery." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil, nil];
        //        [alert setTag:COMPLETION_ALERT];
        //        [alert show];
    } else {
        NSString* filePath = [NSString stringWithFormat:@"%@r-%d.png", tempDir, renderingFrame - 1];
        outputFilePath     = filePath;
        UIImage* image     = [UIImage imageWithContentsOfFile:filePath];
        UIImageWriteToSavedPhotosAlbum(image, self, @selector(image:didFinishSavingWithError:contextInfo:), nil);
        //        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Image was successfully saved in your gallery." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil, nil];
        //        [alert setTag:COMPLETION_ALERT];
        //        [alert show];
    }
    [_cancelButton setHidden:YES];
    [_nextButton setHidden:NO];
    [_nextButton setEnabled:YES];
    [_nextButton setTitle:NSLocalizedString(@"Done", nil) forState:UIControlStateNormal];
    _nextButton.tag = DONE;
}

- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex {
    switch (alertView.tag) {
        case SIGNIN_ALERT_VIEW: {
            if (buttonIndex == CANCEL_BUTTON_INDEX) {
                [self dismissViewControllerAnimated:NO
                                         completion:^{
                                             cancelPressed = YES;
                                             [self.cancelActivityIndicator setHidden:false];
                                             [self.cancelActivityIndicator startAnimating];
                                             [thread cancel];
                                             isCanceled = true;
                                             [self.delegate clearFolder:NSTemporaryDirectory()];
                                             [self.delegate resumeRenderingAnimationScene];
                                             [self deallocMem];
                                         }];
            }
            break;
        }
        case COMPLETION_ALERT: {
            [self performSelectorOnMainThread:@selector(showPreviewInMainThread:) withObject:outputFilePath waitUntilDone:NO];
        }
        default:
            break;
    }
}

- (void)showPreviewInMainThread:(NSString*)outputPath {
    int mediaType = [[outputPath pathExtension] isEqualToString:@"png"] ? 0 : 1;

    if ([Utility IsPadDevice]) {
        MediaPreviewVC* medPreview        = [[MediaPreviewVC alloc] initWithNibName:@"MediaPreviewVC" bundle:nil mediaType:mediaType medPath:outputPath];
        medPreview.delegate               = self;
        medPreview.modalPresentationStyle = UIModalPresentationFullScreen;
        [self presentViewController:medPreview animated:YES completion:nil];
    } else {
        MediaPreviewVC* medPreview        = [[MediaPreviewVC alloc] initWithNibName:[[AppHelper getAppHelper] iPhone6Plus] ? @"MediaPreviewVCPhone@2x" : @"MediaPreviewVCPhone" bundle:nil mediaType:mediaType medPath:outputPath];
        medPreview.delegate               = self;
        medPreview.modalPresentationStyle = UIModalPresentationFullScreen;
        [self presentViewController:medPreview animated:YES completion:nil];
    }
}

- (IBAction)toolTipAction:(id)sender {
    [[AppHelper getAppHelper] toggleHelp:self Enable:YES];
}

- (IBAction)cameraResolutionChanged:(id)sender {
    resolutionType = (int)self.resolutionSegment.selectedSegmentIndex;
    [self.delegate cameraResolutionChanged:resolutionType];
}

- (IBAction)cancelButtonAction:(id)sender {
    cancelPressed = YES;
    [self.cancelActivityIndicator setHidden:false];
    [self.cancelActivityIndicator startAnimating];
    [thread cancel];
    isCanceled = true;
    [self.delegate clearFolder:NSTemporaryDirectory()];
    [self.delegate resumeRenderingAnimationScene];
    [self dismissViewControllerAnimated:YES completion:nil];
    [self deallocMem];
}

- (void)closeView {
    [self cancelButtonAction:nil];
}

- (IBAction)colorPickerAction:(id)sender {
    _bgColorProp                                = [[TextColorPicker alloc] initWithNibName:@"TextColorPicker" bundle:nil TextColor:nil];
    _bgColorProp.delegate                       = self;
    self.popoverController                      = [[WEPopoverController alloc] initWithContentViewController:_bgColorProp];
    self.popoverController.popoverContentSize   = CGSizeMake(200, 200);
    self.popoverController.popoverLayoutMargins = UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
    self.popoverController.animationType        = WEPopoverAnimationTypeCrossFade;
    [_popUpVc.view setClipsToBounds:YES];
    CGRect rect = _colorPickerBtn.frame;
    rect        = [self.view convertRect:rect fromView:_colorPickerBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self.view
                          permittedArrowDirections:([Utility IsPadDevice]) ? UIPopoverArrowDirectionUp : UIPopoverArrowDirectionRight
                                          animated:NO];
}

- (void)connection:(NSURLConnection*)connection didReceiveData:(NSData*)data {
}

- (void)connection:(NSURLConnection*)connection didFailWithError:(NSError*)error {
    NSLog(@"Faliled with error %@", error.description);
}

- (void)connectionDidFinishLoading:(NSURLConnection*)connection {
}

- (void)renderBeginFunction:(int)credits {
    [self beginViewHideAndShow:false];
    [self.resolutionSegment setEnabled:false];
    [self.startButtonText setEnabled:false];
    resolutionType = (int)self.resolutionSegment.selectedSegmentIndex;
    if (resolutionType == THOUSAND_EIGHTY_P) {
        cameraResolutionWidth  = 1920.0f;
        cameraResolutionHeight = 1080.0f;
    } else if (resolutionType == SEVEN_HUNDRED_TWENTY_P) {
        cameraResolutionWidth  = 1280.0f;
        cameraResolutionHeight = 720.0f;
    } else if (resolutionType == FOUR_HUNDRED_EIGHTY_P) {
        cameraResolutionWidth  = 848.0f;
        cameraResolutionHeight = 480.0f;
    } else if (resolutionType == THREE_HUNDRED_SIXTY_P) {
        cameraResolutionWidth  = 640.0f;
        cameraResolutionHeight = 360.0f;
    } else {
        cameraResolutionWidth  = 352.0f;
        cameraResolutionHeight = 240.0f;
    }
    [[AppHelper getAppHelper] saveToUserDefaults:[NSString stringWithFormat:@"%d", resolutionType] withKey:@"cameraResolution"];
    [UIApplication sharedApplication].idleTimerDisabled = YES;

    [self.activityIndicatorView setHidden:NO];
    [self.activityIndicatorView startAnimating];
    for (UIView* subView in [self.view subviews]) {
        if (subView != self.renderedImageView && subView != self.renderingProgressBar && subView != self.renderingProgressLabel && subView != self.topBar && subView != self.cancelButton)
            [subView setHidden:YES];
    }

    [self performSelectorInBackground:@selector(syncPhysicsAndStartRendering:) withObject:[NSNumber numberWithInt:credits]];
}

- (void)syncPhysicsAndStartRendering:(NSNumber*)object {
    [self.delegate syncSceneWithPhysicsWorld];

    int credits                      = [object intValue];
    self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d/%d", (int)_trimControl.leftValue, (int)_trimControl.rightValue];
    thread                           = [[NSThread alloc] initWithTarget:self selector:@selector(renderingProgress:) object:[NSNumber numberWithInt:credits]];
    [thread start];

    [self.activityIndicatorView setHidden:YES];
}

- (void)doRenderingOnMainThread {
    if (isAppInBg)
        return;

    if (!cancelPressed) {
        [self.activityIndicatorView setHidden:false];
        [self.activityIndicatorView startAnimating];
        if (renderingExportImage == RENDER_IMAGE) {
            self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d", renderingStartFrame + 1];
        } else if (renderingExportImage == RENDER_VIDEO) {
            self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d/%d", renderingFrame, (int)_trimControl.rightValue];
        } else if (renderingExportImage == RENDER_GIF) {
            self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d/%d", renderingFrame, (int)_trimControl.rightValue];
        }
        self.renderingProgressBar.progress = ((float)(renderingFrame - (int)_trimControl.leftValue)) / (((int)_trimControl.rightValue - (int)_trimControl.leftValue));

        [self.delegate renderFrame:renderingFrame isImage:(renderingExportImage == RENDER_IMAGE)];

        NSString* tempDir       = NSTemporaryDirectory();
        NSString* imageFilePath = [NSString stringWithFormat:@"%@/r-%d.png", tempDir, renderingFrame];
        [self.renderedImageView setImage:[UIImage imageWithContentsOfFile:imageFilePath]];

        [self.activityIndicatorView stopAnimating];
        [self.activityIndicatorView setHidden:true];
    }
}

- (void)MakeVideo {
    imagesArray       = [[NSMutableArray alloc] init];
    NSString* tempDir = NSTemporaryDirectory();

    for (int i = _trimControl.leftValue; i <= _trimControl.rightValue; i++) {
        NSString* file = [tempDir stringByAppendingPathComponent:[NSString stringWithFormat:@"r-%d.png", i]];
        [imagesArray addObject:file];
    }

    NSString* fileName = @"myMovie.mov";
    self.videoFilePath = [tempDir stringByAppendingPathComponent:fileName];

    NSError*       error   = nil;
    NSFileManager* fileMgr = [[NSFileManager alloc] init];
    [fileMgr removeItemAtPath:self.videoFilePath error:&error];

    if (self.videoFilePath == nil || [self.videoFilePath isEqualToString:@""])
        return;

    AVAssetWriter* videoWriter = [[AVAssetWriter alloc] initWithURL:[NSURL fileURLWithPath:self.videoFilePath] fileType:AVFileTypeQuickTimeMovie error:&error];
    if (error)
        NSLog(@" Video Error %@ ", error.localizedDescription);
    NSDictionary* videoSettings = [NSDictionary dictionaryWithObjectsAndKeys:AVVideoCodecH264, AVVideoCodecKey, [NSNumber numberWithInt:cameraResolutionWidth], AVVideoWidthKey, [NSNumber numberWithInt:cameraResolutionHeight], AVVideoHeightKey, nil];

    AVAssetWriterInput* videoWriterInput = [AVAssetWriterInput
        assetWriterInputWithMediaType:AVMediaTypeVideo
                       outputSettings:videoSettings];

    AVAssetWriterInputPixelBufferAdaptor* adaptor = [AVAssetWriterInputPixelBufferAdaptor
        assetWriterInputPixelBufferAdaptorWithAssetWriterInput:videoWriterInput
                                   sourcePixelBufferAttributes:nil];

    videoWriterInput.expectsMediaDataInRealTime = YES;
    [videoWriter addInput:videoWriterInput];

    [videoWriter startWriting];
    [videoWriter startSessionAtSourceTime:kCMTimeZero];

    CVPixelBufferRef buffer = NULL;

    for (int i = 0; i < [imagesArray count]; i++) {
        @autoreleasepool {
            UIImage* img = [UIImage imageWithContentsOfFile:[imagesArray objectAtIndex:i]];
            buffer       = pixelBufferFromCGImage([img CGImage], CGSizeMake(cameraResolutionWidth, cameraResolutionHeight));

            BOOL append_ok = NO;
            int  j         = 0;
            while (!append_ok && j < 30) {
                if (adaptor.assetWriterInput.readyForMoreMediaData) {
                    CMTime frameTime = CMTimeMake(i, (int32_t)24);
                    append_ok        = [adaptor appendPixelBuffer:buffer withPresentationTime:frameTime];
                    if (!append_ok) {
                        NSError* error = videoWriter.error;
                        if (error != nil) {
                        }
                    } else
                        CVPixelBufferRelease(buffer);
                } else {
                    [NSThread sleepForTimeInterval:0.1];
                }
                j++;
            }
            if (!append_ok) {
                //printf("error appending image %d times %d\n, with error.", i, j);
            }
        }
    }
    //Finish the session:
    [videoWriterInput markAsFinished];
    [videoWriter finishWriting];
}

CVPixelBufferRef pixelBufferFromCGImage(CGImageRef image, CGSize imageSize) {
    NSDictionary*    options  = [NSDictionary dictionaryWithObjectsAndKeys:
                                              [NSNumber numberWithBool:YES], kCVPixelBufferCGImageCompatibilityKey,
                                              [NSNumber numberWithBool:YES], kCVPixelBufferCGBitmapContextCompatibilityKey,
                                              nil];
    CVPixelBufferRef pxbuffer = NULL;
    CVReturn         status   = CVPixelBufferCreate(kCFAllocatorDefault, imageSize.width, imageSize.height, kCVPixelFormatType_32ARGB, (__bridge CFDictionaryRef)options, &pxbuffer);
    if (status != kCVReturnSuccess && pxbuffer == NULL)
        return 0;

    CVPixelBufferLockBaseAddress(pxbuffer, 0);
    void* pxdata = CVPixelBufferGetBaseAddress(pxbuffer);
    if (!pxdata)
        return 0;

    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef    context       = CGBitmapContextCreate(pxdata, imageSize.width, imageSize.height, 8, 4 * imageSize.width, rgbColorSpace, (CGBitmapInfo)kCGImageAlphaNoneSkipFirst);
    if (!context) {
        CGColorSpaceRelease(rgbColorSpace);
        return 0;
    }
    CGAffineTransform trans = CGAffineTransformIdentity;
    CGContextConcatCTM(context, trans);
    CGContextDrawImage(context, CGRectMake(0, 0, CGImageGetWidth(image), CGImageGetHeight(image)), image);
    CGColorSpaceRelease(rgbColorSpace);
    CGContextRelease(context);

    CVPixelBufferUnlockBaseAddress(pxbuffer, 0);

    return pxbuffer;
}

- (void)doMakeVideoUIUpdate {
    self.renderingProgressLabel.text = @"MAKING VIDEO";
    [self.makeVideoLoading setHidden:NO];
    [self.makeVideoLoading startAnimating];
}

- (void)doMakeGifUIUpdate {
    self.renderingProgressLabel.text = @"MAKING GIF";
    [self.makeVideoLoading setHidden:NO];
    [self.makeVideoLoading startAnimating];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)makingGif {
    /*
    imagesArray = [[NSMutableArray alloc] init];
    float fps = 8.0;
    NSString* tempDir = NSTemporaryDirectory();
    for (int i = _trimControl.leftValue; i <= _trimControl.rightValue; i+=3) {
        NSString* file = [tempDir stringByAppendingPathComponent:[NSString stringWithFormat:@"r-%d.png", i]];
        [imagesArray addObject:file];
    }
    NSDictionary *fileProperties = @{(__bridge id)kCGImagePropertyGIFDictionary: @{(__bridge id)kCGImagePropertyGIFLoopCount: @0, // 0 means loop forever
                                                                                   }};
    NSDictionary *frameProperties = @{(__bridge id)kCGImagePropertyGIFDictionary: @{(__bridge id)kCGImagePropertyGIFDelayTime: @(1.0/fps), // a float (not double!) in seconds, rounded to centiseconds in the GIF data
                                                                                    }};
    NSURL *documentsDirectoryURL = [NSURL URLWithString:[NSString stringWithFormat:@"file://%@",tempDir]];
    NSURL *fileURL = [documentsDirectoryURL URLByAppendingPathComponent:@"animated.gif"];
    CGImageDestinationRef destination = CGImageDestinationCreateWithURL((__bridge CFURLRef)fileURL, kUTTypeGIF, [imagesArray count], NULL);
    CGImageDestinationSetProperties(destination, (__bridge CFDictionaryRef)fileProperties);
    for (int i = 0; i < [imagesArray count]; i++) {
        @autoreleasepool {
            NSString* file = [NSString stringWithFormat:@"%@r-%d.png", tempDir, i*3];
            UIImage *shacho = [UIImage imageWithContentsOfFile:file];
            CGImageDestinationAddImage(destination, shacho.CGImage, (CFDictionaryRef)frameProperties);
        }
    }
    if (!CGImageDestinationFinalize(destination)) {
        NSLog(@"failed to finalize image destination");
    }
    CFRelease(destination);
     */
}

- (IBAction)exportButtonAction:(id)sender {
    [self.exportButton setHidden:true];
    [self.shareActivityIndicator setHidden:false];
    [self.shareActivityIndicator startAnimating];

    NSArray*  objectsToShare;
    NSString* tempDir = NSTemporaryDirectory();
    if (renderingExportImage == RENDER_IMAGE) {
        NSString* filePath = [NSString stringWithFormat:@"%@r-%d.png", tempDir, renderingFrame - 1];
        objectsToShare     = [NSArray arrayWithObjects:@"A 3D Scene, I created using Iyan 3D on my iPad", [UIImage imageWithContentsOfFile:filePath], nil];
    } else if (renderingExportImage == RENDER_VIDEO) {
        NSString* filePath  = [NSString stringWithFormat:@"%@/myMovie.mov", tempDir];
        NSURL*    videoPath = [NSURL fileURLWithPath:filePath];
        objectsToShare      = [NSArray arrayWithObjects:@"An Animation video, I created using Iyan 3D on my iPad", videoPath, nil];
    } else if (renderingExportImage == RENDER_GIF) {
        NSString* filePath  = [NSString stringWithFormat:@"%@/animated.gif", tempDir];
        NSURL*    videoPath = [NSURL fileURLWithPath:filePath];
        objectsToShare      = [NSArray arrayWithObjects:@"An Animated Gif, I created using Iyan 3D on my iPad", videoPath, nil];
    }

    UIActivityViewController* controller = [[UIActivityViewController alloc] initWithActivityItems:objectsToShare applicationActivities:nil];

    if ([[UIDevice currentDevice].systemVersion floatValue] >= 8.0) {
        if (!controller.popoverPresentationController.barButtonItem) {
            controller.popoverPresentationController.sourceView = self.view;
            controller.popoverPresentationController.sourceRect = self.exportButton.frame;
        }
    }
    [controller setCompletionHandler:^(NSString* activityType, BOOL completed) {
        [self.shareActivityIndicator stopAnimating];
        [self.shareActivityIndicator setHidden:true];
        [self.exportButton setHidden:false];
    }];

    [self presentViewController:controller animated:YES completion:nil];
}

- (IBAction)rateButtonAction:(id)sender {
    //    NSString *templateReviewURL = @"itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=640516535";

    NSString* templateReviewURLiOS7 = @"https://itunes.apple.com/app/id640516535?mt=8";
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:templateReviewURLiOS7]];
}

- (void)upgradeButtonPressed {
    //[self.delegate upgradeButtonPressed];
    //[[AppHelper getAppHelper] callPaymentGateWayForPremiumUpgrade];
}

- (void)loadingViewStatus:(BOOL)status {
}

- (void)premiumUnlocked {
    if (tempSelectedIndex == 0 || tempSelectedIndex == -1) {
        if (self.watermarkSwitch.isOn)
            [self.watermarkSwitch setOn:NO];
        [self.renderingTypes reloadData];
    } else {
        selectedIndex = tempSelectedIndex;
        [self.renderingTypes reloadData];
    }

    [[UIApplication sharedApplication] endIgnoringInteractionEvents];
}

- (void)premiumUnlockedCancelled {
}

- (void)statusForOBJImport:(NSNumber*)status {
}

- (void)changeVertexColor:(Vector3)vetexColor dragFinish:(BOOL)isDragFinish {
    bgColor = vetexColor;
    if (isDragFinish) {
        [self.delegate changeRenderingBgColor:Vector4(bgColor, (_transparentBgSwitch.isOn) ? 0.0 : 1.0)];
    }
}

- (void)deallocMem {
    thread = nil;
    [self.delegate freezeEditorRender:NO];
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

- (IBAction)transparentBgValueChanged:(id)sender {
    (_transparentBgSwitch.isOn) ? [self.delegate changeRenderingBgColor:(Vector4(bgColor, 0.0))] : [self.delegate changeRenderingBgColor:(Vector4(bgColor, 1.0))];
}

- (void)image:(UIImage*)image didFinishSavingWithError:(NSError*)error contextInfo:(void*)contextInfo {
    if (error == nil)
        [self performSelectorOnMainThread:@selector(showPreviewInMainThread:) withObject:outputFilePath waitUntilDone:NO];
}

- (void)video:(NSString*)videoPath didFinishSavingWithError:(NSError*)error contextInfo:(void*)contextInfo {
    if (error == nil)
        [self performSelectorOnMainThread:@selector(showPreviewInMainThread:) withObject:outputFilePath waitUntilDone:NO];
}

@end
