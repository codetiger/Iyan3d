//
//  CameraSettings.h
//  Iyan3D
//
//  Created by Sankar on 11/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol CameraSettingsDelegate
- (void)cameraPropertyChanged:(float)fov Resolution:(NSInteger)resolution;
- (void) deleteObjectOrAnimation;
@end

@interface CameraSettings : UIViewController{
    float fovValueCamera;
    NSInteger resolutionTypeCamera;
}
@property (weak, nonatomic) IBOutlet UISlider *fovSlider;
@property (weak, nonatomic) IBOutlet UILabel *fovLabel;
@property (strong, nonatomic) id <CameraSettingsDelegate> delegate;
@property (weak, nonatomic) IBOutlet UISegmentedControl *resolutionBtn;

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil FOVvalue:(float)fovValue ResolutionType:(NSInteger)resolutionType;

- (IBAction)resolutionValueChanged:(id)sender;
- (IBAction)fovValueChanged:(id)sender;
- (IBAction)deleteAction:(id)sender;
@end
