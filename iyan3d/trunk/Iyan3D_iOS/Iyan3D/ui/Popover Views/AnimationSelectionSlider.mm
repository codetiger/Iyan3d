//
//  AnimationSelectionSlider.m
//  Iyan3D
//
//  Created by Sankar on 21/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "AnimationSelectionSlider.h"
#import "DownloadTask.h"
#import "AFNetworking.h"
#import "AFHTTPRequestOperation.h"
#define TRENDING 4
#define FEATURED 5
#define TOP_RATED 6
#define MY_ANIMATION 7
#define USER_NAME_ALERT 100
#define CANCEL_BUTTON 0
#define OK_BUTTON 1

@implementation AnimationSelectionSlider

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil withType:(ANIMATION_TYPE)type EditorScene:(SGEditorScene*)editorScene FirstTime:(BOOL)isFirstTime
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        animationType = type;
        editorSceneLocal = editorScene;
        selectedNodeId = editorSceneLocal->selectedNodeId;
        bonecount = editorSceneLocal->nodes[selectedNodeId]->joints.size();
        currentFrame = editorSceneLocal->currentFrame;
        totalFrame = editorSceneLocal->totalFrames;
        isFirstTimeAnimationApplyed = isFirstTime;
        NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        docDirPath = [srcDirPath objectAtIndex:0];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    downloadQueue = [[NSOperationQueue alloc] init];
    [downloadQueue setMaxConcurrentOperationCount:3];
    [self.downloadIndicator setHidden:NO];
    [self.downloadIndicator startAnimating];
    cache = [CacheSystem cacheSystem];
    [self.delegate myAnimation:YES];
    animDownloadQueue = [[NSOperationQueue alloc] init];
    [animDownloadQueue setMaxConcurrentOperationCount:1];
    userid = [[AppHelper getAppHelper] userDefaultsForKey:@"identifierForVendor"];
    [self getAnimationData];
    if([Utility IsPadDevice]){
        [self.animationCollectionView registerNib:[UINib nibWithNibName:@"AnimationSelectionCollectionViewCell" bundle:nil]forCellWithReuseIdentifier:@"CELL"];
    }
    else{
        [self.animationCollectionView registerNib:[UINib nibWithNibName:@"AnimationSelectionCollectionViewCellPhone" bundle:nil]forCellWithReuseIdentifier:@"CELL"];
    }
    self.cancelBtn.layer.cornerRadius = CORNER_RADIUS;
    self.addBtn.layer.cornerRadius = CORNER_RADIUS;
    [self.publishBtn setHidden:YES];
    [self.delegate createDuplicateAssetsForAnimation];
    editorSceneLocal->currentFrame = currentFrame;
    editorSceneLocal->totalFrames = totalFrame;
    editorSceneLocal->updater->setDataForFrame(currentFrame);
    editorSceneLocal->nodes[selectedNodeId]->node->setVisible(false);
    editorSceneLocal->selectMan->unselectObject(selectedNodeId);
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
    [cell.assetImageView setImageInfo:[NSString stringWithFormat:@"%d", assetItem.assetId] forView:(animationCategoryTab == MY_ANIMATION) ? MY_ANIMATION_VIEW : ALL_ANIMATION_VIEW OperationQueue:downloadQueue];    
    
    cell.assetNameLabel.textColor = [UIColor whiteColor];
    cell.assetNameLabel.font=[UIFont fontWithName:@"Helvetica-Bold" size:11];
    if (assetItem.assetId == selectedAssetId) {
        cell.layer.borderWidth = 1.0f;
        cell.layer.borderColor = [UIColor grayColor].CGColor;
    }
    [self.downloadIndicator stopAnimating];
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
                                    [self.delegate myAnimation:YES];
                                    [self.animationCollectionView reloadData];
                                    animationCategoryTab = TRENDING;
                                    selectedAssetId=-1;
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
                                    [self.delegate myAnimation:YES];
                                    animationCategoryTab = FEATURED;
                                    selectedAssetId = -1;
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
                                      [self.delegate myAnimation:YES];
                                      animationCategoryTab = TOP_RATED;
                                      selectedAssetId = -1;
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
                                      [self.delegate myAnimation:YES];
                                      [self.animationCollectionView reloadData];
                                      animationCategoryTab = MY_ANIMATION;
                                      selectedAssetId = -1;
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
    [self applyAnimationKeyToOriginalNode];
    [self.delegate showOrHideLeftView:NO withView:nil];
    [self.view removeFromSuperview];
    [self deallocView];
}

