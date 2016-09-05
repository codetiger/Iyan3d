//
//  EditorViewController.h
//  Iyan3D
//
//  Created by Sankar on 18/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef EditorViewController_h
#define EditorViewController_h

#if !(TARGET_IPHONE_SIMULATOR)
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#endif

#import <UIKit/UIKit.h>
#import <MessageUI/MFMailComposeViewController.h>
#import <GameKit/GameKit.h>

#import "OnBoardVC.h"
#import "RenderingView.h"
#import "ImportImageNew.h"
#import "AssetSelectionSidePanel.h"
#import "AnimationSelectionSlider.h"
#import "TextSelectionSidePanel.h"
#import "RenderingViewController.h"
#import "SGEditorScene.h"
#import "RenderViewManager.h"
#import "LoginViewController.h"
#import "LoggedInViewController.h"
#import "SceneItem.h"
#import "SettingsViewController.h"
#import "ScaleViewController.h"
#import "ObjSidePanel.h"
#import "CommonProps.h"
#import "ScaleForAutoRigViewController.h"
#import "FollowUsVC.h"

@interface EditorViewController : GAITrackedViewController <UITableViewDelegate, UITableViewDataSource, UIActionSheetDelegate, UIAlertViewDelegate, TextSelectionDelegate, MFMailComposeViewControllerDelegate, ImageImportNewDelgate, SliderDelegate, AssetSelectionDelegate, RenderingViewControllerDelegate, RenderViewManagerDelegate,PopUpViewControllerDelegate, WEPopoverControllerDelegate, ScalePropertiesViewControllerDelegate, ObjSliderDelegate, LoginViewControllerDelegate, LoggedinViewControllerDelegat, SettingsViewControllerDelegate, AutoRigScaleViewControllerDelegate, UIGestureRecognizerDelegate, OnBoardDelegate, CommonPropDelegate> {
    
    NSMutableArray *assetsInScenes;
    ImportImageNew *importImageViewVC;
    AnimationSelectionSlider *animationsliderVC;
    TextSelectionSidePanel *textSelectionSlider;
    AssetSelectionSidePanel *assetSelectionSlider;
    LoginViewController *loginVc;
    FollowUsVC *followUsVC;
    ObjSidePanel *objVc;
    bool isMetalSupported;
    BOOL isSelected;
    RenderViewManager *renderViewMan;
    SceneManager *smgr;
    SGEditorScene *editorScene;
    SceneItem *currentScene;
    int cameraResolutionType;
    CacheSystem *cache;
    bool isViewLoaded , isPlaying;
    int previousAction , lightCount,selectedNodeId, selectedMeshBufferId;
    ActionType assetAddType;
    NSString *imgSalt;
    NSTimer *playTimer;
    SettingsViewController *settingsVc;
    CADisplayLink* displayLink;
    ScaleForAutoRigViewController *scaleAutoRig;
    Vector4 renderBgColor;
    NSString *cachesDir , *docDir;
    int ScreenWidth;
    int ScreenHeight;
    BOOL isAppInBG;
    
