//
//  SceneSelectionFrameCell.h
//  Iyan3D
//
//  Created by harishankarn on 16/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WEPopoverController.h"
#import "PopUpViewController.h"
@protocol ScenePropertiesDelegate
- (void)duplicateScene:(int)indexValue;
- (void)deleteScene:(int)indexValue;
- (void)renameScene:(int)indexValue;
- (void)shareScene:(NSNumber*)indexValue;
- (void)setSelectedSceneIndex:(int)index;
- (bool)Isi3dExists:(int)indexValue;
@end

@interface SceneSelectionFrameCell : UICollectionViewCell <PopUpViewControllerDelegate>

@property (weak, nonatomic) IBOutlet UIImageView* image;
@property (weak, nonatomic) IBOutlet UILabel* name;
@property NSInteger                  SelectedindexValue;
@property (weak, nonatomic) IBOutlet UIButton*          propertiesBtn;
@property (nonatomic, strong) WEPopoverController*      popoverController;
@property (nonatomic, strong) PopUpViewController*      popUpVc;
@property (weak, nonatomic) id<ScenePropertiesDelegate> delegate;
- (IBAction)propertiesAction:(id)sender;

@end
