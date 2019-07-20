//
//  ScaleForAutoRigViewController.h
//  Iyan3D
//
//  Created by crazycubes on 02/07/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"

@protocol AutoRigScaleViewControllerDelegate
- (void)scalePropertyChangedInRigView:(float)scaleValue;
- (void)scaleValueForAction:(float)scaleValue;

@end

@interface ScaleForAutoRigViewController : GAITrackedViewController {
    float scale;
}

@property (weak, nonatomic) IBOutlet UISlider* scaleSlider;
@property (weak, nonatomic) IBOutlet UILabel* scaleValue;

@property (weak, nonatomic) id<AutoRigScaleViewControllerDelegate> delegate;
- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil updateScale:(float)currentEnvelopeScale;
- (void)updateScale:(float)scaleValue;
- (IBAction)settingsValueChangedAction:(id)sender;
- (IBAction)changeSliderStarted:(id)sender;

@end
