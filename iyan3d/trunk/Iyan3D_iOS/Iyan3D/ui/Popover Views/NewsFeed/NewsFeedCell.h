//
//  NewsFeedCell.h
//  Iyan3D
//
//  Created by Karthik on 30/06/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface NewsFeedCell : UITableViewCell
{
    NSString* thumbPath;
}

@property (assign) int feedId;
@property (weak, nonatomic) IBOutlet UIImageView *thumbImage;
@property (weak, nonatomic) IBOutlet UILabel *titleLbl;
@property (weak, nonatomic) IBOutlet UILabel *msgLbl;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *thumbActivity;


- (void) setImageWithUrl:(NSString*) thumbUrl InQueue:(NSOperationQueue*) queue;
@end
