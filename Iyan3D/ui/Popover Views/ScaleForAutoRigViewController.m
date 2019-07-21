//
//  ScaleForAutoRigViewController.m
//  Iyan3D
//
//  Created by crazycubes on 02/07/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "ScaleForAutoRigViewController.h"

@interface ScaleForAutoRigViewController ()

@end

@implementation ScaleForAutoRigViewController

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil updateScale:(float)currentEnvelopeScale {
    self  = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    scale = currentEnvelopeScale;
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.scaleSlider.minimumValue = 0.1;
    [self updateScale:scale];
    self.scaleValue.text = [NSString stringWithFormat:@"%0.2f", scale];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)updateScale:(float)scaleValue {
    self.scaleSlider.value = scaleValue;

    self.scaleSlider.maximumValue = (self.scaleSlider.minimumValue + scaleValue) * 2.0;
}

- (IBAction)changeSliderStarted:(id)sender {
    [self updateScale:self.scaleSlider.value];
    [self.delegate scaleValueForAction:self.scaleSlider.value];
}
- (IBAction)settingsValueChangedAction:(id)sender {
    self.scaleValue.text = [NSString stringWithFormat:@"%0.2f", self.scaleSlider.value];
    [self.delegate scalePropertyChangedInRigView:self.scaleSlider.value];
}

@end
