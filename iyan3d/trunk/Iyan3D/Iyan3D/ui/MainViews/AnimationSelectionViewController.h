//
//  AnimationSelectionViewController.h
//  Iyan3D
//
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#ifndef AnimationSelectionViewController_h
#define AnimationSelectionViewController_h


#import <UIKit/UIKit.h>
#import <GameKit/GameKit.h>
#import <GoogleSignIn/GoogleSignIn.h>
#import "HelpViewController.h"
#import "CacheSystem.h"
#import "RenderingView.h"
#import "Constants.h"
#import "NSData+Base64.h"
#import "SGNode.h"
#import "GAI.h"
#import "AppHelper.h"

#import "AuthInspectorViewController.h"
#import "DataPickerState.h"
#import "DataPickerViewController.h"

@class GIDSignInButton;

@protocol AnimationSelectionViewControllerDelegate

- (void) loadNodeInScene:(int)assetId;
- (void) applyAnimationToSelectedNode:(NSString*)filePath;
- (void) resetContext;
- (int) getBoneCount;

@end
static NSString *const kPlaceholderUserName = @"<Name>";
static NSString *const kPlaceholderEmailAddress = @"<Email>";
static NSString *const kPlaceholderAvatarImageName = @"PlaceholderAvatar.png";

// Labels for the cells that have in-cell control elements.
static NSString *const kGetUserProfileCellLabel = @"Get user Basic Profile";
static NSString *const kAllowsSignInWithBrowserLabel = @"Allow Sign In with Browser";
static NSString *const kAllowsSignInWithWebViewLabel = @"Allow Sign In with Web View";
static NSString *const kButtonWidthCellLabel = @"Width";

// Labels for the cells that drill down to data pickers.
static NSString *const kColorSchemeCellLabel = @"Color scheme";
static NSString *const kStyleCellLabel = @"Style";

// Strings for Alert Views.
static NSString *const kSignInViewTitle = @"Sign in View";
static NSString *const kSignOutAlertViewTitle = @"Warning";
static NSString *const kSignOutAlertCancelTitle = @"Cancel";
static NSString *const kSignOutAlertConfirmTitle = @"Continue";

// Accessibility Identifiers.
static NSString *const kCredentialsButtonAccessibilityIdentifier = @"Credentials";

@interface AnimationSelectionViewController : GAITrackedViewController<UICollectionViewDataSource, UICollectionViewDelegate,HelpViewControllerDelegate,UISearchBarDelegate,GIDSignInDelegate, GIDSignInUIDelegate, AppHelperDelegate>
{
    CacheSystem *cache;
    ANIMATION_TYPE animationType;
    string fontFilePath;
    Vector4 textColor;
    int fontSize , bevelValue;
    NSMutableArray* assetArray;
    NSMutableArray *restoreIdArr;
    NSArray *jsonUserArray;
    int tabValue , previousTabValue;
    AnimationItem *asset;
    NSNumberFormatter * _priceFormatter;
    NSOperationQueue *downloadQueue , *animDownloadQueue;
    
    int storeType;
    int _assetId,selectedCell;
    BOOL isanimationExist , isAppEntersBG;
    BOOL isMyLibrary,isCurrentAssetLiked;
    BOOL isRestorePurchasePressed;
    int priceIterator;
    NSString* docDirPath;
    NSTimer *playTimer;
    NSString *userid;
    
    BOOL firstFrame;
    NSThread *renderingThread;
    BOOL pendingLoading;
    id pendingAssetValue;
    
    NSTimer *displayTimer;
    CADisplayLink *displayLink;
    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    GLuint _colorRenderBuffer;
    GLuint _depthRenderBuffer;
    GLuint _frameBuffer;
    float screenScale;
    NSMutableArray *animationJsonArray;
    // This is an array of arrays, each one corresponding to the cell
    // labels for its respective section.
    NSArray *_sectionCellLabels;
    
    // These sets contain the labels corresponding to cells that have various
    // types (each cell either drills down to another table view, contains an
    // in-cell switch, or contains a slider).
    NSArray *_drillDownCells;
    NSArray *_switchCells;
    NSArray *_sliderCells;
    
    // Map that keeps track of which cell corresponds to which DataPickerState.
    NSDictionary *_drilldownCellState;
    // States storing the current set of selected elements for each data picker.
    DataPickerState *_colorSchemeState;
    DataPickerState *_styleState;
    
}

