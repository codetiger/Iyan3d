//
//  AnimationSelectionViewController.m
//  Iyan3D
//
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "AnimationSelectionViewController.h"
#import "SGPreviewScene.h"
#import "Utility.h"
#import "AFNetworking.h"
#import "AFHTTPRequestOperation.h"
#import "AssetCellView.h"
#import "PreviewHelper.h"
#import "DownloadTask.h"

#define ASSET_CAMERA 7
#define ASSET_LIGHT 8
#define TRENDING 4
#define FEATURED 5
#define TOP_RATED 6
#define MY_ANIMATION 7
#define ALL_ANIMATION_TABLE 0
#define COMES_FROM_ASSETSELECTION 5
#define USER_NAME_ALERT 100
#define CANCEL_BUTTON 0
#define OK_BUTTON 1

@implementation AnimationSelectionViewController

PreviewHelper* animPreviewHelper;
SGPreviewScene* animPreviewScene;
SceneManager* sceneMgr;

- (void)setUp
{
    _sectionCellLabels = @[
        @[ kColorSchemeCellLabel, kStyleCellLabel, kButtonWidthCellLabel ],
        @[ kGetUserProfileCellLabel, kAllowsSignInWithBrowserLabel, kAllowsSignInWithWebViewLabel ]
    ];

    // Groupings of cell types.
    _drillDownCells = @[
        kColorSchemeCellLabel,
        kStyleCellLabel
    ];

    _switchCells =
        @[ kGetUserProfileCellLabel, kAllowsSignInWithBrowserLabel, kAllowsSignInWithWebViewLabel ];
    _sliderCells = @[ kButtonWidthCellLabel ];

    // Initialize data picker states.
    NSString* dictionaryPath =
        [[NSBundle mainBundle] pathForResource:@"DataPickerDictionary"
                                        ofType:@"plist"];
    NSDictionary* configOptionsDict = [NSDictionary dictionaryWithContentsOfFile:dictionaryPath];

    NSDictionary* colorSchemeDict = [configOptionsDict objectForKey:kColorSchemeCellLabel];
    NSDictionary* styleDict = [configOptionsDict objectForKey:kStyleCellLabel];

    _colorSchemeState = [[DataPickerState alloc] initWithDictionary:colorSchemeDict];
    _styleState = [[DataPickerState alloc] initWithDictionary:styleDict];

    _drilldownCellState = @{
        kColorSchemeCellLabel : _colorSchemeState,
        kStyleCellLabel : _styleState
    };

    // Make sure the GIDSignInButton class is linked in because references from
    // xib file doesn't count.
    [GIDSignInButton class];

    GIDSignIn* signIn = [GIDSignIn sharedInstance];
    signIn.shouldFetchBasicProfile = YES;
    signIn.delegate = self;
    signIn.uiDelegate = self;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil withType:(ANIMATION_TYPE)type
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        [self setUp];
        [self updateButtons];
        animationType = type;
        screenScale = [[UIScreen mainScreen] scale];
        cache = [CacheSystem cacheSystem];
        NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        docDirPath = [srcDirPath objectAtIndex:0];
        restoreIdArr = [[NSMutableArray alloc] init];
        downloadQueue = [[NSOperationQueue alloc] init];
        [downloadQueue setMaxConcurrentOperationCount:3];
        animDownloadQueue = [[NSOperationQueue alloc] init];
        [animDownloadQueue setMaxConcurrentOperationCount:1];
    }
    return self;
}

