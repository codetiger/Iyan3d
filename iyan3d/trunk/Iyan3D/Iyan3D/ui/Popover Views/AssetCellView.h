//
//  AssetCellView.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 05/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SmartImageView.h"

@interface AssetCellView : UICollectionViewCell

@property (weak, nonatomic) IBOutlet UILabel *assetNameLabel;
@property (weak, nonatomic) IBOutlet SmartImageView *assetImageView;
@property (weak, nonatomic) IBOutlet UILabel *freeTagLabel;
@property (weak, nonatomic) IBOutlet UIImageView *premiumicon;

@end
