//
//  SettingsViewController.h
//  Iyan3D
//
//  Created by Sankar on 06/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef SettingsViewController_h
#define SettingsViewController_h

#import <UIKit/UIKit.h>
#import "GAI.h"
#import "AppHelper.h"


@protocol SettingsViewControllerDelegate
-(void)frameCountDisplayMode;
-(void)cameraPreviewSize;
-(void)cameraPreviewPosition;
-(void)toolbarPosition:(int)selctedIndex;
-(void)multiSelectUpdate:(BOOL)value;
@end

@interface SettingsViewController : GAITrackedViewController<UIGestureRecognizerDelegate,UIScrollViewDelegate, AppHelperDelegate>{
    UIScrollView *myScrollView;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil;
@property (weak, nonatomic) IBOutlet UIButton *restoreBtn;
@property (weak, nonatomic) IBOutlet UISegmentedControl *toolbarPosition;
@property (weak, nonatomic) IBOutlet UISegmentedControl *renderPreviewSize;
@property (weak, nonatomic) IBOutlet UISegmentedControl *frameCountDisplay;
@property (weak, nonatomic) IBOutlet UISegmentedControl *renderPreviewPosition;
@property (weak, nonatomic) IBOutlet UIImageView *toolbarRight;
@property (strong, nonatomic) IBOutlet UIView *masterView;
@property (weak, nonatomic) IBOutlet UIButton *doneBtn;
@property (weak, nonatomic) id <SettingsViewControllerDelegate> delegate;
@property (weak, nonatomic) IBOutlet UIImageView *toolbarLeft;
@property (weak, nonatomic) IBOutlet UIImageView *renderPreviewSizeSmall;
@property (weak, nonatomic) IBOutlet UIImageView *renderPreviewSizeLarge;
@property (weak, nonatomic) IBOutlet UIImageView *framesDisplayCount;
@property (weak, nonatomic) IBOutlet UISwitch *multiSelectSwitch;
@property (weak, nonatomic) IBOutlet UIImageView *framesDisplayDuration;
@property (weak, nonatomic) IBOutlet UIImageView *previewPositionRightBottom;
@property (weak, nonatomic) IBOutlet UIImageView *previewPositionRightTop;
@property (weak, nonatomic) IBOutlet UIImageView *previewPositionLeftBottom;
@property (weak, nonatomic) IBOutlet UIImageView *previewPositionLeftTop;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *restorePurchaseProgress;
@property (weak, nonatomic) IBOutlet UISwitch *speedSwitch;

- (IBAction)qualityOrSpeed:(id)sender;

- (IBAction)restoreAction:(id)sender;
- (IBAction)toolBarPositionChanged:(id)sender;
- (IBAction)renderPreviewSizeChanged:(id)sender;
- (IBAction)frameCountDisplayType:(id)sender;
- (IBAction)previewpositionChanged:(id)sender;
- (IBAction)doneBtnAction:(id)sender;
- (IBAction)multiselectValueChanged:(id)sender;
@end

#endif