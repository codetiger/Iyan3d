//
//  AssetSelectionViewControllerPad.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 05/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "AssetSelectionViewControllerPad.h"
#import "AssetCellView.h"
#import "Utility.h"
#import "Constants.h"
#import "AppDelegate.h"
#import "SGPreviewScene.h"
#import "SceneManager.h"
#import "PreviewHelper.h"
#import "DownloadTask.h"

#define ASSET_ANIMATION 4
#define ASSET_RIGGED 1
#define ASSET_BACKGROUNDS 2
#define ASSET_ACCESSORIES 3
#define ASSET_OBJ 6
#define ASSET_CAMERA 7
#define ASSET_LIGHT 8
#define ASSET_IMAGE 9
#define ASSET_TEXT 10

#define COMESFROMASSETSELECTION 8
#define COMES_FROM_ASSETSELECTION 5
#define SAMPLE_ASSETS 10
#define CLOSE_CURRENT_VIEW 10
#define OBJFile 6

#define RESTORING 11
#define LOAD_NODE 22
#define DOWNLOAD_NODE 33

PreviewHelper* assetPreviewHelper;
SGPreviewScene* previewScene;

@implementation AssetSelectionViewControllerPad

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        screenScale = [[UIScreen mainScreen] scale];
        cache = [CacheSystem cacheSystem];
        assetArray = [cache GetAssetList:CHARACTERS Search:@""];

        NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        docDirPath = [srcDirPath objectAtIndex:0];

        restoreIdArr = [[NSMutableArray alloc] init];
        downloadQueue = [[NSOperationQueue alloc] init];
        [downloadQueue setMaxConcurrentOperationCount:3];
        assetDownloadQueue = [[NSOperationQueue alloc] init];
        [assetDownloadQueue setMaxConcurrentOperationCount:1];

        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    }
    return self;
}
- (void)viewDidLoad
{
    [super viewDidLoad];
    [self.downloadAssetsActivity setHidesWhenStopped:YES];
    [self.downloadAssetsActivity setHidden:NO];
    [self.downloadAssetsActivity startAnimating];
    [self.downloadingActivityView setHidden:YES];

    restoreCounter = downloadCounter = priceIterator = 0;
    firstFrame = true;
    pendingTaskAssetId = NOT_EXISTS;
    addingNode = NO;
    
    if ([assetArray count] > 0) {
        AssetItem* assetItem = assetArray[0];
        selectedCell = assetItem.assetId;
    }
    self.cancelButton.layer.cornerRadius = 5.0;
    self.restorePurchaseButton.layer.cornerRadius = 5.0;
    self.addtoScene.layer.cornerRadius = 5.0;
    if ([Utility IsPadDevice]) {
        [self.assetCollectionView registerNib:[UINib nibWithNibName:@"AssetCellView" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    else {
        [self.assetCollectionView registerNib:[UINib nibWithNibName:@"AssetCellViewPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    tabValue = 0;

    [self.animationLibrary setHidden:YES];
    [self.animationButton setHidden:YES];
    [self changeButtonBG];

    [self.restoreActivityView setHidden:YES];
    assetPreviewHelper = [[PreviewHelper alloc] init];

    if (assetPreviewHelper)
        previewScene = [assetPreviewHelper initScene:self.renderView viewType:ASSET_SELECTION];
    if (assetPreviewHelper)
        [assetPreviewHelper addCameraLight];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersBG) name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersFG) name:@"applicationDidBecomeActive" object:nil];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    self.screenName = @"AssetSelectionView";
    [self setAllAssetsData];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(productsPriceSet) name:@"ProductPriceSet" object:nil];
    [self createDisplayLink];
    isRestorePurchasePressed = leavingView = false;
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    
}

- (void)productsPriceSet
{
    NSLog(@" Products price set ");
    [self performSelectorOnMainThread:@selector(showAssetsView) withObject:nil waitUntilDone:NO];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"ProductPriceSet" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"applicationDidBecomeActive" object:nil];
    
}

- (void)appEntersBG
{
    isAppEntersBG = true;
}

- (void)appEntersFG
{
    isAppEntersBG = false;
}

- (void)setAllAssetsData
{
    if ([[AppHelper getAppHelper] userDefaultsForKey:@"AssetDetailsUpdate"]) {
        NSTimeInterval timeInterval = [[NSDate date] timeIntervalSinceDate:[[AppHelper getAppHelper] userDefaultsForKey:@"AssetDetailsUpdate"]];
        int hours = timeInterval / 3600;
        if (hours < 5) {
            [self showAssetsView];
            [AppHelper getAppHelper].isAssetsUpdated = NO;
        }
        else {
            if ([[AppHelper getAppHelper] checkInternetConnected]) {
                [self.downloadAssetsActivity setHidden:NO];
                [self.downloadAssetsActivity startAnimating];
                [self.assetCollectionView setUserInteractionEnabled:NO];
                [self loadAllAssets];
                [[AppHelper getAppHelper] saveToUserDefaults:[NSDate date] withKey:@"AssetDetailsUpdate"];
            }
            else {
                [self showAssetsView];
                [AppHelper getAppHelper].isAssetsUpdated = NO;
            }
        }
    }
    else {

        if ([[AppHelper getAppHelper] checkInternetConnected] && [assetArray count] == 0) {
            [self.downloadAssetsActivity setHidden:NO];
            [self.downloadAssetsActivity startAnimating];
            [self.assetCollectionView setUserInteractionEnabled:NO];
            [self loadAllAssets];
            [[AppHelper getAppHelper] saveToUserDefaults:[NSDate date] withKey:@"AssetDetailsUpdate"];
        }
        else {
            [self showAssetsView];
            [AppHelper getAppHelper].isAssetsUpdated = NO;
        }
    }
}
- (void)loadAllAssets
{
    [[AppHelper getAppHelper] initHelper];
    if(downloadQueue) {
    [[AppHelper getAppHelper] performReadingJsonInQueue:downloadQueue ForPage:ASSET_SELECTION];
    [AppHelper getAppHelper].isAssetsUpdated = YES;
    }
}
- (void)showAssetsView
{
    if (!isRestoring && [assetArray count] > 0 && assetPreviewHelper && previewScene && previewScene->nodes.size()) {
        [self.downloadAssetsActivity stopAnimating];
        [self.downloadAssetsActivity setHidden:YES];
    }
    [self.assetCollectionView setUserInteractionEnabled:YES];
    [self setPriceFormatter];

    tabValue = previousTabValue = 0;

    [self.animationLibrary setHidden:YES];
    [self.animationButton setHidden:YES];
    [self changeButtonBG];
    [self tabChangingFunction:self.characterButton];

    [self.assetCollectionView reloadData];

    if (self.isViewLoaded)
        [self displayBasedOnSelection:[NSNumber numberWithInt:0]];

    if (assetArray.count > SAMPLE_ASSETS) {
        for (int i = 0; i < SAMPLE_ASSETS; i++) {
            AssetItem* sampleAsset = assetArray[i];
            if (![sampleAsset.iap isEqualToString:@"0"] && [sampleAsset.price isEqualToString:@"BUY"]) {
                if ([[AppHelper getAppHelper] checkInternetConnected]) {
                    [self.downloadAssetsActivity setHidden:NO];
                    [self.downloadAssetsActivity startAnimating];
                    //[self.assetCollectionView setUserInteractionEnabled:NO];
                    [self loadAllAssets];
                    [[AppHelper getAppHelper] saveToUserDefaults:[NSDate date] withKey:@"AssetDetailsUpdate"];
                }
                else
                    [self showInternetError:NO_INTERNET];

                break;
            }
        }
    }
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}
- (void)setPriceFormatter
{
    _priceFormatter = [[NSNumberFormatter alloc] init];
    [_priceFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [_priceFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
    [_priceFormatter setLocale:[[AppHelper getAppHelper] getPriceLocale]];
}
- (void)tabChangingFunction:(UIButton*)sender
{
    self.searchBar.text = @"";
    [self changeButtonBG];
    
    if(tabValue != previousTabValue) {
        [self cancelOperations:assetDownloadQueue];
        [self cancelOperations:downloadQueue];
        /*
        [assetDownloadQueue cancelAllOperations];
        [downloadQueue cancelAllOperations];
         */
    }
    
    previousTabValue = tabValue;
    
    switch (sender.tag) {
    case CHARACTERS:
        tabValue = 0;
        isMyLibrary = 0;
        assetArray = [cache GetAssetList:CHARACTERS Search:@""];
        self.characterButton.backgroundColor = [UIColor grayColor];
        break;
    case BACKGROUNDS:
        tabValue = 1;
        isMyLibrary = 0;
        assetArray = [cache GetAssetList:BACKGROUNDS Search:@""];
        self.backgroundButton.backgroundColor = [UIColor grayColor];
        break;
    case ACCESSORIES:
        tabValue = 2;
        isMyLibrary = 0;
        assetArray = [cache GetAssetList:ACCESSORIES Search:@""];
        self.accessoriesButton.backgroundColor = [UIColor grayColor];
        break;
    case MY_LIBRARY_TYPE:
        tabValue = 3;
        isMyLibrary = 1;
        assetArray = [cache GetAssetList:MY_LIBRARY_TYPE Search:@""];
        [self checkForRestorePurchase];
        [self removeMissingAssets];
        [self.assetCollectionView reloadData];
        self.assetLibrary.backgroundColor = [UIColor grayColor];
        break;
    default:
        break;
    }
    if ([assetArray count] != 0) {
        AssetItem* assetItem = assetArray[0];
        selectedCell = assetItem.assetId;
    }
    [self.assetCollectionView reloadData];
    if (assetArray.count <= 0 && sender.tag != MY_LIBRARY_TYPE) {
        [self showInternetError:SLOW_INTERNET];
        if ([[AppHelper getAppHelper] checkInternetConnected]) {
            [[AppHelper getAppHelper] removeFromUserDefaultsWithKey:@"AssetDetailsUpdate"];
            [self setAllAssetsData];
        }
        else
            [self showInternetError:NO_INTERNET];
    }
    if (self.isViewLoaded)
        [self displayBasedOnSelection:[NSNumber numberWithInt:0]];
}

- (void)checkForRestorePurchase
{
    if (![[AppHelper getAppHelper] getRestoreIds] || [[[AppHelper getAppHelper] getRestoreIds] count] == 0) {
        if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"isPurchaseRestored"] && !isRestoring) {
            [AppHelper getAppHelper].delegate = self;
            isRestoring = true;
            [[AppHelper getAppHelper] addTransactionObserver];
            [[AppHelper getAppHelper] restorePurchasedTransaction];
            [self.downloadAssetsActivity setHidden:NO];
            [self.downloadAssetsActivity startAnimating];
        }
    }
    else if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"isPurchaseRestored"] && !isRestoring && [assetArray count] < [[[AppHelper getAppHelper] getRestoreIds] count]) {
        [self.downloadAssetsActivity setHidden:NO];
        [self.downloadAssetsActivity startAnimating];
        restoreIdArr = [[AppHelper getAppHelper] getRestoreIds];
        [self.view setUserInteractionEnabled:NO];
        [self downloadRestoredContent];
    }
}

