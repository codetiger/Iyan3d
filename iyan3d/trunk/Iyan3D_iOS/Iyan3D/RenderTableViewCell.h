//
//  RenderTableViewCell.h
//  Iyan3D
//
//  Created by Sankar on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface RenderTableViewCell : UITableViewCell
@property (weak, nonatomic) IBOutlet UILabel *dateLabel;
@property (weak, nonatomic) IBOutlet UIButton *downloadBtn;
@property (weak, nonatomic) IBOutlet UILabel *renderProgressLabel;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *downloadProgress;
@property (weak, nonatomic) IBOutlet UILabel *renderlabel;
@end
