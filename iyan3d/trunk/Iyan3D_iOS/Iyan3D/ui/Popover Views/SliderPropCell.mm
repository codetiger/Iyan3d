//
//  SliderPropCell.m
//  Iyan3D
//
//  Created by Karthik on 12/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "SliderPropCell.h"

@implementation SliderPropCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (IBAction)sliderValueChanged:(id)sender {
    [self.delegate actionMadeInTable:_tableIndex AtIndexPath:_indexPath WithValue:Vector4(self.slider.value) AndStatus:NO];
}

- (IBAction)sliderChangeEnds:(id)sender {
    [self.delegate actionMadeInTable:_tableIndex AtIndexPath:_indexPath WithValue:Vector4(self.slider.value) AndStatus:YES];
}
@end
