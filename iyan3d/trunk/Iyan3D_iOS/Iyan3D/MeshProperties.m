//
//  MeshProperties.m
//  Iyan3D
//
//  Created by Sankar on 11/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "MeshProperties.h"

#define MIRROR_OFF 0
#define MIRROR_ON 1
#define MIRROR_DISABLE 2

@interface MeshProperties ()

@end

@implementation MeshProperties

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil RefractionValue:(float)refraction ReflectionValue:(float)reflection LightningValue:(BOOL)lightningValue Visibility:(BOOL)isVisible MirrorState:(int)mirrorState {
    
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        refractionValue=refraction;
        reflectionValue=reflection;
        isLightningValue=lightningValue;
        isVisibleValue=isVisible;
        mirrorStatus = mirrorState;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.refractionSlider setValue:refractionValue];
    [self.reflectionSlider setValue:reflectionValue];
    self.lightingSwitch.on=isLightningValue;
    self.visibleChanged.on=isVisibleValue;
    isFaceNormal = (_faceNormalBtn.isOn) ? true : false;
    [_mirrorBtn setEnabled:(mirrorStatus == MIRROR_DISABLE) ? NO : YES];
    [_mirrorBtn setOn:(mirrorStatus == MIRROR_DISABLE) ? NO : (mirrorStatus == MIRROR_ON) ? YES : NO  animated:YES];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}



- (IBAction)refractionValueChanged:(id)sender {
    refractionValue=self.refractionSlider.value;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue FaceNormal:isFaceNormal];
}

- (IBAction)reflectionValueChanged:(id)sender {
    reflectionValue=self.reflectionSlider.value;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue FaceNormal:isFaceNormal];
}

- (IBAction)lightingSwitchChanged:(id)sender {
    isLightningValue=self.lightingSwitch.on;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue FaceNormal:isFaceNormal];
}

- (IBAction)visibleValueChanged:(id)sender {
    isVisibleValue=self.visibleChanged.on;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue FaceNormal:isFaceNormal];
}

- (IBAction)cloneButtonAction:(id)sender {
    [self.delegate cloneDelegateAction];
}

- (IBAction)deleteBtnAction:(id)sender {
    [self.delegate deleteDelegateAction];
}

- (IBAction)skinBtnAction:(id)sender {
    [self.delegate changeSkinDelgate];
}

- (IBAction)faceNormalAction:(id)sender {
    isFaceNormal = (_faceNormalBtn.isOn) ? true : false;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue FaceNormal:isFaceNormal];
}

- (IBAction)reflectionHqBtnAction:(id)sender {
    UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"PThis Property is only for HighQuality Rendering." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [closeAlert show];   
}

- (IBAction)refractionHqBtnAction:(id)sender {
    UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"PThis Property is only for HighQuality Rendering." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [closeAlert show];
}

- (IBAction)mirrorBtnAction:(id)sender {
    [self.delegate switchMirror];
}
@end
