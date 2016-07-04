//
//  AppHelper.m
//  Iyan3D
//
//  Created by Karthik on 18/02/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//


#import "JDFTooltips.h"

#import <sys/utsname.h>
#import "AppHelper.h"
#import "Constants.h"
#import "DownloadTask.h"
#import "AFHTTPRequestOperation.h"
#import "AFHTTPClient.h"


#define FIVE_THOUSAND_CREDITS @"basicrecharge"
#define TWENTY_THOUSAND_CREDITS @"mediumrecharge"
#define FIFTY_THOUSAND_CREDITS @"megarecharge"

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
    NSDate *previousDownloadTime = [NSDate distantPast];
    if ([[AppHelper getAppHelper] userDefaultsForKey:@"AssetDetailsUpdatev1"])
        previousDownloadTime = [[AppHelper getAppHelper] userDefaultsForKey:@"AssetDetailsUpdatev1"];
    
    NSTimeInterval timeInterval = [[NSDate date] timeIntervalSinceDate:previousDownloadTime];
    int hours = timeInterval / 3600;
    if (hours > 5) {
        if ([self checkInternetConnected]) {
            [self initHelper];
            [[AppHelper getAppHelper] loadAllAssets];
            [AppHelper getAppHelper].isAssetsUpdated = YES;
        } else{
            [self performSelectorOnMainThread:@selector(showInternetErrorAlert) withObject:nil waitUntilDone:NO];
            if(self.delegate)
                [self.delegate performLocalTasks];
        }
    } else {
        if(self.delegate)
            [self.delegate performLocalTasks];
    }
}