- (void)removeMissingAssets
{
    for (int i = 0; i < [assetArray count]; i++) {
        AssetItem* assetItem = assetArray[i];
        if (assetItem.assetId >= 40000 && assetItem.assetId < 50000) {
            NSString* fileName = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.sgr", docDirPath, assetItem.assetId];
            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
                [assetArray removeObjectAtIndex:i];
                i = i - 1;
                [cache DeleteTableData:assetItem.assetId];
            }
        }
        else if (assetItem.assetId >= 20000 && assetItem.assetId < 30000) {
            NSString* fileName = [NSString stringWithFormat:@"%@/Resources/Objs/%d.obj", docDirPath, assetItem.assetId];
            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
                [assetArray removeObjectAtIndex:i];
                i = i - 1;
                [cache DeleteTableData:assetItem.assetId];
            }
        }
    }
}

- (void)showInternetError:(int)connectionError
{
    if (connectionError != SLOW_INTERNET) {
        [self.view endEditing:YES];
        UIAlertView* internetError = [[UIAlertView alloc] initWithTitle:@"Connection Error"
                                                                message:@"Unable to connect to the server, Please check your network settings."
                                                               delegate:nil
                                                      cancelButtonTitle:@"OK"
                                                      otherButtonTitles:nil];
        [internetError performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}

- (void)changeButtonBG
{
    self.characterButton.backgroundColor = [UIColor blackColor];
    self.animationButton.backgroundColor = [UIColor blackColor];
    self.animationLibrary.backgroundColor = [UIColor blackColor];
    self.accessoriesButton.backgroundColor = [UIColor blackColor];
    self.backgroundButton.backgroundColor = [UIColor blackColor];
    self.assetLibrary.backgroundColor = [UIColor blackColor];
}
- (IBAction)restorePurchaseAction:(id)sender
{
    isRestorePurchasePressed = true;
    if ([restoreIdArr count] > 0) {
        [self.assetCollectionView setUserInteractionEnabled:YES];
    }
    else {
        [self.view setUserInteractionEnabled:NO];
        [AppHelper getAppHelper].delegate = self;
        [[AppHelper getAppHelper] addTransactionObserver];
        [[AppHelper getAppHelper] restorePurchasedTransaction];
        [self.downloadAssetsActivity setHidden:NO];
        [self.downloadAssetsActivity startAnimating];
    }
}
- (IBAction)addButtonAction:(id)sender
{
    //[self.downloadAssetsActivity stopAnimating];
    [self addSelectedAssetToScene];
}
- (IBAction)cancelButtonAction:(id)sender
{
    [self.view setUserInteractionEnabled:NO];
    leavingView = true;
    if (displayTimer) {
        [displayTimer invalidate];
        displayTimer = nil;
    }
    if (assetPreviewHelper)
        [assetPreviewHelper removeEngine];

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
- (NSInteger)collectionView:(UICollectionView*)collectionView numberOfItemsInSection:(NSInteger)section
{
    [self showOrHideGroupOfViews:([assetArray count] <= 0) ? YES : NO];
    return [assetArray count];
}

- (void)showOrHideGroupOfViews:(BOOL)status
{
    [self.addtoScene setHidden:status];
    [self.assetNameDisplay setHidden:status];
    [self.priceDisplay setHidden:status];
}

- (AssetCellView*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    if ([assetArray count] > indexPath.row) {
        AssetItem* assetItem = assetArray[indexPath.row];
        AssetCellView* cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
        if ([Utility IsPadDevice])
            cell.layer.cornerRadius = 10.0;
        else
            cell.layer.cornerRadius = 8.0;
        cell.layer.borderWidth = 1.0f;
        cell.layer.borderColor = [UIColor grayColor].CGColor;
        cell.assetNameLabel.text = assetItem.name;
        if (isMyLibrary) {
            cell.premiumicon.hidden = YES;
        }
        else {
            if (![assetItem.iap isEqual:@"0"])
                cell.premiumicon.hidden = NO;
            else
                cell.premiumicon.hidden = YES;
        }
        cell.backgroundColor = [UIColor clearColor];
        [cell.assetImageView setImageInfo:[NSString stringWithFormat:@"%d", assetItem.assetId] forView:ASSET_SELECTION OperationQueue:downloadQueue];

        cell.assetNameLabel.textColor = [UIColor whiteColor];
        if (assetItem.assetId == selectedCell) {
            cell.backgroundColor = [UIColor colorWithRed:37.0f / 255.0f green:37.0f / 255.0f blue:37.0f / 255.0f alpha:1.0f];
            self.assetNameDisplay.text = assetItem.name;
        }
        return cell;
    }
}

- (void)collectionView:(UICollectionView*)collectionView didSelectItemAtIndexPath:(NSIndexPath*)indexPath
{
    [self.downloadAssetsActivity setHidden:NO];
    [self.downloadAssetsActivity startAnimating];

    NSInteger type = NULL;

    type = tabValue + CHARACTERS;
    if (type == ASSET_LIBRARY)
        type = MY_LIBRARY_TYPE;

    storeType = (int)type;
    NSArray* indexPathArr = [collectionView indexPathsForVisibleItems];
    for (int i = 0; i < [indexPathArr count]; i++) {
        NSIndexPath* indexPath = [indexPathArr objectAtIndex:i];
        UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
        cell.backgroundColor = [UIColor clearColor];
    }
    UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
    cell.backgroundColor = [UIColor colorWithRed:37.0f / 255.0f green:37.0f / 255.0f blue:37.0f / 255.0f alpha:1.0f];

    [self displayBasedOnSelection:[NSNumber numberWithInteger:indexPath.row]];
    //[self performSelectorOnMainThread:@selector(displayBasedOnSelection:) withObject:[NSNumber numberWithInteger:indexPath.row] waitUntilDone:NO];
}

- (void)performTasksInBackground:(NSNumber*)rowIndex
{
    [self performSelectorOnMainThread:@selector(displayBasedOnSelection:) withObject:rowIndex waitUntilDone:NO];
}

- (void)displayBasedOnSelection:(NSNumber*)rowIndex
{
    if (!self.isViewLoaded)
        return;
    
    int selectedIndex = [rowIndex intValue];
    if (selectedIndex >= 0 && selectedIndex < [assetArray count]) {
        AssetItem* assetItem = assetArray[selectedIndex];
        if (assetItem == nil)
            return;

        if ([assetItem.iap isEqualToString:@"0"]) {
            [self.addtoScene setTitle:@"ADD TO SCENE" forState:UIControlStateNormal];
            self.priceDisplay.text = @"";
        }
        else {
            if (![cache checkDownloadedAsset:assetItem.assetId]) {
                [self.addtoScene setTitle:@"BUY" forState:UIControlStateNormal];
                NSInteger priceNumber = [assetItem.price floatValue];
                if ([assetItem.price isEqualToString:@"BUY"])
                    self.priceDisplay.text = assetItem.price;
                else
                    self.priceDisplay.text = [_priceFormatter stringFromNumber:[NSNumber numberWithFloat:priceNumber]];
            }
            else {
                [self.addtoScene setTitle:@"ADD TO SCENE" forState:UIControlStateNormal];
                self.priceDisplay.text = @"";
            }
        }
        selectedCell = assetItem.assetId;
        asset = assetItem;
        self.assetNameDisplay.text = assetItem.name;
        _assetId = assetItem.assetId;

        [self removeCurrentObject];
        [self downloadAsset:assetItem ForActivity:LOAD_NODE];
    }
    else {
        if (!isRestoring && [assetArray count] > 0)
            [self.downloadAssetsActivity setHidden:YES];
        [self removeCurrentObject];
    }
}

- (void)removeCurrentObject
{
    if(renderingThread == [NSThread currentThread])
        [self removeObjectInRenderingThread];
    else if(renderingThread)
        [self performSelector:@selector(removeObjectInRenderingThread) onThread:renderingThread withObject:nil waitUntilDone:NO];
}

- (void) removeObjectInRenderingThread
{
    if([NSThread currentThread] != renderingThread)
        NSLog(@"removing in different thread");
    
    @synchronized(assetPreviewHelper)
    {
        if (self.isViewLoaded && previewScene && !leavingView) {
            while (previewScene->nodes.size() > 1) {
                if (previewScene)
                    previewScene->removeObject(previewScene->nodes.size() - 1, false);
            }
        }
    }
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

    type = tabValue + CHARACTERS;
    if (type == ASSET_LIBRARY)
        type = MY_LIBRARY_TYPE;

    assetArray = [cache GetAssetList:(int)type Search:searchText];
    [self.assetCollectionView reloadData];
    [self displayBasedOnSelection:[NSNumber numberWithInt:0]];
}
- (void)addSelectedAssetToScene
{
    [self.downloadingActivityView setHidden:NO];
    [self.addtoScene setHidden:YES];
    [self.downloadingActivityView startAnimating];
    NSInteger length = asset.iap.length;

    // length < 2 item are free assets
    [self.assetCollectionView setUserInteractionEnabled:NO];
    [self.topBarView setUserInteractionEnabled:NO];
    if (length > 2 && storeType >= CHARACTERS && storeType <= ACCESSORIES && ![cache checkDownloadedAsset:_assetId]) {
        [self purchaseModel];
    }
    else {
        [self downloadAsset];
    }
}
- (void)downloadAsset
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirectory = [docPaths objectAtIndex:0];
    AssetItem* assetItem = [cache GetAsset:_assetId];
    NSString *fileName, *url;
    NSNumber* returnId = [NSNumber numberWithInt:_assetId];
    if (assetItem.type == CHARACTERS) {
        [cache AddDownloadedAsset:assetItem];
        if (![[NSString stringWithFormat:@"%d", _assetId] hasPrefix:@"4"]) {
            fileName = [NSString stringWithFormat:@"%@/%d.sgr", cacheDirectory, _assetId];
            url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/mesh/%d.sgr", _assetId];

            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName])
                [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(downloadTextureFileWithReturnId:) priority:NSOperationQueuePriorityVeryHigh];
            else {
                NSDictionary* dictionary = [NSDictionary dictionaryWithObjectsAndKeys:returnId, @"returnId", fileName, @"fileName", nil];
                [self proceedToFileVerification:dictionary];
            }
        }
        else {
            fileName = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.sgr", docDirectory, _assetId];
            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
                [cache DeleteTableData:_assetId];
                [self.view endEditing:YES];
                UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Missing Files" message:@"Some resources are missing. Cannot able to import the model into scene." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
                [message setTag:CLOSE_CURRENT_VIEW];
                [message show];
            }
            else {
                NSDictionary* dictionary = [NSDictionary dictionaryWithObjectsAndKeys:returnId, @"returnId", fileName, @"fileName", nil];
                [self proceedToFileVerification:dictionary];
            }
        }
    }
    else if (assetItem.type == BACKGROUNDS || assetItem.type == ACCESSORIES) {
        [cache AddDownloadedAsset:assetItem];
        fileName = [NSString stringWithFormat:@"%@/%d.sgm", cacheDirectory, _assetId];
        url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/mesh/%d.sgm", _assetId];
        if (![[NSFileManager defaultManager] fileExistsAtPath:fileName])
            [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(downloadTextureFileWithReturnId:) priority:NSOperationQueuePriorityVeryHigh];
        else {
            NSDictionary* dictionary = [NSDictionary dictionaryWithObjectsAndKeys:returnId, @"returnId", fileName, @"fileName", nil];
            [self proceedToFileVerification:dictionary];
        }
    }
    else if (assetItem.type == OBJFile) {
        [cache AddDownloadedAsset:assetItem];
        fileName = [NSString stringWithFormat:@"%@/Resources/Objs/%d.obj", docDirectory, _assetId];
        if (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
            [cache DeleteTableData:_assetId];
            [self.view endEditing:YES];
            UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Missing Files" message:@"Some resources are missing. Cannot able to import the model into scene." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [message setTag:CLOSE_CURRENT_VIEW];
            [message show];
        }
        else {
            NSDictionary* dictionary = [NSDictionary dictionaryWithObjectsAndKeys:returnId, @"returnId", fileName, @"fileName", nil];
            [self proceedToFileVerification:dictionary];
        }
    }
}
- (void)proceedToFileVerification:(id)idObj
{
    if (addingNode)
        return;

    NSString* fileName;
    int assetId;

    if ([idObj isKindOfClass:[DownloadTask class]]) {
        fileName = ((DownloadTask*)idObj).outputPath;
        assetId = [((DownloadTask*)idObj).returnObj intValue];
    }
    else {
        fileName = [idObj objectForKey:@"fileName"];
        assetId = [[idObj objectForKey:@"returnId"] intValue];
    }

    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        addingNode = YES;
        [self addAssetToScene:assetId];
    }
    else {
        [self.view endEditing:YES];
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to connect to the server, Please check your network settings." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}
- (void)addAssetToScene:(int)assetId
{
    [self assetDownloadCompleted:assetId];

    AssetItem* assetItem = [cache GetAsset:assetId];
    [cache AddDownloadedAsset:assetItem];
}
- (void)purchaseModel
{
    transactionCount = 0;
    NSString* iapStr = asset.iap;
    if ([SKPaymentQueue canMakePayments]) {
        [[UIApplication sharedApplication] beginIgnoringInteractionEvents];
        SKPayment* payment = [SKPayment paymentWithProductIdentifier:iapStr];
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    }
    else {
        [self.view endEditing:YES];
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"In-App Purchase is Disabled. Check your settings to enable Purchases." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];

        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
        [self.downloadingActivityView setHidden:YES];
        [self.addtoScene setHidden:NO];
        [self.downloadingActivityView stopAnimating];
        [self.assetCollectionView setUserInteractionEnabled:YES];
        [self.topBarView setUserInteractionEnabled:YES];
    }
}
- (void)downloadAsset:(AssetItem*)assetvalue ForActivity:(int)activity
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];

    NSString *fileName, *url;
    NSNumber* returnId = [NSNumber numberWithInt:assetvalue.assetId];

    if (assetvalue.type == CHARACTERS) {

        if (assetvalue.assetId >= 40000 && assetvalue.assetId < 50000) {
            fileName = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.sgr", docDirPath, assetvalue.assetId];
            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName])
                NSLog(@"Rig file not exists");
            else
                [self loadNodeInRenderingThread:assetvalue.assetId];
        }
        else {
            fileName = [NSString stringWithFormat:@"%@/%d.sgr", cacheDirectory, assetvalue.assetId];
            url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/mesh/%d.sgr", assetvalue.assetId];

            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName] || activity == DOWNLOAD_NODE)
                [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(downloadTextureFileWithReturnId:) priority:NSOperationQueuePriorityHigh];
            else {
                if (activity == LOAD_NODE)
                    [self loadNodeInRenderingThread:assetvalue.assetId];
                else
                    [self updateUIForRestoringPurchase:assetvalue.assetId];
            }
        }
    }
    else if (assetvalue.type == BACKGROUNDS || assetvalue.type == ACCESSORIES) {
        fileName = [NSString stringWithFormat:@"%@/%d.sgm", cacheDirectory, assetvalue.assetId];
        url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/mesh/%d.sgm", assetvalue.assetId];

        if (![[NSFileManager defaultManager] fileExistsAtPath:fileName] || activity == DOWNLOAD_NODE)
            [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(downloadTextureFileWithReturnId:) priority:NSOperationQueuePriorityHigh];
        else {
            if (activity == LOAD_NODE)
                [self loadNodeInRenderingThread:assetvalue.assetId];
            else
                [self updateUIForRestoringPurchase:assetvalue.assetId];
        }
    }
    else if (assetvalue.type == OBJ_FILE) {
        fileName = [NSString stringWithFormat:@"%@/Resources/Objs/%d.obj", docDirPath, assetvalue.assetId];
        if (![[NSFileManager defaultManager] fileExistsAtPath:fileName])
            NSLog(@"Obj file not exists");
        else
            [self loadNodeInRenderingThread:assetvalue.assetId];
    }
}

