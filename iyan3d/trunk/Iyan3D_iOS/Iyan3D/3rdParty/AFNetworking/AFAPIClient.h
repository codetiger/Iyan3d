//
//  AFAPIClient.h
//  Talent
//
//  Created by Amit Upadhyay on 10/06/13.
//  Copyright (c) 2013 Amit Upadhyay. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "AFHTTPClient.h"
@class AFHTTPClient;
@interface AFAPIClient : AFHTTPClient

+ (AFAPIClient *)sharedClient;
- (id)initWithBaseURL:(NSURL *)url;
@end