- (void) showInternetErrorAlert
{
    UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Warining" message:@"Please connect with internet some resources files are missing need to download it." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
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
    
    NSURLSessionConfiguration *sessionConfig = [NSURLSessionConfiguration defaultSessionConfiguration];
    sessionConfig.timeoutIntervalForRequest = 5.0;
    
    NSURLSession *session = [NSURLSession sessionWithConfiguration:sessionConfig];
    [[session dataTaskWithURL:[NSURL URLWithString:@"https://iyan3dapp.com/appapi/json/assetsDetailv5.json"]
            completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
                if(!error && data != nil) {
                    
                    NSData* jsonData = [NSData dataWithData:data];//[Utility decryptData:rawData Password:@"SGmanKindWin5SG"];
                    
                    
                    NSString* jsonStr = [NSString stringWithUTF8String:(const char*)[jsonData bytes]];
                    if (jsonData == nil || [jsonStr length] < 5) {
                        if(self.delegate)
                            [self.delegate performLocalTasks];
                        return;
                    }

                    jsonStr = [jsonStr stringByTrimmingCharactersInSet:[NSCharacterSet controlCharacterSet]];
                    NSError *jsonParserError;
                    
                    jsonArray = [NSJSONSerialization JSONObjectWithData:[jsonStr dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:&jsonParserError];
                    
                    [self setAssetsDetails:ASSET_SELECTION];
                    [[AppHelper getAppHelper] saveToUserDefaults:[NSDate date] withKey:@"AssetDetailsUpdatev1"];
                }
                if(self.delegate)
                    [self.delegate performLocalTasks];
            }] resume];
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

    if (rawData != nil) {
        NSData* jsonData = [NSData dataWithData:rawData];//[Utility decryptData:rawData Password:@"SGmanKindWin5SG"];
        
        NSError* error;
        NSString* jsonStr = [NSString stringWithUTF8String:(const char*)[jsonData bytes]];
        if (jsonData == nil || [jsonStr length] < 5) {
            return;
        }

        jsonStr = [jsonStr stringByTrimmingCharactersInSet:[NSCharacterSet controlCharacterSet]];
        jsonArray = [NSJSONSerialization JSONObjectWithData:[jsonStr dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:&error];
        [self setAssetsDetails:ASSET_SELECTION];
    }
}
- (void)loadAllAnimations:(NSData*)rawData
{
    jsonAnimationArray = [[NSArray alloc] init];

    if (rawData != nil) {
        NSData* jsonData = [NSData dataWithData:rawData];//[Utility decryptData:rawData Password:@"SGmanKindWin5SG"];
        
        NSError* error;
        NSString* jsonStr = [NSString stringWithUTF8String:(const char*)[jsonData bytes]];
        if (jsonData == nil || [jsonStr length] < 5) {
            return;
        }

        jsonStr = [jsonStr stringByTrimmingCharactersInSet:[NSCharacterSet controlCharacterSet]];
        if (jsonStr != nil)
            jsonAnimationArray = [NSJSONSerialization JSONObjectWithData:[jsonStr dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:&error];
    }
    else
        jsonAnimationArray = nil;
    if(self.delegate != nil)
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
            asset.group = [[dict valueForKey:@"group"] intValue];

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

        [allAssets setObject:objImport forKey:objImport.iap];
    }

    [AppHelper getAppHelper].isAssetsUpdated = YES;
    [[AppHelper getAppHelper] saveToUserDefaults:[NSDate date] withKey:@"AssetDetailsUpdate"];

    [[NSNotificationCenter defaultCenter] postNotificationName:@"AssetsSet" object:nil userInfo:nil];
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

//        if ([presentCache checkOBJImporterPurchase]) {

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
    UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Missing Data" message:@"Some resources files are missing." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [closeAlert show];
}
- (void)restorePurchasedTransaction
{
    if ([SKPaymentQueue canMakePayments]) {
        [self performSelectorOnMainThread:@selector(statusForRestorePurchase:) withObject:[NSNumber numberWithBool:NO] waitUntilDone:NO];
        [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
    }
    else {
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"In-App Purchase is Disabled. Check your settings to enable Purchases." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}

- (void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue*)queue
{
    for (SKPaymentTransaction* transaction in queue.transactions) {
        [restoreIdArr addObject:transaction.payment.productIdentifier];
        [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
    for (int i = 0; i < [restoreIdArr count]; i++) {
        if ([[restoreIdArr objectAtIndex:i] isEqual:OBJ_IMPORT_IAP]) {
            [cache addOBJImporterColumn];
            [[AppHelper getAppHelper] saveBoolUserDefaults:[cache checkOBJImporterPurchase] withKey:@"premiumUnlocked"];
            [self performSelectorOnMainThread:@selector(statusForRestorePurchase:) withObject:[NSNumber numberWithBool:YES] waitUntilDone:NO];
            break;
        }
    }
    }
    if([restoreIdArr count] == 0) {
        if(self.delegate != nil)
            [self.delegate transactionCancelled];
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
    [self performSelectorOnMainThread:@selector(statusForRestorePurchase:) withObject:[NSNumber numberWithBool:NO] waitUntilDone:NO];
    if(self.delegate != nil)
        [self.delegate transactionCancelled];
    [self performSelectorInBackground:@selector(transactionCancelled) withObject:nil];
}

- (void)statusForRestorePurchase:(NSNumber*)object
{
    if(self.delegate != nil)
        [self.delegate statusForRestorePurchase:object];
}

- (void)paymentQueue:(SKPaymentQueue*)queue updatedTransactions:(NSArray*)transactions
{
    [restoreIdArr removeAllObjects];
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

            if ([productId isEqualToString:FIVE_THOUSAND_CREDITS]) {
                processTransaction = false;
                [self performSelectorInBackground:@selector(statusForOBJImport:) withObject:[NSNumber numberWithInt:5000]];
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            } else if ([productId isEqualToString:TWENTY_THOUSAND_CREDITS]) {
                processTransaction = false;
                [self performSelectorInBackground:@selector(statusForOBJImport:) withObject:[NSNumber numberWithInt:20000]];
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
            } else if ([productId isEqualToString:FIFTY_THOUSAND_CREDITS]) {
                processTransaction = false;
                [self performSelectorInBackground:@selector(statusForOBJImport:) withObject:[NSNumber numberWithInt:50000]];
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
    if(self.delegate != nil)
        [self.delegate transactionCancelled];
}
- (void)statusForOBJImport:(NSNumber*)status
{
    if(self.delegate != nil)
        [self.delegate statusForOBJImport:status];
}
- (void)premiumUnlocked
{
    if(self.delegate != nil)
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
    NSString* phpPath = [NSString stringWithFormat:@"https://www.iyan3dapp.com/appapi/credits.php"];
    NSURL *url = [NSURL URLWithString:phpPath];
    NSString *postPath = phpPath;
    
    AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSLog(@" \n unique id %@ Deduct credits %d ", uniqueId, credits);
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:uniqueId, @"uniqueid", usageType, @"usagetype", [NSString stringWithFormat:@"%d", credits], @"credits", receiptDataStr, @"receiptdata", nil]];
    
    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        
        NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:responseObject options:NSJSONReadingAllowFragments error:nil];
        int status = [[dict objectForKey:@"result"] intValue];
        NSString* result = [dict objectForKey:@"result"];
        NSLog(@" \n Dictonary %@ ", dict);
        
        if(status > 0) {
            [[NSNotificationCenter defaultCenter] postNotificationName:@"creditsused" object:nil userInfo:[NSDictionary dictionaryWithObjectsAndKeys:result,@"result",nil]];
        } else {
            NSString *message = [dict objectForKey:@"message"];
            [[AppHelper getAppHelper] showErrorAlertViewWithMessage:message];

        }
        
    }
                                     failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                         NSLog(@"Failure: %@", error.localizedDescription);
                                         UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:@"Failure Error" message:@"Check your net connection it was slow. So unable to complete the operation." delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
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
    NSString* hwversion = [self deviceName];
    NSString *deviceToken = [[AppHelper getAppHelper] userDefaultsForKey:@"deviceToken"];

    AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:uniqueId, @"uniqueid", name, @"username", email, @"email", hwversion, @"hwversion", osVersion, @"osversion", deviceId, @"deviceid", [NSString stringWithFormat:@"%d",type], @"signintype",deviceToken, @"pushid", nil]];
    
    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:responseObject options:NSJSONReadingAllowFragments error:nil];
        NSLog(@" Login : %@ ", dict);
        int status = [[dict objectForKey:@"result"] intValue];
        BOOL premium = [[dict objectForKey:@"premium"] boolValue];
        if(status > 0) {
            credits = [[dict objectForKey:@"credits"] intValue];
            [self saveToUserDefaults:[NSNumber numberWithInt:credits] withKey:@"credits"];
        } else {
            NSString *message = [dict objectForKey:@"message"];
            [[AppHelper getAppHelper] showErrorAlertViewWithMessage:message];
        }
        [[NSNotificationCenter defaultCenter] postNotificationName:@"creditsupdate" object:nil userInfo:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:credits], @"credits", [NSNumber numberWithBool:YES], @"network", [NSNumber numberWithBool:premium], @"premium", nil]];
    }
                                     failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                         NSLog(@"Failure: %@", error.localizedDescription);
                                         UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:@"Failure Error" message:@"Check your net connection it was slow. So animation cannot be uploaded." delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
                                         [userNameAlert show];
                                         
                                         [[NSNotificationCenter defaultCenter] postNotificationName:@"creditsupdate" object:nil userInfo:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithInt:credits], @"credits", [NSNumber numberWithBool:NO], @"network", nil]];
                                         
                                     }];
    [operation start];
}