- (void)downloadTextureFileWithReturnId:(DownloadTask*)task
{
    if (!self.isViewLoaded || leavingView || !previewScene || !assetPreviewHelper)
        return;

    NSNumber* returnId = task.returnObj;
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString* fileName = [NSString stringWithFormat:@"%@/%d-cm.png", cacheDirectory, [returnId intValue]];
    NSString* url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/meshtexture/%d.png", [returnId intValue]];

    AssetItem *downloadingAsset = [cache GetAsset:[returnId intValue]];
    
    if ([assetArray count] > 0 && self.addtoScene.isHidden && downloadingAsset && [cache checkDownloadedAsset:downloadingAsset.assetId])
        [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(proceedToFileVerification:) priority:NSOperationQueuePriorityHigh];
    else
        [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(downloadCompleted:) priority:NSOperationQueuePriorityHigh];
}

- (void)addDownloadTaskWithFileName:(NSString*)fileName URL:(NSString*)url returnId:(NSNumber*)returnId andSelector:(SEL)selector priority:(NSOperationQueuePriority)priority
{
   // [assetDownloadQueue cancelAllOperations];
    if(!isRestoring)
    [self cancelOperations:assetDownloadQueue];
    DownloadTask* downloadTask;
    downloadTask = [[DownloadTask alloc] initWithDelegateObject:(id)self selectorMethod:selector returnObject:(id)returnId outputFilePath:fileName andURL:url];
    downloadTask.queuePriority = priority;
    [assetDownloadQueue addOperation:downloadTask];
}

