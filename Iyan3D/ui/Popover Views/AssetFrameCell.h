//
//  AssetFrameCell.h
//  Iyan3D
//
//  Created by sabish on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef AssetFrameCell_h
#define AssetFrameCell_h

#import <UIKit/UIKit.h>
#import "WEPopoverController.h"
#import "PopUpViewController.h"

@protocol AssetFrameCellDelegate

- (void)deleteAssetAtIndex:(int)indexVal;
- (void)cloneAssetAtIndex:(int)indexVal;
- (void)renameAssetAtIndex:(int)indexVal;

@end

@interface                           AssetFrameCell : UICollectionViewCell <PopUpViewControllerDelegate>
@property (weak, nonatomic) IBOutlet UIImage* assetImage;
@property (weak, nonatomic) IBOutlet UILabel* assetName;
@property (weak, nonatomic) IBOutlet UIButton*         propsBtn;
@property (weak, nonatomic) id<AssetFrameCellDelegate> delegate;

@property (assign) int                             cellIndex;
@property (nonatomic, strong) WEPopoverController* popoverController;
@property (nonatomic, strong) PopUpViewController* popUpVc;

- (IBAction)propsAction:(id)sender;

@end

#endif
