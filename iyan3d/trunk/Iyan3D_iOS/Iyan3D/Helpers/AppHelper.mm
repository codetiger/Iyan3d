//
//  AppHelper.m
//  Iyan3D
//
//  Created by Karthik on 18/02/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//


#import <sys/utsname.h>
#import "AppHelper.h"
#import "Constants.h"
#import "DownloadTask.h"
#import "AFHTTPRequestOperation.h"
#import "AFHTTPClient.h"


#define FIVE_HUNDERED_CREDITS @"fivehundredcredits"
#define TWO_THOUSAND_CREDITS @"twothousandcredits"
#define FIVE_THOUSAND_CREDITS @"fivethousandcredits"

@implementation AppHelper

@synthesize productsRequest = _productsRequest;

+ (AppHelper*)getAppHelper
{
    static AppHelper* theAppHelper;
    @synchronized(self)
    {
        if (!theAppHelper)
            theAppHelper = [[self alloc] init];
    }
    return theAppHelper;
}
- (void)addTransactionObserver
{
    cache = [CacheSystem cacheSystem];
    processTransaction = false;
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    restoreIdArr = [[NSMutableArray alloc] init];
}
- (void)removeTransactionObserver
{
    processTransaction = false;
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
}

- (void)downloadJsonData
{
    if ([[AppHelper getAppHelper] userDefaultsForKey:@"AssetDetailsUpdate"]) {
        NSTimeInterval timeInterval = [[NSDate date] timeIntervalSinceDate:[[AppHelper getAppHelper] userDefaultsForKey:@"AssetDetailsUpdate"]];
        int hours = timeInterval / 3600;
        if (hours > 5) {
            if ([self checkInternetConnected]) {
                
                [self initHelper];
                [[AppHelper getAppHelper] loadAllAssets];
                [AppHelper getAppHelper].isAssetsUpdated = YES;
                [[AppHelper getAppHelper] saveToUserDefaults:[NSDate date] withKey:@"AssetDetailsUpdate"];
                
            } else{
                [self performSelectorOnMainThread:@selector(showInternetErrorAlert) withObject:nil waitUntilDone:NO];          }
        }
    }
    else {
        if ([self checkInternetConnected]) {
            
            [self initHelper];
            [[AppHelper getAppHelper] loadAllAssets];
            [AppHelper getAppHelper].isAssetsUpdated = YES;
            [[AppHelper getAppHelper] saveToUserDefaults:[NSDate date] withKey:@"AssetDetailsUpdate"];
            
        } else{
            [self performSelectorOnMainThread:@selector(showInternetErrorAlert) withObject:nil waitUntilDone:NO];
        }
    }
}

- (void) showInternetErrorAlert
{
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Warining" message:@"Please connect with internet some resources files are missing need to download it." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];
    [alert show];
}

- (void)initializeFontListArray
{
    fontListArr = Nil;
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    fontDirPath = [NSString stringWithFormat:@"%@/Resources/Fonts", docDirPath];
    NSArray* fontExtensions = [NSArray arrayWithObjects:@"ttf", @"otf", nil];
    NSArray* filesGathered;

    if (![[NSFileManager defaultManager] fileExistsAtPath:fontDirPath]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:fontDirPath withIntermediateDirectories:YES attributes:Nil error:Nil];
    }
    [self copyFontFilesFromDirectory:docDirPath ToDirectory:fontDirPath withExtensions:fontExtensions];
    filesGathered = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:fontDirPath error:Nil];
    fontListArr = [filesGathered filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", fontExtensions]];
}
- (void)copyFontFilesFromDirectory:(NSString*)sourceDir ToDirectory:(NSString*)destinationDir withExtensions:(NSArray*)extensions
{
    NSArray* fontFilesToCopy = [[[NSFileManager defaultManager] contentsOfDirectoryAtPath:sourceDir error:Nil] filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];

    for (NSString* aFile in fontFilesToCopy) {
        NSError* error;
        if (![[NSFileManager defaultManager] fileExistsAtPath:[destinationDir stringByAppendingPathComponent:aFile]]) {
            [[NSFileManager defaultManager] copyItemAtPath:[sourceDir stringByAppendingPathComponent:aFile] toPath:[destinationDir stringByAppendingPathComponent:aFile] error:&error];
        }
        if (error)
            NSLog(@" Error copying font files %@ due to %@", error.localizedDescription, error.localizedFailureReason);
    }
}

