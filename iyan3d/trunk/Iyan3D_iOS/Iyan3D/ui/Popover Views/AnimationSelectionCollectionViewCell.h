//
//  AnimationSelectionCollectionViewCell.h
//  Iyan3D
//
//  Created by Sankar on 21/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef AnimationSelectionCollectionViewCell_h
#define AnimationSelectionCollectionViewCell_h


#import <UIKit/UIKit.h>

#import "WEPopoverController.h"
#import "PopUpViewController.h"
#import "SmartImageView.h"

@protocol AnimationPropsDelegate
- (void) deleteAnimationAtIndex:(int) indexVal;
- (void) cloneAnimation:(int) indexVal;
- (void) renameAnimation:(int) indexVal;
- (void)publishBtnaction:(id)sender;
- (void) setSelectedAnimationAtIndex:(int)indexVal;

@end


@interface AnimationSelectionCollectionViewCell : UICollectionViewCell < PopUpViewControllerDelegate >


@property (assign) int selectedIndex;
@property (assign) int category;
@property (strong, nonatomic) id parentVC;
@property (weak, nonatomic) IBOutlet UIButton *propsBtn;
@property (weak, nonatomic) IBOutlet UILabel *assetNameLabel;
@property (weak, nonatomic) IBOutlet SmartImageView *assetImageView;

@property (weak, nonatomic) id < AnimationPropsDelegate > delegate;

@property (nonatomic, strong) WEPopoverController *popoverController;
@property (nonatomic, strong) PopUpViewController *popUpVc;

- (IBAction)propsAction:(id)sender;
@end

#endif