//
//  RenderItem.h
//  Iyan3D
//
//  Created by vigneshkumar on 22/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface                              RenderItem : NSObject
@property (nonatomic, strong) NSString* taskName;
@property (nonatomic, strong) NSString* dateAdded;
@property (atomic, assign) int          taskId;
@property (atomic, assign) int          taskProgress;
@property (atomic, assign) int          taskFrames;

@end
