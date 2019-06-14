//
//  LoadingViewControllerPad.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 04/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "LoadingViewControllerPad.h"
#import "SceneSelectionControllerNew.h"
#import "EditorViewController.h"
#import "AppDelegate.h"
#import "Utility.h"
#import "Constants.h"
#import "DownloadTask.h"

#define APP_VERSION @"5.3"


@implementation LoadingViewControllerPad

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        startTime = [NSDate date];
        
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
        NSString *sgm = [resourcesDir stringByAppendingPathComponent:@"/Sgm"];
        NSString *textures = [resourcesDir stringByAppendingPathComponent:@"/Textures"];
        NSString *videos = [resourcesDir stringByAppendingPathComponent:@"/Videos"];
        NSString *anims = [resourcesDir stringByAppendingPathComponent:@"/Animations"];
        NSString *fonts = [resourcesDir stringByAppendingPathComponent:@"/Fonts"];

        if (![fileManager fileExistsAtPath:projectFolderPath]) {
            [[NSFileManager defaultManager] createDirectoryAtPath:projectDir withIntermediateDirectories:NO attributes:nil error:nil];
            [[NSFileManager defaultManager] createDirectoryAtPath:resourcesDir withIntermediateDirectories:NO attributes:nil error:nil];
            [[NSFileManager defaultManager] createDirectoryAtPath:rigs withIntermediateDirectories:NO attributes:nil error:nil];
            [[NSFileManager defaultManager] createDirectoryAtPath:objs withIntermediateDirectories:NO attributes:nil error:nil];
            [[NSFileManager defaultManager] createDirectoryAtPath:anims withIntermediateDirectories:NO attributes:nil error:nil];
            [[NSFileManager defaultManager] createDirectoryAtPath:fonts withIntermediateDirectories:NO attributes:nil error:nil];
            
            [self copyFontFilesToDocumentsDirectory:docsDir];
            
            if (![[NSFileManager defaultManager] fileExistsAtPath:databasePath]) {
                isAppFirstTime = true;
            } else {
                NSFileManager *fm = [NSFileManager defaultManager];
                NSString* resourceDirPath = [NSString stringWithFormat:@"%@/Resources",docsDir];
                NSString* projectDirPath = [NSString stringWithFormat:@"%@/Projects",docsDir];
                NSString* rigDirPath = [NSString stringWithFormat:@"%@/Resources/Rigs",docsDir];
                NSString* objDirPath = [NSString stringWithFormat:@"%@/Resources/Objs",docsDir];
                [fm moveItemAtPath:[docsDir stringByAppendingPathComponent:dbName] toPath:[resourceDirPath stringByAppendingPathComponent:dbName] error:nil];
                NSArray *dirFiles = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:docsDir error:nil];
                // Move existing sgb files in documents directory to Projects directory
                
                NSArray *sceneFiles = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.sgb'"]];
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
                NSArray *objFiles = [cacheDirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self BEGINSWITH '2'"]];
                for (NSString *fileName in objFiles) {
                    NSArray *fileNameArray = [fileName componentsSeparatedByString:@"."];
                    NSString *file =[fileNameArray objectAtIndex:0];
                    if([file length] == 5) { // without extension
                        if([self isFullyInteger:file] && ([file intValue] >= 20000) && ([file intValue] < 30000)){
                            [fm moveItemAtPath:[cachesDir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.png",file]] toPath:[objDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.png",file]] error:nil];
                            [fm moveItemAtPath:[cachesDir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.obj",file]] toPath:[objDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@.obj",file]] error:nil];
                            [fm moveItemAtPath:[cachesDir stringByAppendingPathComponent:[NSString stringWithFormat:@"%@-cm.png",file]] toPath:[objDirPath stringByAppendingPathComponent:[NSString stringWithFormat:@"%@-cm.png",file]] error:nil];
                        }
                    }
                }
                // Move existing sgr files (made by user) in caches directory to Rigs directory
                NSArray *rigFiles = [cacheDirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self BEGINSWITH '4'"]];
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
        
        if(![fileManager fileExistsAtPath:textures])
            [[NSFileManager defaultManager] createDirectoryAtPath:textures withIntermediateDirectories:NO attributes:nil error:nil];
        if(![fileManager fileExistsAtPath:fonts])
            [[NSFileManager defaultManager] createDirectoryAtPath:fonts withIntermediateDirectories:NO attributes:nil error:nil];
        if(![fileManager fileExistsAtPath:sgm])
            [[NSFileManager defaultManager] createDirectoryAtPath:sgm withIntermediateDirectories:NO attributes:nil error:nil];
        if(![fileManager fileExistsAtPath:videos])
            [[NSFileManager defaultManager] createDirectoryAtPath:videos withIntermediateDirectories:NO attributes:nil error:nil];

        NSArray* basicShapes = [NSArray arrayWithObjects:@"60001",@"60002",@"60003",@"60004",@"60005",@"60006",nil];
        /*
         CONE = 60001
         CUBE = 60002
         CYLINDER = 60003
         PLANE = 60004
         SPHERE = 60005
         TORUS = 60006
         */        
        for(int i = 0; i < basicShapes.count; i++){
            NSString *mesh = [[NSBundle mainBundle] pathForResource:[basicShapes objectAtIndex:i] ofType:@"sgm"];
            NSString *fileName = [NSString stringWithFormat:@"%@.sgm",[basicShapes objectAtIndex:i]];
            NSString *sgmFile = [sgm stringByAppendingPathComponent:fileName];
            if([fileManager fileExistsAtPath:mesh]&& ![fileManager fileExistsAtPath:sgmFile]){
                [fileManager copyItemAtPath:mesh toPath:sgmFile error:nil];
            }
        }
        
        NSString *whiteTexFrom = [[NSBundle mainBundle] pathForResource:@"White-texture" ofType:@"png"];
        NSString *fileName = @"White-texture.png";
        NSString *whiteTexTo = [textures stringByAppendingPathComponent:fileName];
        if([fileManager fileExistsAtPath:whiteTexFrom]&& ![fileManager fileExistsAtPath:whiteTexTo]){
            [fileManager copyItemAtPath:whiteTexFrom toPath:whiteTexTo error:nil];
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
        [cache OpenDatabase];
        [cache createTablesForPrice];
        [cache createRenderTaskTables];
        [cache createAnimationTables];
        [cache checkAndCreateGroupColumnInAssetsTable];

        [AppHelper getAppHelper].delegate = self;
        [[AppHelper getAppHelper] downloadJsonData];
        [[AppHelper getAppHelper] moveFilesFromInboxDirectory:cache];
        [[AppHelper getAppHelper] setIdentifierForVendor];
        
    }
    return self;
}

