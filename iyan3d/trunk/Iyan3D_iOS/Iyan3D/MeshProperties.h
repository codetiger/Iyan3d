//
//  MeshProperties.h
//  Iyan3D
//
//  Created by Sankar on 11/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
@protocol MeshPropertiesDelegate
- (void)meshPropertyChanged:(float)brightness Specular:(float)specular Lighting:(BOOL)light Visible:(BOOL)visible;
@end

@interface MeshProperties : UIViewController{
    float brightnessValue;
    float specularValue;
    bool isLightningValue;
    bool isVisibleValue;
    
}
- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil BrightnessValue:(float)brightness SpecularValue:(float)specular LightningValue:(BOOL)lightningValue Visibility:(BOOL)isVisible;

@property (weak, nonatomic) IBOutlet UISlider *brightnessSlider;
@property (weak, nonatomic) IBOutlet UISlider *specularSlider;
@property (weak, nonatomic) IBOutlet UISlider *reflectionSlider;
@property (weak, nonatomic) IBOutlet UISlider *refractionSlider;
@property (weak, nonatomic) IBOutlet UISwitch *lightingSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *visibleChanged;
@property (strong, nonatomic) id <MeshPropertiesDelegate> delegate;

- (IBAction)brightnessValueChanged:(id)sender;
- (IBAction)specularValueChanged:(id)sender;
- (IBAction)reflectionValueChanged:(id)sender;
- (IBAction)refractionValueChanged:(id)sender;
- (IBAction)lightingSwitchChanged:(id)sender;
- (IBAction)visibleValueChanged:(id)sender;

@end
