//
//  AnimationSelectionSlider.h
//  Iyan3D
//
//  Created by Sankar on 21/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppHelper.h"
#include "CacheSystem.h"
#include "AnimationSelectionCollectionViewCell.h"

@protocol SliderDelegate
-(void) dismissAndHideView;
@end

@interface AnimationSelectionSlider : UIViewController<UICollectionViewDataSource, UICollectionViewDelegate,AppHelperDelegate,UIActionSheetDelegate>
{
    NSMutableArray *animationJsonArray;
    NSMutableArray* animationsItems;
    CacheSystem *cache;
    NSOperationQueue *downloadQueue , *animDownloadQueue;
    NSArray *jsonUserArray;
    int tabValue , previousTabValue;

}
@property (weak, nonatomic) id <SliderDelegate> delegate;
@property (weak, nonatomic) IBOutlet UIButton *categoryBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *animationCollectionView;
@property (weak, nonatomic) IBOutlet UIButton *cancelBtn;
@property (weak, nonatomic) IBOutlet UIButton *addBtn;
- (IBAction)categoryBtnFuction:(id)sender;
- (IBAction)addBtnFunction:(id)sender;
- (IBAction)cancelBtnFunction:(id)sender;
@end
