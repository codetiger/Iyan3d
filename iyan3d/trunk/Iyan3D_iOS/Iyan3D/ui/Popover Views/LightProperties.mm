//
//  LightProperties.m
//  Iyan3D
//
//  Created by sabish on 05/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "LightProperties.h"

@interface LightProperties ()

@end

@implementation LightProperties

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil LightColor:(Quaternion)currentLightColor {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        color = currentLightColor;
    }
    return self;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    [self initializeColorWheel];
    [self.colorPreview setBackgroundColor:[UIColor colorWithRed:color.x green:color.y blue:color.z alpha:1.0]];

}

- (void)viewDidAppear:(BOOL)animated{
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    
}

- (void)initializeColorWheel
{
    UIImage* theImage = [UIImage imageNamed:@"wheel2.png"];
    _demoView = [[GetPixelDemo alloc] initWithFrame:CGRectMake(0, 0, 240, ([Utility IsPadDevice]) ? 130 : 88 ) image:[ANImageBitmapRep imageBitmapRepWithImage:theImage]];
    _demoView.delegate = self;
    [self.colorPickerView addSubview:_demoView];
}

- (void)pixelDemoGotPixel:(BMPixel)pixel
{
    color = Quaternion(pixel.red,pixel.green,pixel.blue,_shadowDarkness.value);
    [self.colorPreview setBackgroundColor:[UIColor colorWithRed:pixel.red green:pixel.green blue:pixel.blue alpha:pixel.alpha]];
    [self lightPropsChangeAction];
}

- (void)pixelDemoTouchEnded:(BMPixel)pixel
{
    [self.colorPreview setBackgroundColor:[UIColor colorWithRed:pixel.red green:pixel.green blue:pixel.blue alpha:pixel.alpha]];
    [self lightPropsEndAction];
}

- (IBAction)shadowDarknessChange:(id)sender {
    color = Quaternion(color.x,color.y,color.z,_shadowDarkness.value);
    [self lightPropsChangeAction];
}

- (IBAction)shadowDarknessEnd:(id)sender {
    color = Quaternion(color.x,color.y,color.z,_shadowDarkness.value);
    [self lightPropsEndAction];
}

- (IBAction)distanceChangeAction:(id)sender {
    [self lightPropsChangeAction];
}

- (IBAction)distanceEndAction:(id)sender {
    [self lightPropsEndAction];
}

- (void)lightPropsChangeAction{
    [_delegate changeLightProps:color Distance:_distance.value isStoredProperty:NO];
}

- (void)lightPropsEndAction{
    [_delegate changeLightProps:color Distance:_distance.value isStoredProperty:YES];
}

- (IBAction)deleteAction:(id)sender {
    [self.delegate deleteObjectOrAnimation];
}



@end
