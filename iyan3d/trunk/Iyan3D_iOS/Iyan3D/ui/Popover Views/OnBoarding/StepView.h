//
//  StepView.h
//  Iyan3D
//
//  Created by Karthik on 25/06/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface StepView : UIView

@property (weak, nonatomic) IBOutlet UIImageView *stepImgView;
@property (weak, nonatomic) IBOutlet UIImageView *divider;
@property (weak, nonatomic) IBOutlet UILabel *stepTitle;
@property (weak, nonatomic) IBOutlet UILabel *stepDec;

@end
