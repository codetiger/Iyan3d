//
//  CacheSystem.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 04/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "CacheSystem.h"
#import <CommonCrypto/CommonDigest.h>
#import "Utility.h"
#import <objc/runtime.h>
#define MYANIMATION_TABLE 7
#define MYANIMATION_DOWNLOAD 4
#define MYANIMATION_RATING 6
#define MYANIMATION_FEATURED 5
#define MYANIMATION_RECENT 8
#define MYANIMATION_ALL 9


@implementation CacheSystem


static CacheSystem *_cacheSystem;
static NSString* dbLock = nil;

+ (CacheSystem*) cacheSystem {
    if (_cacheSystem == nil) {
        _cacheSystem = [[CacheSystem alloc] init];
    }
    return _cacheSystem;
}

- (id) init {
    if ((self = [super init])) {
        dbLock = [[NSString alloc] init];
    }
    return self;
}

- (void) OpenDatabase {
    @synchronized (dbLock) {
        char *errMsg = NULL;
        
        NSString* dbName = @"iyan3d-2-0.db";
        NSArray* dirPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* docsDir = [dirPaths objectAtIndex:0];
        NSString* srcFilePath = [NSString stringWithFormat:@"%@/Resources",docsDir];
        NSString* databasePath = [[NSString alloc] initWithString: [srcFilePath stringByAppendingPathComponent:dbName]];
        if (![[NSFileManager defaultManager] fileExistsAtPath:databasePath]) {
            if (sqlite3_open([databasePath UTF8String], &_cacheSystem) == SQLITE_OK) {
                NSString* createAssetTable = [NSString stringWithFormat:@"CREATE TABLE %@ (%@ INTEGER, %@ TEXT, %@ TEXT, %@ TEXT, %@ TEXT, %@ TEXT, %@ INTEGER, %@ INTEGER)", TABLE_ASSET_INFO, ASSET_ID, ASSET_NAME, ASSET_KEYWORDS, ASSET_IAP, ASSET_HASH, ASSET_DATE, ASSET_TYPE, ASSET_BONES];
                if(sqlite3_exec(_cacheSystem, [createAssetTable UTF8String], NULL, NULL, &errMsg) != SQLITE_OK){
                }
                
                NSString* createSceneTable = [NSString stringWithFormat:@"CREATE TABLE %@ (%@ INTEGER PRIMARY KEY AUTOINCREMENT, %@ TEXT, %@ TEXT, %@ TEXT)", TABLE_SCENE_INFO, SCENE_ID, SCENE_NAME, SCENE_DATE, SCENE_FILE];
                if(sqlite3_exec(_cacheSystem, [createSceneTable UTF8String], NULL, NULL, &errMsg) != SQLITE_OK){
                }
                
                NSString* createDownloadedTable = [NSString stringWithFormat:@"CREATE TABLE %@ (%@ INTEGER PRIMARY KEY AUTOINCREMENT, %@ INTEGER, %@ INTEGER)", TABLE_DOWNLOADED_ASSET_INFO, DOWNLOADED_ASSET_PRIMARYID, DOWNLOADED_ASSET_ID, DOWNLOADED_ASSET_TYPE];
                if(sqlite3_exec(_cacheSystem, [createDownloadedTable UTF8String], NULL, NULL, &errMsg) != SQLITE_OK){
                }
                
                sqlite3_free(errMsg);

            } else {
            }
        } else {
            if (sqlite3_open([databasePath UTF8String], &_cacheSystem) != SQLITE_OK) {
        }
        
    }
}
}

static const NSString* TABLE_ASSET_INFO = @"asset_info";
static const NSString* OBJ_IMPORTER_TABLE = @"addons";
static const NSString* ASSET_PRICE_TABLE = @"asset_price_table";
static const NSString* ALL_ANIMATIONS_TABLE = @"animations_table";
static const NSString* MY_ANIMATIONS_TABLE = @"my_animations_table";

static const NSString* ASSET_ID = @"asset_id";
static const NSString* ASSET_NAME = @"asset_name";
static const NSString* ASSET_KEYWORDS = @"asset_keywords";
static const NSString* ASSET_IAP = @"asset_iap";
static const NSString* ASSET_HASH = @"asset_hash";
static const NSString* ASSET_DATE = @"asset_date";
static const NSString* ASSET_TYPE = @"asset_type";
static const NSString* ASSET_BONES = @"asset_bones";
static const NSString* ASSET_PRICE = @"asset_price";
static const NSString* ASSET_GROUP = @"asset_group";

static const NSString* ASSET_PUBLISHED = @"asset_published";
static const NSString* ASSET_RATING = @"asset_rating";
static const NSString* DOWNLOADS = @"downloads";
static const NSString* USER_ID = @"user_id";
static const NSString* USER_NAME = @"user_name";
static const NSString* VIEWER_RATED = @"viewer_rated";
static const NSString* FEATURED_INDEX = @"featuredindex";

static const NSString* OBJ_IMPORTER_IAP = @"column1";

static const NSString* RENDER_TASK_TABLE = @"render_task_version2";
static const NSString* RENDER_TASK_PROJECT_NAME = @"project_name";
static const NSString* RENDER_TASK_ID = @"task_id";
static const NSString* RENDER_TASK_STATUS = @"task_status";
static const NSString* RENDER_TASK_FRAMES = @"task_frames";
static const NSString* RENDER_TASK_DATE = @"task_date";

-(NSString*) insertImportedScene
{
    @synchronized (dbLock) {
        NSDateFormatter *dateFormatter=[[NSDateFormatter alloc]init];
        
        static const NSString* TABLE_SCENE_INFO = @"scene_info";
        static const NSString* SCENE_NAME = @"scene_name";
        static const NSString* SCENE_DATE = @"scene_date";
        static const NSString* SCENE_FILE = @"scene_file";
        
        NSString *currentDateString = @"2015-06-04T21:38:22";
        [dateFormatter setDateFormat:@"yyyy-MM-dd'T'HH:mm:ss"];
        NSDate *currentDate = [dateFormatter dateFromString:currentDateString];
        
        NSMutableArray *scenes = [self GetSceneList];
        NSString * sceneName = [NSString stringWithFormat:@"MyScene%d", [scenes count]+1];
        
        NSString* salt = [NSString stringWithFormat:@"%@:SG:%@", sceneName, currentDate];
        NSString* sceneFile = [Utility getMD5ForString:salt];

        sqlite3_stmt    *statement;
        NSString* insertSceneTable = [NSString stringWithFormat: @"INSERT INTO %@ (%@, %@, %@) VALUES (\"%@\", \"%@\", \"%@\")", TABLE_SCENE_INFO, SCENE_NAME, SCENE_DATE, SCENE_FILE, sceneName, currentDate, sceneFile];
        sqlite3_prepare_v2(_cacheSystem, [insertSceneTable UTF8String], -1, &statement, NULL);
        if (sqlite3_step(statement) != SQLITE_DONE)
            NSLog(@"Failed Inserting Scene %s", sqlite3_errmsg(_cacheSystem));
        sqlite3_finalize(statement);
        return sceneFile;
    }
}


