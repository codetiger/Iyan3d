//
//  LightProperties.h
//  Iyan3D
//
//  Created by sabish on 05/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import "GetPixelDemo.h"
#import "Constants.h"
#import "Utility.h"
#import "Vector3.h"


@protocol LightPropertiesDelegate

- (void) changeLightProps:(Vector4)lightProps Distance:(float)distance LightType:(int)lightType isStoredProperty:(BOOL)isStored;
- (void) setLightDirection;
- (void) deleteObjectOrAnimation;
@end

@interface LightProperties : GAITrackedViewController<GetPixelDemoDelegate>
{
    Vector4 color;
    NODE_TYPE light;
    float previousDistance;
    int selectedSegmentIndex;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil LightColor:(Quaternion)currentLightColor NodeType:(NODE_TYPE)nodeType Distance:(float)distance LightType:(int)lightType;
@property (weak, nonatomic) IBOutlet UIButton *directionBtn;

@property (weak, nonatomic) IBOutlet UIView *colorPickerView;
@property (strong, nonatomic) GetPixelDemo *demoView;
@property (weak, nonatomic) IBOutlet UIView *colorPreview;
@property (strong, nonatomic) id <LightPropertiesDelegate> delegate;
@property (weak, nonatomic) IBOutlet UISlider *shadowDarkness;
@property (weak, nonatomic) IBOutlet UISlider *distance;
@property (weak, nonatomic) IBOutlet UIButton *deleteBtn;
@property (weak, nonatomic) IBOutlet UILabel *distanceLable;
@property (weak, nonatomic) IBOutlet UISegmentedControl *lightTypeSeg;
- (IBAction)setDirection:(id)sender;
- (IBAction)lightTypeChanged:(id)sender;

@end