- (void) verifyRestorePurchase
{
    
    if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"])
        return;
    
    NSData *receiptData = [[AppHelper getAppHelper] getReceiptData];
    
    NSString* phpPath = [NSString stringWithFormat:@"https://www.iyan3dapp.com/appapi/restore.php"];

    NSURL *url = [NSURL URLWithString:phpPath];
    NSString *postPath = phpPath;

    AFHTTPClient *httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSString * uniqueId = [[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"];
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:uniqueId, @"uniqueid", [receiptData base64EncodedStringWithOptions:0], @"receiptdata", nil]];
    
    NSLog(@"Request initiated");
    
    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        NSLog(@"Request Successfull");
        NSLog(@"response %@",[operation responseString]);
        NSDictionary *dict = [NSJSONSerialization JSONObjectWithData:responseObject options:NSJSONReadingAllowFragments error:nil];
        int status = [[dict objectForKey:@"result"] intValue];
        if(status > 0) {
            NSString *message = [dict objectForKey:@"message"];
            UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:@"Success" message:message delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [userNameAlert show];
        } else {
            NSString *message = [dict objectForKey:@"message"];
            [[AppHelper getAppHelper] showErrorAlertViewWithMessage:message];
        }

    }
     
                                     failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                         NSLog(@"Failure: %@", error);
                                         UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:@"Failure Error" message:@"Check your net connection it was slow. So animation cannot be uploaded." delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
                                         [userNameAlert show];
                                         
                                     }];
    [operation start];
}