-(BOOL) checkAndCreateGroupColumnInAssetsTable
{
    BOOL columnAdded = NO;
    
    sqlite3_stmt *statement;
    
    NSString *querySQL = [NSString stringWithFormat:@"SELECT %@ FROM %@", ASSET_GROUP, TABLE_ASSET_INFO];
    if(sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
        columnAdded = YES;
    } else {
        NSString *updateSQL = [NSString stringWithFormat: @"ALTER TABLE %@ ADD COLUMN %@ INTEGER", TABLE_ASSET_INFO, ASSET_GROUP];
        const char *update_stmt = [updateSQL UTF8String];
        sqlite3_prepare_v2(_cacheSystem, update_stmt, -1, &statement, NULL);
        
        if(sqlite3_step(statement)==SQLITE_DONE)
        {
            columnAdded = YES;
        }
        else
        {
            NSLog(@"\n Failed inserting column ");
            columnAdded = NO;
        }
        sqlite3_finalize(statement);
    }
    return columnAdded;
}

-(void) createRenderTaskTables{
    @synchronized (dbLock) {
        char *errMsg;
        NSString* createRenderTaskTables = [NSString stringWithFormat:@"CREATE TABLE %@ (%@ TEXT, %@ INTEGER, %@ INTEGER, %@ INTEGER, %@ TEXT)", RENDER_TASK_TABLE,RENDER_TASK_PROJECT_NAME,RENDER_TASK_ID,RENDER_TASK_STATUS,RENDER_TASK_FRAMES, RENDER_TASK_DATE];
        if(sqlite3_exec(_cacheSystem, [createRenderTaskTables UTF8String], NULL, NULL, &errMsg) != SQLITE_OK){
            NSLog(@" Error creating Render task table %s " , errMsg);
        }
        sqlite3_free(errMsg);
        
    }
}

-(void) addRenderTaskData: (int)taskId estTime:(float)estimatedTime proName:(NSString *)projectName date:(NSString*) dateStr {
    @synchronized (dbLock){
        sqlite3_stmt    *statement;
        int taskStatus=0;
        NSString *querySQL = [NSString stringWithFormat:@"INSERT INTO %@ (%@ ,%@, %@, %@, %@) VALUES (\"%@\",\"%d\", \"%d\", \"%f\", \"%@\")", RENDER_TASK_TABLE,RENDER_TASK_PROJECT_NAME, RENDER_TASK_ID, RENDER_TASK_STATUS, RENDER_TASK_FRAMES, RENDER_TASK_DATE, projectName, taskId,taskStatus ,estimatedTime, dateStr];
        NSLog(@"Successfully Inserted: Task id:%d Estimated time :%f , Project Name: %@",taskId,estimatedTime,projectName);
        sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
        if (sqlite3_step(statement) != SQLITE_DONE)
            NSLog(@"Failed Inserting Asset %s", sqlite3_errmsg(_cacheSystem));
        sqlite3_finalize(statement);
        
    }
}

-(void) deleteRenderTaskData: (int)taskId {
    @synchronized (dbLock){
        sqlite3_stmt    *statement;
        NSString *querySQL = [NSString stringWithFormat:@"DELETE FROM %@ WHERE %@ = %d", RENDER_TASK_TABLE, RENDER_TASK_ID, taskId];
        NSLog(@"Successfully Deleted: Task id:%d",taskId);
        if(sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
        if (sqlite3_step(statement) != SQLITE_DONE)
            NSLog(@"Failed DELETING Asset %s", sqlite3_errmsg(_cacheSystem));
        }
        sqlite3_finalize(statement);
    }
}

-(NSMutableArray *) getRenderTask{
    @synchronized (dbLock){
        NSMutableArray *array1=[[NSMutableArray alloc] init];
        
        NSString *query = [NSString stringWithFormat:@"SELECT * FROM %@ ORDER BY %@ DESC", RENDER_TASK_TABLE, RENDER_TASK_ID];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [query UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            while( sqlite3_step(statement) == SQLITE_ROW) {
                RenderItem* a = [[RenderItem alloc] init];
                a.taskName = [NSString stringWithFormat:@"%s",sqlite3_column_text(statement, 0)];
                a.taskId = sqlite3_column_int(statement, 1);
                a.taskProgress = sqlite3_column_int(statement, 2);
                a.taskFrames = sqlite3_column_int(statement, 3);
                a.dateAdded = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 4)];
                [array1 addObject:a];
            }
            
        }else{
            NSLog(@"Error");
        }
        sqlite3_finalize(statement);
        return array1;
    }
}

- (RenderItem *)getRenderTaskByTaskId:(int) taskId {
    @synchronized (dbLock){
        NSString *query = [NSString stringWithFormat:@"SELECT * FROM %@ WHERE %@ = %d",RENDER_TASK_TABLE, RENDER_TASK_ID, taskId];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [query UTF8String], -1, &statement, NULL) == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
                RenderItem* a = [[RenderItem alloc] init];
                a.taskName = [NSString stringWithFormat:@"%s",sqlite3_column_text(statement, 0)];
                a.taskId = sqlite3_column_int(statement, 1);
                a.taskProgress = sqlite3_column_int(statement, 2);
                a.taskFrames = sqlite3_column_int(statement, 3);
                a.dateAdded = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 4)];
            sqlite3_finalize(statement);
            return a;
        }else{
            NSLog(@"Error");
        }
        sqlite3_finalize(statement);
        return nil;
    }
}

- (RenderItem *)getRenderTaskByDate:(NSString*) dateTime {
    @synchronized (dbLock){
        NSString *query = [NSString stringWithFormat:@"SELECT * FROM %@ WHERE %@ = %@",RENDER_TASK_TABLE, RENDER_TASK_DATE, dateTime];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [query UTF8String], -1, &statement, NULL) == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
            RenderItem* a = [[RenderItem alloc] init];
            a.taskName = [NSString stringWithFormat:@"%s",sqlite3_column_text(statement, 0)];
            a.taskId = sqlite3_column_int(statement, 1);
            a.taskProgress = sqlite3_column_int(statement, 2);
            a.taskFrames = sqlite3_column_int(statement, 3);
            a.dateAdded = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 4)];
            sqlite3_finalize(statement);
            return a;
        }else{
            NSLog(@"Error");
        }
        sqlite3_finalize(statement);
        return nil;
    }
}

