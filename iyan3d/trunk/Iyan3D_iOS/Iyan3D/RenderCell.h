//
//  RenderCell.h
//  Iyan3D
//
//  Created by vigneshkumar on 19/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface RenderCell : UITableViewCell

@property (weak, nonatomic) IBOutlet UILabel *nameLabel;

@property (weak, nonatomic) IBOutlet UILabel *frameLabel;

@property (weak, nonatomic) IBOutlet UIProgressView *renProgress;

@property (weak, nonatomic) IBOutlet UIButton *downloadBtn;

- (IBAction)downloadOutput:(id)sender;

@end