    NSString* maxUnisKey;
    NSString* maxJointsKey;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil SceneItem:(SceneItem*)scene selectedindex:(int)index;
@property (weak, nonatomic) IBOutlet UIButton *toolTipBtn;

@property (weak, nonatomic) IBOutlet RenderingView *renderView;
@property (weak, nonatomic) IBOutlet UIButton *playBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *framesCollectionView;
@property (weak, nonatomic) IBOutlet UIButton *addFrameBtn;
@property (weak, nonatomic) IBOutlet UIButton *viewBtn;
@property (weak, nonatomic) IBOutlet UIButton *infoBtn;
@property (weak, nonatomic) IBOutlet UIButton *editobjectBtn;
@property (weak, nonatomic) IBOutlet UITableView *objectList;
@property (weak, nonatomic) IBOutlet UIButton *exportBtn;
@property (weak, nonatomic) IBOutlet UIButton *animationBtn;
@property (weak, nonatomic) IBOutlet UIButton *importBtn;
@property (weak, nonatomic) IBOutlet UIButton *optionsBtn;
@property (weak, nonatomic) IBOutlet UIButton *moveBtn;
@property (weak, nonatomic) IBOutlet UIButton *rotateBtn;
@property (weak, nonatomic) IBOutlet UIButton *scaleBtn;
@property (weak, nonatomic) IBOutlet UIButton *myObjectsBtn;
@property (weak, nonatomic) IBOutlet UIButton *undoBtn;
@property (weak, nonatomic) IBOutlet UIButton *redoBtn;
@property (weak, nonatomic) IBOutlet UIButton *lastFrameBtn;
@property (weak, nonatomic) IBOutlet UIButton *firstFrameBtn;
@property (weak, nonatomic) IBOutlet UIView *leftView;
@property (weak, nonatomic) IBOutlet UIView *rightView;
@property (weak, nonatomic) IBOutlet UIView *topView;
@property (nonatomic, strong) UIImagePickerController *imagePicker;
@property (nonatomic, strong) WEPopoverController *popoverController;
@property (nonatomic, strong) PopUpViewController *popUpVc;
@property (nonatomic, strong) LoggedInViewController *loggedInVc;
@property (nonatomic, strong) ScaleViewController *scaleProps;
@property (weak, nonatomic) IBOutlet UIButton *loginBtn;
@property (weak, nonatomic) IBOutlet UIButton *moveFirst;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *center_progress;
@property (weak, nonatomic) IBOutlet UILabel *rigScreenLabel;
@property (weak, nonatomic) IBOutlet UIButton *moveLast;
@property (weak, nonatomic) IBOutlet UIButton *addJointBtn;
@property (weak, nonatomic) IBOutlet UIButton *publishBtn;
@property (weak, nonatomic) IBOutlet UIButton *rigCancelBtn;
@property (weak, nonatomic) IBOutlet UIButton *rigAddToSceneBtn;
@property (weak, nonatomic) IBOutlet UIButton *moveBtnAutorig;
@property (weak, nonatomic) IBOutlet UIButton *scaleBtnAutorig;
@property (weak, nonatomic) IBOutlet UIButton *rotateBtnAutorig;
@property (weak, nonatomic) IBOutlet UIView *objTableview;
@property (weak, nonatomic) IBOutlet UIButton *backButton;
@property (weak, nonatomic) IBOutlet UILabel *rigTitle;
@property (weak, nonatomic) IBOutlet UILabel *xLbl;
@property (weak, nonatomic) IBOutlet UILabel *yLbl;
@property (weak, nonatomic) IBOutlet UILabel *zLbl;
@property (weak, nonatomic) IBOutlet UILabel *xValue;
@property (weak, nonatomic) IBOutlet UILabel *yValue;
@property (weak, nonatomic) IBOutlet UILabel *zValue;
@property (weak, nonatomic) IBOutlet UISwitch *autoRigMirrorSwitch;
@property (weak, nonatomic) IBOutlet UILabel *autorigMirrorLable;
@property (weak, nonatomic) IBOutlet UIView *autorigMirrorBtnHolder;
@property (weak, nonatomic) IBOutlet UILabel *sceneMirrorLable;
@property (weak, nonatomic) IBOutlet UISwitch *sceneMirrorSwitch;

@property (weak, nonatomic) IBOutlet UILabel *numberOfItems;
@property (weak, nonatomic) IBOutlet UILabel *autoRigLbl;
@property (weak, nonatomic) IBOutlet UIButton *helpBtn;
@property (weak, nonatomic) IBOutlet UILabel *myObjectsLabel;


- (IBAction)toolTipAction:(id)sender;
- (IBAction)moveLastAction:(id)sender;
- (IBAction)moveFirstAction:(id)sender;
- (IBAction)addJoinAction:(id)sender;
- (IBAction)publishBtnAction:(id)sender;
- (IBAction)editFunction:(id)sender;
- (IBAction)addFrames:(id)sender;
- (IBAction)exportAction:(id)sender;
- (IBAction)loginBtnAction:(id)sender;
- (IBAction)animationBtnAction:(id)sender;
- (IBAction)importBtnAction:(id)sender;
- (IBAction)optionsBtnAction:(id)sender;
- (IBAction)moveBtnAction:(id)sender;
- (IBAction)rotateBtnAction:(id)sender;
- (IBAction)scaleBtnAction:(id)sender;
- (IBAction)undoBtnAction:(id)sender;
- (IBAction)redoBtnAction:(id)sender;
- (IBAction)infoBtnAction:(id)sender;
- (IBAction)viewBtn:(id)sender;
- (IBAction)lastFrameBtnAction:(id)sender;
- (IBAction)firstFrameBtnAction:(id)sender;
- (IBAction)myObjectsBtnAction:(id)sender;
- (IBAction)autorigMirrorSwitchAction:(id)sender;
- (IBAction)sceneMirrorAction:(id)sender;

@end

#endif
