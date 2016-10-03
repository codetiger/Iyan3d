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
    actionStored = false;
    value = 0.0;
    prevValue = 0.0;
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
    
    actionStored = false;
    if(prevValue != _slider.value) {
        prevValue = _slider.value;
        [self.delegate actionMadeInTable:_tableIndex AtIndexPath:_indexPath WithValue:Vector4(prevValue) AndStatus:NO];
    }
    if(_dynamicSlider) {
        
        if(_maxLimit > 0.0 && _slider.value >= _maxLimit)
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
    
    if(_slider.value == _slider.maximumValue && value != _slider.value) {
        value = _slider.value;
        [self.delegate actionMadeInTable:_tableIndex AtIndexPath:_indexPath WithValue:Vector4(value) AndStatus:YES];
        if(_dynamicSlider && _slider.value > 0.0) {
            _xValue.text = [NSString stringWithFormat:@"%.1f", _slider.value];
            _slider.minimumValue = (_slider.value > _offsetValue) ? _slider.value - _offsetValue : 0.0;
            _slider.maximumValue = _slider.value + _offsetValue;
        }
        actionStored = true;
    }
}

- (IBAction)sliderChangeEnds:(id)sender {
    if(!actionStored) {
        [self.delegate actionMadeInTable:_tableIndex AtIndexPath:_indexPath WithValue:Vector4(self.slider.value) AndStatus:YES];
        if(_dynamicSlider && _slider.value > 0.0) {
            _xValue.text = [NSString stringWithFormat:@"%.1f", _slider.value];
            _slider.minimumValue = (_slider.value > _offsetValue) ? _slider.value - _offsetValue : 0.0;
            _slider.maximumValue = _slider.value + _offsetValue;
        }
    }
}
@end
