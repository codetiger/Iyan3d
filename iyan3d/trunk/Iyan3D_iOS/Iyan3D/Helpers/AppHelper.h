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

@protocol AppHelperDelegate
-(void)loadingViewStatus:(BOOL)status;
-(void)statusForOBJImport:(NSNumber*)object;
-(void)setAnimationData:(NSArray*)allAnimations;
-(void) performLocalTasks;
@end

@interface AppHelper : NSObject
{
    CacheSystem* cache;
    BOOL processTransaction;
    int transactionCount;
    NSString *fontDirPath;
    NSDictionary* helpStatements;
    NSMutableArray * toolTips;
}

@property (nonatomic, assign) id  <AppHelperDelegate> delegate;

+(AppHelper *)getAppHelper;
-(void) setIdentifierForVendor;
-(void) parseHelpJson;
-(NSDictionary*) parseJsonFileWithName:(NSString*)jsonFileName;
-(NSString*) getHelpStatementForAction:(int)action;
-(void) initHelper;
-(void) missingAlertView;
-(void) downloadJsonData;

-(void) initializeFontListArray;
-(void) saveBoolUserDefaults:(BOOL)value withKey:(NSString*)key;
-(void) saveToUserDefaults:(id)value withKey:(NSString*)key;
-(void) removeFromUserDefaultsWithKey:(NSString*)key;
-(NSMutableDictionary*) dictionaryForKey:(NSString*)key;
- (id) userDefaultsForKey:(NSString*)key;
-(BOOL) userDefaultsBoolForKey:(NSString*)key;
-(void) moveFontFilesIfNeccasary;
-(void) moveFilesFromInboxDirectory:(CacheSystem*)presentCache;
-(void) resetAppHelper;
-(void) writeDataToFile:(NSData*)data FileName:(NSString*)fileName;

// Tool Tip methods

- (void) toggleHelp:(UIViewController*) vc Enable:(BOOL)enable;
- (void) showTipForView:(UIView*) subView InMainView:(UIView*)view;

-(BOOL)iPhone6Plus;

@end