- (void) updateRenderTask:(int)taskId WithProgress:(int)progress
{
    @synchronized (dbLock) {
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", RENDER_TASK_TABLE, RENDER_TASK_ID, taskId];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_ROW) {
                sqlite3_stmt *statement;
                NSString *querySQL = [NSString stringWithFormat: @"UPDATE %@ SET %@ = %d WHERE %@ = %d", RENDER_TASK_TABLE, RENDER_TASK_STATUS, progress, RENDER_TASK_ID, taskId];
                sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
                if (sqlite3_step(statement) != SQLITE_DONE)
                    NSLog(@"Failed Updating Asset %s", sqlite3_errmsg(_cacheSystem));
            } else
                NSLog(@"Not found");
        } else
            NSLog(@"Not Found");
        
        sqlite3_finalize(statement);
    }
}

-(void) createAnimationTables {
    @synchronized (dbLock) {
        char *errMsg;
        NSString* createAnimationTables = [NSString stringWithFormat:@"CREATE TABLE %@ (%@ INTEGER, %@ TEXT, %@ TEXT, %@ TEXT, %@ INTEGER, %@ INTEGER, %@ TEXT, %@ TEXT, %@ INTEGER, %@ INTEGER, %@ INTEGER, %@ INTEGER)", ALL_ANIMATIONS_TABLE, ASSET_ID, ASSET_NAME, ASSET_KEYWORDS, ASSET_DATE, ASSET_TYPE, ASSET_BONES, USER_ID, USER_NAME, ASSET_RATING, DOWNLOADS, VIEWER_RATED ,FEATURED_INDEX];
        if(sqlite3_exec(_cacheSystem, [createAnimationTables UTF8String], NULL, NULL, &errMsg) != SQLITE_OK){
            //NSLog(@" Error creating animation table %s " , errMsg);
        }
        sqlite3_free(errMsg);
        char *errMsg1;
        NSString* createMyAnimationTables = [NSString stringWithFormat:@"CREATE TABLE %@ (%@ INTEGER, %@ TEXT, %@ TEXT, %@ TEXT, %@ INTEGER, %@ INTEGER, %@ TEXT, %@ TEXT, %@ INTEGER, %@ INTEGER, %@ INTEGER, %@ INTEGER)", MY_ANIMATIONS_TABLE, ASSET_ID, ASSET_NAME, ASSET_KEYWORDS, ASSET_DATE, ASSET_TYPE, ASSET_BONES, USER_ID, USER_NAME, ASSET_RATING, DOWNLOADS, VIEWER_RATED, ASSET_PUBLISHED];
        if(sqlite3_exec(_cacheSystem, [createMyAnimationTables UTF8String], NULL, NULL, &errMsg1) != SQLITE_OK){
           // NSLog(@" Error creating my animation table %s " , errMsg);
        }
        sqlite3_free(errMsg1);

    }
}
- (void) UpdateUserInfoToAnimationTable:(NSArray *)userArray Usrid:(NSString *)userId
{
    @synchronized (dbLock) {
        NSString *querySQL = [NSString stringWithFormat: @"UPDATE %@ SET %@ = 0 WHERE %@ = 1", ALL_ANIMATIONS_TABLE, VIEWER_RATED , VIEWER_RATED];
        sqlite3_stmt *statement;
        sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
        if (sqlite3_step(statement) != SQLITE_DONE)
            NSLog(@"Failed Updating Asset %s", sqlite3_errmsg(_cacheSystem));
        sqlite3_finalize(statement);
        for (int i = 0 ; i < [userArray count]; i++) {
            NSString *querySQL = [NSString stringWithFormat: @"UPDATE %@ SET %@ = 1 WHERE %@ = %@", ALL_ANIMATIONS_TABLE, VIEWER_RATED, ASSET_ID, userArray[i]];
            sqlite3_stmt *statement;
            sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
            if (sqlite3_step(statement) != SQLITE_DONE)
                NSLog(@"Failed Updating Asset %s", sqlite3_errmsg(_cacheSystem));
            sqlite3_finalize(statement);
        }
    }
}
- (void) UpdateAnimation:(AnimationItem *)a {
    @synchronized (dbLock) {
        
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", ALL_ANIMATIONS_TABLE, ASSET_ID, a.assetId];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            
            if (sqlite3_step(statement) == SQLITE_ROW) {
                sqlite3_stmt *statement;
                
                //NSString *name = [Utility encryptString:a.assetName password:@"SGmanKindWin5SG"];
                
                NSString *querySQL = [NSString stringWithFormat: @"UPDATE %@ SET %@ = \"%@\", %@ = \"%@\", %@ = \"%@\", %@ = %d, %@ = %d, %@ = \"%@\", %@ = \"%@\", %@ = %d, %@ = %d, %@ = %d WHERE %@ = %d", ALL_ANIMATIONS_TABLE, ASSET_NAME, a.assetName, ASSET_KEYWORDS, a.keywords, ASSET_DATE, a.modifiedDate, ASSET_TYPE, a.type, ASSET_BONES, a.boneCount, USER_ID, a.userId, USER_NAME, a.userName, ASSET_RATING, a.rating, DOWNLOADS, a.downloads, VIEWER_RATED, a.isViewerRated, ASSET_ID, a.assetId];
                sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
                if (sqlite3_step(statement) != SQLITE_DONE)
                    NSLog(@"Failed Updating Asset %s", sqlite3_errmsg(_cacheSystem));
                sqlite3_finalize(statement);
            } else {
                sqlite3_stmt    *statement;
                
                //NSString *name = [Utility encryptString:a.assetName password:@"SGmanKindWin5SG"];
                
                NSString *querySQL = [NSString stringWithFormat: @"INSERT INTO %@ (%@, %@, %@, %@, %@, %@, %@, %@, %@, %@, %@) VALUES (%d, \"%@\", \"%@\", \"%@\", %d, %d, \"%@\", \"%@\", %d, %d, %d)", ALL_ANIMATIONS_TABLE, ASSET_ID, ASSET_NAME, ASSET_KEYWORDS, ASSET_DATE, ASSET_TYPE, ASSET_BONES, USER_ID, USER_NAME, ASSET_RATING, DOWNLOADS, VIEWER_RATED, a.assetId, a.assetName, a.keywords, a.modifiedDate, a.type, a.boneCount, a.userId, a.userName, a.rating, a.downloads, a.isViewerRated];
                sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
                if (sqlite3_step(statement) != SQLITE_DONE)
                    NSLog(@"Failed Inserting Asset %s", sqlite3_errmsg(_cacheSystem));
                sqlite3_finalize(statement);
            }
            sqlite3_finalize(statement);
        }
    }
}

