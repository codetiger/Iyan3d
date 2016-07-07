//
//  AssetItem.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 03/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "Options.h"

@implementation Options

-(void) dealloc{
    _title = nil;
    if(_subProps != nil) {
        _subProps = nil;
    }
    _selector = nil;
}
@end
