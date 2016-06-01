//
//  FollowUsVC.m
//  Iyan3D
//
//  Created by Karthik on 31/05/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "FollowUsVC.h"

@implementation FollowUsVC


- (IBAction)fbFollowAction:(id)sender {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://www.facebook.com/iyan3d"]];
}

- (IBAction)gPlusFollowAction:(id)sender {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://plus.google.com/communities/108544248627724394883"]];
}

- (IBAction)twitterFollowAction:(id)sender {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://twitter.com/smackallgames"]];
}

- (IBAction)youtubeSubscribe:(id)sender {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://www.youtube.com/user/SmackallGames"]];
}
@end
