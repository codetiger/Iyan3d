//
//  AnimationSelectionSlider.h
//  Iyan3D
//
//  Created by Sankar on 21/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef AnimationSelectionSlider_h
#define AnimationSelectionSlider_h

#import <UIKit/UIKit.h>
#import "AppHelper.h"
#import "Constants.h"
#import "AnimationSelectionCollectionViewCell.h"
#import "SGEditorScene.h"

@protocol SliderDelegate
- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;
- (void) showOrHideProgress:(BOOL) value;
- (void)applyAnimationToSelectedNode:(NSString*)filePath;
- (void) stopPlaying;
- (void) removeTempAnimation;

@end

@interface AnimationSelectionSlider : UIViewController<UICollectionViewDataSource, UICollectionViewDelegate,AppHelperDelegate,UIActionSheetDelegate>
{
    NSMutableArray *animationJsonArray;
    NSMutableArray* animationsItems;
    CacheSystem *cache;
    NSOperationQueue *downloadQueue , *animDownloadQueue;
    NSArray *jsonUserArray;
    int tabValue , previousTabValue;
    int selectedAssetId,animationCategoryTab;
    NSString* docDirPath;
    ANIMATION_TYPE animationType;
    SGEditorScene *editorSceneLocal;
    int selectedNodeId, bonecount, currentFrame, totalFrame;
    bool isFirstTimeEntered;
}


- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil EditorScene:(SGEditorScene*)editorScene FirstTime:(BOOL)isFirstTime;

@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *downloadIndicator;
@property (strong, nonatomic) id <SliderDelegate> delegate;
@property (weak, nonatomic) IBOutlet UIButton *categoryBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *animationCollectionView;
@property (weak, nonatomic) IBOutlet UIButton *cancelBtn;
@property (weak, nonatomic) IBOutlet UIButton *addBtn;
- (IBAction)categoryBtnFuction:(id)sender;
- (IBAction)addBtnFunction:(id)sender;
- (IBAction)cancelBtnFunction:(id)sender;
@end

#endif