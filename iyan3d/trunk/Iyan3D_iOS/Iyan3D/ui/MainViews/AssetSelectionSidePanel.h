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
@protocol AssetSelectionDelegate

- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;
- (void) loadNodeInScene:(AssetItem*)assetItem ActionType:(ActionType)actionType;
- (void) removeTempNodeFromScene;
- (void) showOrHideProgress:(BOOL) value;

@end



@interface AssetSelectionSidePanel : UIViewController<UICollectionViewDelegate,UICollectionViewDataSource,UIActionSheetDelegate,UIAlertViewDelegate>{
    CacheSystem* cache;
    AssetItem* asset;
    NSMutableArray* assetArray;
    NSOperationQueue *downloadQueue , *assetDownloadQueue;
    float screenScale;
    NSString* docDirPath;
    int selectedAsset, modelCategoryTab;
    //BOOL addToScenePressed;
    int viewType;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil Type:(int)type;

@property (weak, nonatomic) IBOutlet UICollectionView *assetsCollectionView;
@property (weak, nonatomic) IBOutlet UIButton *modelCategory;
@property (weak, nonatomic) IBOutlet UIButton *cancelBtn;
@property (weak, nonatomic) IBOutlet UIButton *addToSceneBtn;
@property (weak, nonatomic) id <AssetSelectionDelegate> assetSelectionDelegate;

@end
