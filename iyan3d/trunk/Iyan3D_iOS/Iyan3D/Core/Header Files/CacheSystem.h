//
//  CacheSystem.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 04/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <sqlite3.h>
#import "AssetItem.h"
#import "SceneItem.h"
#import "AnimationItem.h"
#include "RenderItem.h"

@interface CacheSystem : NSObject {
    sqlite3 *_cacheSystem;
    
}
+ (CacheSystem*) cacheSystem;
- (void) OpenDatabase;
- (void) UpdateAsset:(AssetItem *)a;
-(void)updateAssetPrice:(AssetItem*)asset;
- (NSMutableArray*) GetAssetList:(int)type Search:(NSString*)keyword;
-(NSString*)getAssetPrice:(NSString*)iap;
- (AssetItem*) GetAsset:(int)assetId;
- (AssetItem*) GetAssetByName:(NSString*)assetName;
- (AssetItem*) GetAssetItem:(NSString*)assetIap;
- (NSMutableArray*) GetSceneList:(NSString*)keyword;

- (void) UpdateAnimation:(AnimationItem *)a;
- (void) UpdateUserInfoToAnimationTable:(NSArray *)userArray Usrid:(NSString*)userId;
- (void) UpdateMyAnimation:(AnimationItem *)a;
- (AnimationItem*) GetAnimation:(int)assetId fromTable:(int)tableType;
- (NSMutableArray*) GetAnimationList:(int)type fromTable:(int)tableType Search:(NSString*)keyword;
- (int) getNextAnimationAssetId;

- (void) UpdateScene:(SceneItem *)s;
- (bool) AddScene:(SceneItem *)s;
- (void) DeleteScene:(SceneItem *)s;
- (void) DeleteTableData:(int)assetId;
- (NSMutableArray*) GetSceneList;

- (void) AddDownloadedAsset:(AssetItem*)a;
- (int) getNextObjAssetId;
- (bool) checkOBJImporterPurchase;
-(bool) checkDownloadedAsset:(int)assetId;
- (void) addOBJImporterColumn;
- (void) createTablesForPrice;
-(void) createAnimationTables;
- (int) getNextAutoRigAssetId;
-(void) createRenderTaskTables;
-(NSMutableArray *) getRenderTask;
- (void) updateRenderTask:(int)taskId WithProgress:(int)progress;
-(void) deleteRenderTaskData: (int)taskId;
-(void) addRenderTaskData: (int)taskId estTime:(float)estimatedTime proName:(NSString*)projectName;

@end
