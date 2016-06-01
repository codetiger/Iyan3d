//
//  FollowUsVC.h
//  Iyan3D
//
//  Created by Karthik on 31/05/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef FollowUsVC_h
#define FollowUsVC_h

#import <UIKit/UIKit.h>
#import "GAI.h"

@interface FollowUsVC : GAITrackedViewController

- (IBAction)fbFollowAction:(id)sender;
- (IBAction)gPlusFollowAction:(id)sender;
- (IBAction)twitterFollowAction:(id)sender;
- (IBAction)youtubeSubscribe:(id)sender;

@end

#endif /* FollowUsVC_h */
