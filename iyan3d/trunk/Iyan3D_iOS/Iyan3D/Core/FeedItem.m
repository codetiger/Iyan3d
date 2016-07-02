//
//  AssetItem.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 03/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "FeedItem.h"

@implementation FeedItem

-(void) dealloc{
    _itemId = 0;
	_type = 0;
	_title = nil;
	_message = nil;
	_url = nil;
	_thumbImage = nil;
    _isRead = false;
}
@end