- (IBAction)cancelBtnFunction:(id)sender {
    [_delegate stopPlaying];
    editorSceneLocal->nodes[selectedNodeId]->node->setVisible(true);
    editorSceneLocal->loader->removeObject(editorSceneLocal->nodes.size()-1);
    [self.delegate updateAssetListInScenes];
    if(!isFirstTimeAnimationApplyed){
        editorSceneLocal->totalFrames = totalFrame;
        editorSceneLocal->currentFrame = currentFrame;
        editorSceneLocal->actionMan->switchFrame(editorSceneLocal->currentFrame);
        [self.delegate removeTempAnimation];
    }
    [self.delegate myAnimation:YES];
    [self.delegate showOrHideLeftView:NO withView:nil];
    [self.view removeFromSuperview];
    [self deallocView];
}

- (void)publishBtnaction
{
    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"]){
        UIAlertView* userNameAlert = [[UIAlertView alloc] initWithTitle:@"Display Name" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
        [userNameAlert setAlertViewStyle:UIAlertViewStylePlainTextInput];
        [[userNameAlert textFieldAtIndex:0] setPlaceholder:@"Enter Your Name Here"];
        [[userNameAlert textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
        [userNameAlert setTag:USER_NAME_ALERT];
        [userNameAlert show];
        [[userNameAlert textFieldAtIndex:0] becomeFirstResponder];
    }
    else
    {
        UIAlertView* userNameAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Sign in with any of your accounts to publish the animation." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [userNameAlert show];
    }
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
        selectedAssetId = assetItem.assetId;
        asset = assetItem;
        selectedCell=assetItem.assetId;
        if (assetItem)
            [self downloadAnimation:assetItem];
        if (animationCategoryTab == MY_ANIMATION) {
            if (assetItem.published == 0) {
               [self.delegate myAnimation:NO];
            }
            else
            {
               [self.delegate myAnimation:YES];
            }
        }
        else {
            [self.delegate myAnimation:YES];
        }

    }
}

- (void)downloadAnimation:(AnimationItem*)assetItem
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString *fileName, *url;
    NSString* extension = (animationType == RIG_ANIMATION) ? @"sgra" : @"sgta";
    if (animationCategoryTab == MY_ANIMATION) {
        fileName = [NSString stringWithFormat:@"%@/Resources/Animations/%d.%@", docDirPath, assetItem.assetId, extension];
        if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
            [self performSelectorOnMainThread:@selector(applyAnimation:) withObject:[NSNumber numberWithInt:assetItem.boneCount] waitUntilDone:NO];
            NSLog(@"MY Animation Exits");
        }
        else {
            NSLog(@"File not exists");
            NSLog(@"File Path : %@ ",fileName);
        }
    }
    else {
        fileName = [NSString stringWithFormat:@"%@/%d.%@", cacheDirectory, assetItem.assetId, extension];
        if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
            if (assetItem)
            [self performSelectorOnMainThread:@selector(applyAnimation:) withObject:[NSNumber numberWithInt:assetItem.boneCount] waitUntilDone:NO];
        }
        else {
            [self.delegate showOrHideProgress:1];
            url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/animationFile/%d.%@", assetItem.assetId, extension];
            [animDownloadQueue cancelAllOperations];
            //[self cancelOperations:animDownloadQueue];
            DownloadTask* task = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(applyAnimation:) returnObject:[NSNumber numberWithInt:assetItem.boneCount] outputFilePath:fileName andURL:url];
            task.taskType = DOWNLOAD_AND_RETURN_OBJ;
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
    animationsItems = [cache GetAnimationList:animationType fromTable:4 Search:@""];
    [self.animationCollectionView reloadData];
}

#pragma mark Alertview Delegate

#pragma DelegateFunctions