- (void) UpdateMyAnimation:(AnimationItem *)a {
    @synchronized (dbLock) {
        
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", MY_ANIMATIONS_TABLE, ASSET_ID, a.assetId];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            
            if (sqlite3_step(statement) == SQLITE_ROW) {
                sqlite3_stmt *statement;
                
                //NSString *name = [Utility encryptString:a.assetName password:@"SGmanKindWin5SG"];
                
                NSString *querySQL = [NSString stringWithFormat: @"UPDATE %@ SET %@ = \"%@\", %@ = \"%@\", %@ = \"%@\", %@ = %d, %@ = %d, %@ = \"%@\", %@ = \"%@\", %@ = %d, %@ = %d, %@ = %d, %@ = %d WHERE %@ = %d", MY_ANIMATIONS_TABLE, ASSET_NAME, a.assetName, ASSET_KEYWORDS, a.keywords, ASSET_DATE, a.modifiedDate, ASSET_TYPE, a.type, ASSET_BONES, a.boneCount, USER_ID, a.userId, USER_NAME, a.userName, ASSET_RATING, a.rating, DOWNLOADS, a.downloads, VIEWER_RATED, a.isViewerRated, ASSET_PUBLISHED, a.published, ASSET_ID, a.assetId];
                
                sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
                if (sqlite3_step(statement) != SQLITE_DONE)
                    NSLog(@"Failed Updating Asset %s", sqlite3_errmsg(_cacheSystem));
                sqlite3_finalize(statement);
            } else {
                sqlite3_stmt    *statement;
                
                //NSString *name = [Utility encryptString:a.assetName password:@"SGmanKindWin5SG"];
                
                NSString *querySQL = [NSString stringWithFormat: @"INSERT INTO %@ (%@, %@, %@, %@, %@, %@, %@, %@, %@, %@, %@, %@) VALUES (%d, \"%@\", \"%@\", \"%@\", %d, %d, \"%@\", \"%@\", %d, %d, %d, %d)", MY_ANIMATIONS_TABLE, ASSET_ID, ASSET_NAME, ASSET_KEYWORDS, ASSET_DATE, ASSET_TYPE, ASSET_BONES, USER_ID, USER_NAME, ASSET_RATING, DOWNLOADS, VIEWER_RATED, ASSET_PUBLISHED, a.assetId, a.assetName, a.keywords, a.modifiedDate, a.type, a.boneCount, a.userId, a.userName, a.rating, a.downloads, a.isViewerRated, a.published];
                
                sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
                if (sqlite3_step(statement) != SQLITE_DONE)
                    NSLog(@"Failed Inserting Asset %s", sqlite3_errmsg(_cacheSystem));
                sqlite3_finalize(statement);
            }
            sqlite3_finalize(statement);
        }
    }
}

- (void) deleteMyAnimation:(int)assetId
{
    @synchronized (dbLock) {
        sqlite3_stmt    *statement;
        NSString *querySQL = [NSString stringWithFormat: @"DELETE FROM %@ WHERE %@ = %d", MY_ANIMATIONS_TABLE, ASSET_ID, assetId];
        sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
        if (sqlite3_step(statement) != SQLITE_DONE){
            NSLog(@"Failed Deleting Scene %s", sqlite3_errmsg(_cacheSystem));
        }
        sqlite3_finalize(statement);
    }
}

- (AnimationItem*) GetAnimation:(int)assetId fromTable:(int)tableType {
    @synchronized (dbLock) {
        NSString *querySQL = @"";
        if (tableType == MYANIMATION_TABLE)
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", MY_ANIMATIONS_TABLE, ASSET_ID, assetId];
        else
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", ALL_ANIMATIONS_TABLE, ASSET_ID, assetId];
        
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
            
            /*
            NSCharacterSet *notAllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"] invertedSet];
            NSCharacterSet *AllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789. "] invertedSet];
            
            NSString *name = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)] password:@"SGmanKindWin5SG"];
            name = [[name componentsSeparatedByCharactersInSet:AllowedChars] componentsJoinedByString:@""];
            NSString *iap = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 3)] password:@"SGmanKindWin5SG"];
            iap = [[iap componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
            NSString *has = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 4)] password:@"SGmanKindWin5SG"];
            has = [[has componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
            */
            
            AnimationItem* a = [[AnimationItem alloc] init];
            a.assetId = sqlite3_column_int(statement, 0);
            a.assetName = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)];
            a.keywords = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 2)];
            a.modifiedDate = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 3)];
            a.type = sqlite3_column_int(statement, 4);
            a.boneCount = sqlite3_column_int(statement, 5);
            a.userId = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 6)];
            a.userName = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 7)];
            a.rating = sqlite3_column_int(statement, 8);
            a.downloads = sqlite3_column_int(statement, 9);
            a.isViewerRated = sqlite3_column_int(statement, 10);
            if(tableType == MYANIMATION_TABLE)
                a.published = sqlite3_column_int(statement, 11);
            else
                a.featuredindex = sqlite3_column_int(statement, 12);
            sqlite3_finalize(statement);
            return a;
        }
        return nil;
    }
}