- (void)downloadCompleted:(DownloadTask*)task
{
    if([task isCancelled])
        return;
    
    int assetId = [task.returnObj intValue];

    if (isRestoring) {
        [self updateUIForRestoringPurchase:assetId];
    }
    else {
        if (self.isViewLoaded && !leavingView)
            [self loadNodeInRenderingThread:assetId];
    }
}

- (void)updateUIForRestoringPurchase:(int)assetId
{
    if (tabValue == 3) {
        [self.assetCollectionView reloadData];
        assetArray = [cache GetAssetList:MY_LIBRARY_TYPE Search:@""];
    }

    AssetItem* a = [cache GetAsset:assetId];
    NSLog(@" Restoring Asset %@ ", a.name);
    [cache AddDownloadedAsset:a];
    downloadCounter++;
    [self.assetCollectionView reloadData];
    if (downloadCounter >= restoreCounter) {
        downloadCounter = 0;
        [self.view setUserInteractionEnabled:YES];
        [self.restorePurchaseButton setHidden:NO];
        [self.restoreActivityView setHidden:YES];
        [self.restoreActivityView stopAnimating];

        [self displayMyLibraryContent];
        NSLog(@" Restoring Completed ");
        isRestoring = false;
        [[AppHelper getAppHelper] saveBoolUserDefaults:YES withKey:@"isPurchaseRestored"];
        [AppHelper getAppHelper].delegate = nil;
    }
}