- (void) copyFontFilesToDocumentsDirectory:(NSString*) destinationDir
{
    NSArray* extensions = [NSArray arrayWithObjects:@"ttf", @"otf", nil];
    NSString* sourceDir = [[NSBundle mainBundle] bundlePath];
    
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

-(bool) isFullyInteger:(NSString*)str
{
    bool isFullyIntegers = true;
    char* nameArr = (char*)[str UTF8String];
    for(int i = 0;i<[str length];i++) {
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

- (void) viewDidLoad
{
    [super viewDidLoad];
    self.screenName = @"LoadingView iOS";
}

- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (void) performLocalTasks
{
    NSArray* cachepaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [cachepaths objectAtIndex:0];
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0]; // Get documents folder
    NSString *dataPath = [cacheDirectory stringByAppendingPathComponent:@"/texureFile"];
    
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:dataPath])
        [[NSFileManager defaultManager] createDirectoryAtPath:dataPath withIntermediateDirectories:NO attributes:nil error:nil];

    NSArray *extensions = [NSArray arrayWithObjects:@"png", @"PNG", @"jpeg", @"JPEG", @"jpg", @"JPG", nil];
   
    NSArray *dirFiles = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:documentsDirectory error:nil];

    NSArray *textureFile;
    if([dirFiles count] > 0)
        textureFile = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
    
    for(int i = 0 ; i< [textureFile count] ; i++) {
        NSString* srcFilePath = [NSString stringWithFormat:@"%@/%@",documentsDirectory,textureFile[i]];
        NSString* desFilePathForDisplay = [NSString stringWithFormat:@"%@/%@",dataPath,textureFile[i]];
        NSData *imageDataforDisplay = [self convertAndScaleImage:[UIImage imageWithContentsOfFile:srcFilePath] size:64];
        [imageDataforDisplay writeToFile:desFilePathForDisplay atomically:YES];
    }
    [self performSelectorOnMainThread:@selector(loadSceneView) withObject:nil waitUntilDone:NO];
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

- (void) loadSceneView
{    
    if([Utility IsPadDevice]) {
        SceneSelectionControllerNew* sceneSelectionView = [[SceneSelectionControllerNew alloc] initWithNibName:@"SceneSelectionControllerNew" bundle:nil IsFirstTimeOpen:YES];
        sceneSelectionView.fromLoadingView = true;
        sceneSelectionView.isAppFirstTime = isAppFirstTime;
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:sceneSelectionView];
    } else {
        SceneSelectionControllerNew* sceneSelectionView = [[SceneSelectionControllerNew alloc] initWithNibName:([self iPhone6Plus]) ?  @"SceneSelectionControllerNewPhone@2x" : @"SceneSelectionControllerNewPhone" bundle:nil IsFirstTimeOpen:YES];
        sceneSelectionView.fromLoadingView = true;
        sceneSelectionView.isAppFirstTime = isAppFirstTime;
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:sceneSelectionView];
    }
    
    [self deallocMemory];
}

-(BOOL)iPhone6Plus
{
    if (([UIScreen mainScreen].scale > 2.0))
        return YES;
    
    return NO;
}

- (void) deallocMemory
{
	cache = nil;
    jsonArr = nil;
    [AppHelper getAppHelper].delegate = nil;
}

@end
