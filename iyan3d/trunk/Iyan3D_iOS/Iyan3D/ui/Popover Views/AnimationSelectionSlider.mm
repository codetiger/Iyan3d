//
//  AnimationSelectionSlider.m
//  Iyan3D
//
//  Created by Sankar on 21/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "AnimationSelectionSlider.h"
#import "DownloadTask.h"

#define TRENDING 4
#define FEATURED 5
#define TOP_RATED 6
#define MY_ANIMATION 7

@implementation AnimationSelectionSlider

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil EditorScene:(SGEditorScene*)editorScene FirstTime:(BOOL)isFirstTime
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        editorSceneLocal = editorScene;
        selectedNodeId = editorSceneLocal->selectedNodeId;
        bonecount = editorSceneLocal->nodes[selectedNodeId]->joints.size();
        currentFrame = editorSceneLocal->currentFrame;
        totalFrame = editorSceneLocal->totalFrames;
        isFirstTimeEntered = isFirstTime;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    downloadQueue = [[NSOperationQueue alloc] init];
    [downloadQueue setMaxConcurrentOperationCount:3];
    [self.downloadIndicator setHidden:NO];
    cache = [CacheSystem cacheSystem];
    animDownloadQueue = [[NSOperationQueue alloc] init];
    [animDownloadQueue setMaxConcurrentOperationCount:1];
    [self getAnimationData];
    if([Utility IsPadDevice]){
        [self.animationCollectionView registerNib:[UINib nibWithNibName:@"AnimationSelectionCollectionViewCell" bundle:nil]forCellWithReuseIdentifier:@"CELL"];
    }
    else{
        [self.animationCollectionView registerNib:[UINib nibWithNibName:@"AnimationSelectionCollectionViewCellPhone" bundle:nil]forCellWithReuseIdentifier:@"CELL"];
    }
    self.cancelBtn.layer.cornerRadius = 8.0f;
    self.addBtn.layer.cornerRadius = 8.0f;

    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark Collection View delegate methods

-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    NSLog(@"Collectionview count %lu",(unsigned long)[animationsItems count]);
    return [animationsItems count];
}

- (AnimationSelectionCollectionViewCell*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    AnimationItem* assetItem = animationsItems[indexPath.row];
    AnimationSelectionCollectionViewCell* cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
    cell.layer.borderWidth = 1.0f;
    cell.layer.borderColor = [UIColor clearColor].CGColor;
    cell.assetNameLabel.text = assetItem.assetName;
    [cell.assetNameLabel adjustsFontSizeToFitWidth];
    cell.backgroundColor = [UIColor clearColor];
    [cell.assetImageView setImageInfo:[NSString stringWithFormat:@"%d", assetItem.assetId] forView:5 OperationQueue:downloadQueue];
    
    cell.assetNameLabel.textColor = [UIColor whiteColor];
    cell.assetNameLabel.font=[UIFont fontWithName:@"Helvetica-Bold" size:11];
    if (assetItem.assetId == selectedCell) {
        cell.layer.borderWidth = 1.0f;
        cell.layer.borderColor = [UIColor grayColor].CGColor;
    }
    [self.downloadIndicator setHidden:YES];
    return cell;
}

- (void)collectionView:(UICollectionView*)collectionView didSelectItemAtIndexPath:(NSIndexPath*)indexPath
{
    NSArray* indexPathArr = [collectionView indexPathsForVisibleItems];
    for (int i = 0; i < [indexPathArr count]; i++) {
        NSIndexPath* indexPath = [indexPathArr objectAtIndex:i];
        UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
        cell.backgroundColor = [UIColor clearColor];
        cell.layer.borderWidth = 1.0f;
        cell.layer.borderColor = [UIColor clearColor].CGColor;
    }
    UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
    cell.layer.borderWidth = 1.0f;
    cell.layer.borderColor = [UIColor whiteColor].CGColor;
    cell.layer.cornerRadius = 8.0;
    [_delegate stopPlaying];
    if(!isFirstTimeEntered)
    [self.delegate removeTempAnimation];
    editorSceneLocal->selectMan->selectObject(selectedNodeId);
    [self displayBasedOnSelection:[NSNumber numberWithInteger:indexPath.row]];

}
#pragma mark Button actions

