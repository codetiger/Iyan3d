//
//  SliderPropCell.h
//  Iyan3D
//
//  Created by Karthik on 12/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Constants.h"

@protocol SwitchPropDelegate

- (void)actionMadeInTable:(int)tableIndex AtIndexPath:(NSIndexPath*)indexPath WithValue:(Vector4)value AndStatus:(BOOL)status;

@end

@interface                                         SwitchPropCell : UITableViewCell
@property (assign) int                             tableIndex;
@property (weak, nonatomic) id<SwitchPropDelegate> delegate;
@property (strong, nonatomic) NSIndexPath*         indexPath;

@property (weak, nonatomic) IBOutlet UILabel* title;
@property (weak, nonatomic) IBOutlet UISwitch* specSwitch;
@property (weak, nonatomic) IBOutlet UIButton* setBtn;
@property (weak, nonatomic) IBOutlet UIButton* nextBtn;
@property (weak, nonatomic) IBOutlet UIImageView* thumbImgView;

- (IBAction)switchChanged:(id)sender;
- (IBAction)setBtnAction:(id)sender;

@end
