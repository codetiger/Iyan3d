//
//  ScaleViewController.m
//  Iyan3D
//
//  Created by crazycubes on 21/05/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "ScaleViewController.h"

@interface ScaleViewController ()

@end

@implementation ScaleViewController

float previousX = 0, previousY = 0, previousZ = 0;

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil updateXValue:(float)XValue updateYValue:(float)YValue updateZValue:(float)ZValue {
    self          = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    currentXValue = XValue;
    currentYValue = YValue;
    currentZValue = ZValue;
    [self setScaleValueLabelsWithXScale:currentXValue YScale:currentYValue ZScale:currentZValue];
    [self updateScale:currentXValue:currentYValue:currentZValue];
    [self setOverAllLockFuntion];

    return self;
}
- (void)viewDidLoad {
    [super viewDidLoad];
    self.screenName           = @"ScaleView iOS";
    self.xSlider.minimumValue = self.ySlider.minimumValue = self.zSlider.minimumValue = 0.01;

    [self setScaleValueLabelsWithXScale:currentXValue YScale:currentYValue ZScale:currentZValue];
    [self updateScale:currentXValue:currentYValue:currentZValue];
    [self setOverAllLockFuntion];

    // Do any additional setup after loading the view from its nib.
}
- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    [self.xSlider setTag:2];
    [self.ySlider setTag:3];
    [self.zSlider setTag:4];
}
- (void)setOverAllLockFuntion {
    if ([self.xyzLock isOn]) {
        self.lockImage.image = [UIImage imageNamed:@"lock.png"];
        [self.bracketImage setHidden:false];
        float maximumScaleValue = MAX(self.xSlider.value, MAX(self.ySlider.value, self.zSlider.value));
        self.xSlider.value = self.ySlider.value = self.zSlider.value = maximumScaleValue;
        [self.delegate scaleValueForAction:self.xSlider.value YValue:self.ySlider.value ZValue:self.zSlider.value];

    } else {
        self.lockImage.image = [UIImage imageNamed:@"unlock.png"];
        [self.bracketImage setHidden:true];
    }
    [self setScaleValueLabelsWithXScale:self.xSlider.value YScale:self.ySlider.value ZScale:self.zSlider.value];
}
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)setScaleValueLabelsWithXScale:(float)xSCale YScale:(float)yScale ZScale:(float)ZScale {
    self.xValue.text = [NSString stringWithFormat:@"%.02f", xSCale];
    self.yValue.text = [NSString stringWithFormat:@"%.02f", yScale];
    self.zValue.text = [NSString stringWithFormat:@"%.02f", ZScale];
}

- (IBAction)XYZValueChanged:(id)sender {
    [self setScaleValueLabelsWithXScale:self.xSlider.value YScale:self.ySlider.value ZScale:self.zSlider.value];

    if (self.xSlider.value > 0 && self.ySlider.value > 0 && self.zSlider.value > 0)
        [self updateScale:self.xSlider.value:self.ySlider.value:self.zSlider.value];

    [self.delegate scaleValueForAction:self.xSlider.value YValue:self.ySlider.value ZValue:self.zSlider.value];
}
- (IBAction)settingsValueChangedAction:(UISlider*)slider {
    if ([self.xyzLock isOn]) {
        if (previousX > self.xSlider.value || previousX < self.xSlider.value)
            self.ySlider.value = self.zSlider.value = self.xSlider.value;
        else if (previousY > self.ySlider.value || previousY < self.ySlider.value)
            self.xSlider.value = self.zSlider.value = self.ySlider.value;
        else
            self.ySlider.value = self.xSlider.value = self.zSlider.value;

        previousX = self.xSlider.value;
        previousY = self.ySlider.value;
        previousZ = self.zSlider.value;
    }
    self.xValue.text = [NSString stringWithFormat:@"%.02f", self.xSlider.value];
    self.yValue.text = [NSString stringWithFormat:@"%.02f", self.ySlider.value];
    self.zValue.text = [NSString stringWithFormat:@"%.02f", self.zSlider.value];
    [self.delegate scalePropertyChanged:self.xSlider.value YValue:self.ySlider.value ZValue:self.zSlider.value];
}
- (IBAction)scaleLockSwtichChangedAction:(id)sender {
    [self setOverAllLockFuntion];
}
- (void)updateScale:(float)XValue:(float)YValue
                   :(float)ZValue {
    self.xSlider.value = XValue;
    self.ySlider.value = YValue;
    self.zSlider.value = ZValue;

    [self.xSlider setValue:XValue];
    [self.ySlider setValue:YValue];
    [self.zSlider setValue:ZValue];
    previousX = XValue;
    previousY = YValue;
    previousZ = ZValue;

    float minimumValue        = MIN(XValue, MIN(YValue, ZValue));
    self.xSlider.minimumValue = self.ySlider.minimumValue = self.zSlider.minimumValue = (minimumValue) / 2.0;

    float maximumValue        = MAX(XValue, MAX(YValue, ZValue));
    self.xSlider.maximumValue = self.ySlider.maximumValue = self.zSlider.maximumValue = (self.xSlider.minimumValue + maximumValue) * 2.0;
    [self setScaleValueLabelsWithXScale:XValue YScale:YValue ZScale:ZValue];
}

@end
