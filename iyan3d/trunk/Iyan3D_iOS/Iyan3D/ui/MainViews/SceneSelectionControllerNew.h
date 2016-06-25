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
#import <DropboxSDK/DropboxSDK.h>

#import "Utility.h"
#import "HelpViewController.h"
#import "AppHelper.h"
#import "GAI.h"
#import "SceneSelectionFrameCell.h"
#import "EditorViewController.h"
#import "WEPopoverController.h"
#import "SettingsViewController.h"
#import "LoggedInViewController.h"
#import "FollowUsVC.h"


@interface SceneSelectionControllerNew : GAITrackedViewController< UIGestureRecognizerDelegate, DBRestClientDelegate, UICollectionViewDelegate,UICollectionViewDataSource,MFMailComposeViewControllerDelegate,ScenePropertiesDelegate,UIAlertViewDelegate,PopUpViewControllerDelegate ,WEPopoverControllerDelegate,SettingsViewControllerDelegate,LoggedinViewControllerDelegat,LoginViewControllerDelegate,MFMailComposeViewControllerDelegate , OnBoardDelegate>{
    NSMutableArray *scenesArray;
    CacheSystem* cache;
    NSDateFormatter *dateFormatter;
    int currentSelectedScene;
    CGPoint cell_center;
    int sceneToBeRenamed;
    CGFloat screenHeight;
    SettingsViewController *settingsVc;
    LoginViewController *loginVc;
    DBRestClient *restClient;
    
    bool isLoggedin;
    bool isFirstTime;
    int selectedSceneIndex;
    bool isFirstTimeUser;
}

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil IsFirstTimeOpen:(BOOL)value;

@property (weak, nonatomic) IBOutlet UIView *topBar;
@property (nonatomic, strong) NSString *fileBeginsWith;
@property (assign) BOOL fromLoadingView;
@property (assign) BOOL isAppFirstTime;
@property (weak, nonatomic) IBOutlet UIButton *cloneSceneBtn;
@property (weak, nonatomic) IBOutlet UIButton *deleteSceneBtn;
@property (weak, nonatomic) IBOutlet UIButton *mailBtn;
@property (weak, nonatomic) IBOutlet UIButton *addSceneBtn;
@property (weak, nonatomic) IBOutlet UIView *sceneView;
@property (weak, nonatomic) IBOutlet UIImageView *scenePreview;
@property (weak, nonatomic) IBOutlet UITextField *sceneTitle;
@property (weak, nonatomic) IBOutlet UILabel *sceneDate;
@property (weak, nonatomic) IBOutlet UIButton *scenePreviewCloseBtn;
@property (weak, nonatomic) IBOutlet UIButton *openScene;
@property (weak, nonatomic) IBOutlet UIButton *infoBtn;
@property (weak, nonatomic) IBOutlet UIButton *loginBtn;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *centerLoading;

@property (weak, nonatomic) IBOutlet UICollectionView *scenesCollectionView;
@property (nonatomic, strong) PopUpViewController *popUpVc;
@property (nonatomic, strong) FollowUsVC *followUsVC;
@property (nonatomic, strong) WEPopoverController *popoverController;
@property (nonatomic, strong) LoggedInViewController *loggedInVc;
@property (weak, nonatomic) IBOutlet UIButton *helpBtn;

- (IBAction)toolTipAction:(id)sender;
- (IBAction)openSceneAction:(id)sender;
- (IBAction)loginBtnAction:(id)sender;

@end

#endif