- (NSMutableArray*) GetAnimationList:(int)type fromTable:(int)tableType Search:(NSString*)keyword {
    @synchronized (dbLock) {
        NSMutableArray *array = [[NSMutableArray alloc] init];
        NSString *querySQL = @"";
        
        if(tableType == MYANIMATION_DOWNLOAD) {
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", ALL_ANIMATIONS_TABLE, ASSET_TYPE, type];
        }else if(tableType == MYANIMATION_RATING) {
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", ALL_ANIMATIONS_TABLE, ASSET_TYPE, type];
        } else if(tableType == MYANIMATION_TABLE) {
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", MY_ANIMATIONS_TABLE, ASSET_TYPE, type];
        } else if(tableType == MYANIMATION_FEATURED) {
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", ALL_ANIMATIONS_TABLE, ASSET_TYPE, type];
        } else if(tableType == MYANIMATION_RECENT) {
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", ALL_ANIMATIONS_TABLE, ASSET_TYPE, type];
        }
        if([keyword length]>0) {
            switch (tableType) {
                case MYANIMATION_DOWNLOAD:
                    querySQL = [querySQL stringByAppendingFormat:@" AND %@ LIKE '%s%@%s' ORDER BY %@ DESC LIMIT 50", ASSET_KEYWORDS, "% ",[keyword lowercaseString],"%",DOWNLOADS];
                    break;
                case MYANIMATION_RATING:
                    querySQL = [querySQL stringByAppendingFormat:@" AND %@ LIKE '%s%@%s' ORDER BY %@ DESC LIMIT 50", ASSET_KEYWORDS, "% ",[keyword lowercaseString],"%",ASSET_RATING];
                    break;
                case MYANIMATION_FEATURED:
                    querySQL = [querySQL stringByAppendingFormat:@" AND %@ LIKE '%s%@%s' ORDER BY %@ DESC LIMIT 50", ASSET_KEYWORDS, "% ",[keyword lowercaseString],"%",FEATURED_INDEX];
                    break;
                case MYANIMATION_TABLE:
                    querySQL = [querySQL stringByAppendingFormat:@" AND %@ LIKE '%s%@%s'", ASSET_KEYWORDS, "% ",[keyword lowercaseString],"%"];
                    break;
                default:
                    break;
            }
        }else{
            switch (tableType) {
                case MYANIMATION_DOWNLOAD:
                    querySQL = [querySQL stringByAppendingFormat:@" ORDER BY %@ DESC LIMIT 50", DOWNLOADS];
                    break;
                case MYANIMATION_RATING:
                    querySQL = [querySQL stringByAppendingFormat:@" ORDER BY %@ DESC LIMIT 50", ASSET_RATING];
                    break;
                case MYANIMATION_FEATURED:
                    querySQL = [querySQL stringByAppendingFormat:@" ORDER BY %@ DESC LIMIT 50", FEATURED_INDEX];
                    break;
                case MYANIMATION_RECENT:{
                    querySQL = [querySQL stringByAppendingFormat:@" ORDER BY datetime(%@) DESC LIMIT 50", ASSET_DATE];
                    break;
                }
                default:
                    break;
            }
        }
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            while( sqlite3_step(statement) == SQLITE_ROW) {
                AnimationItem* a = [[AnimationItem alloc] init];
                
                /*
                NSCharacterSet *notAllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"] invertedSet];
                NSCharacterSet *AllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789. "] invertedSet];
                
                NSString *name = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)] password:@"SGmanKindWin5SG"];
                name = [[name componentsSeparatedByCharactersInSet:AllowedChars] componentsJoinedByString:@""];
                NSString *iap = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 3)] password:@"SGmanKindWin5SG"];
                iap = [[iap componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
                NSString *has = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 4)] password:@"SGmanKindWin5SG"];
                has = [[has componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
                */
                
                a.assetId = sqlite3_column_int(statement, 0);
                a.assetName = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)];
                a.keywords = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 2)];
                a.modifiedDate = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 3)];
                a.type = sqlite3_column_int(statement, 4);
                a.boneCount = sqlite3_column_int(statement, 5);
                a.userId = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 6)];
                a.userName = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 7)];
                a.rating = sqlite3_column_int(statement, 8);
                a.downloads = sqlite3_column_int(statement, 9);
                a.isViewerRated = sqlite3_column_int(statement, 10);
                if (tableType == MYANIMATION_TABLE)
                    a.published = sqlite3_column_int(statement, 11);
                if(tableType != MYANIMATION_TABLE)
                    a.featuredindex = sqlite3_column_int(statement, 12);
                
                [array addObject:a];
            }
            sqlite3_finalize(statement);
        }
        return array;
    }
}

- (int) getNextAnimationAssetId {
    @synchronized (dbLock) {
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@", MY_ANIMATIONS_TABLE];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            int ObjCount = 0;
            while(sqlite3_step(statement) == SQLITE_ROW)
                ObjCount++;
            
            sqlite3_finalize(statement);
            return ObjCount;
        }
    }
    return -1;
}
- (void) UpdateAsset:(AssetItem *)a {
    @synchronized (dbLock) {
        
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", TABLE_ASSET_INFO, ASSET_ID, a.assetId];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            
            if (sqlite3_step(statement) == SQLITE_ROW) {
                sqlite3_stmt *statement;
                
                NSString *name = [Utility encryptString:a.name password:@"SGmanKindWin5SG"];
                NSString *iap = [Utility encryptString:a.iap password:@"SGmanKindWin5SG"];
                NSString *has = [Utility encryptString:a.hash password:@"SGmanKindWin5SG"];
                
                NSString *querySQL = [NSString stringWithFormat: @"UPDATE %@ SET %@ = \"%@\", %@ = \"%@\", %@ = \"%@\", %@ = \"%@\", %@ = \"%@\", %@ = %d, %@ = %d, %@ = %d WHERE %@ = %d", TABLE_ASSET_INFO, ASSET_NAME, name, ASSET_KEYWORDS, a.keywords, ASSET_IAP, iap, ASSET_HASH, has, ASSET_DATE, a.modifiedDate, ASSET_TYPE, a.type, ASSET_BONES, a.boneCount, ASSET_GROUP, a.group, ASSET_ID, a.assetId];
                
                sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
                if (sqlite3_step(statement) != SQLITE_DONE)
                    NSLog(@"Failed Updating Asset %s", sqlite3_errmsg(_cacheSystem));
                sqlite3_finalize(statement);
            } else {
                sqlite3_stmt    *statement;
                
                NSString *name = [Utility encryptString:a.name password:@"SGmanKindWin5SG"];
                NSString *iap = [Utility encryptString:a.iap password:@"SGmanKindWin5SG"];
                NSString *has = [Utility encryptString:a.hash password:@"SGmanKindWin5SG"];
                
                NSString *querySQL = [NSString stringWithFormat: @"INSERT INTO %@ (%@, %@, %@, %@, %@, %@, %@, %@, %@) VALUES (%d, \"%@\", \"%@\", \"%@\", \"%@\", \"%@\", %d, %d, %d)", TABLE_ASSET_INFO, ASSET_ID, ASSET_NAME, ASSET_KEYWORDS, ASSET_IAP, ASSET_HASH, ASSET_DATE, ASSET_TYPE, ASSET_BONES, ASSET_GROUP, a.assetId, name, a.keywords, iap, has, a.modifiedDate, a.type, a.boneCount, a.group];
                
                sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
                if (sqlite3_step(statement) != SQLITE_DONE)
                    NSLog(@"Failed Inserting Asset %s", sqlite3_errmsg(_cacheSystem));
                sqlite3_finalize(statement);
            }
            sqlite3_finalize(statement);
        }
    }
}

-(void)updateAssetPrice:(AssetItem*)asset
{
     @synchronized (dbLock) {
    NSString *iap = [Utility encryptString:asset.iap password:@"SGmanKindWin5SG"];
         
    NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = \"%@\"", ASSET_PRICE_TABLE, ASSET_IAP, iap];
    sqlite3_stmt *statement;
    if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_ROW) {
                sqlite3_stmt *statement;
            NSString *querySQL = [NSString stringWithFormat: @"UPDATE %@ SET %@ = \"%@\" WHERE %@ = \"%@\"", ASSET_PRICE_TABLE, ASSET_PRICE, asset.price, ASSET_IAP, iap];
                sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
                if (sqlite3_step(statement) != SQLITE_DONE)
                    NSLog(@"Failed Updating Asset %s", sqlite3_errmsg(_cacheSystem));
                sqlite3_finalize(statement);

        } else {
            sqlite3_stmt *statement;
            NSString *querySQL = [NSString stringWithFormat: @"INSERT INTO %@ (%@, %@) VALUES (\"%@\", \"%@\")", ASSET_PRICE_TABLE, ASSET_IAP, ASSET_PRICE, iap, asset.price];
            
            sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
            if (sqlite3_step(statement) != SQLITE_DONE)
                NSLog(@"Failed Inserting Asset %s", sqlite3_errmsg(_cacheSystem));
            sqlite3_finalize(statement);
        }
        sqlite3_finalize(statement);
    }
     }
}

