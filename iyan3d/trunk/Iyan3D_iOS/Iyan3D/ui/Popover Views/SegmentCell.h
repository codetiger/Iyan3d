//
//  SegmentCell.h
//  Iyan3D
//
//  Created by Karthik on 13/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SegmentCell : UITableViewCell

@property (weak, nonatomic) IBOutlet UILabel *titleLabel;
@property (weak, nonatomic) IBOutlet UISegmentedControl *segmentCtrl;
- (IBAction)segmentChanged:(id)sender;

@end
