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

#define NONE 0
#define STATIC 1
#define LIGHT 2
#define MEDIUM 3
#define HEAVY 4
#define CLOTH 5
#define BALLOON 6
#define JELLY 7


@interface MeshProperties ()

@end

@implementation MeshProperties

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil  WithProps:(SGNode*) sgNode AndMirrorState:(BOOL)mirror {
    
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        refractionValue = sgNode->props.refraction;
        reflectionValue = sgNode->props.reflection;
        isLightningValue = sgNode->props.isLighting;
        isVisibleValue = sgNode->props.isVisible;
        mirrorStatus = mirror;
        isHaveLightOption = sgNode->props.isLighting;
        physicsType = (sgNode->props.isPhysicsEnabled) ? sgNode->props.physicsType : NONE;
        velocity = sgNode->props.forceMagnitude;
        
        canApplyPhysics = (sgNode->getType() == NODE_SGM || sgNode->getType() == NODE_OBJ || sgNode->getType() == NODE_TEXT);
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.screenName = @"MeshProperties iOS";
    
    [self.refractionSlider setValue:refractionValue];
    [self.reflectionSlider setValue:reflectionValue];
    self.lightingSwitch.on=isLightningValue;
    self.visibleChanged.on=isVisibleValue;
    isFaceNormal = (_faceNormalBtn.isOn) ? true : false;
    //[_lightingSwitch setEnabled:isHaveLightOption];
    [_mirrorBtn setEnabled:(mirrorStatus == MIRROR_DISABLE) ? NO : YES];
    [_mirrorBtn setOn:(mirrorStatus == MIRROR_DISABLE) ? NO : (mirrorStatus == MIRROR_ON) ? YES : NO  animated:YES];
    
    [self.physicsSegment setSelectedSegmentIndex:physicsType];
    [self.velocitySlider setValue:velocity];
    if(!canApplyPhysics) {
        [self.physicsSegment setEnabled:NO];
        [self.velocitySlider setEnabled:NO];
        [self.directionBtn setEnabled:NO];
    }
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
    UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"This Property is only for HighQuality Rendering." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [closeAlert show];   
}

- (IBAction)refractionHqBtnAction:(id)sender {
    UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"This Property is only for HighQuality Rendering." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [closeAlert show];
}

- (IBAction)mirrorBtnAction:(id)sender {
    [self.delegate switchMirror];
}

- (IBAction)velocityValueChanged:(id)sender {
    [self.delegate velocityChanged:self.velocitySlider.value];
}

- (IBAction)setDirection:(id)sender {
    [self.delegate setDirection];
}

- (IBAction)physicsSegmentChanged:(id)sender {
    
    if(self.physicsSegment.selectedSegmentIndex == NONE)
        [self.delegate setPhysics:false];
    else {
        [self.delegate setPhysics:true];
        [self.delegate setPhysicsType:(int)self.physicsSegment.selectedSegmentIndex];
    }
}

@end
