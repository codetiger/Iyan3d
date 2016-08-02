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

@implementation MeshProperties

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil  WithProps:(SGNode*) sgNode MirrorState:(int)mirror AndSmoothTexture:(int)smoothTex
{
    
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        refractionValue = sgNode->getProperty(REFRACTION).value.x;
        reflectionValue = sgNode->getProperty(REFLECTION).value.x;
        isLightningValue = sgNode->getProperty(LIGHTING).value.x;
        isVisibleValue = sgNode->getProperty(VISIBILITY).value.x;
        mirrorStatus = mirror;
        isHaveLightOption = sgNode->getProperty(LIGHTING).value.x;
        physicsType = (sgNode->getProperty(HAS_PHYSICS).value.x) ? sgNode->getProperty(PHYSICS_KIND).value.x : NONE;
        velocity = sgNode->getProperty(FORCE_MAGNITUDE).value.x;
        smoothTexture = smoothTex;
        
        canApplyPhysics = (sgNode->getType() == NODE_SGM || sgNode->getType() == NODE_OBJ || sgNode->getType() == NODE_TEXT);
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.screenName = @"MeshProperties iOS";
    [_mirrorBtn setEnabled:(mirrorStatus == MIRROR_DISABLE) ? NO : YES];
    [_mirrorBtn setOn:(mirrorStatus == MIRROR_DISABLE) ? NO : (mirrorStatus == MIRROR_ON) ? YES : NO  animated:NO];

    [self.refractionSlider setValue:refractionValue];
    [self.reflectionSlider setValue:reflectionValue];
    
    [self.textureSwitch setEnabled:(smoothTexture == -1) ? NO : YES];
    [self.textureSwitch setOn:(smoothTexture > 0) ? YES : NO];
    
    self.lightingSwitch.on=isLightningValue;
    self.visibleChanged.on=isVisibleValue;
    isFaceNormal = (_faceNormalBtn.isOn) ? true : false;
    //[_lightingSwitch setEnabled:isHaveLightOption];
    
    [self.physicsSegment setSelectedSegmentIndex:physicsType];
    [self.velocitySlider setValue:velocity];
    if(!canApplyPhysics) {
        [self.physicsSegment setEnabled:NO];
        [self.velocitySlider setEnabled:NO];
        [self.directionBtn setEnabled:NO];
    }
    [self.velocityLoading setHidden:YES];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidDisappear:(BOOL)animated
{
    
    [self.delegate showLoadingActivity];
    [self.delegate setUserInteractionStatus:NO];
    
    [self performSelectorInBackground:@selector(applyPhysicsProps) withObject:nil];
    
}

- (void) applyPhysicsProps
{
    if(canApplyPhysics) {
        if(velocity != self.velocitySlider.value) {
            [self.delegate velocityChanged:self.velocitySlider.value];
            [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:YES];
        }
        if(physicsType != (int)self.physicsSegment.selectedSegmentIndex) {
            
            [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:NO];
            
            if(self.physicsSegment.selectedSegmentIndex == NONE)
                [self.delegate setPhysics:false];
            else {
                [self.delegate setPhysics:true];
                [self.delegate setPhysicsType:(int)self.physicsSegment.selectedSegmentIndex];
            }
            
            [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:YES];
        }
    }
    
    [self.delegate hideLoadingActivity];
    [self.delegate setUserInteractionStatus:YES];
}

- (IBAction)reflectionChangeEnded:(id)sender
{
    reflectionValue=self.reflectionSlider.value;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:YES];
}

- (IBAction)refractionChangeEnded:(id)sender
{
    refractionValue=self.refractionSlider.value;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:YES];
}

- (IBAction)refractionValueChanged:(id)sender
{
    refractionValue=self.refractionSlider.value;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:NO];
}

- (IBAction)reflectionValueChanged:(id)sender
{
    reflectionValue=self.reflectionSlider.value;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:NO];
}

- (IBAction)lightingSwitchChanged:(id)sender
{
    isLightningValue=self.lightingSwitch.on;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:NO];
}

- (IBAction)visibleValueChanged:(id)sender
{
    isVisibleValue=self.visibleChanged.on;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:NO];
}

- (IBAction)cloneButtonAction:(id)sender
{
    [self.delegate cloneDelegateAction];
}

- (IBAction)deleteBtnAction:(id)sender
{
    [self.delegate deleteDelegateAction];
}

- (IBAction)skinBtnAction:(id)sender
{
    [self.delegate changeSkinDelgate];
}

- (IBAction)faceNormalAction:(id)sender
{
    isFaceNormal = (_faceNormalBtn.isOn) ? true : false;
    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:NO];
}

- (IBAction)reflectionHqBtnAction:(id)sender
{
    UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Information", nil) message:NSLocalizedString(@"only_hq_options", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
    [closeAlert show];   
}

- (IBAction)refractionHqBtnAction:(id)sender
{
    UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Information", nil) message:NSLocalizedString(@"only_hq_options", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
    [closeAlert show];
}

- (IBAction)mirrorBtnAction:(id)sender
{
    [self.delegate switchMirror];
}

- (IBAction)velocityValueChanged:(id)sender
{
        //[self.delegate velocityChanged:self.velocitySlider.value];
}

- (void) showLoading
{
    if(self && self.delegate != nil)
        [self.delegate showLoadingActivity];
}

- (void) hideLoading
{
    [self.velocityLoading setHidden:YES];
}

- (IBAction)setDirection:(id)sender
{
    [self.delegate setDirection];
}

- (IBAction)physicsSegmentChanged:(id)sender
{
    
//    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:NO];
//    
//    if(self.physicsSegment.selectedSegmentIndex == NONE)
//        [self.delegate setPhysics:false];
//    else {
//        [self.delegate setPhysics:true];
//        [self.delegate setPhysicsType:(int)self.physicsSegment.selectedSegmentIndex];
//    }
//    
//    [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:YES];
}

- (IBAction)velocityChangeEnded:(id)sender
{
//     [self.delegate meshPropertyChanged:refractionValue Reflection:reflectionValue Lighting:isLightningValue Visible:isVisibleValue storeInAction:YES];
}

- (IBAction)textureSmoothSwitchAction:(id)sender
{
    [self.delegate setTextureSmoothStatus:self.textureSwitch.isOn];
    smoothTexture = self.textureSwitch.isOn;
}

@end
