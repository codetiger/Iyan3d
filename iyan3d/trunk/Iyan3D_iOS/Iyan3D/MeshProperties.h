//
//  MeshProperties.h
//  Iyan3D
//
//  Created by Sankar on 11/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef MeshProperties_h
#define MeshProperties_h

#import <UIKit/UIKit.h>
#import "GAI.h"

#import "SGNode.h"

@protocol MeshPropertiesDelegate
- (void)meshPropertyChanged:(float)refraction Reflection:(float)reflection Lighting:(BOOL)light Visible:(BOOL)visible storeInAction:(BOOL)status;
- (void) deleteDelegateAction;
- (void)cloneDelegateAction;
- (void)changeSkinDelgate;
- (void) switchMirror;
- (void) setPhysics:(bool)status;
- (void) setPhysicsType:(int)type;
- (void) velocityChanged:(double)vel;
- (void) setDirection;
@end

@interface MeshProperties : GAITrackedViewController {
    float refractionValue;
    float reflectionValue;
    bool isLightningValue,isHaveLightOption;
    bool isVisibleValue, canApplyPhysics;
    bool isFaceNormal;
    int mirrorStatus;
    int physicsType;
    double velocity;
    
}
- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil WithProps:(SGNode*) sgNode AndMirrorState:(BOOL)mirror;

@property (weak, nonatomic) IBOutlet UISlider *refractionSlider;
@property (weak, nonatomic) IBOutlet UISlider *reflectionSlider;
@property (weak, nonatomic) IBOutlet UISwitch *lightingSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *visibleChanged;
@property (strong, nonatomic) id <MeshPropertiesDelegate> delegate;
@property (weak, nonatomic) IBOutlet UIButton *cloneBtn;
@property (weak, nonatomic) IBOutlet UIButton *deleteBtn;
@property (weak, nonatomic) IBOutlet UIButton *skinBtn;
@property (weak, nonatomic) IBOutlet UISwitch *faceNormalBtn;
@property (weak, nonatomic) IBOutlet UISwitch *mirrorBtn;
@property (weak, nonatomic) IBOutlet UISlider *velocitySlider;
@property (weak, nonatomic) IBOutlet UILabel *velocityLbl;
@property (weak, nonatomic) IBOutlet UISegmentedControl *physicsSegment;
@property (weak, nonatomic) IBOutlet UIButton *directionBtn;

- (IBAction)reflectionChangeEnded:(id)sender;

- (IBAction)refractionChangeEnded:(id)sender;

- (IBAction)refractionValueChanged:(id)sender;
- (IBAction)reflectionValueChanged:(id)sender;
- (IBAction)lightingSwitchChanged:(id)sender;
- (IBAction)visibleValueChanged:(id)sender;
- (IBAction)cloneButtonAction:(id)sender;
- (IBAction)deleteBtnAction:(id)sender;
- (IBAction)skinBtnAction:(id)sender;
- (IBAction)faceNormalAction:(id)sender;
- (IBAction)reflectionHqBtnAction:(id)sender;
- (IBAction)refractionHqBtnAction:(id)sender;
- (IBAction)mirrorBtnAction:(id)sender;
- (IBAction)velocityValueChanged:(id)sender;
- (IBAction)setDirection:(id)sender;
- (IBAction)physicsSegmentChanged:(id)sender;

- (IBAction)velocityChangeEnded:(id)sender;

@end

#endif
