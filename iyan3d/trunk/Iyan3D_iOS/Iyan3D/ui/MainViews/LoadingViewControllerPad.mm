//
//  LoadingViewControllerPad.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 04/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "LoadingViewControllerPad.h"
#import "SceneSelectionViewControllerPad.h"
#import "EditorViewController.h"
#import "AppDelegate.h"
#import "Utility.h"
#import "Constants.h"
#import "AppHelper.h"
#import "DownloadTask.h"

#define INITIAL_PROGRESS 0.01f
#define PROGRESS_INCREMENT 0.05f
#define PROGRESS_COMPLETED 1.0f
#define JSON_DATA_DOWNLOADED 0.60f
#define JSON_DECRYPTED 0.70f
#define ASSET_INFO_STORED 0.65f
#define ASSET_DOWNLOAD_BEGIN 0.80F
#define ASSETS_DOWNLOADED 0.95


@implementation LoadingViewControllerPad

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        progress = INITIAL_PROGRESS;
        maxProgress = INITIAL_PROGRESS;
        NSString* dbName = @"iyan3d-2-0.db";
        NSString* projectFolder = @"Projects";
        NSArray* dirPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* docsDir = [dirPaths objectAtIndex:0];
        NSString* databasePath = [[NSString alloc] initWithString:[docsDir stringByAppendingPathComponent:dbName]];
        NSFileManager *fileManager = [NSFileManager defaultManager];
        NSArray* cachepaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* cacheDirectory = [cachepaths objectAtIndex:0];
        NSString *texture = [cacheDirectory stringByAppendingPathComponent:@"32553-cm.png"];
        NSString *sgrFile = [cacheDirectory stringByAppendingPathComponent:@"32553.sgr"];

        isAppFirstTime = false;
        NSString* projectFolderPath = [[NSString alloc] initWithString:[docsDir stringByAppendingPathComponent:projectFolder]];
        NSString *projectDir = [docsDir stringByAppendingPathComponent:@"/Projects"];
        NSString *resourcesDir = [docsDir stringByAppendingPathComponent:@"/Resources"];
        NSString *rigs = [resourcesDir stringByAppendingPathComponent:@"/Objs"];
        NSString *objs = [resourcesDir stringByAppendingPathComponent:@"/Rigs"];
        NSString *anims = [resourcesDir stringByAppendingPathComponent:@"/Animations"];

        if (![fileManager fileExistsAtPath:projectFolderPath]) {
            [[NSFileManager defaultManager] createDirectoryAtPath:projectDir withIntermediateDirectories:NO attributes:nil error:nil];
            [[NSFileManager defaultManager] createDirectoryAtPath:resourcesDir withIntermediateDirectories:NO attributes:nil error:nil];
            [[NSFileManager defaultManager] createDirectoryAtPath:rigs withIntermediateDirectories:NO attributes:nil error:nil];
            [[NSFileManager defaultManager] createDirectoryAtPath:objs withIntermediateDirectories:NO attributes:nil error:nil];
            [[NSFileManager defaultManager] createDirectoryAtPath:anims withIntermediateDirectories:NO attributes:nil error:nil];
            
            if (![[NSFileManager defaultManager] fileExistsAtPath:databasePath]) {
                isAppFirstTime = true;
                
            }else{
                NSFileManager *fm = [NSFileManager defaultManager];
                NSString* resourceDirPath = [NSString stringWithFormat:@"%@/Resources",docsDir];
                NSString* projectDirPath = [NSString stringWithFormat:@"%@/Projects",docsDir];
                NSString* rigDirPath = [NSString stringWithFormat:@"%@/Resources/Rigs",docsDir];
                NSString* objDirPath = [NSString stringWithFormat:@"%@/Resources/Objs",docsDir];
                [fm moveItemAtPath:[docsDir stringByAppendingPathComponent:dbName] toPath:[resourceDirPath stringByAppendingPathComponent:dbName] error:nil];
                NSArray *dirFiles = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:docsDir error:nil];
                // Move existing sgb files in documents directory to Projects directory
                sceneFiles = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.sgb'"]];
                for (NSString *fileName in sceneFiles) {
                    NSArray *subStrings = [fileName componentsSeparatedByString:@"."];
                    NSString* sceneSGBFile = [NSString stringWithFormat:@"%@.sgb",[subStrings objectAtIndex:0]];
                    NSString* sceneTexture = [NSString stringWithFormat:@"%@.png",[subStrings objectAtIndex:0]];
                    [fm moveItemAtPath:[docsDir stringByAppendingPathComponent:sceneSGBFile] toPath:[projectDirPath stringByAppendingPathComponent:sceneSGBFile] error:nil];
                    if([fm fileExistsAtPath:[docsDir stringByAppendingPathComponent:sceneTexture]])
                    [fm moveItemAtPath:[docsDir stringByAppendingPathComponent:sceneTexture] toPath:[projectDirPath stringByAppendingPathComponent:sceneTexture] error:nil];
                }
                // Move existing obj files in caches directory to Obj directory in app Documents
                NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
                NSString* cachesDir = [paths objectAtIndex:0];
                NSArray *cacheDirFiles = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:cachesDir error:nil];
                objFiles = [cacheDirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self BEGINSWITH '2'"]];
                for (NSString *fileName in objFiles) {
                    NSArray *fileNameArray = [fileName componentsSeparatedByString:@"."];
                    NSString *file =[fileNameArray objectAtIndex:0];
                    if([file length] == 5){ // without extension
                        if([self isFullyInteger:file] && ([file intValue] >= 20000) && ([file intValue] < 30000)){
                            [fm moveItemAtPath:[cachesDir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.png",file]] toPath:[objDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.png",file]] error:nil];
                            [fm moveItemAtPath:[cachesDir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.obj",file]] toPath:[objDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.obj",file]] error:nil];
                            [fm moveItemAtPath:[cachesDir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@-cm.png",file]] toPath:[objDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@-cm.png",file]] error:nil];
                        }
                    }
                }
                // Move existing sgr files (made by user) in caches directory to Rigs directory
                rigFiles = [cacheDirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self BEGINSWITH '4'"]];
                for (NSString *fileName in rigFiles) {
                    NSArray *fileNameArray = [fileName componentsSeparatedByString:@"."];
                    NSString *file =[fileNameArray objectAtIndex:0];
                    if([file length] == 5){ // without extension
                        if([self isFullyInteger:file] && ([file intValue] >= 40000) && ([file intValue] < 50000)){
                            [fm moveItemAtPath:[cachesDir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.sgr",file]] toPath:[rigDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.sgr",file]] error:nil];
                            [fm moveItemAtPath:[cachesDir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.png",file]] toPath:[rigDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.png",file]] error:nil];
                            [fm moveItemAtPath:[cachesDir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@-cm.png",file]] toPath:[rigDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@-cm.png",file]] error:nil];
                        }
                    }
                }
            }
        }
        
        NSError *error;
        NSString *thumnail = [projectDir stringByAppendingPathComponent:@"1c8ccd62ec29cc2fb116ecc6892cbab2.png"];
        NSString *sgbPath = [projectDir stringByAppendingPathComponent:@"1c8ccd62ec29cc2fb116ecc6892cbab2.sgb"];
        
        // Copy sample project files
        if (![fileManager fileExistsAtPath:sgbPath])
        {
            NSString *resourcePath1 = [[NSBundle mainBundle] pathForResource:@"1c8ccd62ec29cc2fb116ecc6892cbab2" ofType:@"png"];
            [fileManager copyItemAtPath:resourcePath1 toPath:thumnail error:&error];
            NSString *resourcePath2 = [[NSBundle mainBundle] pathForResource:@"1c8ccd62ec29cc2fb116ecc6892cbab2" ofType:@"sgb"];
            [fileManager copyItemAtPath:resourcePath2 toPath:sgbPath error:&error];
            NSString *resourcePath3 = [[NSBundle mainBundle] pathForResource:@"32553-cm" ofType:@"png"];
            if([fileManager fileExistsAtPath:resourcePath3]){
                [fileManager copyItemAtPath:resourcePath3 toPath:texture error:&error];
                NSString *resourcePath4 = [[NSBundle mainBundle] pathForResource:@"32553" ofType:@"sgr"];
                [fileManager copyItemAtPath:resourcePath4 toPath:sgrFile error:&error];
            }
        }
        NSString *resourcePath3 = [[NSBundle mainBundle] pathForResource:@"32553-cm" ofType:@"png"];
        NSString *resourcePath4 = [[NSBundle mainBundle] pathForResource:@"32553" ofType:@"sgr"];
        if([fileManager fileExistsAtPath:resourcePath3] && ![fileManager fileExistsAtPath:texture]) {
            NSString *resourcePath3 = [[NSBundle mainBundle] pathForResource:@"32553-cm" ofType:@"png"];
            [fileManager copyItemAtPath:resourcePath3 toPath:texture error:nil];
        }
        if([fileManager fileExistsAtPath:resourcePath4]&& ![fileManager fileExistsAtPath:sgrFile]){
            NSString *resourcePath4 = [[NSBundle mainBundle] pathForResource:@"32553" ofType:@"sgr"];
            [fileManager copyItemAtPath:resourcePath4 toPath:sgrFile error:nil];
        }

        NSString* animationFolderPath = [[NSString alloc] initWithString:[docsDir stringByAppendingPathComponent:anims]];
        if (![fileManager fileExistsAtPath:animationFolderPath]) {
            [[NSFileManager defaultManager] createDirectoryAtPath:anims withIntermediateDirectories:NO attributes:nil error:nil];
        }
        
        cache = [CacheSystem cacheSystem];
        priceFormatter = [[NSNumberFormatter alloc] init];
        [priceFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        [priceFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
        
    }
    return self;
}
-(bool) isFullyInteger:(NSString*)str {
    bool isFullyIntegers = true;
    char* nameArr = (char*)[str UTF8String];
    for(int i = 0;i<[str length];i++){
        int num = nameArr[i];
        if(!(num >= 48 && num <= 57))
            isFullyIntegers  = false;
    }
    return isFullyIntegers;
}
- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}
- (void)introMovieFinished:(NSNotification *)notification
{
    if(maxProgress == PROGRESS_COMPLETED)
    [self performSelectorOnMainThread:@selector(loadSceneView) withObject:nil waitUntilDone:NO];
}
- (void) viewDidLoad
{
    [super viewDidLoad];
    self.screenName = @"LoadingView";
    
    [cache OpenDatabase];
    [cache createTablesForPrice];
    [cache createRenderTaskTables];
    [cache createAnimationTables];
    [self performSelectorInBackground:@selector(progressTimerSelector) withObject:nil];
    [self performSelectorInBackground:@selector(performBackgroundTasks) withObject:nil];

    [[AppHelper getAppHelper] setIdentifierForVendor];


}
-(void)progressTimerSelector
{
    if(progress < maxProgress)
        progress += PROGRESS_INCREMENT;
    
    if(progress >= PROGRESS_COMPLETED) {
        [self performSelectorOnMainThread:@selector(loadSceneView) withObject:nil waitUntilDone:NO];
    }
    else {
        [NSThread sleepForTimeInterval:(PROGRESS_INCREMENT+0.05)];
        [self performSelectorInBackground:@selector(progressTimerSelector) withObject:nil];
    }
    [self performSelectorOnMainThread:@selector(setProgressStatus) withObject:nil waitUntilDone:NO];

}
- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (void) performBackgroundTasks
{
    
    [[AppHelper getAppHelper] downloadJsonData];
    
    maxProgress = JSON_DATA_DOWNLOADED;
    maxProgress = JSON_DECRYPTED;
    if(![[AppHelper getAppHelper] userDefaultsForKey:@"APP_VERSION"]){
        [[AppHelper getAppHelper] saveBoolUserDefaults:[cache checkOBJImporterPurchase] withKey:@"premiumUnlocked"];
    }else if([[[AppHelper getAppHelper] userDefaultsForKey:@"APP_VERSION"] isEqualToString:@"4.0"]){
        [[AppHelper getAppHelper] saveBoolUserDefaults:[cache checkOBJImporterPurchase] withKey:@"premiumUnlocked"];
    }
    if(![[AppHelper getAppHelper] userDefaultsForKey:@"APP_VERSION"]) {
        [cache insertSampleScene];
        [[AppHelper getAppHelper] saveToUserDefaults:@"4.1" withKey:@"APP_VERSION"];
        
    }else if([[AppHelper getAppHelper] userDefaultsForKey:@"APP_VERSION"]) {
        [[AppHelper getAppHelper] saveToUserDefaults:@"4.1" withKey:@"APP_VERSION"];
    }
    
    //TODO :
    
     //[[AppHelper getAppHelper] saveBoolUserDefaults:true withKey:@"premiumUnlocked"];
    
    if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"])
       [[AppHelper getAppHelper] initializeFontListArray];
    
    maxProgress = ASSET_DOWNLOAD_BEGIN;
    
    NSArray* cachepaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [cachepaths objectAtIndex:0];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0]; // Get documents folder
    NSString *dataPath = [cacheDirectory stringByAppendingPathComponent:@"/texureFile"];
    
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:dataPath])
        [[NSFileManager defaultManager] createDirectoryAtPath:dataPath withIntermediateDirectories:NO attributes:nil error:nil]; //Create folder
    NSArray *extensions = [NSArray arrayWithObjects:@"png", @"jpeg", @"jpg", @"JPEG", @"PNG", nil];
    
    
    NSArray *dirFiles = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:documentsDirectory error:nil];
    if([dirFiles count] > 0)
    textureFile = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
    
    for(int i = 0 ; i< [textureFile count] ; i++)
    {
        NSString* srcFilePath = [NSString stringWithFormat:@"%@/%@",documentsDirectory,textureFile[i]];
        NSString* desFilePathForDisplay = [NSString stringWithFormat:@"%@/%@",dataPath,textureFile[i]];
        NSData *imageDataforDisplay = [self convertAndScaleImage:[UIImage imageWithContentsOfFile:srcFilePath] size:64];
        [imageDataforDisplay writeToFile:desFilePathForDisplay atomically:YES];
    }
    maxProgress = ASSETS_DOWNLOADED;
    //[[AppHelper getAppHelper] moveFontFilesIfNeccasary];
    //[self downloadMissingAssets];
    maxProgress = PROGRESS_COMPLETED;
}
- (NSData*) convertAndScaleImage:(UIImage*)image size:(int)textureRes
{
    float target = 0;
    target = (float)textureRes;
    
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(target, target), NO, 1.0);
    [image drawInRect:CGRectMake(0, 0, target, target)];
    UIImage* nImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    NSData* data = UIImagePNGRepresentation(nImage);
    return data;
}

//Crash Removed unused functions

- (void) setProgressStatus
{
    [self.progressView setProgress:progress];
}

- (void) loadSceneView
{
    
  
    
    if([Utility IsPadDevice]) {
        EditorViewController* editorView = [[EditorViewController alloc] initWithNibName:@"EditorViewController" bundle:nil];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:editorView];
    } else {
        EditorViewController* editorView = [[EditorViewController alloc] initWithNibName:@"EditorViewControllerPhone" bundle:nil];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:editorView];
    }
    [self deallocMemory];
}

//Crash Removed unused methods

-(void)changeProgressLabel:(NSString*)labelString
{
    self.ProgessLabel.text = labelString;
}

- (void) deallocMemory
{
	cache = nil;
    jsonArr = nil;
    priceFormatter = nil;
    _allProducts = nil;
    _allAssets = nil;
    textureFile = nil;
}

@end