- (void)initHelper
{
    cache = [CacheSystem cacheSystem];
    productIdentifierList = [[NSMutableArray alloc] init];
    allAssets = [[NSMutableDictionary alloc] init];
    allProducts = [[NSArray alloc] init];
    priceFormatter = [[NSNumberFormatter alloc] init];
    [priceFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [priceFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
}

- (void)setIdentifierForVendor
{
    if (![[AppHelper getAppHelper] userDefaultsForKey:@"identifierForVendor"]) {
        if ([UIDevice instancesRespondToSelector:@selector(identifierForVendor)]) {
            NSString* uniqueIdentifier = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
            [[AppHelper getAppHelper] saveToUserDefaults:uniqueIdentifier withKey:@"identifierForVendor"];
        }
    }
}

- (BOOL)checkInternetConnected
{

    Reachability* reachability = [Reachability reachabilityForInternetConnection];
    NetworkStatus networkStatus = [reachability currentReachabilityStatus];
    return !(networkStatus == NotReachable);
}

- (NSLocale*)getPriceLocale;
{
    return priceFormatter.locale;
}

- (void) loadAllFontsInQueue:(NSOperationQueue*)queue WithDelegate:(id)delegateObj AndSelectorMethod:(SEL)selectorMethod
{
    if(!cache)
        cache = [CacheSystem cacheSystem];
    
    fontArr = [cache GetAssetList:FONT Search:@""];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    
//    NSLog(@" Cache %@ queue %@ delegate %@ font count %lu " , cache, queue , delegateObj , (unsigned long)[fontArr count]);
    for (int i = 0; i < [fontArr count]; i++) {
        AssetItem* asset = fontArr[i];
        AssetItem* assetItem = [cache GetAsset:asset.assetId];
        NSString *fileName, *url;
        if (assetItem.type == FONT) {
            fileName = [NSString stringWithFormat:@"%@/%@", cacheDirectory, assetItem.name];
            url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/font/%d.%@", assetItem.assetId, [assetItem.name pathExtension]];
            DownloadTask *task = [[DownloadTask alloc] initWithDelegateObject:delegateObj selectorMethod:selectorMethod returnObject:assetItem.name outputFilePath:fileName andURL:url];
            task.queuePriority = NSOperationQueuePriorityHigh;
            [queue addOperation:task];
        }
    }
}

- (void) loadAllAssets
{
    jsonArray = [[NSArray alloc] init];
    
    NSURL *urlForJson = [NSURL URLWithString:@"https://iyan3dapp.com/appapi/json/assetsDetailv5.json"];
    NSURLRequest *request = [NSURLRequest requestWithURL:urlForJson];
    if([NSURLConnection connectionWithRequest:request delegate:self]) {
        NSData *rawData = [NSData dataWithContentsOfURL:urlForJson];
        if(rawData){
            NSError *error;
            NSData* jsonData = [NSData dataWithData:rawData];
            if(jsonData == nil) {
                return;
            }
            
            NSString* jsonStr = [NSString stringWithUTF8String:(const char*)[jsonData bytes]];
            jsonStr = [jsonStr stringByTrimmingCharactersInSet:[NSCharacterSet controlCharacterSet]];
            jsonArray = [NSJSONSerialization JSONObjectWithData:[jsonStr dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:&error];
            
            [self setAssetsDetails:ASSET_SELECTION];
        }
    }
}

- (void)performReadingJsonInQueue:(NSOperationQueue*)queue ForPage:(int)viewType
{
    SEL selectorForTask = nil;
    NSString* urlString;
    if (viewType == (int)ASSET_SELECTION || viewType == (int)TEXT_VIEW) {
        selectorForTask = @selector(loadAllAssets:);
        urlString = @"https://iyan3dapp.com/appapi/json/assetsDetailv5.json";
    }
    else if (viewType == (int)ALL_ANIMATION_VIEW) {
        selectorForTask = @selector(loadAllAnimations:);
        urlString = @"https://iyan3dapp.com/appapi/json/animationDetail.json";
    }

    DownloadTask* task = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:selectorForTask returnObject:nil outputFilePath:@"" andURL:urlString];
    task.taskType = DOWNLOAD_AND_READ;
    task.queuePriority = NSOperationQueuePriorityVeryHigh;
    [queue addOperation:task];
}

- (void)loadAllAssets:(NSData*)rawData
{
    jsonArray = [[NSArray alloc] init];

    if (rawData) {
        //NSLog(@"Raw data %@ " , rawData);
        NSError* error;
        NSData* jsonData = [NSData dataWithData:rawData];//[Utility decryptData:rawData Password:@"SGmanKindWin5SG"];
        
        if (jsonData == nil) {
            return;
        }
        
        NSString* jsonStr = [NSString stringWithUTF8String:(const char*)[jsonData bytes]];
        jsonStr = [jsonStr stringByTrimmingCharactersInSet:[NSCharacterSet controlCharacterSet]];
        jsonArray = [NSJSONSerialization JSONObjectWithData:[jsonStr dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:&error];
        [self setAssetsDetails:ASSET_SELECTION];
    }
}
- (void)loadAllAnimations:(NSData*)rawData
{
    jsonAnimationArray = [[NSArray alloc] init];

    if (rawData) {
        NSError* error;
        NSData* jsonData = [NSData dataWithData:rawData];//[Utility decryptData:rawData Password:@"SGmanKindWin5SG"];
        if (jsonData == nil) {
            return nil;
        }
        NSString* jsonStr = [NSString stringWithUTF8String:(const char*)[jsonData bytes]];
        jsonStr = [jsonStr stringByTrimmingCharactersInSet:[NSCharacterSet controlCharacterSet]];
        if (jsonStr != nil)
            jsonAnimationArray = [NSJSONSerialization JSONObjectWithData:[jsonStr dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:&error];
    }
    else
        jsonAnimationArray = nil;

    [self.delegate setAnimationData:jsonAnimationArray];
}
- (void)setAssetsDetails
{
    [self setAssetsDetails:SCENE_SELECTION];
}

- (void)setAssetsDetails:(int)fromPage
{
    if (fromPage == ASSET_SELECTION) {
        for (int i = 0; i < jsonArray.count; i++) {
            AssetItem* asset = [[AssetItem alloc] init];
            NSDictionary* dict = jsonArray[i];
            asset.assetId = [[dict valueForKey:@"id"] intValue];
            asset.type = [[dict valueForKey:@"type"] intValue];
            asset.boneCount = [[dict valueForKey:@"nbones"] intValue];
            asset.name = [dict valueForKey:@"name"];
            asset.modifiedDate = [dict valueForKey:@"datetime"];
            asset.hash = [dict valueForKey:@"hash"];
            asset.iap = [dict valueForKey:@"iap"];
            asset.keywords = [dict valueForKey:@"keywords"];

            AssetItem* checkAsset = [cache GetAsset:asset.assetId];
            if ([checkAsset.modifiedDate length] > 0) {
                if ([asset.modifiedDate isEqualToString:checkAsset.modifiedDate])
                    [self saveBoolUserDefaults:NO withKey:[NSString stringWithFormat:@"updateImage%d", asset.assetId]];
                else {

                    [self saveBoolUserDefaults:YES withKey:[NSString stringWithFormat:@"updateImage%d", asset.assetId]];
                }
            }
            else
                [self saveBoolUserDefaults:YES withKey:[NSString stringWithFormat:@"updateImage%d", asset.assetId]];

            [productIdentifierList addObject:asset.iap];
            [allAssets setObject:asset forKey:asset.iap];

            [cache UpdateAsset:asset];
        }
    }
    else if (fromPage == SCENE_SELECTION) {
        AssetItem* objImport = [[AssetItem alloc] init];
        objImport.assetId = 0;
        objImport.type = 0;
        objImport.boneCount = 0;
        objImport.name = @"OBJ";
        objImport.modifiedDate = @"0";
        objImport.hash = @"0";
        objImport.iap = OBJ_IMPORT_IAP;
        objImport.keywords = @"obj";

        [productIdentifierList addObject:objImport.iap];
        [allAssets setObject:objImport forKey:objImport.iap];
    }

    [AppHelper getAppHelper].isAssetsUpdated = YES;
    [[AppHelper getAppHelper] saveToUserDefaults:[NSDate date] withKey:@"AssetDetailsUpdate"];

    [[NSNotificationCenter defaultCenter] postNotificationName:@"AssetsSet" object:nil userInfo:nil];

    if (self.productsRequest) {
        self.productsRequest.delegate = nil;
        [self.productsRequest cancel];
        self.productsRequest = nil;
    }

    if (productIdentifierList && [productIdentifierList count] > 0) {
        self.productsRequest = [[SKProductsRequest alloc] initWithProductIdentifiers:[NSSet setWithArray:productIdentifierList]];
        self.productsRequest.delegate = self;
        [self.productsRequest start];
    }
    else {
    }
}

- (void)productsRequest:(SKProductsRequest*)request didReceiveResponse:(SKProductsResponse*)response
{

    allProducts = response.products;
    for (SKProduct* product in allProducts) {
        AssetItem* asset = [allAssets objectForKey:product.productIdentifier];
        [priceFormatter setLocale:product.priceLocale];
        asset.price = [NSString stringWithFormat:@"%@", product.price];
        [cache updateAssetPrice:asset];
    }

    self.productsRequest.delegate = nil;
    [self.productsRequest cancel];
    self.productsRequest = nil;

    [[NSNotificationCenter defaultCenter] postNotificationName:@"ProductPriceSet" object:nil userInfo:nil];
}

- (void)writeDataToFile:(NSData*)data FileName:(NSString*)fileName
{
    NSMutableDictionary* dictionary = [[NSMutableDictionary alloc] init];
    [dictionary setObject:data forKey:@"data"];
    [dictionary setObject:fileName forKey:@"file"];
    [self performSelectorOnMainThread:@selector(writeDataToFile:) withObject:dictionary waitUntilDone:YES];
    while (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        [NSThread sleepForTimeInterval:50.0];
    }
}

- (void)writeDataToFile:(NSMutableDictionary*)dictionary
{
    NSData* data = [dictionary objectForKey:@"data"];
    NSString* fileName = [dictionary objectForKey:@"file"];
    if (data)
        [data writeToFile:fileName atomically:YES];
}

- (void)request:(SKProductsRequest*)request didFailWithError:(NSError*)error
{
    NSLog(@"Error: %@ ", error.localizedDescription);
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Status" message:@"In-App Purchase is Disabled. Check your settings to enable Purchases." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil];

    [alert show];

    [[NSNotificationCenter defaultCenter] postNotificationName:@"ProductPriceSet" object:nil userInfo:nil];
}

- (BOOL)userDefaultsBoolForKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    BOOL val = NO;
    if (standardUserDefaults)
        val = [standardUserDefaults boolForKey:key];
    return val;
}
- (id)userDefaultsForKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    NSString* val = nil;

    if (standardUserDefaults)
        val = [standardUserDefaults objectForKey:key];

    return val;
}

- (NSMutableDictionary*)dictionaryForKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    NSMutableDictionary* val = nil;

    if (standardUserDefaults)
        val = [standardUserDefaults objectForKey:key];

    return val;
}

- (void)removeFromUserDefaultsWithKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    [standardUserDefaults removeObjectForKey:key];
    [standardUserDefaults synchronize];
}