- (void) deleteMyAsset:(int)assetId
{
    @synchronized (dbLock) {
        sqlite3_stmt    *statement;
        NSString *querySQL = [NSString stringWithFormat: @"DELETE FROM %@ WHERE %@ = %d", TABLE_DOWNLOADED_ASSET_INFO, DOWNLOADED_ASSET_ID, assetId];
        sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
        if (sqlite3_step(statement) != SQLITE_DONE){
            NSLog(@"Failed Deleting Scene %s", sqlite3_errmsg(_cacheSystem));
        }
        sqlite3_finalize(statement);
        
        sqlite3_stmt    *statement2;
        NSString *querySQL2 = [NSString stringWithFormat: @"DELETE FROM %@ WHERE %@ = %d", TABLE_ASSET_INFO, ASSET_ID, assetId];
        sqlite3_prepare_v2(_cacheSystem, [querySQL2 UTF8String], -1, &statement2, NULL);
        if (sqlite3_step(statement2) != SQLITE_DONE){
            NSLog(@"Failed Deleting Scene %s", sqlite3_errmsg(_cacheSystem));
        }
        sqlite3_finalize(statement2);

    }
}

- (NSMutableArray*) GetAssetList:(int)type Search:(NSString*)keyword {
    @synchronized (dbLock) {
        NSMutableArray *array = [[NSMutableArray alloc] init];
        NSString *querySQL = @"";
        
        if(type == 5 || type == 6){/*Type 5 For Minecraft Category 6 for FNAF Category */
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", TABLE_ASSET_INFO, ASSET_GROUP, (type == 5) ? 1 : 2];
        } else if(type > 2) {
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", TABLE_ASSET_INFO, ASSET_TYPE, type];
        } else if(type == -1) {
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@, %@ WHERE %@.%@ <= 6 AND %@.%@ != 4 AND %@.%@ = %@.%@", TABLE_ASSET_INFO, TABLE_DOWNLOADED_ASSET_INFO, TABLE_ASSET_INFO, ASSET_TYPE,TABLE_ASSET_INFO, ASSET_TYPE, TABLE_ASSET_INFO, ASSET_ID, TABLE_DOWNLOADED_ASSET_INFO, DOWNLOADED_ASSET_ID];
        } else if(type == -2) {
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@, %@ WHERE %@.%@ = 4 AND %@.%@ = %@.%@", TABLE_ASSET_INFO, TABLE_DOWNLOADED_ASSET_INFO, TABLE_ASSET_INFO, ASSET_TYPE, TABLE_ASSET_INFO, ASSET_ID, TABLE_DOWNLOADED_ASSET_INFO, DOWNLOADED_ASSET_ID];
        } else if(type == 2) {
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d AND %@ > 30000", TABLE_ASSET_INFO, ASSET_TYPE, type,ASSET_ID];
        } else if(type == 1) {
            querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d AND %@ < 40000", TABLE_ASSET_INFO, ASSET_TYPE, type, ASSET_ID];
        }else if(type == 0){
            querySQL = [NSString stringWithFormat:@"SELECT * FROM %@ WHERE %@ = %s OR %@ = %s OR %@ = %s",TABLE_ASSET_INFO,ASSET_TYPE,"1",ASSET_TYPE,"2",ASSET_TYPE,"3"];
        }
        
        if(type == 0)
        {
            querySQL = [querySQL stringByAppendingFormat:@" ORDER BY datetime(%@) DESC", ASSET_DATE];
        }
        
        if([keyword length]) {
            querySQL = [querySQL stringByAppendingFormat:@" AND %@ LIKE '%s%@%s'", ASSET_KEYWORDS, "% ",[keyword lowercaseString],"%"];
        }
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            while( sqlite3_step(statement) == SQLITE_ROW) {
                AssetItem* a = [[AssetItem alloc] init];
                
                NSCharacterSet *notAllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"] invertedSet];
                NSCharacterSet *AllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789. "] invertedSet];
                
                NSString *name = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)] password:@"SGmanKindWin5SG"];
                name = [[name componentsSeparatedByCharactersInSet:AllowedChars] componentsJoinedByString:@""];
                NSString *iap = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 3)] password:@"SGmanKindWin5SG"];
                iap = [[iap componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
                NSString *has = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 4)] password:@"SGmanKindWin5SG"];
                has = [[has componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];

                a.assetId = sqlite3_column_int(statement, 0);
                a.name = name;
                a.keywords = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 2)];
                a.iap = iap;
                a.hash = has;
                a.modifiedDate = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 5)];
                a.type = sqlite3_column_int(statement, 6);
                a.boneCount = sqlite3_column_int(statement, 7);
                a.price = [self getAssetPrice:a.iap];
                [array addObject:a];
            }
            sqlite3_finalize(statement);
        }
        
        return array;
    }
}

-(NSString*)getAssetPrice:(NSString*)iap
{
    NSCharacterSet *notAllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789. "] invertedSet];
    
    NSString *encryptedIap = [Utility encryptString:iap password:@"SGmanKindWin5SG"];
    encryptedIap = [[encryptedIap componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];

    @synchronized (dbLock) {
    NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = \"%@\"", ASSET_PRICE_TABLE, ASSET_IAP, encryptedIap];
    sqlite3_stmt *statement;
        const char* errMsg;
    if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, &errMsg) == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
        NSString *assetPrice = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)];
        sqlite3_finalize(statement);
        return assetPrice;
    } else {
        // Asset is free or it's iap is invalid
        sqlite3_finalize(statement);
        return @"BUY";
    }
        sqlite3_free((char*)errMsg);
    }
}

- (AssetItem*) GetAsset:(int)assetId {
    @synchronized (dbLock) {
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", TABLE_ASSET_INFO, ASSET_ID, assetId];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
            
            
            NSCharacterSet *notAllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"] invertedSet];
            NSCharacterSet *AllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789. "] invertedSet];
            
            NSString *name = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)] password:@"SGmanKindWin5SG"];
            name = [[name componentsSeparatedByCharactersInSet:AllowedChars] componentsJoinedByString:@""];
            NSString *iap = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 3)] password:@"SGmanKindWin5SG"];
            iap = [[iap componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
            NSString *has = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 4)] password:@"SGmanKindWin5SG"];
            has = [[has componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
            
            AssetItem* a = [[AssetItem alloc] init];
            a.assetId = sqlite3_column_int(statement, 0);
            a.name = name;
            a.keywords = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 2)];
            a.iap = iap;
            a.hash = has;
            a.modifiedDate = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 5)];
            a.type = sqlite3_column_int(statement, 6);
            a.boneCount = sqlite3_column_int(statement, 7);
            a.price = [self getAssetPrice:a.iap];
            
            sqlite3_finalize(statement);
            return a;
        }
        return nil;
    }
}

