//
//  AnimationSelectionCollectionViewCell.h
//  Iyan3D
//
//  Created by Sankar on 21/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "SmartImageView.h"

@interface AnimationSelectionCollectionViewCell : UICollectionViewCell

@property (weak, nonatomic) IBOutlet UILabel *assetNameLabel;
@property (weak, nonatomic) IBOutlet SmartImageView *assetImageView;
@end
