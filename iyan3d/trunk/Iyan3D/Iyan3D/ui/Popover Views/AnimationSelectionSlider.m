//
//  AnimationSelectionSlider.m
//  Iyan3D
//
//  Created by Sankar on 21/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "AnimationSelectionSlider.h"

@interface AnimationSelectionSlider ()

@end

@implementation AnimationSelectionSlider

- (void)viewDidLoad {
    [super viewDidLoad];

    downloadQueue = [[NSOperationQueue alloc] init];
    [downloadQueue setMaxConcurrentOperationCount:3];
    cache = [CacheSystem cacheSystem];
    [self getAnimationData];
    [self.animationCollectionView registerNib:[UINib nibWithNibName:@"AnimationSelectionCollectionViewCell" bundle:nil]forCellWithReuseIdentifier:@"CELL"];

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
    cell.layer.borderColor = [UIColor darkGrayColor].CGColor;
    cell.assetNameLabel.text = assetItem.assetName;
    [cell.assetNameLabel adjustsFontSizeToFitWidth];
    cell.backgroundColor = [UIColor clearColor];
    [cell.assetImageView setImageInfo:[NSString stringWithFormat:@"%d", assetItem.assetId] forView:5 OperationQueue:downloadQueue];
    
    cell.assetNameLabel.textColor = [UIColor whiteColor];
    cell.assetNameLabel.font=[UIFont fontWithName:@"Helvetica-Bold" size:11];
    return cell;
}

#pragma mark Button actions

- (IBAction)categoryBtnFuction:(id)sender
{
   
        UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:nil
                                                                 delegate:self
                                                        cancelButtonTitle:nil
                                                   destructiveButtonTitle:nil
                                                        otherButtonTitles:@"Trending",@"Top Rated", @"Featured",@"My Animation", nil];
        CGRect rect = [self.view convertRect:self.categoryBtn.frame fromView:self.categoryBtn.superview];
        [actionSheet showFromRect:rect inView:self.view animated:YES];

    
}

- (IBAction)addBtnFunction:(id)sender {
}

- (IBAction)cancelBtnFunction:(id)sender {
    [self.delegate dismissAndHideView];
    [self.view removeFromSuperview];
}

#pragma mark - ActionSheet Delegate Functions

-(void) actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex
{
    NSString *category= [actionSheet buttonTitleAtIndex:buttonIndex];
    if([category isEqualToString:@"Trending"])
    {
        animationsItems = [cache GetAnimationList:0 fromTable:4 Search:@""];
        [self.categoryBtn setTitle:category forState:UIControlStateNormal];
        [self.animationCollectionView reloadData];
    }
    else if([category isEqualToString:@"Top Rated"])
    {
        animationsItems = [cache GetAnimationList:0 fromTable:5 Search:@""];
        [self.categoryBtn setTitle:category forState:UIControlStateNormal];
        [self.animationCollectionView reloadData];
    }
    else if([category isEqualToString:@"Featured"])
    {
        animationsItems = [cache GetAnimationList:0 fromTable:6 Search:@""];
        [self.categoryBtn setTitle:category forState:UIControlStateNormal];
        [self.animationCollectionView reloadData];
    }
    else if([category isEqualToString:@"My Animation"])
    {
        animationsItems = [cache GetAnimationList:0 fromTable:7 Search:@""];
        [self.categoryBtn setTitle:category forState:UIControlStateNormal];
        [self.animationCollectionView reloadData];
    }
   
}
- (void)willPresentActionSheet:(UIActionSheet *)actionSheet {
    
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


- (void)dealloc
{
    [downloadQueue cancelAllOperations];
    downloadQueue = nil;
    cache = nil;
}

@end
