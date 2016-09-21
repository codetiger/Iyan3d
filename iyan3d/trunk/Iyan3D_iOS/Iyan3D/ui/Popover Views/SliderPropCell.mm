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
    sliderMoving = false;
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (IBAction)sliderValueChanged:(id)sender {
    
    if(_slider.value < 0.0) {
        _slider.value = 0.0;
        return;
    }
    
    [self.delegate actionMadeInTable:_tableIndex AtIndexPath:_indexPath WithValue:Vector4(self.slider.value) AndStatus:NO];
    if(_dynamicSlider) {
        
        if(_slider.value >= _maxLimit)
            _slider.value = _maxLimit;
        
        _xValue.text = [NSString stringWithFormat:@"%.1f", _slider.value];
        if(!sliderMoving && _slider.value > 0.0 && (_slider.value == _slider.minimumValue || _slider.value == _slider.maximumValue)) {
            sliderMoving = true;
            _slider.minimumValue = _slider.value - _offsetValue;
            _slider.maximumValue = _slider.value + _offsetValue;
            [_slider setValue:_slider.maximumValue - _offsetValue animated:YES];
            sliderMoving = false;
        }
    }
}

- (IBAction)sliderChangeEnds:(id)sender {
    [self.delegate actionMadeInTable:_tableIndex AtIndexPath:_indexPath WithValue:Vector4(self.slider.value) AndStatus:YES];
    if(_dynamicSlider && _slider.value > 0.0) {
        _xValue.text = [NSString stringWithFormat:@"%.1f", _slider.value];
        _slider.minimumValue = (_slider.value > _offsetValue) ? _slider.value - _offsetValue : 0.0;
        _slider.maximumValue = _slider.value + _offsetValue;
    }
}
@end
