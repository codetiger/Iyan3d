//
//  NewsFeedVC.h
//  Iyan3D
//
//  Created by Karthik on 30/06/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "FeedItem.h"
#import "CacheSystem.h"
#import "GAI.h"

@protocol NewsFeedDelegate

- (void) performActionForSelectedFeed:(FeedItem*) feed;

@end

@interface NewsFeedVC : GAITrackedViewController < UITableViewDelegate, UITableViewDataSource >
{
    NSOperationQueue *downloadQueue;
    NSMutableArray *feedItems;
    CacheSystem * cache;
}

@property (weak, nonatomic) IBOutlet UITableView *feedTableView;
@property (weak, nonatomic) IBOutlet UIButton *clearAllBtn;
@property (weak, nonatomic) id <NewsFeedDelegate> delegate;
- (IBAction)clearAllAction:(id)sender;
@property (weak, nonatomic) IBOutlet UILabel *newsFeedTitleLabel;

@end
