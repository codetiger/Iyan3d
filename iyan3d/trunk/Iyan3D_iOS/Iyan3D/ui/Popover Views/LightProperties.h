//
//  LightProperties.h
//  Iyan3D
//
//  Created by sabish on 05/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GetPixelDemo.h"
#import "Vector3.h"
#import "Constants.h"


@protocol LightPropertiesDelegate

- (void) changeLightProps:(Quaternion)lightProps Distance:(float)distance isStoredProperty:(BOOL)isStored;
@end

@interface LightProperties : UIViewController<GetPixelDemoDelegate>
{
    Quaternion color;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil LightColor:(Quaternion)scene;

@property (weak, nonatomic) IBOutlet UIView *colorPickerView;
@property (strong, nonatomic) GetPixelDemo *demoView;
@property (weak, nonatomic) IBOutlet UIView *colorPreview;
@property (strong, nonatomic) id <LightPropertiesDelegate> delegate;
@property (weak, nonatomic) IBOutlet UISlider *shadowDarkness;
@property (weak, nonatomic) IBOutlet UISlider *distance;

@end
