//
//  NewsFeedVC.m
//  Iyan3D
//
//  Created by Karthik on 30/06/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "NewsFeedVC.h"
#import "AppHelper.h"
#import "AFHTTPRequestOperation.h"
#import "AFHTTPClient.h"
#import "NewsFeedCell.h"

@interface NewsFeedVC ()

@end

@implementation NewsFeedVC

- (void)viewDidLoad
{
    [super viewDidLoad];
    //[self.feedTableView registerClass:[NewsFeedCell class] forCellReuseIdentifier:@"NewsFeedCell"];
    downloadQueue = [[NSOperationQueue alloc] init];
    [downloadQueue setMaxConcurrentOperationCount:3];
    NSString *nibName = @"NewsFeedCell";
    if(![Utility IsPadDevice])
        nibName = @"NewsFeedCellPhone";
    
    [self.feedTableView registerNib:[UINib nibWithNibName:nibName bundle:nil] forCellReuseIdentifier:nibName];
    cache = [CacheSystem cacheSystem];
    [self setTableDataArray];
    
    [self.newsFeedTitleLabel setText:NSLocalizedString(@"News Feed", nil)];
    [self.clearAllBtn setTitle:NSLocalizedString(@"Clear All", nil) forState:UIControlStateNormal];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void) loadNewsFeedData
{
    
    NSURL *url = [NSURL URLWithString:@"https://www.iyan3dapp.com/appapi/newsfeed.php"];
    NSString *postPath = @"https://www.iyan3dapp.com/appapi/newsfeed.php";
    
    NSString* lastid = @"0";
    if([[AppHelper getAppHelper] userDefaultsForKey:@"lastfeedid"])
        lastid = [[AppHelper getAppHelper] userDefaultsForKey:@"lastfeedid"];
    
    AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:lastid, @"lastid", nil]];
    
    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        NSError * jsonError;
        NSArray* jsonArray = [[NSArray alloc] init];
        jsonArray = [NSJSONSerialization JSONObjectWithData:responseObject options:NSJSONReadingAllowFragments error:&jsonError];
        if(!jsonError && [jsonArray count] > 0) {
            for(int i = 0; i < [jsonArray count]; i++) {
                FeedItem *f = [[FeedItem alloc] init];
                f.itemId = [[[jsonArray objectAtIndex:i] objectForKey:@"id"] intValue];
                f.type = [[[jsonArray objectAtIndex:i] objectForKey:@"type"] intValue];
                f.title = [[jsonArray objectAtIndex:i] objectForKey:@"title"];
                f.message = [[jsonArray objectAtIndex:i] objectForKey:@"message"];
                f.url = [[jsonArray objectAtIndex:i] objectForKey:@"url"];
                f.thumbImage = [[jsonArray objectAtIndex:i] objectForKey:@"thumbnailurl"];
                f.isRead = false;
                [cache addNewsFeed:f];
                
                if(i == 0) {
                    [[AppHelper getAppHelper] saveToUserDefaults:[NSString stringWithFormat:@"%d", f.itemId] withKey:@"lastfeedid"];
                }
                
                [self performSelectorOnMainThread:@selector(setTableDataArray) withObject:nil waitUntilDone:NO];
            }
        } else {
            NSLog(@" Json Error %@ ", jsonError);
            [self performSelectorOnMainThread:@selector(setTableDataArray) withObject:nil waitUntilDone:NO];
        }
    }
                                     failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                         NSLog(@"Failure: %@", error.localizedDescription);
                                         UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:NSLocalizedString(@"Failure", nil) message:NSLocalizedString(@"unable_connect_server", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:nil];
                                         [userNameAlert show];
                                     }];
    [operation start];

}

 - (void) setTableDataArray
{
    feedItems = [cache getNewsFeedsFromLocal];
    [self.feedTableView reloadData];
    
    for(int i = 0; i < [feedItems count]; i ++) {
        FeedItem *f = [feedItems objectAtIndex:i];
        f.isRead = true;
        [cache updateFeed:f.itemId WithStatus:f.isRead];
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [feedItems count];
}

- (NewsFeedCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSString* identitifier = @"NewsFeedCell";
    if(![Utility IsPadDevice])
        identitifier = @"NewsFeedCellPhone";
    
    NewsFeedCell *cell = (NewsFeedCell *)[tableView dequeueReusableCellWithIdentifier:identitifier];
    FeedItem* f = [feedItems objectAtIndex:indexPath.row];
    cell.feedId = f.itemId;
    cell.titleLbl.text = f.title;
    cell.msgLbl.text = f.message;
    [cell setImageWithUrl:f.thumbImage InQueue:downloadQueue];
    return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.row < [feedItems count]) {
        FeedItem *f = feedItems[indexPath.row];
        [self.delegate performActionForSelectedFeed:f];
        
    }
}

- (void) dealloc
{
    feedItems = nil;
}

- (IBAction)clearAllAction:(id)sender
{
    [cache clearAllFeeds];
    [self setTableDataArray];
}

@end