- (void)saveToUserDefaults:(id)value withKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];

    if (standardUserDefaults) {
        [standardUserDefaults setObject:value forKey:key];
        [standardUserDefaults synchronize];
    }
}

- (void)saveBoolUserDefaults:(BOOL)value withKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    if (standardUserDefaults) {
        [standardUserDefaults setBool:value forKey:key];
        [standardUserDefaults synchronize];
    }
}

- (void)moveFilesFromInboxDirectory:(CacheSystem*)presentCache
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* inboxDirectoryPath = [docDirPath stringByAppendingPathComponent:@"Inbox"];
    NSString* fontDirectoryPath = [docDirPath stringByAppendingPathComponent:@"Resources/Fonts"];
    NSArray* fontExtensions = [NSArray arrayWithObjects:@"ttf", @"otf", nil];
    NSArray* modelExtensions = [NSArray arrayWithObjects:@"obj", @"png", nil];
    NSArray* filesGathered;

    if ([[NSFileManager defaultManager] fileExistsAtPath:inboxDirectoryPath]) {
        filesGathered = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:inboxDirectoryPath error:Nil];
        NSArray* modelRelatedFiles;
        if([filesGathered count] > 0)
            modelRelatedFiles = [filesGathered filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", modelExtensions]];

        for (NSString* aFile in modelRelatedFiles) {
            NSError* error;
            if (![[NSFileManager defaultManager] fileExistsAtPath:[docDirPath stringByAppendingPathComponent:aFile]]) {
                [[NSFileManager defaultManager] moveItemAtPath:[inboxDirectoryPath stringByAppendingPathComponent:aFile] toPath:[docDirPath stringByAppendingPathComponent:aFile] error:&error];
            }
            if (error)
                NSLog(@" Error copying font files %@ due to %@", error.localizedDescription, error.localizedFailureReason);
            else
                [[NSFileManager defaultManager] removeItemAtPath:[inboxDirectoryPath stringByAppendingPathComponent:aFile] error:nil];
        }

        if ([presentCache checkOBJImporterPurchase]) {

            if (![[NSFileManager defaultManager] fileExistsAtPath:fontDirectoryPath])
                [[NSFileManager defaultManager] createDirectoryAtPath:fontDirectoryPath withIntermediateDirectories:YES attributes:Nil error:Nil];
            NSArray* fontFiles;
            if([filesGathered count] > 0)
                fontFiles = [filesGathered filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", fontExtensions]];

            for (NSString* aFile in fontFiles) {
                NSError* error;
                if (![[NSFileManager defaultManager] fileExistsAtPath:[fontDirectoryPath stringByAppendingPathComponent:aFile]]) {
                    [[NSFileManager defaultManager] moveItemAtPath:[inboxDirectoryPath stringByAppendingPathComponent:aFile] toPath:[fontDirectoryPath stringByAppendingPathComponent:aFile] error:&error];
                }
                if (error)
                    NSLog(@" Error copying font files %@ due to %@", error.localizedDescription, error.localizedFailureReason);
                else
                    [[NSFileManager defaultManager] removeItemAtPath:[inboxDirectoryPath stringByAppendingPathComponent:aFile] error:nil];
            }
        }
    }
}