- (void) loadNodeInRenderingThread:(int)assetId
{
    if([NSThread currentThread] == renderingThread)
        [self loadNode:[NSNumber numberWithInt:assetId]];
    else if(renderingThread)
        [self performSelector:@selector(loadNode:) onThread:renderingThread withObject:[NSNumber numberWithInt:assetId] waitUntilDone:NO];
    else {
        [self.downloadAssetsActivity setHidden:NO];
        pendingTaskAssetId = assetId;
    }
    
}

- (void) loadNode:(NSNumber*)assetIdNum
{
    if([NSThread currentThread] != renderingThread)
        NSLog(@"loading in different thread");
    
    int assetId = [assetIdNum intValue];
    if (assetPreviewHelper && self.isViewLoaded && !leavingView && assetId == selectedCell) {

        AssetItem* currentAsset = [cache GetAsset:assetId];
        if ([assetArray count] > 0 && currentAsset) {
            if(!isRestoring)
            [self.view setUserInteractionEnabled:NO];
            [self removeCurrentObject];
            BOOL status = true;
            @synchronized(assetPreviewHelper)
            {
                if (currentAsset)
                    status = [assetPreviewHelper loadNodeInScene:currentAsset.type AssetId:currentAsset.assetId AssetName:[self getwstring:currentAsset.name]];
            }

            if (!status && currentAsset)
                [self downloadAsset:currentAsset ForActivity:DOWNLOAD_NODE];
            else {
                if (!isRestoring) {
                    [self.downloadAssetsActivity setHidden:YES];
                }
            }
            if(!isRestoring)
            [self.view setUserInteractionEnabled:YES];
        }
    }
}
- (void)assetDownloadCompleted:(int)assetId
{
    NSNumber *assetValue = [NSNumber numberWithInt:assetId];
    if(renderingThread == [NSThread currentThread])
        [self actionOnDownloadCompletion:assetValue];
    else if (renderingThread)
        [self performSelector:@selector(actionOnDownloadCompletion:) onThread:renderingThread withObject:assetValue waitUntilDone:YES];
    else
        [self performSelectorOnMainThread:@selector(actionOnDownloadCompletion:) withObject:assetValue waitUntilDone:YES];
    
    [self dismissViewControllerAnimated:YES completion:nil];
    [self.delegate loadNodeInScene:assetId];
}
- (void)actionOnDownloadCompletion:(NSNumber*) assetValue
{
    leavingView = true;
    if (displayTimer) {
        [displayTimer invalidate];
        displayTimer = nil;
    }
    if (assetPreviewHelper)
        [assetPreviewHelper removeEngine];
    

}
- (void)refreshCollectionView
{
    [self tabChangingFunction:self.assetLibrary];
}
/*
- (void) paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue
{

    [[AppHelper getAppHelper] moveFontFilesIfNeccasary];
	[self performSelectorInBackground:@selector(statusForRestorePurchase:) withObject:[NSNumber numberWithBool:YES]];
    [self tabChangingFunction:self.assetLibrary];
    [self.assetCollectionView reloadData];
}
*/

