//
//  AssetItem.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 03/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface FeedItem : NSObject

@property (atomic, assign) int itemId;
@property (atomic, assign) int type;
@property (atomic, copy) NSString* title;
@property (atomic, copy) NSString* message;
@property (atomic, copy) NSString* url;
@property (atomic, copy) NSString* thumbImage;
@property (atomic, assign) bool isRead;

@end
