//
//  SceneSelectionControllerNew.h
//  Iyan3D
//
//  Created by harishankarn on 16/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SceneSelectionControllerNew_h
#define SceneSelectionControllerNew_h

#import <UIKit/UIKit.h>
#import <MessageUI/MFMailComposeViewController.h>
#import <MediaPlayer/MediaPlayer.h>

#import "Utility.h"
#import "AppHelper.h"
#import "SceneSelectionFrameCell.h"
#import "EditorViewController.h"
#import "WEPopoverController.h"
#import "SettingsViewController.h"

@interface SceneSelectionControllerNew : UIViewController <UIGestureRecognizerDelegate, UICollectionViewDelegate, UICollectionViewDataSource, MFMailComposeViewControllerDelegate, ScenePropertiesDelegate, UIAlertViewDelegate, PopUpViewControllerDelegate, WEPopoverControllerDelegate, SettingsViewControllerDelegate, MFMailComposeViewControllerDelegate, OnBoardDelegate, UIDocumentInteractionControllerDelegate> {
    NSMutableArray*         scenesArray;
    CacheSystem*            cache;
    NSDateFormatter*        dateFormatter;
    CGPoint                 cell_center;
    int                     sceneToBeRenamed;
    CGFloat                 screenHeight;
    SettingsViewController* settingsVc;

    bool isFirstTime;
    int  selectedSceneIndex;
    bool isFirstTimeUser;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil IsFirstTimeOpen:(BOOL)value;

@property (weak, nonatomic) IBOutlet UIView* topBar;
@property (nonatomic, strong) NSString*      fileBeginsWith;
@property (assign) BOOL                      fromLoadingView;
@property (assign) BOOL                      isAppFirstTime;
@property (weak, nonatomic) IBOutlet UIButton* addSceneBtn;
@property (weak, nonatomic) IBOutlet UIButton* infoBtn;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView* centerLoading;

@property (weak, nonatomic) IBOutlet UICollectionView* scenesCollectionView;
@property (nonatomic, strong) PopUpViewController*     popUpVc;
@property (nonatomic, strong) WEPopoverController*     popoverController;

@property (nonatomic, strong) UIDocumentInteractionController* docController;
@property (weak, nonatomic) IBOutlet UIButton* helpBtn;
@property (weak, nonatomic) IBOutlet UILabel* sceneTitleLabel;

- (IBAction)toolTipAction:(id)sender;

@end

#endif