- (void)moveFontFilesIfNeccasary
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* fontDirectoryPath = [docDirPath stringByAppendingPathComponent:@"/Resources/Fonts"];
    NSString* bundlePath = [[NSBundle mainBundle] bundlePath];
    NSArray* fontExtensions = [NSArray arrayWithObjects:@"ttf", @"otf", nil];
    NSArray* filesGathered;

    if ([cache checkOBJImporterPurchase]) {
        if ([[NSFileManager defaultManager] fileExistsAtPath:fontDirectoryPath]) {
            filesGathered = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:fontDirectoryPath error:Nil];
        }
        else {
            [[NSFileManager defaultManager] createDirectoryAtPath:fontDirectoryPath withIntermediateDirectories:YES attributes:Nil error:Nil];

            NSArray* fontFilesToCopy = [[[NSFileManager defaultManager] contentsOfDirectoryAtPath:bundlePath error:Nil] filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", fontExtensions]];

            for (NSString* aFile in fontFilesToCopy) {
                NSError* error;
                if (![[NSFileManager defaultManager] fileExistsAtPath:[fontDirectoryPath stringByAppendingPathComponent:aFile]]) {
                    [[NSFileManager defaultManager] copyItemAtPath:[bundlePath stringByAppendingPathComponent:aFile] toPath:[fontDirectoryPath stringByAppendingPathComponent:aFile] error:&error];
                }
                if (error)
                    NSLog(@" Error copying font files %@ due to %@", error.localizedDescription, error.localizedFailureReason);
            }
        }
    }
}

