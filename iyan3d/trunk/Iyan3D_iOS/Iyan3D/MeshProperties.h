//
//  MeshProperties.h
//  Iyan3D
//
//  Created by Sankar on 11/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
@protocol MeshPropertiesDelegate
- (void)meshPropertyChanged:(float)refraction Reflection:(float)reflection Lighting:(BOOL)light Visible:(BOOL)visible FaceNormal:(BOOL)isHaveFaceNormal;
-(void) deleteDelegateAction;
-(void)cloneDelegateAction;
-(void)changeSkinDelgate;
- (void) switchMirror;
@end

@interface MeshProperties : UIViewController{
    float refractionValue;
    float reflectionValue;
    bool isLightningValue;
    bool isVisibleValue;
    bool isFaceNormal;
    int mirrorStatus;
    
}
- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil RefractionValue:(float)refraction ReflectionValue:(float)reflection LightningValue:(BOOL)lightningValue Visibility:(BOOL)isVisible MirrorState:(int)mirrorState;

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




@end
