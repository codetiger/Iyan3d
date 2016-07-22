//
//  AssetSelectionSidePanel.m
//  Iyan3D
//
//  Created by sabish on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "AssetSelectionSidePanel.h"
#import "Utility.h"
#import "DownloadTask.h"
#import "Constants.h"
#import "AppDelegate.h"

#define RESTORING 11
#define LOAD_NODE 22
#define DOWNLOAD_NODE 33
#define CLOSE_CURRENT_VIEW 10
#define OBJFile 6
#define IMPORT_MODELS 0
#define IMPORT_PARTICLE 13
#define MINECRAFT 5
#define FNAF 6

#define RENAME_ALERT 0

#define CANCEL 0
#define OK 1

@implementation AssetSelectionSidePanel

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil Type:(int)type ScreenWidth:(int)screenWidth ScreenHeight:(int)screenHeight

{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if(self){
        ScreenHeight = screenHeight;
        ScreenWidth = screenWidth;
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

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.screenName = @"AssetSelection iOS";
    
    if([Utility IsPadDevice])
       [self.assetsCollectionView registerNib:[UINib nibWithNibName:@"AssetFrameCell" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    else
        [self.assetsCollectionView registerNib:[UINib nibWithNibName:@"AssetFrameCellPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];

        self.cancelBtn.layer.cornerRadius = CORNER_RADIUS;
    self.addToSceneBtn.layer.cornerRadius = CORNER_RADIUS;
    
    [self.cancelBtn setTitle:NSLocalizedString(@"CANCEL", nil) forState:UIControlStateNormal];
    [self.addToSceneBtn setTitle:NSLocalizedString(@"ADD TO SCENE", nil) forState:UIControlStateNormal];
    
    selectedAsset = -1;
    modelCategoryTab = 0;
    //addToScenePressed = false;
    [_addToSceneBtn setEnabled:NO];
    [_modelCategory setHidden:(viewType == IMPORT_MODELS) ? NO : YES];
}

 - (void)viewDidAppear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(responseCame) name:@"AssetsSet" object:nil];
    
    if(assetArray == nil || [assetArray count] == 0) {
        [self.assetLoading setHidden:NO];
        [self.assetLoading setHidesWhenStopped:YES];
        [self.assetLoading startAnimating];
        [self performSelectorInBackground:@selector(downloadAssetsData) withObject:nil];
    } else
        [self.assetLoading setHidden:YES];
    
    UITapGestureRecognizer* tapGest = [[UITapGestureRecognizer alloc] initWithTarget:self action:nil];
    tapGest.delegate = self;
    [self.view addGestureRecognizer:tapGest];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
    return NO;
}

- (void) downloadAssetsData
{
    [[AppHelper getAppHelper] downloadJsonData];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AssetsSet" object:nil];
}

- (void) responseCame
{
    [self performSelectorOnMainThread:@selector(reloadAssetsView) withObject:nil waitUntilDone:NO];
}

- (void) reloadAssetsView
{
    assetArray = [cache GetAssetList:(viewType == IMPORT_MODELS) ? ALLMODELS : IMPORT_PARTICLE  Search:@""];
    [self.assetLoading setHidden:YES];
    [self.assetsCollectionView reloadData];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

#pragma Button Actions

- (IBAction)cancelButtonAction:(id)sender
{
    [self.assetSelectionDelegate showOrHideProgress:1];
    [self cancelOperations:downloadQueue];
    [self.assetSelectionDelegate removeTempNodeFromScene];
    [self.assetSelectionDelegate showOrHideLeftView:NO withView:nil];
    [self.view removeFromSuperview];
    [self.assetSelectionDelegate showOrHideProgress:0];
    [self.assetSelectionDelegate deallocSubViews];
    [self deallocMem];
}

- (IBAction)addToSceneButtonAction:(id)sender
{
     //addToScenePressed = YES;
    [self.assetSelectionDelegate showOrHideProgress:1];
    [self cancelOperations:downloadQueue];
    [self downloadAsset:[cache GetAsset:selectedAsset] ForActivity:LOAD_NODE isTempAsset:false];
    [self.assetSelectionDelegate showOrHideLeftView:NO withView:nil];
    [self.view removeFromSuperview];
    [self.assetSelectionDelegate deallocSubViews];
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
        if(modelCategoryTab == MY_LIBRARY_TYPE)
            [cell.propsBtn setHidden:NO];
        else
            [cell.propsBtn setHidden:YES];
        
        cell.delegate = self;
        cell.cellIndex = (int)indexPath.row;
        cell.assetName.text = assetItem.name;
        [cell.assetImage setImageInfo:[NSString stringWithFormat:@"%d", assetItem.assetId] forView:ASSET_SELECTION OperationQueue:downloadQueue ScreenWidth:ScreenWidth ScreenHeight:ScreenHeight];
        return cell;
    }
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    [self.assetSelectionDelegate showOrHideProgress:1];
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
    UIAlertController * view=   [UIAlertController alertControllerWithTitle:NSLocalizedString(@"Category", nil) message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    
    UIAlertAction* allmodels = [UIAlertAction actionWithTitle:NSLocalizedString(@"All Models", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * action)
                                {
                                    if(modelCategoryTab == ALLMODELS)
                                        return;
                                    [_assetSelectionDelegate showOrHideProgress:1];
                                    [_modelCategory setTitle:NSLocalizedString(@"All Models", nil) forState:UIControlStateNormal];
                                    assetArray = [cache GetAssetList:ALLMODELS Search:@""];
                                    [_assetsCollectionView reloadData];
                                    modelCategoryTab = ALLMODELS;
                                    selectedAsset = -1;
                                    [view dismissViewControllerAnimated:YES completion:nil];
                                    [self.assetSelectionDelegate removeTempNodeFromScene];
                                    [_addToSceneBtn setEnabled:NO];
                                    [_assetSelectionDelegate showOrHideProgress:0];
                                }];
    UIAlertAction* character = [UIAlertAction actionWithTitle:NSLocalizedString(@"Character", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * action)
                                {
                                    if(modelCategoryTab == CHARACTERS)
                                        return;
                                    [_assetSelectionDelegate showOrHideProgress:1];
                                    [_modelCategory setTitle:NSLocalizedString(@"Characters", nil) forState:UIControlStateNormal];
                                    assetArray = [cache GetAssetList:CHARACTERS Search:@""];
                                    [_assetsCollectionView reloadData];
                                    modelCategoryTab = CHARACTERS;
                                    selectedAsset = -1;
                                    [view dismissViewControllerAnimated:YES completion:nil];
                                    [self.assetSelectionDelegate removeTempNodeFromScene];
                                    [_addToSceneBtn setEnabled:NO];
                                    [_assetSelectionDelegate showOrHideProgress:0];
                                }];
    UIAlertAction* backgrounds = [UIAlertAction actionWithTitle:NSLocalizedString(@"Backgrounds", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * action)
                                  {
                                      if(modelCategoryTab == BACKGROUNDS)
                                          return;
                                      [_assetSelectionDelegate showOrHideProgress:1];
                                      [_modelCategory setTitle:NSLocalizedString(@"Backgrounds", nil) forState:UIControlStateNormal];
                                      assetArray = [cache GetAssetList:BACKGROUNDS Search:@""];
                                      [_assetsCollectionView reloadData];
                                      modelCategoryTab = BACKGROUNDS;
                                      selectedAsset = -1;
                                      [view dismissViewControllerAnimated:YES completion:nil];
                                      [self.assetSelectionDelegate removeTempNodeFromScene];
                                      [_addToSceneBtn setEnabled:NO];
                                      [_assetSelectionDelegate showOrHideProgress:0];
                                  }];
    UIAlertAction* accessories = [UIAlertAction actionWithTitle:NSLocalizedString(@"Accessories", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * action)
                                  {
                                      if(modelCategoryTab == ACCESSORIES)
                                          return;
                                      [_assetSelectionDelegate showOrHideProgress:1];
                                      [_modelCategory setTitle:NSLocalizedString(@"Accessories", nil) forState:UIControlStateNormal];
                                      assetArray = [cache GetAssetList:ACCESSORIES Search:@""];
                                      [_assetsCollectionView reloadData];
                                      modelCategoryTab = ACCESSORIES;
                                      selectedAsset = -1;
                                      [view dismissViewControllerAnimated:YES completion:nil];
                                      [self.assetSelectionDelegate removeTempNodeFromScene];
                                      [_addToSceneBtn setEnabled:NO];
                                      [_assetSelectionDelegate showOrHideProgress:0];
                                  }];
    UIAlertAction* minecraft = [UIAlertAction actionWithTitle:NSLocalizedString(@"Minecraft", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * action)
                                  {
                                      if(modelCategoryTab == MINECRAFT)
                                          return;
                                      [_assetSelectionDelegate showOrHideProgress:1];
                                      [_modelCategory setTitle:NSLocalizedString(@"Minecraft", nil) forState:UIControlStateNormal];
                                      assetArray = [cache GetAssetList:MINECRAFT Search:@""];
                                      [_assetsCollectionView reloadData];
                                      modelCategoryTab = MINECRAFT;
                                      selectedAsset = -1;
                                      [view dismissViewControllerAnimated:YES completion:nil];
                                      [self.assetSelectionDelegate removeTempNodeFromScene];
                                      [_addToSceneBtn setEnabled:NO];
                                      [_assetSelectionDelegate showOrHideProgress:0];
                                  }];
    UIAlertAction* fnaf = [UIAlertAction actionWithTitle:NSLocalizedString(@"FNAF", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * action)
                                  {
                                      if(modelCategoryTab == FNAF)
                                          return;
                                      [_assetSelectionDelegate showOrHideProgress:1];
                                      [_modelCategory setTitle:NSLocalizedString(@"FNAF", nil) forState:UIControlStateNormal];
                                      assetArray = [cache GetAssetList:FNAF Search:@""];
                                      [_assetsCollectionView reloadData];
                                      modelCategoryTab = FNAF;
                                      selectedAsset = -1;
                                      [view dismissViewControllerAnimated:YES completion:nil];
                                      [self.assetSelectionDelegate removeTempNodeFromScene];
                                      [_addToSceneBtn setEnabled:NO];
                                      [_assetSelectionDelegate showOrHideProgress:0];
                                  }];
    UIAlertAction* mylibrary = [UIAlertAction actionWithTitle:NSLocalizedString(@"My Library", nil) style:UIAlertActionStyleDefault handler:^(UIAlertAction * action)
                                  {
                                      if(modelCategoryTab == MY_LIBRARY_TYPE)
                                          return;
                                      [self openMyLibrary];
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
        case MINECRAFT:
            [minecraft setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
        case FNAF:
            [fnaf setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
        case ASSET_LIBRARY:
            [mylibrary setValue:[[UIImage imageNamed:@"selected_mark.png"] imageWithRenderingMode:UIImageRenderingModeAlwaysOriginal] forKey:@"image"];
            break;
    }
    [view addAction:allmodels];
    [view addAction:character];
    [view addAction:backgrounds];
    [view addAction:accessories];
    [view addAction:minecraft];
    [view addAction:fnaf];
    [view addAction:mylibrary];
    UIPopoverPresentationController *popover = view.popoverPresentationController;
    popover.sourceView = _modelCategory;
    popover.permittedArrowDirections = UIPopoverArrowDirectionUp;
    popover.sourceRect = _modelCategory.bounds;
    [self presentViewController:view animated:YES completion:nil];
}

- (void) openMyLibrary
{
    [_assetSelectionDelegate showOrHideProgress:1];
    [_modelCategory setTitle:NSLocalizedString(@"My Library", nil) forState:UIControlStateNormal];
    assetArray = [cache GetAssetList:MY_LIBRARY_TYPE Search:@""];
    [_assetsCollectionView reloadData];
    modelCategoryTab = MY_LIBRARY_TYPE;
    selectedAsset = -1;
    [self.assetSelectionDelegate removeTempNodeFromScene];
    [_addToSceneBtn setEnabled:NO];
    [_assetSelectionDelegate showOrHideProgress:0];
}

#pragma AssetFrameCell delegate

- (void) deleteAssetAtIndex:(int) indexVal
{
    if(indexVal >= [assetArray count])
        return;
    
    NSFileManager* fm = [NSFileManager defaultManager];
    AssetItem *a = assetArray[indexVal];
    NSString* meshFilePath;
    NSString* thumbPath;
    NSString* imgFilePath;
    
    if(a.type == CHARACTERS) {
        meshFilePath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.sgr", docDirPath, a.assetId];
        thumbPath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.png", docDirPath, a.assetId];
        imgFilePath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d-cm.png", docDirPath, a.assetId];
    } else {
        meshFilePath = [NSString stringWithFormat:@"%@/Resources/Sgm/%d.sgm", docDirPath, a.assetId];
        thumbPath = [NSString stringWithFormat:@"%@/Resources/Sgm/%d.png", docDirPath, a.assetId];
        imgFilePath = [NSString stringWithFormat:@"%@/Resources/Textures/%d-cm.png", docDirPath, a.assetId];
    }
    
    if([fm fileExistsAtPath:meshFilePath])
        [fm removeItemAtPath:meshFilePath error:nil];
    if([fm fileExistsAtPath:thumbPath])
        [fm removeItemAtPath:thumbPath error:nil];
    if([fm fileExistsAtPath:imgFilePath])
        [fm removeItemAtPath:imgFilePath error:nil];
    
    [cache deleteMyAsset:a.assetId];
    [self openMyLibrary];
}

- (void) cloneAssetAtIndex:(int) indexVal
{
    NSDate *currDate = [NSDate date];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc]init];
    [dateFormatter setDateFormat:@"YY-MM-DD HH:mm:ss"];
    NSString *dateString = [dateFormatter stringFromDate:currDate];
    
    NSFileManager* fm = [NSFileManager defaultManager];
    AssetItem *a = assetArray[indexVal];
    NSString* meshFilePath;
    NSString* thumbPath;
    NSString* imgFilePath;
    
    NSString* newMeshPath;
    NSString* newThumbPath;
    NSString* newTexPath;
    
    AssetItem* cloneAsset = [[AssetItem alloc] init];

    if(a.type == CHARACTERS) {
        meshFilePath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.sgr", docDirPath, a.assetId];
        thumbPath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.png", docDirPath, a.assetId];
        imgFilePath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d-cm.png", docDirPath, a.assetId];
        
        cloneAsset.assetId = 40000 + [cache getNextAutoRigAssetId];
        cloneAsset.type = CHARACTERS;
        cloneAsset.name = [NSString stringWithFormat:@"autorig%d",[cache getNextAutoRigAssetId]];
        
        newMeshPath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.sgr", docDirPath, cloneAsset.assetId];
        newThumbPath = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.png", docDirPath, cloneAsset.assetId];
        newTexPath = [NSString stringWithFormat:@"%@/Resources/Sgm/%d-cm.png", docDirPath, cloneAsset.assetId];

    } else {
        meshFilePath = [NSString stringWithFormat:@"%@/Resources/Sgm/%d.sgm", docDirPath, a.assetId];
        thumbPath = [NSString stringWithFormat:@"%@/Resources/Sgm/%d.png", docDirPath, a.assetId];
        imgFilePath = [NSString stringWithFormat:@"%@/Resources/Textures/%d-cm.png", docDirPath, a.assetId];
        
        cloneAsset.assetId = 20000 + [cache getNextObjAssetId];
        NSLog(@" a.assetId %d new ID %d ", a.assetId, cloneAsset.assetId);
        cloneAsset.type = NODE_SGM;
        cloneAsset.name = [NSString stringWithFormat:@"%@copy", a.name];
        
        newMeshPath = [NSString stringWithFormat:@"%@/Resources/Sgm/%d.sgm", docDirPath, cloneAsset.assetId];
        newThumbPath = [NSString stringWithFormat:@"%@/Resources/Sgm/%d.png", docDirPath, cloneAsset.assetId];
        newTexPath = [NSString stringWithFormat:@"%@/Resources/Textures/%d-cm.png", docDirPath, cloneAsset.assetId];

    }
    
    NSError *error;
    if([fm fileExistsAtPath:meshFilePath])
        [fm copyItemAtPath:meshFilePath toPath:newMeshPath error:&error];
    if([fm fileExistsAtPath:thumbPath])
        [fm copyItemAtPath:thumbPath toPath:newThumbPath error:&error];
    if([fm fileExistsAtPath:imgFilePath])
        [fm copyItemAtPath:imgFilePath toPath:newTexPath error:&error];
    
    
    cloneAsset.iap = 0;
    cloneAsset.keywords = [NSString stringWithFormat:@" %@",a.name];
    cloneAsset.boneCount = 0;
    cloneAsset.hash = [self getMD5ForNonReadableFile:newMeshPath];
    cloneAsset.modifiedDate = dateString;
    
    [cache UpdateAsset:cloneAsset];
    [cache AddDownloadedAsset:cloneAsset];
    [self openMyLibrary];
}

- (void) renameAssetAtIndex:(int) indexVal
{
    AssetItem *a = assetArray[indexVal];
    
    UIAlertView* renameScene = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Rename Object", nil) message:NSLocalizedString(@"Please enter a name", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"Ok", nil), nil];
    [renameScene setAlertViewStyle:UIAlertViewStylePlainTextInput];
    [[renameScene textFieldAtIndex:0] setPlaceholder:a.name];
    [[renameScene textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
    [renameScene setTag:RENAME_ALERT];
    [renameScene setAccessibilityIdentifier:[NSString stringWithFormat:@"%d", indexVal]];
    [renameScene show];
    [[renameScene textFieldAtIndex:0] becomeFirstResponder];

}

-(NSString*) getMD5ForNonReadableFile:(NSString*) path
{
    NSData* data = [NSData dataWithContentsOfFile:path];
    unsigned char result[CC_MD5_DIGEST_LENGTH];
    CC_MD5([data bytes], (int)[data length], result);
    NSMutableString *hashString = [NSMutableString stringWithCapacity:CC_MD5_DIGEST_LENGTH * 2];
    for(int i = 0; i < CC_MD5_DIGEST_LENGTH; ++i) {
        [hashString appendFormat:@"%02x", result[i]];
    }
    return [NSString stringWithString:hashString];
}

#pragma mark - AlertView delegate

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    switch (alertView.tag) {
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
                        AssetItem *a = assetArray[indexVal];
                        a.name = name;
                        a.keywords = [NSString stringWithFormat:@" %@", name];
                        [cache UpdateAsset:a];
                        [self openMyLibrary];
                    }
                }
            }
            [alertView resignFirstResponder];
            
            break;
        }
            
            
        default:
            break;
    }
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
            NSString* meshFileName = [NSString stringWithFormat:@"%@/%d.sgm", cacheDirectory, assetvalue.assetId];
            NSString* texFileName = [NSString stringWithFormat:@"%@/%d-cm.png", cacheDirectory, assetvalue.assetId];
            url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/particles/%d.json", assetvalue.assetId];
            
            NSLog(@"File Name : %@", fileName);
            
            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName] || ![[NSFileManager defaultManager] fileExistsAtPath:meshFileName] || ![[NSFileManager defaultManager] fileExistsAtPath:texFileName]) {
                
                [self.assetSelectionDelegate showOrHideProgress:1];
                [self addDownloadTaskWithFileName:fileName URL:url returnId:returnId andSelector:@selector(downloadParticleMesh:) priority:NSOperationQueuePriorityHigh];
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
            NSString* textureFile = [NSString stringWithFormat:@"%@/%d-cm.png", cacheDirectory, assetvalue.assetId];

            NSLog(@"File Path : %@",fileName);
            url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/mesh/%d.sgr", assetvalue.assetId];
            
            if (![[NSFileManager defaultManager] fileExistsAtPath:fileName] || activity == DOWNLOAD_NODE || ![[NSFileManager defaultManager] fileExistsAtPath:textureFile]){
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
    else if (assetvalue.type == BACKGROUNDS || assetvalue.type == ACCESSORIES || assetvalue.type == OBJ_FILE) {
        
        fileName = [NSString stringWithFormat:@"%@/%d.sgm", cacheDirectory, assetvalue.assetId];
        NSString* textureFile = [NSString stringWithFormat:@"%@/%d-cm.png", cacheDirectory, assetvalue.assetId];
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
        else if (![[NSFileManager defaultManager] fileExistsAtPath:fileName] || activity == DOWNLOAD_NODE || ![[NSFileManager defaultManager] fileExistsAtPath:textureFile]){
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

- (void) downloadParticleMesh:(DownloadTask*)task
{
    NSNumber* returnId = task.returnObj;
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString* jsonFileName = [NSString stringWithFormat:@"%@/%d.json", cacheDirectory, [returnId intValue]];

    if ([[NSFileManager defaultManager] fileExistsAtPath:jsonFileName]) {
        NSString* jsonContent = [NSString stringWithContentsOfFile:jsonFileName encoding:NSUTF8StringEncoding error:nil];
        NSLog(@" \n Json : %@ ", jsonContent);
        
        NSString* meshFileName = [NSString stringWithFormat:@"%@/%d.sgm", cacheDirectory, [returnId intValue]];
        NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/mesh/%d.sgm", [returnId intValue]];
        NSLog(@" \n Particles mesh %@ ", url);
        [self addDownloadTaskWithFileName:meshFileName URL:url returnId:returnId andSelector:@selector(downloadParticleTexture:) priority:NSOperationQueuePriorityHigh];
    }
    
    /*
    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        int assetId = [task.returnObj intValue];        
        AssetItem *assetItem = [cache GetAsset:assetId];
        //if(!addToScenePressed)
            assetItem.isTempAsset = true;
        [self.assetSelectionDelegate loadNodeInScene:assetItem ActionType:IMPORT_ASSET_ACTION];
        [self.assetSelectionDelegate showOrHideProgress:0];
    }
    */

}

- (void) downloadParticleTexture:(DownloadTask*)task
{
    NSNumber* returnId = task.returnObj;
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString* meshFileName = [NSString stringWithFormat:@"%@/%d.sgm", cacheDirectory, [returnId intValue]];
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:meshFileName]) {
        NSString* texFileName = [NSString stringWithFormat:@"%@/%d-cm.png", cacheDirectory, [returnId intValue]];
        NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/meshtexture/%d.png", [returnId intValue]];
        
        [self addDownloadTaskWithFileName:texFileName URL:url returnId:returnId andSelector:@selector(downloadParticleCompleted:) priority:NSOperationQueuePriorityHigh];
    }
}

- (void) downloadParticleCompleted:(DownloadTask*)task
{
    NSNumber* returnId = task.returnObj;
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    NSString* texFileName = [NSString stringWithFormat:@"%@/%d.png", cacheDirectory, [returnId intValue]];

    if ([[NSFileManager defaultManager] fileExistsAtPath:texFileName])
        [self proceedToFileVerification:task];
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
        idObj = nil;
    }
    else {
        fileName = [idObj objectForKey:@"fileName"];
        assetId = [[idObj objectForKey:@"returnId"] intValue];
    }
    
    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        AssetItem * assetItem = [cache GetAsset:assetId];
        assetItem.isTempAsset = true;
        [self.assetSelectionDelegate loadNodeInScene:assetItem ActionType:IMPORT_ASSET_ACTION];
        [self.assetSelectionDelegate showOrHideProgress:0];
        [_addToSceneBtn setEnabled:YES];
    }
    else {
        [_addToSceneBtn setEnabled:NO];
        [self.assetSelectionDelegate showOrHideProgress:0];
        [self.view endEditing:YES];
        UIAlertView* message = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Connection Error", nil) message:NSLocalizedString(@"unable_connect_server", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}

- (void)downloadCompleted:(DownloadTask*)task
{
    if([task isCancelled])
        return;
    [self proceedToFileVerification:task];
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