- (AssetItem*) GetAssetByName:(NSString*)searchName {
    @synchronized (dbLock) {
        NSString *assetName  = [Utility encryptString:searchName password:@"SGmanKindWin5SG"];
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = '%@'", TABLE_ASSET_INFO, ASSET_NAME, assetName];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {

            NSCharacterSet *notAllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"] invertedSet];
            NSCharacterSet *AllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789. "] invertedSet];
            
            NSString *name = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)] password:@"SGmanKindWin5SG"];
            name = [[name componentsSeparatedByCharactersInSet:AllowedChars] componentsJoinedByString:@""];
            NSString *iap = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 3)] password:@"SGmanKindWin5SG"];
            iap = [[iap componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
            NSString *has = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 4)] password:@"SGmanKindWin5SG"];
            has = [[has componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
            
            AssetItem* a = [[AssetItem alloc] init];
            a.assetId = sqlite3_column_int(statement, 0);
            a.name = name;
            a.keywords = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 2)];
            a.iap = iap;
            a.hash = has;
            a.modifiedDate = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 5)];
            a.type = sqlite3_column_int(statement, 6);
            a.boneCount = sqlite3_column_int(statement, 7);
            a.price = [self getAssetPrice:a.iap];
            
            sqlite3_finalize(statement);
            return a;
        }
        return nil;
    }
}

- (AssetItem*) GetAssetItem:(NSString*)assetIap {
    @synchronized (dbLock) {
        NSString *iap = [Utility encryptString:assetIap password:@"SGmanKindWin5SG"];
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = '%s'", TABLE_ASSET_INFO, ASSET_IAP,  [iap UTF8String]];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
            
            NSCharacterSet *notAllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789"] invertedSet];
            NSCharacterSet *AllowedChars = [[NSCharacterSet characterSetWithCharactersInString:@"0abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ123456789. "] invertedSet];
            
            NSString *name = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)] password:@"SGmanKindWin5SG"];
            name = [[name componentsSeparatedByCharactersInSet:AllowedChars] componentsJoinedByString:@""];
            NSString *iap = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 3)] password:@"SGmanKindWin5SG"];
            iap = [[iap componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
            NSString *has = [Utility decryptString:[NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 4)] password:@"SGmanKindWin5SG"];
            has = [[has componentsSeparatedByCharactersInSet:notAllowedChars] componentsJoinedByString:@""];
            
            AssetItem* a = [[AssetItem alloc] init];
            a.assetId = sqlite3_column_int(statement, 0);
            a.name = name;
            a.keywords = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 2)];
            a.iap = iap;
            a.hash = has;
            a.modifiedDate = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 5)];
            a.type = sqlite3_column_int(statement, 6);
            a.boneCount = sqlite3_column_int(statement, 7);
            a.price = [self getAssetPrice:a.iap];
            sqlite3_finalize(statement);
            return a;
        }
        return nil;
    }
}

static const NSString* TABLE_SCENE_INFO = @"scene_info";

static const NSString* SCENE_ID = @"scene_id";
static const NSString* SCENE_NAME = @"scene_name";
static const NSString* SCENE_DATE = @"scene_date";
static const NSString* SCENE_FILE = @"scene_file";

- (void) UpdateScene:(SceneItem *)s {
    @synchronized (dbLock) {
        sqlite3_stmt *statement;
        NSString *querySQL = [NSString stringWithFormat: @"UPDATE %@ SET %@ = \"%@\", %@ = \"%@\", %@ = \"%@\" WHERE %@ = %d", TABLE_SCENE_INFO, SCENE_NAME, s.name, SCENE_DATE, s.createdDate, SCENE_FILE, s.sceneFile, SCENE_ID, s.sceneId];
        
        sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
        if (sqlite3_step(statement) != SQLITE_DONE){
        }
        sqlite3_finalize(statement);
    }
}

- (bool) AddScene:(SceneItem *)s {
    @synchronized (dbLock) {
        NSString* salt = [NSString stringWithFormat:@"%@:SG:%@", s.name, s.createdDate];
        s.sceneFile = [Utility getMD5ForString:salt];
        
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = \"%@\"", TABLE_SCENE_INFO, SCENE_NAME, s.name];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_ROW) {
                return false;
            } else {
                sqlite3_stmt    *statement;
                NSString *querySQL = [NSString stringWithFormat: @"INSERT INTO %@ (%@, %@, %@) VALUES (\"%@\", \"%@\", \"%@\")", TABLE_SCENE_INFO, SCENE_NAME, SCENE_DATE, SCENE_FILE, s.name, s.createdDate, s.sceneFile];
                
                sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
                if (sqlite3_step(statement) != SQLITE_DONE)
                    NSLog(@"Failed Inserting Scene %s", sqlite3_errmsg(_cacheSystem));
                sqlite3_finalize(statement);
            }
            sqlite3_finalize(statement);
        }
    }
    return true;
}

- (void) DeleteScene:(SceneItem *)s {
    @synchronized (dbLock) {
        sqlite3_stmt    *statement;
        NSString *querySQL = [NSString stringWithFormat: @"DELETE FROM %@ WHERE %@ = '%@'", TABLE_SCENE_INFO, SCENE_NAME, s.name];
        sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
        if (sqlite3_step(statement) != SQLITE_DONE){
            NSLog(@"Failed Deleting Scene %s", sqlite3_errmsg(_cacheSystem));
        }
        sqlite3_finalize(statement);
    }
}
- (void) DeleteTableData:(int)assetId {
    @synchronized (dbLock) {
        sqlite3_stmt    *statement;
        NSString *querySQL = [NSString stringWithFormat: @"DELETE FROM %@ WHERE %@ = '%d'", TABLE_ASSET_INFO, ASSET_ID, assetId];
        sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
        if (sqlite3_step(statement) != SQLITE_DONE){
            NSLog(@"Failed Deleting Scene %s", sqlite3_errmsg(_cacheSystem));
        }
        sqlite3_finalize(statement);
    }
}

- (NSMutableArray*) GetSceneList {
    @synchronized (dbLock) {
        NSMutableArray *array = [[NSMutableArray alloc] init];
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@", TABLE_SCENE_INFO];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            while( sqlite3_step(statement) == SQLITE_ROW) {
                SceneItem* s = [[SceneItem alloc] init];
                s.sceneId = sqlite3_column_int(statement, 0);
                s.name = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)];
                s.createdDate = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 2)];
                s.sceneFile = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 3)];
                [array addObject:s];
            }
            sqlite3_finalize(statement);
        }
        
        return array;
    }
}

