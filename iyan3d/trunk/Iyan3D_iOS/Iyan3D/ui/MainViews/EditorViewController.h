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
#import "PopUpViewController.h"
#import "LightProperties.h"
#import <GameKit/GameKit.h>
#import "RenderingView.h"
#import "ImportImageNew.h"
#import "AnimationSelectionSlider.h"
#import "TextSelectionSidePanel.h"
#import "AssetSelectionSidePanel.h"
#import "RenderingViewController.h"
#import "SGEditorScene.h"
#import "FPPopoverController.h"
#import "RenderViewManager.h"
#import "WEPopoverController.h"
#import "LoginViewController.h"
#import "LoggedInViewController.h"
#import "SceneItem.h"
#import "SettingsViewController.h"
#import "ScaleViewController.h"
#import "ObjSidePanel.h"

@interface EditorViewController : UIViewController <UITableViewDelegate, UITableViewDataSource, UIActionSheetDelegate, UIAlertViewDelegate,ImageImportNewDelgate,SliderDelegate,TextSelectionDelegate,AssetSelectionDelegate,RenderingViewControllerDelegate,RenderViewManagerDelegate,PopUpViewControllerDelegate ,WEPopoverControllerDelegate,LightPropertiesDelegate,ScalePropertiesViewControllerDelegate,ObjSliderDelegate>{
    int totalFrames;
    NSMutableArray *assetsInScenes;
    ImportImageNew *importImageViewVC;
    AnimationSelectionSlider *animationsliderVC;
    TextSelectionSidePanel *textSelectionSlider;
    AssetSelectionSidePanel *assetSelectionSlider;
    LoginViewController *loginVc;
    ObjSidePanel *objVc;
    LightProperties *lightProperties;
    bool isMetalSupported;
    bool isLoggedin;
    RenderViewManager *renderViewMan;
    SceneManager *smgr;
    SGEditorScene *editorScene;
    SceneItem *currentScene;
    int IndexOfSelected;
    CacheSystem *cache;
    bool isViewLoaded , isPlaying;
    int assetAddType , previousAction , lightCount;
    NSString *imgSalt;
    NSTimer *playTimer;
    SettingsViewController *settingsVc;
    CADisplayLink* displayLink;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil SceneItem:(SceneItem*)scene selectedindex:(int)index;


@property (weak, nonatomic) IBOutlet RenderingView *renderView;
@property (weak, nonatomic) IBOutlet UIButton *playBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *framesCollectionView;
@property (weak, nonatomic) IBOutlet UIButton *addFrameBtn;
@property (weak, nonatomic) IBOutlet UIButton *extrenderBtn;
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
@property (nonatomic, strong) UIImagePickerController *imagePicker;
@property (nonatomic, strong) FPPopoverController* popOverView;
@property (nonatomic, strong) WEPopoverController *popoverController;
@property (nonatomic, strong) PopUpViewController *popUpVc;
@property (nonatomic, strong) LoggedInViewController *loggedInVc;
@property (nonatomic, strong) LightProperties *lightProp;
@property (nonatomic, strong) ScaleViewController *scaleProps;

@property (weak, nonatomic) IBOutlet UIButton *loginBtn;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *center_progress;


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


@end

#endif
