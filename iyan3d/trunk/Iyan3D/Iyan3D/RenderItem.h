//
//  RenderItem.h
//  Iyan3D
//
//  Created by vigneshkumar on 22/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface RenderItem : NSObject
@property (atomic, assign) NSString* taskName;
@property (atomic, assign) int taskId;
@property (atomic, assign) int taskProgress;
@property (atomic, assign) float taskTime;

@end
