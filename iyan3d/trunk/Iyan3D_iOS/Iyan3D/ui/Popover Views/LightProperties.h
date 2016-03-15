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
#import "Vector3.h"
#import "Constants.h"
#import "Utility.h"



@protocol LightPropertiesDelegate

- (void) changeLightProps:(Quaternion)lightProps Distance:(float)distance isStoredProperty:(BOOL)isStored;
- (void) deleteObjectOrAnimation;
@end

@interface LightProperties : GAITrackedViewController<GetPixelDemoDelegate>
{
    Quaternion color;
    NODE_TYPE light;
    float previousDistance;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil LightColor:(Quaternion)currentLightColor LightType:(NODE_TYPE)lightType Distance:(float)distance;

@property (weak, nonatomic) IBOutlet UIView *colorPickerView;
@property (strong, nonatomic) GetPixelDemo *demoView;
@property (weak, nonatomic) IBOutlet UIView *colorPreview;
@property (strong, nonatomic) id <LightPropertiesDelegate> delegate;
@property (weak, nonatomic) IBOutlet UISlider *shadowDarkness;
@property (weak, nonatomic) IBOutlet UISlider *distance;
@property (weak, nonatomic) IBOutlet UIButton *deleteBtn;
@property (weak, nonatomic) IBOutlet UILabel *distanceLable;

@end
