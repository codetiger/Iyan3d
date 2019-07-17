//
//  ObjCellView.h
//  Iyan3D
//
//  Created by crazycubes on 22/04/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "WEPopoverController.h"
#import "PopUpViewController.h"

@protocol ObjCellViewDelegate

- (void)deleteAssetAtIndex:(int)indexVal;

@end

@interface ObjCellView : UICollectionViewCell <PopUpViewControllerDelegate>

@property (weak, nonatomic) IBOutlet UILabel* assetNameLabel;
@property (weak, nonatomic) IBOutlet UIImageView*   assetImageView;
@property (weak, nonatomic) id<ObjCellViewDelegate> delegate;

@property (assign) int                             cellIndex;
@property (nonatomic, strong) WEPopoverController* popoverController;
@property (nonatomic, strong) PopUpViewController* popUpVc;

@property (weak, nonatomic) IBOutlet UIButton* propsBtn;
- (IBAction)propsAction:(id)sender;

@end