- (IBAction)categoryBtnFuction:(id)sender
{
    UIAlertController * view=   [UIAlertController
                                 alertControllerWithTitle:@"Category"
                                 message:nil
                                 preferredStyle:UIAlertControllerStyleActionSheet];
    UIAlertAction* trending = [UIAlertAction
                                actionWithTitle:@"Trending"
                                style:UIAlertActionStyleDefault
                                handler:^(UIAlertAction * action)
                                {
                                    if(animationCategoryTab == TRENDING)
                                        return;
                                    [self.categoryBtn setTitle: @"Trending" forState:UIControlStateNormal];
                                    animationsItems = [cache GetAnimationList:0 fromTable:4 Search:@""];
                                    [self.animationCollectionView reloadData];
                                    animationCategoryTab = TRENDING;
                                    selectedCell=0;
                                    [view dismissViewControllerAnimated:YES completion:nil];
                                    
                                }];
    UIAlertAction* featured = [UIAlertAction
                                actionWithTitle:@"Featured"
                                style:UIAlertActionStyleDefault
                                handler:^(UIAlertAction * action)
                                {
                                    if(animationCategoryTab == FEATURED)
                                        return;
                                    animationsItems = [cache GetAnimationList:0 fromTable:6 Search:@""];
                                    [self.categoryBtn setTitle:@"Featured" forState:UIControlStateNormal];
                                    [self.animationCollectionView reloadData];

                                    animationCategoryTab = FEATURED;
                                    selectedCell = -1;
                                    [view dismissViewControllerAnimated:YES completion:nil];
                                    
                                }];
    
    UIAlertAction* toprated = [UIAlertAction
                                  actionWithTitle:@"Top Rated"
                                  style:UIAlertActionStyleDefault
                                  handler:^(UIAlertAction * action)
                                  {
                                      if(animationCategoryTab == TOP_RATED)
                                          return;
                                      [self.categoryBtn setTitle: @"Top Rated" forState:UIControlStateNormal];
                                      animationsItems = [cache GetAnimationList:0 fromTable:5 Search:@""];
                                      [self.categoryBtn setTitle:@"Top Rated" forState:UIControlStateNormal];
                                      [self.animationCollectionView reloadData];
                                      animationCategoryTab = TOP_RATED;
                                      selectedCell = -1;
                                      [view dismissViewControllerAnimated:YES completion:nil];
                                      
                                  }];
    
    UIAlertAction* myanimation = [UIAlertAction
                                  actionWithTitle:@"My Animations"
                                  style:UIAlertActionStyleDefault
                                  handler:^(UIAlertAction * action)
                                  {
                                      if(animationCategoryTab == MY_ANIMATION)
                                          return;
                                      [self.categoryBtn setTitle: @"My Animations" forState:UIControlStateNormal];
                                      animationsItems = [cache GetAnimationList:0 fromTable:7 Search:@""];
                                      [self.animationCollectionView reloadData];
                                      animationCategoryTab = MY_ANIMATION;
                                      selectedCell = -1;
                                      [view dismissViewControllerAnimated:YES completion:nil];
                                      
                                  }];
    switch (animationCategoryTab) {
        case TRENDING:
            [trending setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
        case TOP_RATED:
            [toprated setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
        case FEATURED:
            [featured setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
        case MY_ANIMATION:
            [myanimation setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
    }
    
    [view addAction:trending];
    [view addAction:toprated];
    [view addAction:featured];
    [view addAction:myanimation];
    UIPopoverPresentationController *popover = view.popoverPresentationController;
    popover.sourceView = self.categoryBtn;
    
    popover.permittedArrowDirections = UIPopoverArrowDirectionUp;
    [self presentViewController:view animated:YES completion:nil];
}

- (IBAction)addBtnFunction:(id)sender {
    
}

- (IBAction)cancelBtnFunction:(id)sender {


    editorSceneLocal->currentFrame = currentFrame;
    editorSceneLocal->totalFrames = totalFrame;
    if(!isFirstTimeEntered)
        [self.delegate removeTempAnimation];
    [self.delegate showOrHideLeftView:NO withView:nil];
    [self deallocView];
    [self.view removeFromSuperview];
}

#pragma mark animations related data methods

- (void)getAnimationData
{
    animationJsonArray = [[NSMutableArray alloc] init];
    [AppHelper getAppHelper].delegate = self;
    if ([[AppHelper getAppHelper] checkInternetConnected]) {
        [[AppHelper getAppHelper] performReadingJsonInQueue:downloadQueue ForPage:5];
       
    }
}

- (void)displayBasedOnSelection:(NSNumber*)rowIndex
{
    if ([animationsItems count] > [rowIndex intValue]) {
        AnimationItem* assetItem = animationsItems[[rowIndex intValue]];
        selectedCell = assetItem.assetId;
        _assetId = assetItem.assetId;
        if (assetItem)
            [self downloadAnimation:assetItem];
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
            isFirstTimeEntered = false;
            [self.delegate applyAnimationToSelectedNode:fileName];
        }
        else {
            [self.delegate showOrHideProgress:1];
            url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/animationFile/%d.%@", assetItem.assetId, extension];
            //[animDownloadQueue cancelAllOperations];
            [self cancelOperations:animDownloadQueue];
            DownloadTask* task = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(loadNode:) returnObject:[NSNumber numberWithInt:assetItem.assetId] outputFilePath:fileName andURL:url];
            task.queuePriority = NSOperationQueuePriorityHigh;
            [animDownloadQueue addOperation:task];
        }
    }    
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
        
        NSString* alternatePath = (tabValue == 7) ? cacheDirectory : animDirPath;
        filePath = [NSString stringWithFormat:@"%@/%@", alternatePath, fileName];
    }
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:filePath]) {
        NSLog(@"Download Completed");
    }
    else {
        [self.view endEditing:YES];
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to connect to the server, Please check your network settings." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}



- (void) cancelOperations:(NSOperationQueue*) queue
{
    [queue cancelAllOperations];
    
    for (DownloadTask *task in [queue operations]) {
        [task cancel];
    }
}


# pragma mark AppHelper delegates

-(void)loadingViewStatus:(BOOL)status
{
    
}

-(void)statusForOBJImport:(NSNumber*)object
{
    
}

-(void)premiumUnlocked
{
    
}

-(void)addRestoreId:(NSString*)productIdentifier
{
    
}

-(void)statusForRestorePurchase:(NSNumber *)object
{
    
}
-(void)transactionCancelled
{
    
}

- (void)setAnimationData:(NSArray*)allAnimations
{
    [AppHelper getAppHelper].delegate = nil;
    if (allAnimations != nil && allAnimations.count > 0)
        animationJsonArray = [NSMutableArray arrayWithArray:allAnimations];
    
    if (animationJsonArray != nil && [animationJsonArray count] > 0)
        [self storeDataToLocalDB];
    
    animationsItems = [cache GetAnimationList:0 fromTable:4 Search:@""];
    [self.animationCollectionView reloadData];
}


#pragma mark OpenGl related Functions

- (void)loadNode:(id)returnValue
{
    NSLog(@"Download Completed");
   [self.delegate showOrHideProgress:0];
}


- (void)deallocView
{
    [downloadQueue cancelAllOperations];
    downloadQueue = nil;
    cache = nil;
    self.delegate=nil;
}

@end
