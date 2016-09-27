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

#define MY_ANIMATION 7

#define CANCEL_BUTTON 0
#define OK_BUTTON 1

#define RENAME_ALERT 0
#define USER_NAME_ALERT 1
#define SIGNIN_ALERT 2

#define CANCEL 0
#define OK 1

@implementation AnimationSelectionSlider

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil withType:(ANIMATION_TYPE)type EditorScene:(SGEditorScene*)editorScene FirstTime:(BOOL)isFirstTime ScreenWidth:(int)screenWidth ScreenHeight:(int)screenHeight
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        animationType = type;
        self.tableType = animationCategoryTab = MY_ANIMATION;
        editorSceneLocal = editorScene;
        selectedNodeId = editorSceneLocal->selectedNodeId;
        bonecount = (int)editorSceneLocal->nodes[selectedNodeId]->joints.size();
        currentFrame = editorSceneLocal->currentFrame;
        totalFrame = editorSceneLocal->totalFrames;
        isFirstTimeAnimationApplyed = isFirstTime;
        NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        docDirPath = [srcDirPath objectAtIndex:0];
        ScreenWidth =screenWidth;
        ScreenHeight = screenHeight;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.screenName = @"AnimationSelection iOS";
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
    cache = [CacheSystem cacheSystem];
    animationsItems = [cache GetAnimationList:animationType fromTable:self.tableType Search:@""];
    [self.animationCollectionView reloadData];    
    [self.delegate myAnimation:YES];
    
    userid = [[AppHelper getAppHelper] userDefaultsForKey:@"identifierForVendor"];
    
    UITapGestureRecognizer* tapGest = [[UITapGestureRecognizer alloc] initWithTarget:self action:nil];
    tapGest.delegate = self;
    [self.view addGestureRecognizer:tapGest];
    
    [self.addBtn setTitle:NSLocalizedString(@"ADD", nil) forState:UIControlStateNormal];
    [self.cancelBtn setTitle:NSLocalizedString(@"CANCEL", nil) forState:UIControlStateNormal];
    [self.categoryBtn setTitle:NSLocalizedString(@"My Animations", nil) forState:UIControlStateNormal];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
    return NO;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

#pragma mark Collection View delegate methods

