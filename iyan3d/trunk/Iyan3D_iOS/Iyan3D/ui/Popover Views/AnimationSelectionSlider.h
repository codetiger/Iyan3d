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
#import "AnimationSelectionCollectionViewCell.h"
#import "GAI.h"
#import "AppHelper.h"
#import "Constants.h"
#import "SGEditorScene.h"
#import "Vector3.h"

@protocol SliderDelegate
- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;
- (void) showOrHideProgress:(BOOL) value;
- (void) applyAnimationToSelectedNode:(NSString*)filePath SelectedNodeId:(int)originalId SelectedFrame:(int)selectedFrame;
- (void) stopPlaying;
-(void) myAnimation:(BOOL)showorHide;
- (void) removeTempAnimation;
- (void) createDuplicateAssetsForAnimation;
- (bool) removeNodeFromScene:(int)nodeIndex;
- (void) updateAssetListInScenes;
- (void) removeTempNodeFromScene;
- (void)deallocSubViews;
- (void) reloadFrames;

@end

@interface AnimationSelectionSlider : GAITrackedViewController< UICollectionViewDataSource, UICollectionViewDelegate,AppHelperDelegate,UIActionSheetDelegate, UIGestureRecognizerDelegate, AnimationPropsDelegate , UIAlertViewDelegate>
{
    NSMutableArray *animationJsonArray;
    NSMutableArray* animationsItems;
    CacheSystem *cache;
    NSArray *jsonUserArray;
    AnimationItem* asset;
    int tabValue , previousTabValue;
    int selectedAssetId,animationCategoryTab;
    NSString* docDirPath;
    ANIMATION_TYPE animationType;
    SGEditorScene *editorSceneLocal;
    int selectedNodeId, bonecount, currentFrame, totalFrame;
    bool isFirstTimeAnimationApplyed;
    NSString *userid;
    int _assetId,selectedCell;
    int ScreenWidth,ScreenHeight;
}


- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil withType:(ANIMATION_TYPE)type EditorScene:(SGEditorScene*)editorScene FirstTime:(BOOL)isFirstTime ScreenWidth:(int)screenWidth ScreenHeight:(int)screenHeight;

@property (assign) int tableType;
@property (weak, nonatomic) IBOutlet UIButton *publishBtn;

@property (strong, nonatomic) id <SliderDelegate> delegate;
@property (weak, nonatomic) IBOutlet UIButton *categoryBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *animationCollectionView;
@property (weak, nonatomic) IBOutlet UIButton *cancelBtn;
@property (weak, nonatomic) IBOutlet UIButton *addBtn;
- (IBAction)categoryBtnFuction:(id)sender;
- (IBAction)addBtnFunction:(id)sender;
- (IBAction)cancelBtnFunction:(id)sender;

- (void) openMyAnimations;
@end

#endif
