//
//  RenderItem.m
//  Iyan3D
//
//  Created by vigneshkumar on 22/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import "RenderItem.h"

@implementation RenderItem

@synthesize dateAdded = _dateAdded;

-(void) dealloc{
    _dateAdded = nil;
    _taskName = nil;
}


@end