- (void) toggleHelp:(UIViewController*) vc Enable:(BOOL)enable
{
    if(toolTips == nil)
        toolTips = [[NSMutableArray alloc] init];
    
    if(!enable || [toolTips count] > 0) {
        for(int i = 0; i < [toolTips count]; i++) {
            JDFTooltipView *tooltip = [toolTips objectAtIndex:i];
            [tooltip hideAnimated:YES];
            tooltip = nil;
        }
        [toolTips removeAllObjects];
        toolTips = nil;
    } else {
        [self checkToolTip:vc.view MainView:vc.view];
    }
}

- (void) showTipForView:(UIView*) subView InMainView:(UIView*)view
{
    if(toolTips == nil)
        toolTips = [[NSMutableArray alloc] init];
    
    NSString* hint = [subView accessibilityHint];
    int arrowDirection = [[subView accessibilityIdentifier] intValue];
    
    if([hint length] > 8) {
        JDFTooltipView *tooltip = [[JDFTooltipView alloc] initWithTargetView:subView hostView:view tooltipText:hint arrowDirection:arrowDirection width:220.0f];
        tooltip.font = [UIFont fontWithName:tooltip.font.fontName size:([Utility IsPadDevice] || [self iPhone6Plus]) ? 12 : 9];
        [tooltip show];
        [toolTips addObject:tooltip];
    } else {
        for( int i = 0; i < [toolTips count]; i++) {
            JDFTooltipView *t = [toolTips objectAtIndex:i];
            if(t.targetView == subView) {
                [t hideAnimated:YES];
                [toolTips removeObject:t];
                t = nil;
            }
        }
    }
}

- (void) checkToolTip:(UIView*) view MainView:(UIView*) mainView
{
    for( UIView* subView in view.subviews) {
        if(![subView isHidden] && subView.frame.origin.x >= 0 && subView.frame.origin.x < mainView.frame.size.width) {
            NSString* hint = [subView accessibilityHint];
            int arrowDirection = [[subView accessibilityIdentifier] intValue];
            
            if([hint length] > 8) {
                JDFTooltipView *tooltip = [[JDFTooltipView alloc] initWithTargetView:subView hostView:mainView tooltipText:hint arrowDirection:arrowDirection width:220.0f];
                tooltip.font = [UIFont fontWithName:tooltip.font.fontName size:([Utility IsPadDevice] || [self iPhone6Plus]) ? 12 : 9];
                [tooltip show];
                [toolTips addObject:tooltip];
            }
            [self checkToolTip:subView MainView:mainView];
        }
    }
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

-(BOOL)iPhone6Plus{
    if (([UIScreen mainScreen].scale > 2.0)) return YES;
    return NO;
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
