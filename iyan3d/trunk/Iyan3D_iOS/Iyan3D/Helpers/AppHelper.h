//
//  AppHelper.h
//  Iyan3D
//
//  Created by Karthik on 18/02/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "StoreKit/StoreKit.h"
#import "Utility.h"
#import "CacheSystem.h"
#import "Reachability.h"

@protocol AppHelperDelegate
-(void)loadingViewStatus:(BOOL)status;
-(void)statusForOBJImport:(NSNumber*)object;
-(void)premiumUnlocked;
-(void)addRestoreId:(NSString*)productIdentifier;
-(void)statusForRestorePurchase:(NSNumber *)object;
-(void)transactionCancelled;
-(void)setAnimationData:(NSArray*)allAnimations;
@end

@interface AppHelper : NSObject <SKProductsRequestDelegate,SKPaymentTransactionObserver>
{
    NSArray *jsonArray,*fontArr,*fontListArr,*jsonAnimationArray;
    NSMutableArray *productIdentifierList;
    NSMutableDictionary * allAssets;
    NSArray *allProducts;
    CacheSystem* cache;
    NSNumberFormatter * priceFormatter;
    BOOL processTransaction;
    int transactionCount;
    NSMutableArray *restoreIdArr;
    NSString *fontDirPath;
    NSDictionary* helpStatements;
}

@property (nonatomic, assign) id  <AppHelperDelegate> delegate;
@property(assign) BOOL isAssetsUpdated;
@property (nonatomic, strong) SKProductsRequest *productsRequest;

+(AppHelper *)getAppHelper;
-(void) addTransactionObserver;
-(void) setIdentifierForVendor;
-(void) parseHelpJson;
-(NSDictionary*) parseJsonFileWithName:(NSString*)jsonFileName;
-(NSString*) getHelpStatementForAction:(int)action;
-(void) removeTransactionObserver;
-(void) initHelper;
-(void) missingAlertView;
-(void) downloadJsonData;
-(BOOL) checkInternetConnected;
-(NSLocale*) getPriceLocale;
-(NSMutableArray*) getRestoreIds;
-(void) initializeFontListArray;
- (void) loadAllAssets;
-(void)performReadingJsonInQueue:(NSOperationQueue*)queue ForPage:(int)viewType;
-(void) loadAllFontsInQueue:(NSOperationQueue*)queue WithDelegate:(id)delegateObj AndSelectorMethod:(SEL)selectorMethod;
-(void) setAssetsDetails;
-(void) setAssetsDetails:(int)fromPage;
-(void) saveBoolUserDefaults:(BOOL)value withKey:(NSString*)key;
-(void) saveToUserDefaults:(id)value withKey:(NSString*)key;
-(void) removeFromUserDefaultsWithKey:(NSString*)key;
-(NSMutableDictionary*) dictionaryForKey:(NSString*)key;
- (id) userDefaultsForKey:(NSString*)key;
-(BOOL) userDefaultsBoolForKey:(NSString*)key;
-(void) callPaymentGateWayForProduct:(NSString*) productId;
-(void) restorePurchasedTransaction;
-(void) moveFontFilesIfNeccasary;
-(void) moveFilesFromInboxDirectory:(CacheSystem*)presentCache;
-(void) resetAppHelper;
-(void) writeDataToFile:(NSData*)data FileName:(NSString*)fileName;


// User and credits related methods

- (void) useOrRechargeCredits:(NSString*) uniqueId credits:(int) credits For:(NSString*)usageType;
- (void) getCreditsForUniqueId: (NSString*)uniqueId Name:(NSString*) name Email:(NSString*) email SignInType:(int) type;
- (NSString*) deviceName;
- (NSData*) getReceiptData;
- (void) verifyRestorePurchase;


@end
