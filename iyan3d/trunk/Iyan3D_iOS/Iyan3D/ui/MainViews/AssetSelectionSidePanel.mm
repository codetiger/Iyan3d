//
//  AssetSelectionSidePanel.m
//  Iyan3D
//
//  Created by sabish on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "AssetSelectionSidePanel.h"
#import "AssetFrameCell.h"
#import "Utility.h"
#import "DownloadTask.h"
#import "SceneSelectionControllerNew.h"
#import "AppDelegate.h"

#define RESTORING 11
#define LOAD_NODE 22
#define DOWNLOAD_NODE 33
#define CLOSE_CURRENT_VIEW 10
#define OBJFile 6
#define IMPORT_MODELS 0
#define IMPORT_PARTICLE 13


@implementation AssetSelectionSidePanel

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil Type:(int)type
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if(self){
        screenScale = [[UIScreen mainScreen] scale];
        cache = [CacheSystem cacheSystem];
        viewType = type;
        assetArray = [cache GetAssetList:(viewType == IMPORT_MODELS) ? ALLMODELS : IMPORT_PARTICLE  Search:@""];
        NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        docDirPath = [srcDirPath objectAtIndex:0];
        downloadQueue = [[NSOperationQueue alloc]init];
        [downloadQueue setMaxConcurrentOperationCount:3];
        assetDownloadQueue = [[NSOperationQueue alloc]init];
        [assetDownloadQueue setMaxConcurrentOperationCount:1];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    if([Utility IsPadDevice]){
       [self.assetsCollectionView registerNib:[UINib nibWithNibName:@"AssetFrameCell" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    else{
        [self.assetsCollectionView registerNib:[UINib nibWithNibName:@"AssetFrameCellPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    self.cancelBtn.layer.cornerRadius = CORNER_RADIUS;
    self.addToSceneBtn.layer.cornerRadius = CORNER_RADIUS;
    selectedAsset = -1;
    modelCategoryTab = 0;
    addToScenePressed = false;
    [_addToSceneBtn setEnabled:YES];
    [_modelCategory setHidden:(viewType == IMPORT_MODELS) ? NO : YES];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

#pragma Button Actions

- (IBAction)cancelButtonAction:(id)sender {
    [self.assetSelectionDelegate removeTempNodeFromScene];
    [self.assetSelectionDelegate showOrHideLeftView:NO withView:nil];
    [self.assetSelectionDelegate showOrHideProgress:0];
    [self.view removeFromSuperview];
    [self deallocMem];
}

- (IBAction)addToSceneButtonAction:(id)sender {
     addToScenePressed = YES;
    [self downloadAsset:[cache GetAsset:selectedAsset] ForActivity:LOAD_NODE isTempAsset:false];
    [self.assetSelectionDelegate showOrHideLeftView:NO withView:nil];
    [self.assetSelectionDelegate showOrHideProgress:0];
    [self.view removeFromSuperview];
    [self deallocMem];
}

#pragma mark CollectionView Delegate methods

- (NSInteger)collectionView:(UICollectionView*)collectionView numberOfItemsInSection:(NSInteger)section
{
    return [assetArray count];
}

- (AssetFrameCell*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    if ([assetArray count] > indexPath.row) {
        AssetItem* assetItem = assetArray[indexPath.row];
        AssetFrameCell* cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
        if(assetItem.assetId == selectedAsset){
            cell.layer.borderWidth = 1.0f;
            cell.layer.borderColor = [UIColor whiteColor].CGColor;
        }
        else{
            cell.layer.borderWidth = 0.0f;
            cell.layer.borderColor = [UIColor clearColor].CGColor;
        }
        cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
       
        cell.assetName.text = assetItem.name;
        [cell.assetImage setImageInfo:[NSString stringWithFormat:@"%d", assetItem.assetId] forView:ASSET_SELECTION OperationQueue:downloadQueue];
        return cell;
    }
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
        [_addToSceneBtn setEnabled:NO];
    NSArray* indexPathArr = [collectionView indexPathsForVisibleItems];
   
    for (int i = 0; i < [indexPathArr count]; i++) {
        NSIndexPath* indexPath = [indexPathArr objectAtIndex:i];
        UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
        cell.layer.borderWidth = 0.0f;
        cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
        cell.layer.borderColor = [UIColor colorWithRed:15.0/255.0 green:15.0/255.0 blue:15.0/255.0 alpha:1.0].CGColor;
    }
    UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
    cell.layer.backgroundColor = [UIColor colorWithRed:71.0/255.0 green:71.0/255.0 blue:71.0/255.0 alpha:1.0].CGColor;
    [self displayBasedOnSelection:[NSNumber numberWithInteger:indexPath.row]];
}

- (void)displayBasedOnSelection:(NSNumber*)rowIndex
{
    if (!self.isViewLoaded)
        return;
    
    int selectedIndex = [rowIndex intValue];
    if (selectedIndex >= 0 && selectedIndex < [assetArray count]) {
        AssetItem* assetItem = assetArray[selectedIndex];
        if (assetItem == nil)
            return;
        selectedAsset = assetItem.assetId;
        NSLog(@"Selected Asset %d " , assetItem.assetId);
        [cache checkDownloadedAsset:assetItem.assetId];
        [self downloadAsset:assetItem ForActivity:LOAD_NODE isTempAsset:true];
    }
}



#pragma mark Button actions

- (IBAction)categoryBtnFuction:(id)sender
{
    UIAlertController * view=   [UIAlertController
                                 alertControllerWithTitle:@"Category"
                                 message:nil
                                 preferredStyle:UIAlertControllerStyleActionSheet];
    
    
    
    UIAlertAction* allmodels = [UIAlertAction
                                actionWithTitle:@"All Models"
                                style:UIAlertActionStyleDefault
                                handler:^(UIAlertAction * action)
                                {
                                    if(modelCategoryTab == ALLMODELS)
                                        return;
                                    [_modelCategory setTitle: @"All Models" forState:UIControlStateNormal];
                                    assetArray = [cache GetAssetList:ALLMODELS Search:@""];
                                    [_assetsCollectionView reloadData];
                                    modelCategoryTab = ALLMODELS;
                                    selectedAsset = -1;
                                    [view dismissViewControllerAnimated:YES completion:nil];
                                    
                                }];
    UIAlertAction* character = [UIAlertAction
                                actionWithTitle:@"Character"
                                style:UIAlertActionStyleDefault
                                handler:^(UIAlertAction * action)
                                {
                                    if(modelCategoryTab == CHARACTERS)
                                        return;
                                    [_modelCategory setTitle: @"Characters" forState:UIControlStateNormal];
                                    assetArray = [cache GetAssetList:CHARACTERS Search:@""];
                                    [_assetsCollectionView reloadData];
                                    modelCategoryTab = CHARACTERS;
                                    selectedAsset = -1;
                                    [view dismissViewControllerAnimated:YES completion:nil];
                                    
                                }];
    
    UIAlertAction* backgrounds = [UIAlertAction
                                  actionWithTitle:@"Backgrounds"
                                  style:UIAlertActionStyleDefault
                                  handler:^(UIAlertAction * action)
                                  {
                                      if(modelCategoryTab == BACKGROUNDS)
                                          return;
                                      [_modelCategory setTitle: @"Backgrounds" forState:UIControlStateNormal];
                                      assetArray = [cache GetAssetList:BACKGROUNDS Search:@""];
                                      [_assetsCollectionView reloadData];
                                      modelCategoryTab = BACKGROUNDS;
                                      selectedAsset = -1;
                                      [view dismissViewControllerAnimated:YES completion:nil];
                                      
                                  }];
    
    UIAlertAction* accessories = [UIAlertAction
                                  actionWithTitle:@"Accessories"
                                  style:UIAlertActionStyleDefault
                                  handler:^(UIAlertAction * action)
                                  {
                                      if(modelCategoryTab == ACCESSORIES)
                                          return;
                                      [_modelCategory setTitle: @"Accessories" forState:UIControlStateNormal];
                                      assetArray = [cache GetAssetList:ACCESSORIES Search:@""];
                                      [_assetsCollectionView reloadData];
                                      modelCategoryTab = ACCESSORIES;
                                      selectedAsset = -1;
                                      [view dismissViewControllerAnimated:YES completion:nil];
                                      
                                  }];
    
    UIAlertAction* mylibrary = [UIAlertAction
                                  actionWithTitle:@"My Library"
                                  style:UIAlertActionStyleDefault
                                  handler:^(UIAlertAction * action)
                                  {
                                      if(modelCategoryTab == MY_LIBRARY_TYPE)
                                          return;
                                      [_modelCategory setTitle: @"My Library" forState:UIControlStateNormal];
                                      assetArray = [cache GetAssetList:MY_LIBRARY_TYPE Search:@""];
                                      [_assetsCollectionView reloadData];
                                      modelCategoryTab = MY_LIBRARY_TYPE;
                                      selectedAsset = -1;
                                      [view dismissViewControllerAnimated:YES completion:nil];
                                      
                                  }];
    
    switch (modelCategoryTab) {
        case ALLMODELS:
            [allmodels setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
        case CHARACTERS:
            [character setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
        case BACKGROUNDS:
            [backgrounds setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
        case ACCESSORIES:
            [accessories setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
        case ASSET_LIBRARY:
            [mylibrary setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
    }
    [view addAction:allmodels];
    [view addAction:character];
    [view addAction:backgrounds];
    [view addAction:accessories];
    [view addAction:mylibrary];
    UIPopoverPresentationController *popover = view.popoverPresentationController;
    popover.sourceView = _modelCategory;
    popover.permittedArrowDirections = UIPopoverArrowDirectionUp;
    popover.sourceRect = _modelCategory.bounds;
    [self presentViewController:view animated:YES completion:nil];
   }

#pragma mark - ActionSheet Delegate Functions

-(void) actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    
}

- (void)downloadAsset:(AssetItem*)assetvalue ForActivity:(int)activity isTempAsset:(bool)isTempAsset
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    
    NSString *fileName, *url;
    NSNumber* returnId = [NSNumber numberWithInt:assetvalue.assetId];
    
    if(assetvalue.type == IMPORT_PARTICLE){
        if(assetvalue.assetId >= 50000 && assetvalue.assetId <= 60000){
            fileName = [NSString stringWithFormat:@"%@/%d.json", cacheDirectory, assetvalue.assetId];
            url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/particles/%d.json", assetvalue.assetId];
            
            NSLog(@"File Name : %@", fileName);
            
           if (![[NSFileManager defaultManager] fileExistsAtPath:fileName]){
                [self.assetSelectionDelegate showOrHideProgress:1];
                [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(downloadParticleJson:) priority:NSOperationQueuePriorityHigh];
            }
            else {
                if (activity == LOAD_NODE){
                    [self.assetSelectionDelegate showOrHideProgress:0];
                    assetvalue.isTempAsset = isTempAsset;
                    [_addToSceneBtn setEnabled:YES];
                    [self.assetSelectionDelegate loadNodeInScene:assetvalue ActionType:IMPORT_ASSET_ACTION];
                }
                else{
                    //[self updateUIForRestoringPurchase:assetvalue.assetId];
                }
            }
        }
    }
    else if (assetvalue.type == CHARACTERS) {
        if (assetvalue.assetId >= 40000 && assetvalue.assetId < 50000) {
            fileName = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.sgr", docDirPath, assetvalue.assetId];
            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName])
                NSLog(@"Rig file not exists");
            else{
                assetvalue.isTempAsset = isTempAsset;
                [_addToSceneBtn setEnabled:YES];
                [self.assetSelectionDelegate loadNodeInScene:assetvalue ActionType:IMPORT_ASSET_ACTION];
            }
        }
        else {
            fileName = [NSString stringWithFormat:@"%@/%d.sgr", cacheDirectory, assetvalue.assetId];
            NSLog(@"File Path : %@",fileName);
            url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/mesh/%d.sgr", assetvalue.assetId];
            
            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName] || activity == DOWNLOAD_NODE){
                [self.assetSelectionDelegate showOrHideProgress:1];
                [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(downloadTextureFileWithReturnId:) priority:NSOperationQueuePriorityHigh];
            }
            else {
                if (activity == LOAD_NODE){
                    [self.assetSelectionDelegate showOrHideProgress:0];
                    assetvalue.isTempAsset = isTempAsset;
                    [_addToSceneBtn setEnabled:YES];
                    [self.assetSelectionDelegate loadNodeInScene:assetvalue ActionType:IMPORT_ASSET_ACTION];
                }
                else{
                   //[self updateUIForRestoringPurchase:assetvalue.assetId];
                }
            }
        }
    }
    else if (assetvalue.type == BACKGROUNDS || assetvalue.type == ACCESSORIES) {
        
        fileName = [NSString stringWithFormat:@"%@/%d.sgm", cacheDirectory, assetvalue.assetId];
        url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/mesh/%d.sgm", assetvalue.assetId];
        
        if (assetvalue.assetId >= 20000 && assetvalue.assetId <= 30000) {
            fileName = [NSString stringWithFormat:@"%@/Resources/Sgm/%d.sgm", docDirPath, assetvalue.assetId];
            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName])
                NSLog(@"Rig file not exists");
            else{
                assetvalue.isTempAsset = isTempAsset;
                [_addToSceneBtn setEnabled:YES];
                [self.assetSelectionDelegate loadNodeInScene:assetvalue ActionType:IMPORT_ASSET_ACTION];
            }
        }
        else if (![[NSFileManager defaultManager] fileExistsAtPath:fileName] || activity == DOWNLOAD_NODE){
            [self.assetSelectionDelegate showOrHideProgress:1];
            [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(downloadTextureFileWithReturnId:) priority:NSOperationQueuePriorityHigh];
        }
        else {
            if (activity == LOAD_NODE){
                [self.assetSelectionDelegate showOrHideProgress:0];
                assetvalue.isTempAsset = isTempAsset;
                [_addToSceneBtn setEnabled:YES];
                [self.assetSelectionDelegate loadNodeInScene:assetvalue ActionType:IMPORT_ASSET_ACTION];
            }
            else{
                //[self updateUIForRestoringPurchase:assetvalue.assetId];
            }
        }
    }
    else if (assetvalue.type == OBJ_FILE) {
        fileName = [NSString stringWithFormat:@"%@/Resources/Objs/%d.obj", docDirPath, assetvalue.assetId];
        if (![[NSFileManager defaultManager] fileExistsAtPath:fileName])
            NSLog(@"Obj file not exists");
        else{
        }
    }
    
}

- (void) downloadParticleJson:(DownloadTask*)task
{
    NSNumber* returnId = task.returnObj;
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString* fileName = [NSString stringWithFormat:@"%@/%d.json", cacheDirectory, [returnId intValue]];

    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        int assetId = [task.returnObj intValue];        
        AssetItem *assetItem = [cache GetAsset:assetId];
        if(!addToScenePressed)
            assetItem.isTempAsset = true;
        [self.assetSelectionDelegate loadNodeInScene:assetItem ActionType:IMPORT_ASSET_ACTION];
        [self.assetSelectionDelegate showOrHideProgress:0];
    }

}

- (void)downloadTextureFileWithReturnId:(DownloadTask*)task
{
    NSNumber* returnId = task.returnObj;
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString* fileName = [NSString stringWithFormat:@"%@/%d-cm.png", cacheDirectory, [returnId intValue]];
    NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/meshtexture/%d.png", [returnId intValue]];
    
    AssetItem *downloadingAsset = [cache GetAsset:[returnId intValue]];
    
    if ([assetArray count] > 0  && downloadingAsset && [cache checkDownloadedAsset:downloadingAsset.assetId])
        [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(proceedToFileVerification:) priority:NSOperationQueuePriorityHigh];
    else
        [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(downloadCompleted:) priority:NSOperationQueuePriorityHigh];
}

- (void)proceedToFileVerification:(id)idObj
{
    NSString* fileName;
    int assetId;
    
    if ([idObj isKindOfClass:[DownloadTask class]]) {
        fileName = ((DownloadTask*)idObj).outputPath;
        assetId = [((DownloadTask*)idObj).returnObj intValue];
    }
    else {
        fileName = [idObj objectForKey:@"fileName"];
        assetId = [[idObj objectForKey:@"returnId"] intValue];
    }
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        AssetItem * assetItem = [cache GetAsset:assetId];
        if(!addToScenePressed)
            assetItem.isTempAsset = true;
        [self.assetSelectionDelegate loadNodeInScene:assetItem ActionType:IMPORT_ASSET_ACTION];
        [self.assetSelectionDelegate showOrHideProgress:0];
    }
    else {
        [self.assetSelectionDelegate showOrHideProgress:0];
        [self.view endEditing:YES];
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to connect to the server, Please check your network settings." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}

- (void)downloadCompleted:(DownloadTask*)task
{
    
    if([task isCancelled])
        return;
    
    int assetId = [task.returnObj intValue];
    
    AssetItem *assetItem = [cache GetAsset:assetId];
    if(!addToScenePressed)
        assetItem.isTempAsset = true;
    [self.assetSelectionDelegate loadNodeInScene:assetItem ActionType:IMPORT_ASSET_ACTION];
    [self.assetSelectionDelegate showOrHideProgress:0];
    [_addToSceneBtn setEnabled:YES];
    
}


- (void)addDownloadTaskWithFileName:(NSString*)fileName URL:(NSString*)url returnId:(NSNumber*)returnId andSelector:(SEL)selector priority:(NSOperationQueuePriority)priority
{

    DownloadTask* downloadTask;
    downloadTask = [[DownloadTask alloc] initWithDelegateObject:(id)self selectorMethod:selector returnObject:(id)returnId outputFilePath:fileName andURL:url];
    downloadTask.queuePriority = priority;
    [assetDownloadQueue addOperation:downloadTask];
}


- (void) cancelOperations:(NSOperationQueue*) queue
{
    [queue cancelAllOperations];
    
    for (DownloadTask *task in [queue operations]) {
        [task cancel];
    }
}

- (void)deallocMem
{
    [self cancelOperations:downloadQueue];
    downloadQueue = nil;
    [self cancelOperations:assetDownloadQueue];
    assetDownloadQueue = nil;
    cache =nil;
    asset = nil;
    assetArray = nil;
    docDirPath = nil;
    screenScale = nil;
    _assetSelectionDelegate = nil;
}

@end