- (void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (alertView.tag == USER_NAME_ALERT) {
        NSCharacterSet* set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];
        
        if (buttonIndex == OK_BUTTON) {
            NSString* name = [[alertView textFieldAtIndex:0].text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
            if ([name length] == 0) {
                [self.view endEditing:YES];
                UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"User name cannot be empty." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                [errorAlert show];
            }
            else {
                [self.view endEditing:YES];
                if ([name rangeOfCharacterFromSet:set].location != NSNotFound) {
                    UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"User Name cannot contain any special characters." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                    [errorAlert show];
                }
                else {
                    if ([[AppHelper getAppHelper] checkInternetConnected]) {
                        [self.publishBtn setHidden:YES];
                        [self.view setUserInteractionEnabled:NO];
                        [self publishAssetWithUserName:[alertView textFieldAtIndex:0].text];
                    }
                }
            }
        }
    }
}



- (void)publishAssetWithUserName:(NSString*)userName{
    
    if ([asset.userId isEqualToString:@""])
        asset.userId = userid;
    
    if (![userName isEqualToString:@""])
        asset.userName = userName;
    
    if (![asset.userId isEqualToString:@""]) {
        NSString* extension,*uniqueId,*email,*fbid,*fbname,*twitterId,*twitterName;
        NSString* imgPathLocation = [NSString stringWithFormat:@"%@/Resources/Animations/%d.png", docDirPath, selectedCell];
        
        if (animationType == 0)
            extension = @".sgra";
        else
            extension = @".sgta";
        
        NSString* filePathLocation = [NSString stringWithFormat:@"%@/Resources/Animations/%d%@", docDirPath, selectedCell, extension];
        NSString* name = [NSString stringWithFormat:@"%@", asset.assetName];
        NSString* keyword = [NSString stringWithFormat:@"%@", asset.keywords];
        NSString* username = [NSString stringWithFormat:@"%@", asset.userName];
        NSString* type = [NSString stringWithFormat:@"%d", animationType];
            if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"])
                {
                    uniqueId = [NSString stringWithFormat:@"%@", [[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"]];
                    email = [NSString stringWithFormat:@"%@", [[AppHelper getAppHelper] userDefaultsForKey:@"email"]];
                    userName = [NSString stringWithFormat:@"%@", [[AppHelper getAppHelper] userDefaultsForKey:@"username"]];
                }
        
        NSString* asset_id = [NSString stringWithFormat:@"%d", asset.assetId];
        NSString* bonecountanim = [NSString stringWithFormat:@"%d", asset.boneCount];
        NSData* animationFile = [NSData dataWithContentsOfFile:filePathLocation];
        NSData* animationImgFile = [NSData dataWithContentsOfFile:imgPathLocation];
        NSURL* url = [NSURL URLWithString:@"https://www.iyan3dapp.com/appapi/publish.php"];
        NSString* postPath = @"https://www.iyan3dapp.com/appapi/publish.php";
        AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
        NSMutableURLRequest* request = [httpClient multipartFormRequestWithMethod:@"POST"
                                                                             path:postPath
                                                                       parameters:nil
                                                        constructingBodyWithBlock:^(id<AFMultipartFormData> formData) {
                                                            if (animationImgFile != nil)
                                                                [formData appendPartWithFileData:animationImgFile name:@"animationImgFile" fileName:[NSString stringWithFormat:@"%d.png", selectedCell] mimeType:@"image/png"];
                                                            if (animationFile != nil)
                                                                [formData appendPartWithFileData:animationFile name:@"animationFile" fileName:[NSString stringWithFormat:@"%d%@", selectedCell, extension] mimeType:@"image/png"];
                                                            [formData appendPartWithFormData:[userid dataUsingEncoding:NSUTF8StringEncoding] name:@"userid"];
                                                            if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"])
                                                            {
                                                                [formData appendPartWithFormData:[uniqueId dataUsingEncoding:NSUTF8StringEncoding] name:@"uniqueid"];
                                                                [formData appendPartWithFormData:[email dataUsingEncoding:NSUTF8StringEncoding] name:@"email"];
                                                            }
                                                                                                                        
                                                            [formData appendPartWithFormData:[username dataUsingEncoding:NSUTF8StringEncoding] name:@"username"];
                                                            [formData appendPartWithFormData:[name dataUsingEncoding:NSUTF8StringEncoding] name:@"asset_name"];
                                                            [formData appendPartWithFormData:[keyword dataUsingEncoding:NSUTF8StringEncoding] name:@"keyword"];
                                                            [formData appendPartWithFormData:[bonecountanim dataUsingEncoding:NSUTF8StringEncoding] name:@"bonecount"];
                                                            [formData appendPartWithFormData:[asset_id dataUsingEncoding:NSUTF8StringEncoding] name:@"asset_id"];
                                                            [formData appendPartWithFormData:[type dataUsingEncoding:NSUTF8StringEncoding] name:@"type"];
                                                        }];
        
        AFHTTPRequestOperation* operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
        
        __block BOOL complete = NO;
        [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation* operation, id responseObject) {
            //ret = [self handle:data];
            complete = YES;
            asset.published = [[operation responseString] intValue];
            NSLog(@"Publishid : %d",asset.published);
            [cache UpdateMyAnimation:asset];
            [self.view setUserInteractionEnabled:YES];
            if ([animationsItems containsObject:asset]) {
                int indexRow = (int)[animationsItems indexOfObject:asset];
                [self displayBasedOnSelection:[NSNumber numberWithInt:indexRow]];
                [self performSelectorOnMainThread:@selector(reloadCollectionView) withObject:nil waitUntilDone:YES];
            }
        } failure:^(AFHTTPRequestOperation* operation, NSError* error) {
            NSLog(@"Failure: %@", error);
            [self.view setUserInteractionEnabled:YES];
            [self.view endEditing:YES];
            UIAlertView* userNameAlert = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to publish, Please check your network settings." delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
            [userNameAlert show];
            complete = YES;
        }];
        [operation start];
    }

        
}
    
- (void)reloadCollectionView
{
    [self.animationCollectionView reloadData];
}
#pragma mark OpenGl related Functions

- (void)applyAnimationKeyToOriginalNode{
    [_delegate stopPlaying];
    editorSceneLocal->nodes[selectedNodeId]->node->setVisible(true);
    editorSceneLocal->animMan->copyKeysOfNode((int)editorSceneLocal->nodes.size()-1, selectedNodeId);
    editorSceneLocal->loader->removeObject(editorSceneLocal->nodes.size()-1);
    [self.delegate updateAssetListInScenes];
    editorSceneLocal->selectMan->selectObject(selectedNodeId,false);
    editorSceneLocal->actionMan->storeAddOrRemoveAssetAction(ACTION_APPLY_ANIM, 0);
    editorSceneLocal->updater->setDataForFrame(currentFrame);
}

- (void)applyAnimation:(id)returnValue
{
    [_delegate stopPlaying];
    editorSceneLocal->loader->removeObject(editorSceneLocal->nodes.size()-1);
    [self.delegate updateAssetListInScenes];
    editorSceneLocal->selectMan->selectObject(selectedNodeId,false);
    [self.delegate createDuplicateAssetsForAnimation];
    if(!isFirstTimeAnimationApplyed) {
        editorSceneLocal->currentFrame = currentFrame;
        editorSceneLocal->totalFrames = totalFrame;
        editorSceneLocal->actionMan->switchFrame(editorSceneLocal->currentFrame);
        [self.delegate removeTempAnimation];
    }
    int animBoneCount = [returnValue intValue];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString *fileName;
    NSString* extension = (animationType == RIG_ANIMATION) ? @"sgra" : @"sgta";
    if(animationCategoryTab == MY_ANIMATION)
        fileName = [NSString stringWithFormat:@"%@/Resources/Animations/%d.%@", docDirPath, selectedAssetId, extension];
    else
        fileName = [NSString stringWithFormat:@"%@/%d.%@", cacheDirectory, selectedAssetId, extension];

    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        isFirstTimeAnimationApplyed = false;
        if(animationType == RIG_ANIMATION){
            if(animBoneCount == bonecount){
                [self.delegate applyAnimationToSelectedNode:fileName SelectedNodeId:selectedNodeId SelectedFrame:currentFrame];
            }
            else{
                UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Bone count cannot match." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
                [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
            }
        }
        else
            [self.delegate applyAnimationToSelectedNode:fileName SelectedNodeId:selectedNodeId SelectedFrame:currentFrame];
    }
   [self.delegate showOrHideProgress:0];
}


- (void)deallocView
{
    [downloadQueue cancelAllOperations];
    downloadQueue = nil;
    cache = nil;
    self.delegate=nil;
    animationJsonArray = nil;
    animationsItems = nil;
    animDownloadQueue = nil;
    jsonUserArray = nil;
    docDirPath = nil;
    editorSceneLocal = nil;
    _delegate = nil;
}

@end
