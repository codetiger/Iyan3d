//
//  AFAPIClient.h
//  Talent
//
//  Created by Amit Upadhyay on 10/06/13.
//  Copyright (c) 2013 Amit Upadhyay. All rights reserved.
//

#import "AFAPIClient.h"
#import "AFNetworking.h"
#import "AFJSONRequestOperation.h"
//#import "Defines.h"
#define BaseUrl @"https://iyan3dapp.com/appapi/"
@implementation AFAPIClient

+ (AFAPIClient *)sharedClient {
    static AFAPIClient *_sharedClient = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _sharedClient = [[AFAPIClient alloc] initWithBaseURL:[NSURL URLWithString:BaseUrl]];
    });
    return _sharedClient;
}

- (id)initWithBaseURL:(NSURL *)url {
    self = [super initWithBaseURL:url];
    if (!self) {
        return nil;
    }
    [self registerHTTPOperationClass:[AFJSONRequestOperation class]];
    [AFJSONRequestOperation addAcceptableContentTypes:[NSSet setWithObject:@"text/html"]];
    [self setDefaultHeader:@"Accept" value:@"application/json"];
    return self;
}


@end
