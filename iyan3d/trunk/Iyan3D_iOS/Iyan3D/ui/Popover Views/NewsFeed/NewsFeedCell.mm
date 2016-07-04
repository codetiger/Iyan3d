//
//  NewsFeedCell.m
//  Iyan3D
//
//  Created by Karthik on 30/06/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "NewsFeedCell.h"
#import "DownloadTask.h"

@implementation NewsFeedCell

- (void)awakeFromNib {
    [super awakeFromNib];
    // Initialization code
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated {
    [super setSelected:selected animated:animated];

    // Configure the view for the selected state
}

- (void) setImageWithUrl:(NSString*) thumbImageName InQueue:(NSOperationQueue*) queue
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [paths objectAtIndex:0];
    thumbPath = [NSString stringWithFormat:@"%@/feed_%d.png", cacheDirectory, _feedId];
    if(![[NSFileManager defaultManager] fileExistsAtPath:thumbPath]) {
        NSString *thumbUrl = [NSString stringWithFormat:@"https://www.iyan3dapp.com/appapi/newsfeed/%@", thumbImageName];
        DownloadTask* d = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(downloadCompleted:) returnObject:[NSNumber numberWithInt:_feedId] outputFilePath:thumbPath andURL:thumbUrl];
        d.taskType = DOWNLOAD_AND_WRITE_IMAGE;
        d.queuePriority = NSOperationQueuePriorityNormal;
        [queue addOperation:d];
    } else {
        [self.thumbImage setImage:[UIImage imageWithContentsOfFile:thumbPath]];
    }
}

- (void) downloadCompleted:(NSNumber*) returnId
{
    if([returnId intValue] == _feedId) {
        if([[NSFileManager defaultManager] fileExistsAtPath:thumbPath])
            [self.thumbImage setImage:[UIImage imageWithContentsOfFile:thumbPath]];
    }
}

@end