-(NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
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
    cell.category = animationCategoryTab;
    if(animationCategoryTab != MY_ANIMATION)
        [cell.propsBtn setHidden:YES];
    else
        [cell.propsBtn setHidden:NO];
    
    cell.selectedIndex = (int)indexPath.row;
    cell.parentVC = self;
    cell.backgroundColor = [UIColor clearColor];
    [cell.assetImageView setImageInfo:[NSString stringWithFormat:@"%d", assetItem.assetId] forView:MY_ANIMATION_VIEW OperationQueue:nil ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
    
    cell.assetNameLabel.textColor = [UIColor whiteColor];
    cell.assetNameLabel.font=[UIFont fontWithName:@"Helvetica-Bold" size:11];
    if (assetItem.assetId == selectedAssetId) {
        cell.layer.borderWidth = 1.0f;
        cell.layer.borderColor = [UIColor grayColor].CGColor;
    }
    return cell;
}

- (void)collectionView:(UICollectionView*)collectionView didSelectItemAtIndexPath:(NSIndexPath*)indexPath
{
    [self.delegate showOrHideProgress:1];
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

#pragma mark AssetCellView Delegate methods

- (void) deleteAnimationAtIndex:(int) indexVal
{
    if(animationCategoryTab != MY_ANIMATION)
        return;
    
    NSFileManager *fm = [NSFileManager defaultManager];
    AnimationItem *a = animationsItems[indexVal];
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirectory = [docPaths objectAtIndex:0];
    NSString* extension;
    if (animationType == 0)
        extension = @".sgra";
    else
        extension = @".sgta";

    NSString* imagePath = [NSString stringWithFormat:@"%@/Resources/Animations/%d.png", docDirectory, a.assetId];
    NSString* animPath = [NSString stringWithFormat:@"%@/Resources/Animations/%d%@", docDirectory, a.assetId, extension];
    if([fm fileExistsAtPath:imagePath])
        [fm removeItemAtPath:imagePath error:nil];
    if([fm fileExistsAtPath:animPath])
        [fm removeItemAtPath:animPath error:nil];
    
    [cache deleteMyAnimation:a.assetId];
    [animationsItems removeObject:a];
    [self.animationCollectionView reloadData];
}

- (void) cloneAnimation:(int) indexVal
{
    
    if(animationCategoryTab != MY_ANIMATION)
        return;
    
    NSFileManager *fm = [NSFileManager defaultManager];
    AnimationItem *a = animationsItems[indexVal];
    
    int assetId = ANIMATIONS_ID + [cache getNextAnimationAssetId];

    bool status = false;
    
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirectory = [docPaths objectAtIndex:0];
    NSString* extension;
    if (animationType == 0)
        extension = @".sgra";
    else
        extension = @".sgta";

    NSString* imagePath = [NSString stringWithFormat:@"%@/Resources/Animations/%d.png", docDirectory, a.assetId];
    NSString* animPath = [NSString stringWithFormat:@"%@/Resources/Animations/%d%@", docDirectory, a.assetId, extension];
    
    NSString* newImgPath = [NSString stringWithFormat:@"%@/Resources/Animations/%d.png", docDirectory, assetId];
    NSString* newAnimPath = [NSString stringWithFormat:@"%@/Resources/Animations/%d%@", docDirectory, assetId, extension];

    if([fm fileExistsAtPath:imagePath])
        [fm copyItemAtPath:imagePath toPath:newImgPath error:nil];
    if([fm fileExistsAtPath:animPath]) {
        status = true;
        [fm copyItemAtPath:animPath toPath:newAnimPath error:nil];
    }

    
    if (status) {
        AnimationItem* animItem = [[AnimationItem alloc] init];
        animItem.assetId = assetId;
        animItem.assetName = [NSString stringWithFormat:@"%@copy",a.assetName];
        animItem.published = 0;
        animItem.rating = 0;
        animItem.boneCount = a.boneCount;
        animItem.isViewerRated = 0;
        animItem.type = (int)animationType;
        animItem.userId = [[AppHelper getAppHelper] userDefaultsForKey:@"identifierForVendor"];
        animItem.userName = @"anonymous";
        animItem.keywords = [NSString stringWithFormat:@" %@", animItem.assetName];
        animItem.modifiedDate = @"";
        [cache UpdateMyAnimation:animItem];
    }
    
    
    [self openMyAnimations];
}

- (void) renameAnimation:(int) indexVal
{
    if(animationCategoryTab != MY_ANIMATION)
        return;
    
    AnimationItem *a = animationsItems[indexVal];

    UIAlertView* renameScene = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Rename Animation", nil) message:NSLocalizedString(@"Please enter a name", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"Ok", nil), nil];
    [renameScene setAlertViewStyle:UIAlertViewStylePlainTextInput];
    [[renameScene textFieldAtIndex:0] setPlaceholder:a.assetName];
    [[renameScene textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
    [renameScene setTag:RENAME_ALERT];
    [renameScene setAccessibilityIdentifier:[NSString stringWithFormat:@"%d", indexVal]];
    [renameScene show];
    [[renameScene textFieldAtIndex:0] becomeFirstResponder];
}

#pragma mark Button actions

- (IBAction)categoryBtnFuction:(id)sender
{
    UIAlertController * view=   [UIAlertController
                                 alertControllerWithTitle:NSLocalizedString(@"Category", nil)
                                 message:nil
                                 preferredStyle:UIAlertControllerStyleActionSheet];
    
    UIAlertAction* myanimation = [UIAlertAction
                                  actionWithTitle:NSLocalizedString(@"My Animations", nil)
                                  style:UIAlertActionStyleDefault
                                  handler:^(UIAlertAction * action)
                                  {
                                      if(animationCategoryTab == MY_ANIMATION)
                                          return;
                                      [self openMyAnimations];
                                      [view dismissViewControllerAnimated:YES completion:nil];

                                  }];
    switch (animationCategoryTab) {
        case MY_ANIMATION:
            [myanimation setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
    }
    
    [view addAction:myanimation];
    UIPopoverPresentationController *popover = view.popoverPresentationController;
    popover.sourceView = self.categoryBtn;
    
    popover.permittedArrowDirections = UIPopoverArrowDirectionUp;
    if(view != nil)
        [self presentViewController:view animated:YES completion:nil];
}

- (void) openMyAnimations
{
    [_delegate showOrHideProgress:1];
    [self.categoryBtn setTitle:NSLocalizedString(@"My Animations", nil) forState:UIControlStateNormal];
    animationsItems = [cache GetAnimationList:animationType fromTable:MY_ANIMATION Search:@""];
    [self.delegate myAnimation:YES];
    [self.animationCollectionView reloadData];
    animationCategoryTab = MY_ANIMATION;
    selectedAssetId = -1;
    [_delegate showOrHideProgress:0];
}

- (IBAction)addBtnFunction:(id)sender
{
    [self applyAnimationKeyToOriginalNode];
    [self.delegate myAnimation:YES];
    [self.delegate showOrHideLeftView:NO withView:nil];
    editorSceneLocal->loader->removeTempNodeIfExists();
    [self.view removeFromSuperview];
    [self.delegate reloadFrames];
    [self.delegate deallocSubViews];
    [self deallocView];
}

- (IBAction)cancelBtnFunction:(id)sender
{
    [self.delegate showOrHideProgress:0];
    [_delegate stopPlaying];
    editorSceneLocal->nodes[selectedNodeId]->node->setVisible(true);
    editorSceneLocal->loader->removeObject(editorSceneLocal->nodes.size()-1);
    editorSceneLocal->loader->removeTempNodeIfExists();
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
    [self.delegate reloadFrames];
    [self.delegate deallocSubViews];
    [self deallocView];
}

- (void) setSelectedAnimationAtIndex:(int)indexVal
{
    asset = animationsItems[indexVal];
}

#pragma mark animations related data methods

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
    NSString *fileName;
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

- (BOOL)proceedToFileVerification:(id)object
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
        return true;
    }
    else {
        [self.view endEditing:YES];
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Connection Error", nil) message:NSLocalizedString(@"unable_connect_server", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
        return false;
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
    [self performSelectorInBackground:@selector(refreshDataInBackground:) withObject:allAnimations];
}

- (void) refreshDataInBackground:(NSArray*) allAnimations
{
    [AppHelper getAppHelper].delegate = nil;
    if (allAnimations != nil && allAnimations.count > 0)
        animationJsonArray = [NSMutableArray arrayWithArray:allAnimations];
    if (animationJsonArray != nil && [animationJsonArray count] > 0)
        [self storeDataToLocalDB];
    animationsItems = [cache GetAnimationList:animationType fromTable:self.tableType Search:@""];
    [self performSelectorOnMainThread:@selector(reloadCollectionView) withObject:nil waitUntilDone:NO];
}

- (void) reloadCollectionView
{
    [self.downloadIndicator stopAnimating];
    [self.downloadIndicator setHidden:YES];
    [self.animationCollectionView reloadData];
}

#pragma mark Alertview Delegate

#pragma DelegateFunctions

- (void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    
    switch (alertView.tag) {
        case USER_NAME_ALERT: {
            NSCharacterSet* set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];
            
            if (buttonIndex == OK_BUTTON) {
                NSString* name = [[alertView textFieldAtIndex:0].text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
                if ([name length] == 0) {
                    [self.view endEditing:YES];
                    UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"username_isempty", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil];
                    [errorAlert show];
                    [self.publishBtn setHidden:NO];
                    
                }
                else {
                    [self.view endEditing:YES];
                    if ([name rangeOfCharacterFromSet:set].location != NSNotFound) {
                        UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"username_has_special_char", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil];
                        [errorAlert show];
                        [self.publishBtn setHidden:NO];
                    }
                    else {
                    }
                }
            }
        }
            
        case RENAME_ALERT: {
            NSCharacterSet* set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];
            
            if (buttonIndex == CANCEL) {
            }
            else if (buttonIndex == OK) {
                NSString* name = [[alertView textFieldAtIndex:0].text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
                if ([name length] == 0) {
                    [self.view endEditing:YES];
                    UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"Animation name cannot be empty.", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil];
                    [errorAlert show];
                }
                else
                {
                    if ([name rangeOfCharacterFromSet:set].location != NSNotFound) {
                        UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"Animation_Name_special", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil];
                        [errorAlert show];
                    }
                    else
                    {
                        int indexVal = [[alertView accessibilityIdentifier] intValue];
                        AnimationItem *a = animationsItems[indexVal];
                        a.assetName = name;
                        a.keywords = [NSString stringWithFormat:@" %@", name];
                        [cache UpdateMyAnimation:a];
                        [self openMyAnimations];
                    }
                }
            }
            [alertView resignFirstResponder];
            
            break;
        }
            
        case SIGNIN_ALERT: {
            break;
        }
        default:
            break;
    }

}