- (void)updateRenderer
{
    //[assetPreviewHelper setCurrentContext]
    if (leavingView || isAppEntersBG ||  !self.isViewLoaded)
        return;
    
    @synchronized(assetPreviewHelper)
    {
        @autoreleasepool
        {
                if (previewScene && previewScene->nodes.size() > 0)
                {
                    if(firstFrame) {
                        renderingThread = [NSThread currentThread];
                        firstFrame = false;
                    }
                    previewScene->renderAll();
                    
                    if(renderingThread && pendingTaskAssetId != NOT_EXISTS) {
                        [self loadNodeInRenderingThread:pendingTaskAssetId];
                        pendingTaskAssetId = NOT_EXISTS;
                    }
                }
        }

        if (assetPreviewHelper)
            [assetPreviewHelper presentRenderBuffer];
    }
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

- (void)downloadRestoredContent
{
    isRestoring = true;
    for (int i = 0; i < [restoreIdArr count]; i++) {
        if ([[restoreIdArr objectAtIndex:i] isEqual:OBJ_IMPORT_IAP]) {
            [cache addOBJImporterColumn];
        }
        else {
            AssetItem* assetvalue = [cache GetAssetItem:[restoreIdArr objectAtIndex:i]];
            if (assetvalue) {
                restoreCounter++;
                [self downloadAsset:assetvalue ForActivity:RESTORING];
            }
        }
    }
    if([restoreIdArr count] <= 0 || ([restoreIdArr count] == 1  && [[restoreIdArr objectAtIndex:0] isEqual:OBJ_IMPORT_IAP])) {
        isRestoring = false;
        [self.view setUserInteractionEnabled:YES];
        [self.downloadAssetsActivity setHidden:YES];
    }
}

- (void)displayMyLibraryContent
{
    [self.downloadAssetsActivity stopAnimating];
    [self.downloadAssetsActivity setHidden:YES];
    [self.assetCollectionView reloadData];
    [self tabChangingFunction:self.assetLibrary];
}

- (void)paymentQueue:(SKPaymentQueue*)queue updatedTransactions:(NSArray*)transactions
{
    if (transactions.count > 1) {
        for (int i = 1; i < transactions.count; i++)
            [[SKPaymentQueue defaultQueue] finishTransaction:transactions[i]];
    }

    for (SKPaymentTransaction* transaction in transactions) {

        if (transactionCount > 0 && (transaction.transactionState != SKPaymentTransactionStatePurchasing)) {
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
        }
        else
            transactionCount++;

        switch (transaction.transactionState) {
        case SKPaymentTransactionStatePurchased:
            [self performSelectorOnMainThread:@selector(downloadAsset) withObject:nil waitUntilDone:NO];
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            [[UIApplication sharedApplication] endIgnoringInteractionEvents];
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            break;
        case SKPaymentTransactionStateFailed: {
            [[UIApplication sharedApplication] endIgnoringInteractionEvents];
            if (transaction.error.code != SKErrorPaymentCancelled) {
                [self.view endEditing:YES];
                UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Error" message:transaction.error.localizedDescription delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
                [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
            }
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            [self.downloadingActivityView setHidden:YES];
            [self.addtoScene setHidden:NO];
            [self.downloadingActivityView stopAnimating];
            [self.assetCollectionView setUserInteractionEnabled:YES];
            [self.topBarView setUserInteractionEnabled:YES];
        } break;
        /*
			case SKPaymentTransactionStateRestored:
				[restoreIdArr addObject:transaction.payment.productIdentifier];
				[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                [self.assetCollectionView reloadData];
			break;
             */

        default:
            break;
        }
    }
}

/*
-(void)statusForOBJImport:(NSNumber *)object
{
	if([object boolValue]){
		[self.view setUserInteractionEnabled:NO];
		[self.importObj setHidden:YES];

	}
	else{
		[self.view setUserInteractionEnabled:YES];
		[self.importObj setHidden:NO];

	}
}
 */

- (void)statusForRestorePurchase:(NSNumber*)object
{
    isRestoring = true;
    BOOL isRestoreCompleted = [object boolValue];

    //[self.assetCollectionView setUserInteractionEnabled:isRestoreCompleted];
    //[self.restorePurchaseButton setHidden:!isRestoreCompleted];
    //[self.restoreActivityView setHidden:isRestoreCompleted];
    //isRestoreCompleted ? [self.restoreActivityView stopAnimating] : [self.restoreActivityView startAnimating];

    if (isRestoreCompleted) {
        restoreIdArr = [[AppHelper getAppHelper] getRestoreIds];
        if ([restoreIdArr count] > 0)
            [self performSelectorOnMainThread:@selector(downloadRestoredContent) withObject:nil waitUntilDone:NO];
        [[AppHelper getAppHelper] removeTransactionObserver];
    }
}
- (void)transactionCancelled
{
    if(isRestoring) {
        [self.view setUserInteractionEnabled:YES];
        [self.downloadAssetsActivity setHidden:YES];
        isRestoring = false;
    }
    [self.assetCollectionView setUserInteractionEnabled:YES];
    [self.topBarView setUserInteractionEnabled:YES];
    [self.restorePurchaseButton setHidden:NO];
    [self.restoreActivityView setHidden:YES];
    [self.restoreActivityView stopAnimating];
    [self.downloadAssetsActivity setHidden:YES];
    [[AppHelper getAppHelper] removeTransactionObserver];
}
- (void)loadingViewStatus:(BOOL)status
{
}
- (void)statusForOBJImport:(NSNumber*)object
{
}
- (void)premiumUnlocked
{
}
- (void)premiumUnlockedCancelled
{
}
- (void)addRestoreId:(NSString*)productIdentifier
{
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


- (void)dealloc
{
    //        if([EAGLContext currentContext] == _context)
    //            [EAGLContext setCurrentContext:nil];
    self.delegate = nil;
    leavingView = true;
    if (displayTimer) {
        [displayTimer invalidate];
        displayTimer = nil;
    }
    if (assetPreviewHelper)
        [assetPreviewHelper removeEngine];

//    [downloadQueue cancelAllOperations];
    [self cancelOperations:downloadQueue];
    downloadQueue = nil;
    [self cancelOperations:assetDownloadQueue];
//    [assetDownloadQueue cancelAllOperations];
    assetDownloadQueue = nil;

    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
    
    [AppHelper getAppHelper].delegate = nil;
    [[AppHelper getAppHelper] removeTransactionObserver];
    [[AppHelper getAppHelper] resetAppHelper];
    if (assetPreviewHelper)
        assetPreviewHelper = nil;

    cache = nil;
    assetArray = nil;
    restoreIdArr = nil;
    _characterButton = nil;
    _animationButton = nil;
    _assetLibrary = nil;
    _animationLibrary = nil;
    _accessoriesButton = nil;
    _backgroundButton = nil;
    _priceFormatter = nil;
    self.priceLocale = nil;
}

@end
