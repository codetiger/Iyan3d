//
//  AssetFrameCell.h
//  Iyan3D
//
//  Created by sabish on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SmartImageView.h"

@interface AssetFrameCell : UICollectionViewCell
@property (weak, nonatomic) IBOutlet SmartImageView *assetImage;
@property (weak, nonatomic) IBOutlet UILabel *assetName;

@end