- (void)publishAssetWithUserName:(NSString*)userName
{
    
    if ([asset.userId isEqualToString:@""])
        asset.userId = userid;
    
    if (![userName isEqualToString:@""])
        asset.userName = userName;
    
    if (![asset.userId isEqualToString:@""]) {
        NSString* extension, *uniqueId, *email;
        NSString* imgPathLocation = [NSString stringWithFormat:@"%@/Resources/Animations/%d.png", docDirPath, asset.assetId];
        
        if (animationType == 0)
            extension = @".sgra";
        else
            extension = @".sgta";
        
        NSString* filePathLocation = [NSString stringWithFormat:@"%@/Resources/Animations/%d%@", docDirPath, asset.assetId, extension];
        
        NSLog(@"\nAnimation File Locations : Image %@ \n Animation %@ \n" , imgPathLocation,filePathLocation);
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
            else{
                uniqueId =  @"99999999999999";
                email = @"anonymous@mail.com";
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
                                                                [formData appendPartWithFileData:animationImgFile name:@"animationImgFile" fileName:[NSString stringWithFormat:@"%d.png", asset.assetId] mimeType:@"image/png"];
                                                            if (animationFile != nil)
                                                                [formData appendPartWithFileData:animationFile name:@"animationFile" fileName:[NSString stringWithFormat:@"%d%@", asset.assetId, extension] mimeType:@"image/png"];
                                                            [formData appendPartWithFormData:[userid dataUsingEncoding:NSUTF8StringEncoding] name:@"userid"];
                                                                [formData appendPartWithFormData:[uniqueId dataUsingEncoding:NSUTF8StringEncoding] name:@"uniqueId"];
                                                                [formData appendPartWithFormData:[email dataUsingEncoding:NSUTF8StringEncoding] name:@"email"];
                                                            [formData appendPartWithFormData:[username dataUsingEncoding:NSUTF8StringEncoding] name:@"username"];
                                                            [formData appendPartWithFormData:[name dataUsingEncoding:NSUTF8StringEncoding] name:@"asset_name"];
                                                            [formData appendPartWithFormData:[keyword dataUsingEncoding:NSUTF8StringEncoding] name:@"keyword"];
                                                            [formData appendPartWithFormData:[bonecountanim dataUsingEncoding:NSUTF8StringEncoding] name:@"bonecount"];
                                                            [formData appendPartWithFormData:[asset_id dataUsingEncoding:NSUTF8StringEncoding] name:@"asset_id"];
                                                            [formData appendPartWithFormData:[type dataUsingEncoding:NSUTF8StringEncoding] name:@"type"];
                                                            NSLog(@" name %@ animationImgFile %@ ", name, [NSString stringWithFormat:@"%d.png", asset.assetId]);
                                                        }];
        
        AFHTTPRequestOperation* operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
        __block BOOL complete = NO;
        [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation* operation, id responseObject) {

            UIAlertView* userNameAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Success", nil) message:NSLocalizedString(@"success_animation_publish", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil];
            [userNameAlert show];
            
            complete = YES;
            asset.published = [[operation responseString] intValue];
            NSLog(@"Publishid : %d",asset.published);
            if(asset.published > 0) {
                [cache UpdateMyAnimation:asset];
                [self.view setUserInteractionEnabled:YES];
                if ([animationsItems containsObject:asset]) {
                    int indexRow = (int)[animationsItems indexOfObject:asset];
                    [self displayBasedOnSelection:[NSNumber numberWithInt:indexRow]];
                    [self performSelectorOnMainThread:@selector(reloadCollectionView) withObject:nil waitUntilDone:YES];
                    [_publishBtn setHidden:YES];
                }
            }
                                    [self.delegate showOrHideProgress:0];
        } failure:^(AFHTTPRequestOperation* operation, NSError* error) {
            NSLog(@"Failure: %@", error);
            [self.view setUserInteractionEnabled:YES];
            [self.view endEditing:YES];
            UIAlertView* userNameAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Connection Error", nil) message:NSLocalizedString(@"unable_connect_server", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
            [userNameAlert show];
            complete = YES;
            [_publishBtn setHidden:NO];
                                    [self.delegate showOrHideProgress:0];
        }];
        [operation start];
    }
}
    