- (void)callPaymentGateWayForProduct:(NSString*) productId
{
    transactionCount = 0;
    if ([SKPaymentQueue canMakePayments]) {
        processTransaction = true;
        SKPayment* payment = [SKPayment paymentWithProductIdentifier:productId];
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    }
    else {
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"In-App Purchase is Disabled. Check your settings to enable Purchases." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
        [self performSelectorInBackground:@selector(statusForOBJImport:) withObject:[NSNumber numberWithBool:NO]];
    }
}

- (void)missingAlertView
{
    UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Missing Data" message:@"Some resources files are missing." delegate:nil cancelButtonTitle:@"NO" otherButtonTitles:nil];
    [closeAlert show];
}
- (void)restorePurchasedTransaction
{
    if ([SKPaymentQueue canMakePayments]) {
        [self statusForRestorePurchase:[NSNumber numberWithBool:NO]];
        [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
    }
    else {
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"In-App Purchase is Disabled. Check your settings to enable Purchases." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue*)queue
{
    for (int i = 0; i < [restoreIdArr count]; i++) {
        if ([[restoreIdArr objectAtIndex:i] isEqual:OBJ_IMPORT_IAP]) {
            [cache addOBJImporterColumn];
            [[AppHelper getAppHelper] saveBoolUserDefaults:[cache checkOBJImporterPurchase] withKey:@"premiumUnlocked"];
            [self performSelectorInBackground:@selector(statusForRestorePurchase:) withObject:[NSNumber numberWithBool:YES]];
            break;
        }
    }
    [self removeTransactionObserver];
}

- (NSMutableArray*)getRestoreIds
{
    return restoreIdArr;
}

- (void)paymentQueue:(SKPaymentQueue*)queue restoreCompletedTransactionsFailedWithError:(NSError*)error
{
    UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Error"
                                                      message:error.localizedDescription
                                                     delegate:nil
                                            cancelButtonTitle:@"OK"
                                            otherButtonTitles:nil];
    [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    [self performSelectorInBackground:@selector(statusForRestorePurchase:) withObject:[NSNumber numberWithBool:NO]];
    [self.delegate transactionCancelled];
    [self performSelectorInBackground:@selector(transactionCancelled) withObject:nil];
}

- (void)statusForRestorePurchase:(NSNumber*)object
{
    [self.delegate statusForRestorePurchase:object];
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
        case SKPaymentTransactionStatePurchased: {
//            [self.delegate loadingViewStatus:NO];
            NSString *productId = transaction.payment.productIdentifier;
            NSLog(@"Purchased %@ ", transaction.payment.productIdentifier);

            if ([productId isEqualToString:FIVE_HUNDERED_CREDITS]) {
                processTransaction = false;
                [self performSelectorInBackground:@selector(statusForOBJImport:) withObject:[NSNumber numberWithInt:500]];
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            } else if ([productId isEqualToString:TWO_THOUSAND_CREDITS]) {
                processTransaction = false;
                [self performSelectorInBackground:@selector(statusForOBJImport:) withObject:[NSNumber numberWithInt:2000]];
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            } else if ([productId isEqualToString:FIVE_THOUSAND_CREDITS]) {
                processTransaction = false;
                [self performSelectorInBackground:@selector(statusForOBJImport:) withObject:[NSNumber numberWithInt:5000]];
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            }

            break;
        }

        case SKPaymentTransactionStateFailed: {
//            [self.delegate loadingViewStatus:NO];
            processTransaction = false;
            NSLog(@"Cancelled");

            [[UIApplication sharedApplication] endIgnoringInteractionEvents];
            [self performSelectorInBackground:@selector(statusForOBJImport:) withObject:[NSNumber numberWithInt:0]];
            [self performSelectorInBackground:@selector(transactionCancelled) withObject:nil];
            if (transaction.error.code != SKErrorPaymentCancelled) {
                UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Error" message:transaction.error.localizedDescription delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
                [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
            }
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            break;
        }

        case SKPaymentTransactionStateRestored: {
            processTransaction = false;
            [restoreIdArr addObject:transaction.payment.productIdentifier];
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            break;
        }

        default: {
            break;
        }
        }
    }
}

-(void) showErrorAlertViewWithMessage:(NSString*)message
{
    UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Error" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [errorAlert show];

}

- (void)transactionCancelled
{
    [self.delegate transactionCancelled];
}
- (void)statusForOBJImport:(NSNumber*)status
{
    [self.delegate statusForOBJImport:status];
}
- (void)premiumUnlocked
{
    [self.delegate premiumUnlocked];
}

- (void)parseHelpJson
{
    /*
     ifstream jsonFile(constants::BundlePath + "/helpStatements.json");
     Json::Reader reader;
     if(!reader.parse(jsonFile, helpStatements, false)){
     Logger::log(ERROR, "Unable to parse helpStatements.json", "AppHelper");
     }
     */

    helpStatements = [self parseJsonFileWithName:@"helpStatements"];
}

- (NSDictionary*)parseJsonFileWithName:(NSString*)jsonFileName
{
    NSError* error;
    NSString* jsonPath = [[NSBundle mainBundle] pathForResource:jsonFileName ofType:@"json"];
    if(![[NSFileManager defaultManager] fileExistsAtPath:jsonPath])
        return nil;
    
    NSString* jsonStr = [NSString stringWithContentsOfFile:jsonPath encoding:NSUTF8StringEncoding error:nil];

    jsonStr = [jsonStr stringByTrimmingCharactersInSet:[NSCharacterSet controlCharacterSet]];
    NSDictionary* outputDict = [NSJSONSerialization JSONObjectWithData:[jsonStr dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:&error];
    if (error)
        NSLog(@" Error %@ ", error);
    return outputDict;
}

- (NSString*)getHelpStatementForAction:(int)action
{
    // use int action to get array of statements from the parsed json
    NSArray* actionStatements = [helpStatements objectForKey:[NSString stringWithFormat:@"%d", action]];

    // use size of array to get a random number
    if (actionStatements.count) {
        long randindex = arc4random() % (int)actionStatements.count;
        // use rand index to get the final statement
        return [actionStatements objectAtIndex:randindex];
    }
    return @"";
}

- (void) useOrRechargeCredits:(NSString*) uniqueId credits:(int) credits For:(NSString*)usageType
{
    if(uniqueId.length < 5) {
        NSLog(@" \n Show alert ");
        return;
    }
    
    NSData *receiptData = [[AppHelper getAppHelper] getReceiptData];
    
    NSString* receiptDataStr = (credits < 0) ? @"" : [receiptData base64EncodedStringWithOptions:0];
    __block int balance = -1;
    NSURL *url = [NSURL URLWithString:@"https://www.iyan3dapp.com/appapi/credits.php"];
    NSString *postPath = @"https://www.iyan3dapp.com/appapi/credits.php";
    
    AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSLog(@" \n unique id %@ Deduct credits %d ", uniqueId, credits);
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:uniqueId, @"uniqueid", usageType, @"usagetype", [NSString stringWithFormat:@"%d", credits], @"credits", receiptDataStr, @"receiptdata", nil]];
    
    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:responseObject options:NSJSONReadingAllowFragments error:nil];
        int status = [[dict objectForKey:@"result"] intValue];
        NSLog(@" \n Dictonary %@ ", dict);
        if(status > 0) {
            balance = [[dict objectForKey:@"balance"] intValue];
            NSLog(@" \n Balance %d ", balance);
            [self saveToUserDefaults:[NSNumber numberWithInt:balance] withKey:@"credits"];
        } else {
            NSString *message = [dict objectForKey:@"message"];
            [[AppHelper getAppHelper] showErrorAlertViewWithMessage:message];

        }
        
        if(([receiptDataStr isEqualToString:@""] && status > 0) || receiptDataStr.length > 5)
            [[NSNotificationCenter defaultCenter] postNotificationName:@"creditsupdate" object:nil];
    }
                                     failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                         NSLog(@"Failure: %@", error.localizedDescription);
                                         UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:@"Failure Error" message:@"Check your net connection it was slow. So animation cannot be uploaded." delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
                                         [userNameAlert show];
                                     }];
    [operation start];
}


