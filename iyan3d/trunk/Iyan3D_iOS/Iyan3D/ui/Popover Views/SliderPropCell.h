//
//  SliderPropCell.h
//  Iyan3D
//
//  Created by Karthik on 12/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Constants.h"

@protocol SliderPropDelegate

- (void) actionMadeInTable:(int) tableIndex AtIndexPath:(NSIndexPath*) indexPath WithValue:(Vector4) value AndStatus:(BOOL) status;

@end

@interface SliderPropCell : UITableViewCell

@property (assign) int tableIndex;
@property (weak, nonatomic) id < SliderPropDelegate > delegate;
@property (strong, nonatomic) NSIndexPath* indexPath;

@property (weak, nonatomic) IBOutlet UILabel *title;
@property (weak, nonatomic) IBOutlet UISlider *slider;
- (IBAction)sliderValueChanged:(id)sender;
- (IBAction)sliderChangeEnds:(id)sender;

@end