- (NSMutableArray*) GetSceneList:(NSString*)keyword {
    @synchronized (dbLock) {
        NSMutableArray *array = [[NSMutableArray alloc] init];
        NSString *querySQL = @"";
        
        querySQL = [NSString stringWithFormat: @"SELECT * FROM %@", TABLE_SCENE_INFO];
        
        if([keyword length]) {
            querySQL = [querySQL stringByAppendingFormat:@" WHERE %@ LIKE '%@%s'", SCENE_NAME, [keyword lowercaseString], "%"];
        }
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            while( sqlite3_step(statement) == SQLITE_ROW) {
                SceneItem* s = [[SceneItem alloc] init];
                s.sceneId = sqlite3_column_int(statement, 0);
                s.name = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 1)];
                s.createdDate = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 2)];
                s.sceneFile = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 3)];
                [array addObject:s];
            }
            sqlite3_finalize(statement);
        }
        return array;
    }
}


- (void)dealloc {
    sqlite3_close(_cacheSystem);
}

static const NSString* TABLE_DOWNLOADED_ASSET_INFO = @"downloaded_asset_info";

static const NSString* DOWNLOADED_ASSET_PRIMARYID = @"asset_primaryid";
static const NSString* DOWNLOADED_ASSET_ID = @"asset_id";
static const NSString* DOWNLOADED_ASSET_TYPE = @"asset_type";

- (void) AddDownloadedAsset:(AssetItem*)a {
    @synchronized (dbLock) {
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", TABLE_DOWNLOADED_ASSET_INFO, DOWNLOADED_ASSET_ID, a.assetId];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_ROW) {
            } else {
                sqlite3_stmt	*statement;
                NSString *querySQL = [NSString stringWithFormat: @"INSERT INTO %@ (%@, %@) VALUES (%d, %d)", TABLE_DOWNLOADED_ASSET_INFO, DOWNLOADED_ASSET_ID, DOWNLOADED_ASSET_TYPE, a.assetId, a.type];
                
                sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
                if (sqlite3_step(statement) != SQLITE_DONE)
                    NSLog(@"Failed Inserting Downloaded Asset %s", sqlite3_errmsg(_cacheSystem));
                sqlite3_finalize(statement);
            }
            sqlite3_finalize(statement);
        }
    }
}

- (int) getNextObjAssetId {
    @synchronized (dbLock) {
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ where %@ >= 20000 and %@ < 30000", TABLE_DOWNLOADED_ASSET_INFO,ASSET_ID,ASSET_ID];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            int ObjCount = 0;
            while(sqlite3_step(statement) == SQLITE_ROW)
                ObjCount++;
            
            sqlite3_finalize(statement);
            return ObjCount;
        }
    }
    return -1;
}

- (bool) checkOBJImporterPurchase{
    @synchronized (dbLock) {
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@", OBJ_IMPORTER_TABLE];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK && sqlite3_step(statement) == SQLITE_ROW) {
            
            // get from database
            NSString *uniqueIAPStrInDB = [NSString stringWithFormat:@"%s", sqlite3_column_text(statement, 0)];
            NSString *uniqueIapStr = [NSString stringWithFormat:@"objimporter%@",[self getDBCreationDate]];
            NSString *encryptUniqueIapStr = [Utility encryptString:uniqueIapStr password:@"SGmanKindWin5SG"];
            if([encryptUniqueIapStr isEqualToString:uniqueIAPStrInDB]){
                return true;
            }
        }
    }
    return false;
}

-(bool) checkDownloadedAsset:(int)assetId {
    @synchronized (dbLock) {
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ WHERE %@ = %d", TABLE_DOWNLOADED_ASSET_INFO, DOWNLOADED_ASSET_ID, assetId];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            if (sqlite3_step(statement) == SQLITE_ROW) {
                return true;
            }else
                return false;
        } else
            return false;
    }
    return false;
}

-(void) createTablesForPrice {
    @synchronized (dbLock) {
        char *errMsg;
        NSString* createAssetPriceTable = [NSString stringWithFormat:@"CREATE TABLE %@ (%@ TEXT, %@ TEXT)", ASSET_PRICE_TABLE, ASSET_IAP, ASSET_PRICE];
        if(sqlite3_exec(_cacheSystem, [createAssetPriceTable UTF8String], NULL, NULL, &errMsg) != SQLITE_OK){
            
        }
        sqlite3_free(errMsg);
    }
}

- (void) addOBJImporterColumn{
    NSString *uniqueIapStr = [NSString stringWithFormat:@"objimporter%@",[self getDBCreationDate]];
    NSString *encryptUniqueIapStr = [Utility encryptString:uniqueIapStr password:@"SGmanKindWin5SG"];
    
    @synchronized (dbLock) {
        
        char *errMsg;
        NSString* createOBJImporterTable = [NSString stringWithFormat:@"CREATE TABLE %@ (%@ TEXT)", OBJ_IMPORTER_TABLE, OBJ_IMPORTER_IAP];
        if(sqlite3_exec(_cacheSystem, [createOBJImporterTable UTF8String], NULL, NULL, &errMsg) == SQLITE_OK){
            
            sqlite3_stmt	*statement;
            NSString *querySQL = [NSString stringWithFormat: @"INSERT INTO %@ (%@) VALUES (\"%@\")", OBJ_IMPORTER_TABLE, OBJ_IMPORTER_IAP,encryptUniqueIapStr];
            sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL);
            
            if (sqlite3_step(statement) != SQLITE_DONE)
                NSLog(@"Failed Inserting ObjImporter %s", sqlite3_errmsg(_cacheSystem));
            
            sqlite3_finalize(statement);
        }else {
        }
        sqlite3_free(errMsg);
    }
}

-(NSString*) getDBCreationDate{  
    NSString* dbName = @"iyan3d-2-0.db";
    NSArray* dirPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docsDir = [dirPaths objectAtIndex:0];
    NSString* dbPath = [NSString stringWithFormat:@"%@/Resources",docsDir];
    NSString* databasePath = [[NSString alloc] initWithString: [dbPath stringByAppendingPathComponent:dbName]];
    NSFileManager* filMngr = [NSFileManager defaultManager];
    NSDictionary* attrs = [filMngr attributesOfItemAtPath:databasePath error:nil];
    NSDate *date;
    if (attrs != nil){
        date = (NSDate*)[attrs objectForKey: NSFileCreationDate];
    }
    return [date description];
}

- (int) getNextAutoRigAssetId {
    @synchronized (dbLock) {
        NSString *querySQL = [NSString stringWithFormat: @"SELECT * FROM %@ where %@ >= 40000 and %@ < 60000", TABLE_DOWNLOADED_ASSET_INFO,ASSET_ID,ASSET_ID];
        sqlite3_stmt *statement;
        if (sqlite3_prepare_v2(_cacheSystem, [querySQL UTF8String], -1, &statement, NULL) == SQLITE_OK) {
            int AutoRigCount = 0;
            while(sqlite3_step(statement) == SQLITE_ROW)
                AutoRigCount++;
            
            sqlite3_finalize(statement);
            return AutoRigCount;
        }
    }
    return -1;
}

@end