- (void) getCreditsForUniqueId: (NSString*)uniqueId Name:(NSString*) name Email:(NSString*) email SignInType:(int) type
{
    if(uniqueId.length < 5) {
        NSLog(@" \n Show alert ");
        return;
    }
    
    __block int credits = -1;
    NSURL *url = [NSURL URLWithString:@"https://www.iyan3dapp.com/appapi/login.php"];
    NSString *postPath = @"https://www.iyan3dapp.com/appapi/login.php";
    
    NSString* deviceId = [[AppHelper getAppHelper] userDefaultsForKey:@"identifierForVendor"];
    NSString* osVersion = [UIDevice currentDevice].systemVersion;
    NSDictionary* deviceNames = [[AppHelper getAppHelper] parseJsonFileWithName:@"deviceCodes"];
    NSString* hwversion = @"Unknown";
    if(deviceNames != nil && [deviceNames objectForKey:[self deviceName]]) {
        hwversion = [deviceNames objectForKey:[self deviceName]];
    }
    
    AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:uniqueId, @"uniqueid", name, @"username", email, @"email", hwversion, @"hwversion", osVersion, @"osversion", deviceId, @"deviceid", [NSString stringWithFormat:@"%d",type], @"signintype", nil]];
    
    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:responseObject options:NSJSONReadingAllowFragments error:nil];
        int status = [[dict objectForKey:@"result"] intValue];
        if(status > 0) {
            credits = [[dict objectForKey:@"credits"] intValue];
            [self saveToUserDefaults:[NSNumber numberWithInt:credits] withKey:@"credits"];
        } else {
            NSString *message = [dict objectForKey:@"message"];
            [[AppHelper getAppHelper] showErrorAlertViewWithMessage:message];
        }
        [[NSNotificationCenter defaultCenter] postNotificationName:@"creditsupdate" object:nil];
    }
                                     failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                         NSLog(@"Failure: %@", error.localizedDescription);
                                         UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:@"Failure Error" message:@"Check your net connection it was slow. So animation cannot be uploaded." delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
                                         [userNameAlert show];
                                     }];
    [operation start];
}