- (id)initWithCoder:(NSCoder*)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        [self setUp];
        self.title = kSignInViewTitle;
    }
    return self;
}
- (void)viewDidLoad
{
    [super viewDidLoad];
    pendingLoading = false;
    firstFrame = true;
    NSLog(@"googleAuthentication %@", [[AppHelper getAppHelper] userDefaultsForKey:@"googleUserId"]);
    [self.signInButton setHidden:YES];
    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"googleAuthentication"]) {
        //[self.signInButton setHidden:YES];
        [self.signInImgView setHidden:YES];
        [self.signOutButton setHidden:NO];
    }
    else {
        //[self.signInButton setHidden:NO];
        [self.signInImgView setHidden:NO];
        [self.signOutButton setHidden:YES];
    }

    [_signInImgView setUserInteractionEnabled:YES];
    UITapGestureRecognizer* tapGest = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(signInTapped:)];
    [_signInImgView addGestureRecognizer:tapGest];

    [self minimumButtonWidth];
    [self.signInButton setTintColor:[UIColor clearColor]];
    [self.signInButton setBackgroundColor:[UIColor clearColor]];
    self.screenName = @"AnimationSelectionView";
    [self.likeActivityView setHidden:YES];
    [self.publishButton setHidden:YES];
    [self.boneCountLbl setHidden:YES];
    self.cancelButton.layer.cornerRadius = 5.0;
    self.restorePurchaseButton.layer.cornerRadius = 5.0;
    self.addtoScene.layer.cornerRadius = 5.0;
    self.publishButton.layer.cornerRadius = 5.0;
    
    animPreviewHelper = [[PreviewHelper alloc] init];
    animPreviewScene = [animPreviewHelper initScene:self.renderView viewType:ALL_ANIMATION_VIEW];
    [self createDisplayLink];
    [animPreviewHelper addCameraLight];

    [self showAssetsView];
    [self showOrHideViews:YES];

    [AppHelper getAppHelper].delegate = self;
    if ([Utility IsPadDevice]) {
        [self.assetCollectionView registerNib:[UINib nibWithNibName:@"AssetCellView" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    else {
        [self.assetCollectionView registerNib:[UINib nibWithNibName:@"AssetCellViewPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    [self setAllAssetsData];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersBG) name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersFG) name:@"applicationDidBecomeActive" object:nil];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"applicationDidBecomeActive" object:nil];
}

- (void) loadSelectedNodeInRenderingThread
{
    AssetItem* currentAsset = [cache GetAssetByName:_currentAssetName];
    
    if (animationType == RIG_ANIMATION && currentAsset)
        [animPreviewHelper loadNodeInScene:currentAsset.type AssetId:currentAsset.assetId AssetName:[self getwstring:currentAsset.name]];
    else {
        fontFilePath = _currentTextNode->optionalFilePath;
        fontSize = _currentTextNode->props.fontSize;
        textColor = Vector4(_currentTextNode->props.vertexColor.x, _currentTextNode->props.vertexColor.y, _currentTextNode->props.vertexColor.z, 1.0);
        bevelValue = _currentTextNode->props.nodeSpecificFloat;
        [animPreviewHelper loadNodeInScene:FONT AssetId:0 AssetName:[self getwstring:_currentAssetName] FontSize:fontSize BevelValue:bevelValue TextColor:textColor FontPath:fontFilePath];
    }

}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{

}

- (void)signInTapped:(UIGestureRecognizer*)gesture
{
    NSLog(@"Tapped ");
    [[GIDSignIn sharedInstance] signIn];
    //[self showAuthInspector:nil];
}

#pragma mark - GIDSignInDelegate

- (void)signIn:(GIDSignIn*)signIn
    didSignInForUser:(GIDGoogleUser*)user
           withError:(NSError*)error
{
    if (error) {
        _signInAuthStatus.text = [NSString stringWithFormat:@"Status: Authentication error: %@", error];
        return;
    }
    [self reportAuthStatus];
    [self updateButtons];
}

- (void)signIn:(GIDSignIn*)signIn
    didDisconnectWithUser:(GIDGoogleUser*)user
                withError:(NSError*)error
{
    if (error) {
        _signInAuthStatus.text = [NSString stringWithFormat:@"Status: Failed to disconnect: %@", error];
    }
    else {
        _signInAuthStatus.text = [NSString stringWithFormat:@"Status: Disconnected"];
    }
    [self reportAuthStatus];
    [self updateButtons];
}

- (void)presentSignInViewController:(UIViewController*)viewController
{
    [[self navigationController] pushViewController:viewController animated:YES];
}

// Updates the GIDSignIn shared instance and the GIDSignInButton
// to reflect the configuration settings that the user set
- (void)adoptUserSettings
{
 }

// Temporarily force the sign in button to adopt its minimum allowed frame
// so that we can find out its minimum allowed width (used for setting the
// range of the width slider).
- (CGFloat)minimumButtonWidth
{
    CGRect frame = self.signInButton.frame;

    //    if([Utility IsPadDevice]) {
    //        self.signInButton.frame = CGRectMake(26, 121, 150, 44);
    //    }
    //    else {
    //        self.signInButton.frame = CGRectMake(8, 54, 80, 30);
    //    }
    //
    //    CGFloat minimumWidth = self.signInButton.frame.size.width;
    ////    self.signInButton.frame = frame;

    return 0;
}

- (void)reportAuthStatus
{
    GIDGoogleUser* googleUser = [[GIDSignIn sharedInstance] currentUser];
    if (googleUser.authentication) {
        _signInAuthStatus.text = @"Status: Authenticated";
        [[AppHelper getAppHelper] saveBoolUserDefaults:YES withKey:@"googleAuthentication"];
        [[AppHelper getAppHelper] saveToUserDefaults:[NSString stringWithFormat:@"%@", [GIDSignIn sharedInstance].currentUser.profile.email] withKey:@"googleEmail"];
        [[AppHelper getAppHelper] saveToUserDefaults:[NSString stringWithFormat:@"%@", [GIDSignIn sharedInstance].currentUser.userID] withKey:@"googleUserId"];
    }
    else {
        // To authenticate, use Google+ sign-in button.
        _signInAuthStatus.text = @"Status: Not authenticated";
        [[AppHelper getAppHelper] saveBoolUserDefaults:NO withKey:@"googleAuthentication"];
    }

    [self refreshUserInfo];
}

- (void)updateButtons
{
    BOOL authenticated = ([GIDSignIn sharedInstance].currentUser.authentication != nil);
    //self.signInButton.enabled = !authenticated;
    self.signOutButton.enabled = authenticated;
    self.disconnectButton.enabled = authenticated;
    self.credentialsButton.hidden = !authenticated;

    if (authenticated) {
        //[self.signInButton setHidden:YES];
        [self.signInImgView setHidden:YES];
        [self.signOutButton setHidden:NO];
    }
    else {
        //[self.signInButton setHidden:NO];
        [self.signInImgView setHidden:NO];
        [self.signOutButton setHidden:YES];
    }
}

// Update the interface elements containing user data to reflect the
// currently signed in user.
- (void)refreshUserInfo
{
    if ([GIDSignIn sharedInstance].currentUser.authentication == nil) {
        //        self.userName.text = kPlaceholderUserName;
        //        self.userEmailAddress.text = kPlaceholderEmailAddress;
        //        self.userAvatar.image = [UIImage imageNamed:kPlaceholderAvatarImageName];
        return;
    }
    self.userEmailAddress.text = [GIDSignIn sharedInstance].currentUser.profile.email;
    self.userName.text = [GIDSignIn sharedInstance].currentUser.profile.name;

    if (![GIDSignIn sharedInstance].currentUser.profile.hasImage) {
        // There is no Profile Image to be loaded.
        return;
    }
    // Load avatar image asynchronously, in background
    dispatch_queue_t backgroundQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    __weak AnimationSelectionViewController* weakSelf = self;

    dispatch_async(backgroundQueue, ^{
        NSUInteger dimension = round(self.userAvatar.frame.size.width * [[UIScreen mainScreen] scale]);
        NSURL* imageURL =
            [[GIDSignIn sharedInstance]
                    .currentUser.profile imageURLWithDimension:dimension];
        NSData* avatarData = [NSData dataWithContentsOfURL:imageURL];

        if (avatarData) {
            // Update UI from the main thread when available
            dispatch_async(dispatch_get_main_queue(), ^{
                AnimationSelectionViewController* strongSelf = weakSelf;
                if (strongSelf) {
                    strongSelf.userAvatar.image = [UIImage imageWithData:avatarData];
                }
            });
        }
    });
}

#pragma mark - IBActions

- (IBAction)signOut:(id)sender
{
    [[GIDSignIn sharedInstance] signOut];
    [self reportAuthStatus];
    [self updateButtons];
}

- (IBAction)disconnect:(id)sender
{
    [[GIDSignIn sharedInstance] disconnect];
}

- (IBAction)showAuthInspector:(id)sender
{
    AuthInspectorViewController* authInspector = [[AuthInspectorViewController alloc] init];
    [[self navigationController] pushViewController:authInspector animated:YES];
}

- (IBAction)checkSignIn:(id)sender
{
    [self reportAuthStatus];
}

- (void)toggleBasicProfile:(UISwitch*)sender
{
    [GIDSignIn sharedInstance].shouldFetchBasicProfile = sender.on;
}

- (void)toggleAllowSignInWithBrowser:(UISwitch*)sender
{
    [GIDSignIn sharedInstance].allowsSignInWithBrowser = sender.on;
}

- (void)toggleAllowSignInWithWebView:(UISwitch*)sender
{
    [GIDSignIn sharedInstance].allowsSignInWithWebView = sender.on;
}

#pragma end

- (void)appEntersBG
{
    isAppEntersBG = true;
}
- (void)appEntersFG
{
    isAppEntersBG = false;
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    if ([[AppHelper getAppHelper] userDefaultsForKey:@"identifierForVendor"]) {
        [[AppHelper getAppHelper] setIdentifierForVendor];
    }
    userid = [[AppHelper getAppHelper] userDefaultsForKey:@"identifierForVendor"];

    if ([[AppHelper getAppHelper] userDefaultsForKey:@"AnimationUpdate"]) {

        NSTimeInterval timeInterval = [[NSDate date] timeIntervalSinceDate:[[AppHelper getAppHelper] userDefaultsForKey:@"AnimationUpdate"]];
        float hours = timeInterval / 3600;
        float minutes = hours * 60;
        if (minutes > 15.0) {
            [self.downloadAssetsActivity setHidden:NO];
            [self.downloadAssetsActivity startAnimating];
            [self getAnimationData];
        }
        else {
            [self displayBasedOnSelection:[NSNumber numberWithInt:0]];
            if ([assetArray count] <= 0)
                [self getAnimationData];
        }
    }
    else {
        [self.downloadAssetsActivity setHidden:NO];
        [self.downloadAssetsActivity startAnimating];
        [self getAnimationData];
    }
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

#pragma beginFunctions

- (void)showAssetsView
{
    [self.downloadAssetsActivity stopAnimating];
    [self.downloadAssetsActivity setHidden:YES];
    tabValue = previousTabValue = TRENDING;
    [self.trendingButton setHidden:NO];
    [self.featuredButton setHidden:NO];
    [self.topRatedButton setHidden:NO];
    [self.animationLibrary setHidden:NO];
    [self changeButtonBG];
    [self tabChangingFunction:self.trendingButton];
}
- (void)getAnimationData
{
    animationJsonArray = [[NSMutableArray alloc] init];
    if ([[AppHelper getAppHelper] checkInternetConnected]) {
        [[AppHelper getAppHelper] performReadingJsonInQueue:downloadQueue ForPage:ALL_ANIMATION_VIEW];
        [self loadAllUserData];
    }
}

- (void)setAnimationData:(NSArray*)allAnimations
{
    if (allAnimations != nil && allAnimations.count > 0)
        animationJsonArray = [NSMutableArray arrayWithArray:allAnimations];

    if (animationJsonArray != nil && [animationJsonArray count] > 0)
        [self storeDataToLocalDB];

    assetArray = [cache GetAnimationList:animationType fromTable:tabValue Search:@""];
    [self displayBasedOnSelection:[NSNumber numberWithInt:0]];
    if ([assetArray count] == 0 && tabValue != MY_ANIMATION)
        [self showInternetError:NO_INTERNET];
    priceIterator = 0;
    [self.publishButton setHidden:YES];
    if ([assetArray count] > 0) {
        AssetItem* assetItem = assetArray[0];
        selectedCell = assetItem.assetId;
    }
    [self reloadCollectionView];
}

- (void)reloadCollectionView
{
    [self.assetCollectionView reloadData];
    [self.downloadAssetsActivity setHidden:YES];
}

- (void)storeUserDataToLocalDB
{
    [cache UpdateUserInfoToAnimationTable:jsonUserArray Usrid:userid];
    assetArray = [cache GetAnimationList:animationType fromTable:tabValue Search:@""];
    [self reloadCollectionView];
}
- (void)loadAllUserData
{
    DownloadTask* task = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(setUserData:) returnObject:nil outputFilePath:@"" andURL:[NSString stringWithFormat:@"http://iyan3dapp.com/appapi/json/%@.json", userid]];
    task.queuePriority = NSOperationQueuePriorityHigh;
    task.taskType = DOWNLOAD_AND_READ;
    [downloadQueue addOperation:task];
}

- (void)setUserData:(NSData*)rawData
{
    jsonUserArray = [[NSArray alloc] init];

    if (rawData) {
        NSError* error;
        NSData* jsonData = [NSData dataWithData:rawData];//[Utility decryptData:rawData Password:@"SGmanKindWin5SG"];
        if (jsonData == nil) {
            return;
        }
        NSString* jsonStr = [NSString stringWithUTF8String:(const char*)[jsonData bytes]];
        if (jsonStr) {
            jsonStr = [jsonStr stringByTrimmingCharactersInSet:[NSCharacterSet controlCharacterSet]];
            jsonUserArray = [NSJSONSerialization JSONObjectWithData:[jsonStr dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:&error];
            [self storeUserDataToLocalDB];
        }
    }
    else
        jsonUserArray = nil;
}

- (void)storeDataToLocalDB
{
    AnimationItem* animItem = [[AnimationItem alloc] init];
    for (int i = 0; i < [animationJsonArray count]; i++) {
        NSDictionary* dict = animationJsonArray[i];
        animItem.assetId = [[dict valueForKey:@"id"] intValue];
        animItem.type = [[dict valueForKey:@"type"] intValue];
        animItem.boneCount = [[dict valueForKey:@"bonecount"] intValue];
        animItem.published = 1;
        animItem.modifiedDate = [dict valueForKey:@"uploaded"];
        animItem.rating = [[dict valueForKey:@"rating"] intValue];
        animItem.featuredindex = [[dict valueForKey:@"featuredindex"] intValue];
        animItem.downloads = [[dict valueForKey:@"downloads"] intValue];
        animItem.keywords = [dict valueForKey:@"keyword"];
        animItem.assetName = [dict valueForKey:@"name"];
        animItem.userId = [dict valueForKey:@"userid"];
        animItem.userName = [dict valueForKey:@"username"];
        [cache UpdateAnimation:animItem];
    }
    [[AppHelper getAppHelper] saveToUserDefaults:[NSDate date] withKey:@"AnimationUpdate"];
}
- (void)addCameraLight
{
    [animPreviewHelper loadNodeInScene:ASSET_CAMERA AssetId:0 AssetName:L"CAMERA"];
    [animPreviewHelper loadNodeInScene:ASSET_LIGHT AssetId:0 AssetName:L"LIGHT"];
}
- (void)setAllAssetsData
{
}
- (void)loadAllAssets
{
    /*
     [[AppHelper getAppHelper] initHelper];
     [[AppHelper getAppHelper] loadAllAssets];
     [AppHelper getAppHelper].isAssetsUpdated = YES;
     */
}
- (void)tabChangingFunction:(UIButton*)sender
{
    self.searchBar.text = @"";
    [self changeButtonBG];
    tabValue = (int)sender.tag;
    
    if(tabValue != previousTabValue){
        [self cancelOperations:animDownloadQueue];
        [self cancelOperations:downloadQueue];
    }
    
    [self.boneCountLbl setHidden:YES];
    switch (sender.tag) {
    case TRENDING:
        isMyLibrary = 0;
        assetArray = [cache GetAnimationList:animationType fromTable:TRENDING Search:@""];
        self.trendingButton.backgroundColor = [UIColor grayColor];
        break;
    case FEATURED:
        isMyLibrary = 1;
        assetArray = [cache GetAnimationList:animationType fromTable:FEATURED Search:@""];
        self.featuredButton.backgroundColor = [UIColor grayColor];
        break;
    case TOP_RATED:
        isMyLibrary = 1;
        assetArray = [cache GetAnimationList:animationType fromTable:TOP_RATED Search:@""];
        self.topRatedButton.backgroundColor = [UIColor grayColor];
        break;
    case MY_ANIMATION:
        isMyLibrary = 1;
        assetArray = [cache GetAnimationList:animationType fromTable:MY_ANIMATION Search:@""];
        self.animationLibrary.backgroundColor = [UIColor grayColor];
        break;
    default:
        break;
    }
    if ([assetArray count] > 0) {
        AssetItem* assetItem = assetArray[0];
        selectedCell = assetItem.assetId;
    }
    else {
        [self showOrHideViews:YES];
    }
    [self.assetCollectionView reloadData];
    if (assetArray.count <= 0 && sender.tag != MY_ANIMATION) {
        [self showInternetError:SLOW_INTERNET];
        if ([[AppHelper getAppHelper] checkInternetConnected]) {
            [self setAllAssetsData];
        }
        else
            [self showInternetError:NO_INTERNET];
    }
    [self displayBasedOnSelection:[NSNumber numberWithInt:0]];
}
- (void)showInternetError:(int)connectionError
{
    if (connectionError != SLOW_INTERNET) {
        [self.view endEditing:YES];
        UIAlertView* internetError = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to connect to the server, Please check your network settings." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [internetError performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];

        //[self getAnimationData];
    }
}
- (void)showOrHideViews:(BOOL)status
{
    [self.likeButton setHidden:status];
    [self.totalLikeCount setHidden:status];
    [self.totalDownloadCount setHidden:status];
    [self.createdbyText setHidden:status];
    [self.createdDateText setHidden:status];
    [self.username setHidden:status];
    [self.uploadDate setHidden:status];
    [self.downloadText setHidden:status];
}
- (void)myanimationEnableDisable
{
    [self.likeButton setHidden:NO];
    [self.totalLikeCount setHidden:NO];
    [self.totalDownloadCount setHidden:NO];
    [self.createdbyText setHidden:YES];
    [self.createdDateText setHidden:YES];
    [self.username setHidden:YES];
    [self.uploadDate setHidden:YES];
    [self.downloadText setHidden:NO];
}
- (void)changeButtonBG
{
    self.characterButton.backgroundColor = [UIColor blackColor];
    self.trendingButton.backgroundColor = [UIColor blackColor];
    self.featuredButton.backgroundColor = [UIColor blackColor];
    self.topRatedButton.backgroundColor = [UIColor blackColor];
    self.animationLibrary.backgroundColor = [UIColor blackColor];
    self.accessoriesButton.backgroundColor = [UIColor blackColor];
    self.backgroundButton.backgroundColor = [UIColor blackColor];
    self.assetLibrary.backgroundColor = [UIColor blackColor];
}

- (void)likeOperation
{
    [self.likeActivityView setHidden:NO];
}

#pragma end

#pragma buttonAction

- (IBAction)likeButtonAction:(id)sender
{
    [self performSelectorInBackground:@selector(likeOperation) withObject:nil];
    [self.view setUserInteractionEnabled:NO];
    if (![userid isEqualToString:@""]) {
        if (isCurrentAssetLiked)
            isCurrentAssetLiked = FALSE;
        else
            isCurrentAssetLiked = TRUE;
        if ([[AppHelper getAppHelper] checkInternetConnected]) {
            NSString* post;
            AnimationItem* animItem;
            if (tabValue == MY_ANIMATION) {
                animItem = [cache GetAnimation:_assetId fromTable:tabValue];
                post = [NSString stringWithFormat:@"userid=%@&asset_id=%d&rating=%d&download=0", userid, animItem.published, isCurrentAssetLiked];
            }
            else {
                animItem = [cache GetAnimation:_assetId fromTable:ALL_ANIMATION_TABLE];
                post = [NSString stringWithFormat:@"userid=%@&asset_id=%d&rating=%d&download=0", userid, _assetId, isCurrentAssetLiked];
            }
            NSData* postData = [post dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
            NSString* postLength = [NSString stringWithFormat:@"%lu", (unsigned long)[postData length]];
            NSMutableURLRequest* request = [[NSMutableURLRequest alloc] init];
            [request setURL:[NSURL URLWithString:@"http://www.iyan3dapp.com/appapi/createUserInfo.php"]];
            [request setHTTPMethod:@"POST"];
            [request setValue:postLength forHTTPHeaderField:@"Content-Length"];
            [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
            [request setHTTPBody:postData];
            NSURLConnection* conn = [[NSURLConnection alloc] initWithRequest:request delegate:self];
            if (conn) {
                [self loadAllUserData];
                if (animItem) {
                    if (isCurrentAssetLiked) {
                        animItem.isViewerRated = 1;
                        animItem.rating += 1;
                    }
                    else {
                        animItem.isViewerRated = 0;
                        if (animItem.rating > 0)
                            animItem.rating -= 1;
                    }

                    if (tabValue != MY_ANIMATION)
                        [cache UpdateAnimation:animItem];
                    else
                        [cache UpdateMyAnimation:animItem];

                    [self updateLikeAndDownloadStatus:animItem];
                }
            }
            else {
                NSLog(@"Connection could not be made");
            }
        }
    }
    [self.view setUserInteractionEnabled:YES];
    [self.likeActivityView setHidden:YES];
}

- (IBAction)publishButtonAction:(id)sender
{
    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"googleAuthentication"]) {
        [self.view endEditing:YES];
        UIAlertView* userNameAlert = [[UIAlertView alloc] initWithTitle:@"Display Name" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
        [userNameAlert setAlertViewStyle:UIAlertViewStylePlainTextInput];
        [[userNameAlert textFieldAtIndex:0] setPlaceholder:@"Enter Your Name Here"];
        [[userNameAlert textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
        [userNameAlert setTag:USER_NAME_ALERT];
        [userNameAlert show];
        [[userNameAlert textFieldAtIndex:0] becomeFirstResponder];
    }
    else {
        [self.view endEditing:YES];
        UIAlertView* userNameAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Sign in with google to publish the animation." delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
        [userNameAlert show];
    }
}
- (IBAction)addButtonAction:(id)sender
{
    [self addanimationToScene];
}
- (IBAction)cancelButtonAction:(id)sender
{
    [displayTimer invalidate];
    displayTimer = nil;
    [animPreviewHelper removeEngine];
    [self.delegate resetContext];
    [[AppHelper getAppHelper] resetAppHelper];
    [self dismissViewControllerAnimated:YES completion:nil];
}
- (IBAction)helpButtonAction:(id)sender
{
    if ([Utility IsPadDevice]) {
        HelpViewController* morehelpView = [[HelpViewController alloc] initWithNibName:@"HelpViewController" bundle:nil CalledFrom:COMES_FROM_ASSETSELECTION];
        morehelpView.delegate = self;
        morehelpView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:morehelpView animated:YES completion:nil];
        CGRect rect = CGRectInset(morehelpView.view.frame, -80, -0);
        morehelpView.view.superview.backgroundColor = [UIColor clearColor];
        morehelpView.view.frame = rect;
    }
    else {
        HelpViewController* morehelpView = [[HelpViewController alloc] initWithNibName:@"HelpViewControllerPhone" bundle:nil CalledFrom:COMES_FROM_ASSETSELECTION];
        morehelpView.delegate = self;
        morehelpView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:morehelpView animated:YES completion:nil];
    }
}

#pragma end

#pragma OtherFunctions

- (void)publishAssetWithUserName:(NSString*)userName
{
    if ([asset.userId isEqualToString:@""])
        asset.userId = userid;

    if (![userName isEqualToString:@""])
        asset.userName = userName;

    if (![asset.userId isEqualToString:@""]) {
        NSString* extension;
        NSString* imgPathLocation = [NSString stringWithFormat:@"%@/Resources/Animations/%d.png", docDirPath, selectedCell];

        if (animationType == 0)
            extension = @".sgra";
        else
            extension = @".sgta";

        NSString* filePathLocation = [NSString stringWithFormat:@"%@/Resources/Animations/%d%@", docDirPath, selectedCell, extension];
        NSString* name = [NSString stringWithFormat:@"%@", asset.assetName];
        NSString* keyword = [NSString stringWithFormat:@"%@", asset.keywords];
        NSString* username = [NSString stringWithFormat:@"%@", asset.userName];
        NSString* type = [NSString stringWithFormat:@"%d", animationType];
        NSString* uniqueId = [NSString stringWithFormat:@"%@", [[AppHelper getAppHelper] userDefaultsForKey:@"googleUserId"]];
        NSString* email = [NSString stringWithFormat:@"%@", [[AppHelper getAppHelper] userDefaultsForKey:@"googleEmail"]];
        NSString* asset_id = [NSString stringWithFormat:@"%d", asset.assetId];
        NSString* bonecount = [NSString stringWithFormat:@"%d", asset.boneCount];
        NSData* animationFile = [NSData dataWithContentsOfFile:filePathLocation];
        NSData* animationImgFile = [NSData dataWithContentsOfFile:imgPathLocation];

        NSURL* url = [NSURL URLWithString:@"http://www.iyan3dapp.com/appapi/publish.php"];
        NSString* postPath = @"http://www.iyan3dapp.com/appapi/publish.php";

        AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
        NSMutableURLRequest* request = [httpClient multipartFormRequestWithMethod:@"POST"
                                                                             path:postPath
                                                                       parameters:nil
                                                        constructingBodyWithBlock:^(id<AFMultipartFormData> formData) {
                                                            if (animationImgFile != nil)
                                                                [formData appendPartWithFileData:animationImgFile name:@"animationImgFile" fileName:[NSString stringWithFormat:@"%d.png", selectedCell] mimeType:@"image/png"];
                                                            if (animationFile != nil)
                                                                [formData appendPartWithFileData:animationFile name:@"animationFile" fileName:[NSString stringWithFormat:@"%d%@", selectedCell, extension] mimeType:@"image/png"];
                                                            [formData appendPartWithFormData:[userid dataUsingEncoding:NSUTF8StringEncoding] name:@"userid"];
                                                            [formData appendPartWithFormData:[uniqueId dataUsingEncoding:NSUTF8StringEncoding] name:@"uniqueId"];
                                                            [formData appendPartWithFormData:[email dataUsingEncoding:NSUTF8StringEncoding] name:@"email"];
                                                            [formData appendPartWithFormData:[username dataUsingEncoding:NSUTF8StringEncoding] name:@"username"];
                                                            [formData appendPartWithFormData:[name dataUsingEncoding:NSUTF8StringEncoding] name:@"asset_name"];
                                                            [formData appendPartWithFormData:[keyword dataUsingEncoding:NSUTF8StringEncoding] name:@"keyword"];
                                                            [formData appendPartWithFormData:[bonecount dataUsingEncoding:NSUTF8StringEncoding] name:@"bonecount"];
                                                            [formData appendPartWithFormData:[asset_id dataUsingEncoding:NSUTF8StringEncoding] name:@"asset_id"];
                                                            [formData appendPartWithFormData:[type dataUsingEncoding:NSUTF8StringEncoding] name:@"type"];
                                                        }];

        AFHTTPRequestOperation* operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];

        __block BOOL complete = NO;
        [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation* operation, id responseObject) {
            //ret = [self handle:data];
            complete = YES;
            asset.published = [[operation responseString] intValue];
            [cache UpdateMyAnimation:asset];
            [self.downloadAssetsActivity setHidden:YES];
            [self.view setUserInteractionEnabled:YES];
            if ([assetArray containsObject:asset]) {
                int indexRow = (int)[assetArray indexOfObject:asset];
                [self displayBasedOnSelection:[NSNumber numberWithInt:indexRow]];
                [self performSelectorOnMainThread:@selector(reloadCollectionView) withObject:nil waitUntilDone:YES];
            }
        } failure:^(AFHTTPRequestOperation* operation, NSError* error) {
            NSLog(@"Failure: %@", error);
            [self.view setUserInteractionEnabled:YES];
            [self.view endEditing:YES];
            UIAlertView* userNameAlert = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to publish, Please check your network settings." delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
            [userNameAlert show];
            complete = YES;
        }];
        [operation start];
    }
}

#pragma DelegateFunctions

- (void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (alertView.tag == USER_NAME_ALERT) {
        NSCharacterSet* set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];

        if (buttonIndex == OK_BUTTON) {
            NSString* name = [[alertView textFieldAtIndex:0].text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
            if ([name length] == 0) {
                [self.view endEditing:YES];
                UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"User name cannot be empty." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                [errorAlert show];
            }
            else {
                [self.view endEditing:YES];
                if ([name rangeOfCharacterFromSet:set].location != NSNotFound) {
                    UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"User Name cannot contain any special characters." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                    [errorAlert show];
                }
                else {
                    if ([[AppHelper getAppHelper] checkInternetConnected]) {
                        [self.publishButton setHidden:YES];
                        [self.view setUserInteractionEnabled:NO];
                        [self.downloadAssetsActivity setHidden:NO];
                        [self publishAssetWithUserName:[alertView textFieldAtIndex:0].text];
                    }
                }
            }
        }
    }
}

- (NSInteger)collectionView:(UICollectionView*)collectionView numberOfItemsInSection:(NSInteger)section
{
    if ([assetArray count] == 0) {
        [self.addtoScene setHidden:YES];
        [self.assetNameDisplay setHidden:YES];
    }
    else {
        [self.assetNameDisplay setHidden:NO];
    }
    return [assetArray count];
}
- (AssetCellView*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    if ([assetArray count] > indexPath.row) {
        AnimationItem* assetItem = assetArray[indexPath.row];
        AssetCellView* cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
        NSString* extension;
        if ([Utility IsPadDevice]) {
            cell.layer.cornerRadius = 10.0;
            extension = @"";
        }
        else {
            cell.layer.cornerRadius = 8.0;
            extension = @"-phone";
        }
        cell.layer.borderWidth = 1.0f;
        cell.layer.borderColor = [UIColor grayColor].CGColor;
        cell.premiumicon.hidden = YES;
        cell.assetNameLabel.text = assetItem.assetName;
        cell.backgroundColor = [UIColor clearColor];
        VIEW_TYPE viewType = (tabValue == MY_ANIMATION) ? MY_ANIMATION_VIEW : ALL_ANIMATION_VIEW;
        [cell.assetImageView setImageInfo:[NSString stringWithFormat:@"%d", assetItem.assetId] forView:viewType OperationQueue:downloadQueue];
        cell.assetNameLabel.textColor = [UIColor whiteColor];
        if (assetItem.assetId == selectedCell) {
            cell.backgroundColor = [UIColor colorWithRed:37.0f / 255.0f green:37.0f / 255.0f blue:37.0f / 255.0f alpha:1.0f];
        }
        return cell;
    }
}
- (void)collectionView:(UICollectionView*)collectionView didSelectItemAtIndexPath:(NSIndexPath*)indexPath
{
    [self.downloadAssetsActivity setHidden:NO];
    [self.downloadAssetsActivity startAnimating];

    NSInteger type = NULL;
    type = tabValue;
    storeType = (int)type;
    NSArray* indexPathArr = [collectionView indexPathsForVisibleItems];
    for (int i = 0; i < [indexPathArr count]; i++) {
        NSIndexPath* indexPath = [indexPathArr objectAtIndex:i];
        UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
        cell.backgroundColor = [UIColor clearColor];
    }
    UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
    cell.backgroundColor = [UIColor colorWithRed:37.0f / 255.0f green:37.0f / 255.0f blue:37.0f / 255.0f alpha:1.0f];

    if ([assetArray count] > (int)indexPath.row) {
        AnimationItem* animItem = assetArray[indexPath.row];
        if (_assetId != animItem.assetId) {
            [self stopAllAnimations];
            [self displayBasedOnSelection:[NSNumber numberWithInteger:indexPath.row]];
        }
        else {
            if (!self.addtoScene.isHidden || !self.boneCountLbl.isHidden) {
                [self.downloadAssetsActivity stopAnimating];
                [self.downloadAssetsActivity setHidden:YES];
            }
        }
    }
    else {
        [self stopAllAnimations];
    }
}

- (void)displayBasedOnSelection:(NSNumber*)rowIndex
{
    [self.addtoScene setHidden:YES];

    if ([assetArray count] > [rowIndex intValue]) {
        AnimationItem* assetItem = assetArray[[rowIndex intValue]];
        [self.addtoScene setTitle:@"ADD TO SCENE" forState:UIControlStateNormal];
        self.priceDisplay.text = @"";
        selectedCell = assetItem.assetId;
        asset = assetItem;
        self.assetNameDisplay.text = assetItem.assetName;
        _assetId = assetItem.assetId;
        if (assetItem)
            [self downloadAnimation:assetItem];
        NSString* extension;
        if ([Utility IsPadDevice])
            extension = @"";
        else
            extension = @"-phone";
        if (tabValue == MY_ANIMATION) {
            if (assetItem.published == 0) {
                [self.publishButton setHidden:NO];
                [self.likeButton setHidden:YES];
                [self showOrHideViews:YES];
            }
            else {
                //_assetId = assetItem.published;
                [self.publishButton setHidden:YES];
                [self showOrHideViews:YES];
                AnimationItem* publishedAsset = [cache GetAnimation:_assetId fromTable:MY_ANIMATION];
                AnimationItem* published = [cache GetAnimation:assetItem.published fromTable:ALL_ANIMATION_TABLE];
                if (publishedAsset) {
                    if (published)
                        self.totalLikeCount.text = [NSString stringWithFormat:@"%d", published.rating];
                    else
                        self.totalLikeCount.text = [NSString stringWithFormat:@"%d", publishedAsset.rating];
                    self.totalDownloadCount.text = [NSString stringWithFormat:@"%d", published.downloads];
                    if (publishedAsset.isViewerRated == 0)
                        [self.likeButton setImage:[UIImage imageNamed:[NSString stringWithFormat:@"like%@.png", extension]] forState:UIControlStateNormal];
                    else
                        [self.likeButton setImage:[UIImage imageNamed:[NSString stringWithFormat:@"like-fill%@.png", extension]] forState:UIControlStateNormal];
                    isCurrentAssetLiked = publishedAsset.isViewerRated;
                }
                else {
                    self.totalLikeCount.text = @"0";
                    self.totalDownloadCount.text = @"0";
                }
                [self.likeButton setHidden:NO];
                [self.totalLikeCount setHidden:NO];
                [self.totalDownloadCount setHidden:NO];
                [self.downloadText setHidden:NO];
            }
        }
        else {
            [self.publishButton setHidden:YES];
            [self.likeButton setHidden:NO];
            [self showOrHideViews:NO];
            self.assetNameDisplay.text = assetItem.assetName;

            NSDateFormatter* dateFormatter = [[NSDateFormatter alloc] init];
            dateFormatter.dateFormat = @"yyyy-MM-dd HH:mm:ss";
            NSDate* yourDate = [dateFormatter dateFromString:assetItem.modifiedDate];
            dateFormatter.dateFormat = @"dd-MMM-yyyy";
            [self updateLikeAndDownloadStatus:assetItem];
            self.uploadDate.text = [dateFormatter stringFromDate:yourDate];
            self.username.text = [assetItem.userName uppercaseString];
        }
        if (animPreviewHelper) {
            if (assetItem.boneCount == [animPreviewHelper getBoneCount] || assetItem.type == TEXT_ANIMATION)
                [self.boneCountLbl setHidden:YES];
            else
                [self.boneCountLbl setHidden:NO];
        }
    }
    else {
        [self stopAllAnimations];
    }
}

- (void)updateLikeAndDownloadStatus:(AnimationItem*)animItem
{
    NSString* extension;
    if ([Utility IsPadDevice])
        extension = @"";
    else
        extension = @"-phone";

    if (animItem) {
        if (animItem.isViewerRated == 0)
            [self.likeButton setImage:[UIImage imageNamed:[NSString stringWithFormat:@"like%@.png", extension]] forState:UIControlStateNormal];
        else
            [self.likeButton setImage:[UIImage imageNamed:[NSString stringWithFormat:@"like-fill%@.png", extension]] forState:UIControlStateNormal];

        self.totalDownloadCount.text = [NSString stringWithFormat:@" %d ", animItem.downloads];
        self.totalLikeCount.text = [NSString stringWithFormat:@" %d ", animItem.rating];

        isCurrentAssetLiked = (animItem.isViewerRated == 0) ? FALSE : TRUE;
    }
    [self.likeActivityView setHidden:YES];
}

- (void)connection:(NSURLConnection*)connection didReceiveData:(NSData*)data
{
}
- (void)connection:(NSURLConnection*)connection didFailWithError:(NSError*)error
{
}
- (void)connectionDidFinishLoading:(NSURLConnection*)connection
{
}
- (BOOL)searchBarShouldBeginEditing:(UISearchBar*)searchBar
{
    UITextField* searchBarTextField = nil;
    for (UIView* subView in self.searchBar.subviews) {
        for (UIView* sndSubView in subView.subviews) {
            if ([sndSubView isKindOfClass:[UITextField class]]) {
                searchBarTextField = (UITextField*)sndSubView;
                break;
            }
        }
    }
    searchBarTextField.enablesReturnKeyAutomatically = NO;
    return YES;
}
- (void)searchBarSearchButtonClicked:(UISearchBar*)searchBar
{
    [self.view endEditing:YES];
}
- (void)searchBar:(UISearchBar*)searchBar textDidChange:(NSString*)searchText
{
    NSInteger type = NULL;
    type = tabValue;

    assetArray = [cache GetAnimationList:animationType fromTable:(int)type Search:searchText];
    [self.assetCollectionView reloadData];
    [self displayBasedOnSelection:[NSNumber numberWithInt:0]];
}
- (void)addanimationToScene
{
    [self.downloadingActivityView setHidden:NO];
    [self.addtoScene setHidden:YES];
    [self.downloadingActivityView startAnimating];

    [self downloadAsset];
}
- (void)downloadAsset
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirectory = [docPaths objectAtIndex:0];
    NSString* animDirPath = [docDirectory stringByAppendingPathComponent:@"Resources/Animations"];
    AnimationItem* assetItem = [cache GetAnimation:_assetId fromTable:(tabValue == MY_ANIMATION) ? MY_ANIMATION : ALL_ANIMATION_TABLE];
    NSString *fileName, *url;
    NSString* extension = (animationType == RIG_ANIMATION) ? @"sgra" : @"sgta";

    if (tabValue == MY_ANIMATION) {
        fileName = [NSString stringWithFormat:@"%@/%d.%@", animDirPath, assetItem.assetId, extension];
        [self proceedToFileVerification:fileName];
    }
    else {
        fileName = [NSString stringWithFormat:@"%@/%d.%@", cacheDirectory, assetItem.assetId, extension];
        url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/animationFile/%d.%@", assetItem.assetId, extension];
        //[animDownloadQueue cancelAllOperations];
        [self cancelOperations:animDownloadQueue];
        DownloadTask* task = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(proceedToFileVerification:) returnObject:fileName outputFilePath:fileName andURL:url];
        task.queuePriority = NSOperationQueuePriorityVeryHigh;
        [animDownloadQueue addOperation:task];
    }
}
- (void)proceedToFileVerification:(id)object
{
    NSString* filePath;
    if([object isKindOfClass:[DownloadTask class]])
        filePath = ((DownloadTask*)object).outputPath;
    else
       filePath = object;
       

    if (![[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* cacheDirectory = [paths objectAtIndex:0];
        NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* docDirectory = [docPaths objectAtIndex:0];
        NSString* animDirPath = [docDirectory stringByAppendingPathComponent:@"Resources/Animations"];

        NSString* fileName = [filePath lastPathComponent];

        NSString* alternatePath = (tabValue == MY_ANIMATION) ? cacheDirectory : animDirPath;
        filePath = [NSString stringWithFormat:@"%@/%@", alternatePath, fileName];
    }

    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        [self addAssetToScene:filePath];
    }
    else {
        [self.view endEditing:YES];
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to connect to the server, Please check your network settings." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}

#pragma end

#pragma Other Functions

- (void)addAssetToScene:(NSString*)filePath
{
    [self assetDownloadCompleted:filePath];
    if ([[AppHelper getAppHelper] checkInternetConnected]) {
        if (tabValue != MY_ANIMATION) {
            NSString* post = [NSString stringWithFormat:@"userid=%@&asset_id=%d&rating=0&download=1", userid, _assetId];
            NSData* postData = [post dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
            NSString* postLength = [NSString stringWithFormat:@"%lu", (unsigned long)[postData length]];
            NSMutableURLRequest* request = [[NSMutableURLRequest alloc] init];
            [request setURL:[NSURL URLWithString:@"http://www.iyan3dapp.com/appapi/createUserInfo.php"]];
            [request setHTTPMethod:@"POST"];
            [request setValue:postLength forHTTPHeaderField:@"Content-Length"];
            [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Content-Type"];
            [request setHTTPBody:postData];
            NSURLConnection* conn = [[NSURLConnection alloc] initWithRequest:request delegate:self];
            if (conn) {
                int responseValue;
                NSHTTPURLResponse* urlResponse = nil;
                NSError* error = [[NSError alloc] init];
                NSData* responseData = [NSURLConnection sendSynchronousRequest:request returningResponse:&urlResponse error:&error];
                NSString* result = [[NSString alloc] initWithData:responseData encoding:NSUTF8StringEncoding];
                //NSLog(@"Response Code: %d", [urlResponse statusCode]);

                if ([urlResponse statusCode] >= 200 && [urlResponse statusCode] < 300) {
                    responseValue = [result intValue];
                    if (responseValue != -1) {
                        AnimationItem* animItem = [cache GetAnimation:_assetId fromTable:ALL_ANIMATION_TABLE];
                        if (animItem) {
                            animItem.downloads += 1;
                            [cache UpdateAnimation:animItem];
                            [self updateLikeAndDownloadStatus:animItem];
                        }
                    }
                }
            }
            else {
                NSLog(@"Connection could not be made");
            }
        }
    }
}

- (void)downloadAnimation:(AnimationItem*)assetItem
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString *fileName, *url;
    NSString* extension = (animationType == RIG_ANIMATION) ? @"sgra" : @"sgta";
    if (tabValue == MY_ANIMATION) {
        fileName = [NSString stringWithFormat:@"%@/Resources/Animations/%d.%@", docDirPath, assetItem.assetId, extension];
        if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
            [self loadNode:[NSNumber numberWithInt:assetItem.assetId]];
        }
        else {
            NSLog(@"File not exists");
        }
    }
    else {
        fileName = [NSString stringWithFormat:@"%@/%d.%@", cacheDirectory, assetItem.assetId, extension];
        if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
            if (assetItem)
                [self loadNode:[NSNumber numberWithInt:assetItem.assetId]];
        }
        else {
            url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/animationFile/%d.%@", assetItem.assetId, extension];
            //[animDownloadQueue cancelAllOperations];
            [self cancelOperations:animDownloadQueue];
            DownloadTask* task = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(loadNode:) returnObject:[NSNumber numberWithInt:assetItem.assetId] outputFilePath:fileName andURL:url];
            task.queuePriority = NSOperationQueuePriorityHigh;
            [animDownloadQueue addOperation:task];
        }
    }
}

- (void)loadNode:(id)returnValue
{
    if([NSThread currentThread] == renderingThread)
        [self loadAnimationInRenderingThread:returnValue];
    else if(renderingThread)
        [self performSelector:@selector(loadAnimationInRenderingThread:) onThread:renderingThread withObject:returnValue waitUntilDone:YES];
    else {
        [self.downloadAssetsActivity setHidden:NO];
        pendingLoading = true;
        pendingAssetValue = returnValue;
    }
}

-(void) loadAnimationInRenderingThread:(id)value
{
    if([NSThread currentThread] != renderingThread)
        NSLog(@"loading in different thread");
       

    int assetId;
    
    if ([value isKindOfClass:[DownloadTask class]])
        assetId = [((DownloadTask*)value).returnObj intValue];
    else
        assetId = [value intValue];
    
    if (self.isViewLoaded) {
        if (assetArray.count > 0) {
            AnimationItem* currentAsset = [cache GetAnimation:assetId fromTable:tabValue];
            if (currentAsset.assetId == selectedCell) {
                if (currentAsset.type == RIG_ANIMATION && currentAsset.boneCount != [animPreviewHelper getBoneCount]) {
                    [self stopAllAnimations];
                    [self.boneCountLbl setHidden:NO];
                    [self.addtoScene setHidden:YES];
                }
                else {
                    [self.addtoScene setHidden:NO];
                    [self stopAllAnimations];
                    [animPreviewHelper loadNodeInScene:ANIMATIONS AssetId:currentAsset.assetId AssetName:[self getwstring:currentAsset.assetName]];
                }
            }
            else
                [self.addtoScene setHidden:YES];
            
            [self.downloadAssetsActivity setHidden:YES];
        }
    }

}

- (void)stopAllAnimations
{
    if([NSThread currentThread] == renderingThread)
        [self stopAnimationsInRenderingThread];
    else if (renderingThread)
        [self performSelector:@selector(stopAnimationsInRenderingThread) onThread:renderingThread withObject:nil waitUntilDone:YES];
}

- (void) stopAnimationsInRenderingThread
{
    if([NSThread currentThread] != renderingThread)
         NSLog(@"removing in different thread");
       

    @synchronized(animPreviewHelper)
    {
        if (self.isViewLoaded && animPreviewHelper && animPreviewScene)
            animPreviewScene->stopAnimation();
    }
}

- (void)assetDownloadCompleted:(NSString*)filePath
{
    [displayTimer invalidate];
    displayTimer = nil;
    [animPreviewHelper removeEngine];
    [self.delegate resetContext];
    [self dismissViewControllerAnimated:YES completion:nil];
    [self.delegate applyAnimationToSelectedNode:filePath];
}
- (void)refreshCollectionView
{
    [self tabChangingFunction:self.assetLibrary];
}

- (void)updateRenderer
{
    if (isAppEntersBG || !self.isViewLoaded)
        return;
    @synchronized(animPreviewHelper)
    {
        @autoreleasepool
        {
            if(animPreviewScene) {
                animPreviewScene->renderAll();
                
                if(firstFrame) {
                    renderingThread = [NSThread currentThread];
                    [self loadSelectedNodeInRenderingThread];
                    firstFrame = false;
                }
                
                if(pendingLoading) {
                    [self loadNode:pendingAssetValue];
                    pendingAssetValue = nil;
                    pendingLoading = false;
                }
            }
        }
        [animPreviewHelper presentRenderBuffer];
    }
}

- (void)renderAll
{

    if (isAppEntersBG && !self.isViewLoaded)
        return;

    @autoreleasepool
    {
        animPreviewScene->renderAll();
    }
    [animPreviewHelper presentRenderBuffer];
}

- (void)createDisplayLink
{
    displayTimer = [NSTimer scheduledTimerWithTimeInterval:1.0f / 24.0f target:self selector:@selector(updateRenderer) userInfo:nil repeats:YES];

    [[NSRunLoop mainRunLoop] addTimer:displayTimer forMode:NSDefaultRunLoopMode];
    /*
     displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateRenderer)];
     [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
     */
}

- (void) cancelOperations:(NSOperationQueue*) queue
{
    [queue cancelAllOperations];
    
    for (DownloadTask *task in [queue operations]) {
        [task cancel];
    }
}

#if TARGET_RT_BIG_ENDIAN
const NSStringEncoding kEncoding_wchar_t =
CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32BE);
#else
const NSStringEncoding kEncoding_wchar_t =
CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
#endif


-(NSString*) stringWithwstring:(const std::wstring&)ws
{
    char* data = (char*)ws.data();
    unsigned size = (int)ws.size() * sizeof(wchar_t);
    
    NSString* result = [[NSString alloc] initWithBytes:data length:size encoding:kEncoding_wchar_t];
    return result;
}

-(std::wstring) getwstring:(NSString*) sourceString
{
    NSData* asData = [sourceString dataUsingEncoding:kEncoding_wchar_t];
    return std::wstring((wchar_t*)[asData bytes], [asData length] / sizeof(wchar_t));
}

#pragma end

- (void)dealloc
{
    //[downloadQueue cancelAllOperations];
    [self cancelOperations:downloadQueue];
    downloadQueue = nil;
    [self cancelOperations:animDownloadQueue];
    //[animDownloadQueue cancelAllOperations];
    animDownloadQueue = nil;
    [animPreviewHelper removeEngine];
    animPreviewHelper = nil;
    [[AppHelper getAppHelper] resetAppHelper];
    cache = nil;
    assetArray = nil;
    jsonUserArray = nil;
    animationJsonArray = nil;
}

@end