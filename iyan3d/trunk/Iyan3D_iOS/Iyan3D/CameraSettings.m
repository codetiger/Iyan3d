//
//  CameraSettings.m
//  Iyan3D
//
//  Created by Sankar on 11/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "CameraSettings.h"

@interface CameraSettings ()

@end

@implementation CameraSettings

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil FOVvalue:(float)fovValue ResolutionType:(NSInteger)resolutionType{
    
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self)
    {
        resolutionTypeCamera=resolutionType;
        fovValueCamera=fovValue;
    }
    return self;
}


- (void)viewDidLoad {
    [super viewDidLoad];
    _fovLabel.text = [NSString stringWithFormat:@"%g", fovValueCamera];
    [_fovSlider setValue:fovValueCamera];
    _resolutionBtn.selectedSegmentIndex=resolutionTypeCamera;
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (IBAction)resolutionValueChanged:(id)sender {
    resolutionTypeCamera=_resolutionBtn.selectedSegmentIndex;
    [self.delegate cameraPropertyChanged:fovValueCamera Resolution:resolutionTypeCamera];
}

- (IBAction)fovValueChanged:(id)sender {
    _fovLabel.text = [NSString stringWithFormat:@"%g", _fovSlider.value];
    fovValueCamera=_fovSlider.value;
    [self.delegate cameraPropertyChanged:fovValueCamera Resolution:resolutionTypeCamera];
}

- (IBAction)deleteAction:(id)sender
{
    [self.delegate deleteObjectOrAnimation];
}


@end