- (void) verifyRestorePurchase
{
    
    if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"])
        return;
    
    NSData *receiptData = [[AppHelper getAppHelper] getReceiptData];
    
    NSURL *url = [NSURL URLWithString:@"https://www.iyan3dapp.com/appapi/restore.php"];
    NSString *postPath = @"https://www.iyan3dapp.com/appapi/restore.php";

    AFHTTPClient *httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSString * uniqueId = [[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"];
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:uniqueId, @"uniqueid", [receiptData base64EncodedStringWithOptions:0], @"receiptdata", nil]];
    
    NSLog(@"Request initiated");
    
    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        NSLog(@"Request Successfull");
        NSLog(@"response %@",[operation responseString]);
        NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:responseObject options:NSJSONReadingAllowFragments error:nil];
    }
     
                                     failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                         NSLog(@"Failure: %@", error);
                                         UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:@"Failure Error" message:@"Check your net connection it was slow. So animation cannot be uploaded." delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
                                         [userNameAlert show];
                                         
                                     }];
    [operation start];
}

-(NSString*) deviceName
{
    struct utsname systemInfo;
    uname(&systemInfo);
    
    return [NSString stringWithCString:systemInfo.machine
                              encoding:NSUTF8StringEncoding];
}

- (NSData*) getReceiptData
{
    NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
    NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];
    return receipt;
}

-(NSString*) stringWithwstring:(const std::wstring&)ws
{
    char* data = (char*)ws.data();
    unsigned size = (int)ws.size() * sizeof(wchar_t);
    
    NSString* result = [[NSString alloc] initWithBytes:data length:size encoding:NSUTF16StringEncoding];
    return result;
}

-(std::wstring) getwstring:(NSString*) sourceString
{
    NSData* asData = [sourceString dataUsingEncoding:NSUTF16StringEncoding];
    return std::wstring((wchar_t*)[asData bytes], [asData length] / sizeof(wchar_t));
}

- (void)resetAppHelper
{
    cache = nil;

    if (productIdentifierList && [productIdentifierList count])
        [productIdentifierList removeAllObjects];
    productIdentifierList = nil;

    if (self.productsRequest) {
        self.productsRequest.delegate = nil;
        [self.productsRequest cancel];
        self.productsRequest = nil;
    }

    if (allAssets && [allAssets count])
        [allAssets removeAllObjects];
    allAssets = nil;

    allProducts = nil;
    priceFormatter = nil;
    jsonArray = nil;
}

@end
