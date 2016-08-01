//
//  CommonProps.h
//  Iyan3D
//
//  Created by Karthik on 05/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import "GetPixelDemo.h"
#import "Utility.h"
#import "Options.h"
#import "Constants.h"
#import "SliderPropCell.h"
#import "SwitchPropCell.h"
#include "SegmentCell.h"
#include "TexturePropCell.h"

@protocol CommonPropDelegate

- (void) dismissView:(UIViewController*) VC WithProperty:(Property) physicsProp;
- (void) changedPropertyAtIndex:(PROP_INDEX) index WithValue:(Vector4) value AndStatus:(BOOL) status;
- (void) applyPhysicsProps:(Property) property;

- (void)meshPropertyChanged:(float)refraction Reflection:(float)reflection Lighting:(BOOL)light Visible:(BOOL)visible storeInAction:(BOOL)status;
- (void) deleteDelegateAction;
- (void)cloneDelegateAction;
- (void)changeSkinDelgate;
- (void) switchMirror;
- (void) setPhysics:(bool)status;
- (void) setPhysicsType:(int)type;
- (void) velocityChanged:(double)vel;
- (void) setDirection;
- (void) showLoadingActivity;
- (void) hideLoadingActivity;
- (void) setUserInteractionStatus:(BOOL) status;
- (void) setTextureSmoothStatus:(BOOL) status;

@end

@interface CommonProps : GAITrackedViewController < UITableViewDelegate, UITableViewDataSource , GetPixelDemoDelegate, SwitchPropDelegate, SliderPropDelegate>
{
    NSMutableArray* sectionHeaders;
    std::map< string , vector< Property > > groupedData;
    
    
    NSMutableArray* subSectionHeaders;
    std::map< string , vector< Property > > subGroupedData;
    
    UITableViewCell* selectedCell;
    Property selectedParentProp;
    Property physicsProperty;
    Property selectedListProp;
    
    Vector4 colorValue;
}

@property (weak, nonatomic) id < CommonPropDelegate > delegate;

@property (strong, nonatomic) GetPixelDemo *demoView;

@property (weak, nonatomic) IBOutlet UIView *colorView;

@property (weak, nonatomic) IBOutlet UIView *colorPickerView;

@property (weak, nonatomic) IBOutlet UIView *selectedColorView;

@property (weak, nonatomic) IBOutlet UIView *subPropView;

@property (weak, nonatomic) IBOutlet UITableView *subPropTable;

@property (weak, nonatomic) IBOutlet UITableView *propTableView;

- (IBAction)backPressed:(id)sender;

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil WithProps:(std::map < PROP_INDEX, Property >) props;

@end
