//
//  AssetSelectionSidePanel.h
//  Iyan3D
//
//  Created by sabish on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SmartImageView.h"
#import "CacheSystem.h"
#import "Constants.h"
#import "GAI.h"

@protocol AssetSelectionDelegate

- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;
- (void) loadNodeInScene:(AssetItem*)assetItem ActionType:(ActionType)actionType;
- (void) removeTempNodeFromScene;
- (void) showOrHideProgress:(BOOL) value;
- (void)deallocSubViews;

@end



@interface AssetSelectionSidePanel : GAITrackedViewController<UICollectionViewDelegate,UICollectionViewDataSource,UIActionSheetDelegate,UIAlertViewDelegate>{
    CacheSystem* cache;
    AssetItem* asset;
    NSMutableArray* assetArray;
    NSOperationQueue *downloadQueue , *assetDownloadQueue;
    float screenScale;
    NSString* docDirPath;
    int selectedAsset, modelCategoryTab;
    //BOOL addToScenePressed;
    int viewType;
    int ScreenWidth, ScreenHeight;

}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil Type:(int)type ScreenWidth:(int)screenWidth ScreenHeight:(int)screenHeight
;

@property (weak, nonatomic) IBOutlet UICollectionView *assetsCollectionView;
@property (weak, nonatomic) IBOutlet UIButton *modelCategory;
@property (weak, nonatomic) IBOutlet UIButton *cancelBtn;
@property (weak, nonatomic) IBOutlet UIButton *addToSceneBtn;
@property (weak, nonatomic) id <AssetSelectionDelegate> assetSelectionDelegate;
- (IBAction)cancelButtonAction:(id)sender;

@end
