//
//  SceneItem.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 04/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface SceneItem : NSObject {
}

@property (atomic, assign) int sceneId;
@property (atomic, copy) NSString* sceneFile;
@property (atomic, copy) NSString* name;
@property (atomic, copy) NSString* createdDate;

@end
