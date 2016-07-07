//
//  Options.h
//  Iyan3D
//
//  Created by Karthik on 05/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef Options_h
#define Options_h

#import <Foundation/Foundation.h>

@interface Options : NSObject {
}

@property (atomic, assign) int type;
@property (atomic, assign) int subPropsCount;
@property (atomic, copy) NSString* title;
@property (atomic, copy) NSArray* subProps;
@property (atomic, assign) float x;
@property (atomic, assign) float y;
@property (atomic, assign) float z;
@property (atomic, assign) float w;
@property (atomic, assign) SEL selector;
@property (atomic, assign) BOOL isEnabled;

@end

#endif /* Options_h */