- (void)hideOrShowPublishBtn:(NSNumber*)value
{
    [_publishBtn setHidden:[value boolValue]];
}

#pragma mark OpenGl related Functions

- (void)applyAnimationKeyToOriginalNode
{
    [_delegate stopPlaying];
    editorSceneLocal->nodes[selectedNodeId]->node->setVisible(true);
    editorSceneLocal->animMan->copyKeysOfNode((int)editorSceneLocal->nodes.size()-1, selectedNodeId);
    editorSceneLocal->animMan->copyPropsOfNode((int)editorSceneLocal->nodes.size()-1, selectedNodeId, true);
    editorSceneLocal->loader->removeObject(editorSceneLocal->nodes.size()-1);
    [self.delegate updateAssetListInScenes];
    editorSceneLocal->selectMan->selectObject(selectedNodeId, NOT_SELECTED, false);
    editorSceneLocal->actionMan->storeAddOrRemoveAssetAction(ACTION_APPLY_ANIM, 0);
    editorSceneLocal->updater->setDataForFrame(currentFrame);
}

- (void)applyAnimation:(id)returnValue
{
    [_delegate stopPlaying];
    
    int animBoneCount = [returnValue intValue];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString *fileName;
    NSString* extension = (animationType == RIG_ANIMATION) ? @"sgra" : @"sgta";
    if(animationCategoryTab == MY_ANIMATION)
        fileName = [NSString stringWithFormat:@"%@/Resources/Animations/%d.%@", docDirPath, selectedAssetId, extension];
    else
        fileName = [NSString stringWithFormat:@"%@/%d.%@", cacheDirectory, selectedAssetId, extension];

    if(animBoneCount != bonecount && animationType != TEXT_ANIMATION) {
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Information", nil) message:NSLocalizedString(@"bone_count_mismatch", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
        [self.delegate showOrHideProgress:0];
        return;
    }
    if(!editorSceneLocal || !editorSceneLocal->loader)
        return;
    
    editorSceneLocal->loader->removeObject(editorSceneLocal->nodes.size()-1);
    [self.delegate updateAssetListInScenes];
    editorSceneLocal->selectMan->selectObject(selectedNodeId, NOT_SELECTED, false);
    [self.delegate createDuplicateAssetsForAnimation];
    if(!isFirstTimeAnimationApplyed) {
        editorSceneLocal->currentFrame = currentFrame;
        editorSceneLocal->totalFrames = totalFrame;
        editorSceneLocal->actionMan->switchFrame(editorSceneLocal->currentFrame);
        [self.delegate removeTempAnimation];
    }
    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        isFirstTimeAnimationApplyed = false;
        if(animationType == RIG_ANIMATION || animationType == TEXT_ANIMATION){
            if(animBoneCount == bonecount || animationType == TEXT_ANIMATION){
                [self.delegate applyAnimationToSelectedNode:fileName SelectedNodeId:selectedNodeId SelectedFrame:currentFrame];
            } else {
                UIAlertView* message = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Information", nil) message:NSLocalizedString(@"bone_count_mismatch", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
                [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
            }
        }
        else {
            NSLog(@" \n Animation file not exists %@ ", fileName);
        }
            //[self.delegate applyAnimationToSelectedNode:fileName SelectedNodeId:selectedNodeId SelectedFrame:currentFrame];
    }
   [self.delegate showOrHideProgress:0];
}

- (void)deallocView
{
    cache = nil;
    self.delegate=nil;
    animationJsonArray = nil;
    animationsItems = nil;
    jsonUserArray = nil;
    docDirPath = nil;
    editorSceneLocal = nil;
    _delegate = nil;
}

@end
