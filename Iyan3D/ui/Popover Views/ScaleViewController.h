//
//  ScaleViewController.h
//  Iyan3D
//
//  Created by crazycubes on 21/05/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ScalePropertiesViewControllerDelegate
- (void)scaleValueForAction:(float)XValue YValue:(float)YValue ZValue:(float)ZValue;
- (void)scalePropertyChanged:(float)XValue YValue:(float)YValue ZValue:(float)ZValue;
@end

@interface ScaleViewController : UIViewController {
    float currentXValue, currentYValue, currentZValue;
}

@property (weak, nonatomic) IBOutlet UISlider* xSlider;
@property (weak, nonatomic) IBOutlet UISlider* ySlider;
@property (weak, nonatomic) IBOutlet UISlider* zSlider;

@property (weak, nonatomic) IBOutlet UISwitch* xyzLock;
@property (weak, nonatomic) IBOutlet UIImageView* lockImage;
@property (weak, nonatomic) IBOutlet UIImageView* bracketImage;
@property (weak, nonatomic) IBOutlet UILabel* xValue;
@property (weak, nonatomic) IBOutlet UILabel* yValue;
@property (weak, nonatomic) IBOutlet UILabel* zValue;

@property (weak, nonatomic) id<ScalePropertiesViewControllerDelegate> delegate;

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil updateXValue:(float)XValue updateYValue:(float)YValue updateZValue:(float)ZValue;
- (IBAction)settingsValueChangedAction:(UISlider*)sender;
- (IBAction)scaleLockSwtichChangedAction:(id)sender;
- (void)updateScale:(float)xValue yScale:(float)yValue zScale:(float)zValue;

@end
