//
//  ObjCellView.h
//  Iyan3D
//
//  Created by crazycubes on 22/04/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ShaderCell : UICollectionViewCell

@property (weak, nonatomic) IBOutlet UILabel *ShaderCellName;
@property (weak, nonatomic) IBOutlet UIImageView *ShaderImage;
@property (weak, nonatomic) IBOutlet UILabel *purchaseLabel;

@end
