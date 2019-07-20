//
//  TextFrameCell.h
//  Iyan3D
//
//  Created by sabish on 21/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface                           TextFrameCell : UICollectionViewCell
@property (weak, nonatomic) IBOutlet UILabel* displayText;
@property (weak, nonatomic) IBOutlet UILabel* fontName;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView* progress;

@end