@property (nonatomic, copy) void (^confirmActionBlock)(void);
@property (nonatomic, copy) void (^cancelActionBlock)(void);

@property (nonatomic, strong) NSLocale *priceLocale;
@property (nonatomic, strong) NSString *currentAssetName;
@property (nonatomic, assign) SGNode *currentTextNode;
@property(weak, nonatomic) IBOutlet GIDSignInButton *signInButton;
@property (weak, nonatomic) IBOutlet UIImageView *signInImgView;
@property (weak, nonatomic) IBOutlet UICollectionView *assetCollectionView;
@property (weak, nonatomic) IBOutlet UIButton *characterButton;
@property (weak, nonatomic) IBOutlet UIButton *accessoriesButton;
@property (weak, nonatomic) IBOutlet UIButton *backgroundButton;
@property (weak, nonatomic) IBOutlet UIButton *assetLibrary;
@property (weak, nonatomic) IBOutlet UIButton *animationLibrary;
@property (weak, nonatomic) IBOutlet UIButton *trendingButton;
@property (weak, nonatomic) IBOutlet UIButton *featuredButton;
@property (weak, nonatomic) IBOutlet UIButton *topRatedButton;
@property (weak, nonatomic) IBOutlet UIButton *restorePurchaseButton;
@property (weak, nonatomic) IBOutlet UIButton *cancelButton;
@property (weak, nonatomic) IBOutlet UIButton *helpButton;
@property (weak, nonatomic) IBOutlet UIButton *addtoScene;
@property (weak, nonatomic) IBOutlet UISearchBar *searchBar;
@property (weak, nonatomic) IBOutlet UIButton *importObj;
@property (weak, nonatomic) IBOutlet UILabel *priceDisplay;
@property (weak, nonatomic) IBOutlet UILabel *assetNameDisplay;
@property (weak, nonatomic) IBOutlet UILabel *totalLikeCount;
@property (weak, nonatomic) IBOutlet UILabel *totalDownloadCount;
@property (weak, nonatomic) IBOutlet UILabel *uploadDate;
@property (weak, nonatomic) IBOutlet UILabel *username;
@property (weak, nonatomic) IBOutlet UILabel *createdbyText;
@property (weak, nonatomic) IBOutlet UILabel *createdDateText;
@property (weak, nonatomic) IBOutlet UILabel *downloadText;
@property (weak, nonatomic) IBOutlet UILabel *boneCountLbl;
@property (weak, nonatomic) IBOutlet UIView *displayImage;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *restoreActivityView;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *downloadAssetsActivity;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *downloadingActivityView;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *likeActivityView;
@property (weak, nonatomic) id <AnimationSelectionViewControllerDelegate> delegate;
@property (weak, nonatomic) IBOutlet RenderingView *renderView;
@property (weak, nonatomic) IBOutlet UIButton *publishButton;
@property (weak, nonatomic) IBOutlet UIButton *likeButton;

@property(weak, nonatomic) IBOutlet UILabel *signInAuthStatus;
// A label to display the signed-in user's display name.
@property(weak, nonatomic) IBOutlet UILabel *userName;
// A label to display the signed-in user's email address.
@property(weak, nonatomic) IBOutlet UILabel *userEmailAddress;
// An image view to display the signed-in user's avatar image.
@property(weak, nonatomic) IBOutlet UIImageView *userAvatar;
// A button to sign out of this application.
@property(weak, nonatomic) IBOutlet UIButton *signOutButton;
// A button to disconnect user from this application.
@property(weak, nonatomic) IBOutlet UIButton *disconnectButton;
// A button to inspect the authorization object.
@property(weak, nonatomic) IBOutlet UIButton *credentialsButton;
// A dynamically-created slider for controlling the sign-in button width.
@property(weak, nonatomic) UISlider *signInButtonWidthSlider;


- (IBAction)cancelButtonAction:(id)sender;
- (IBAction)addButtonAction:(id)sender;
- (IBAction)helpButtonAction:(id)sender;
- (IBAction)tabChangingFunction:(UIButton *)sender;
- (IBAction)publishButtonAction:(id)sender;
- (IBAction)likeButtonAction:(id)sender;
// Called when the user presses the "Sign out" button.
- (IBAction)signOut:(id)sender;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil withType:(ANIMATION_TYPE)type;
- (void)downloadAnimation:(AnimationItem *)asset;

@end

#endif
