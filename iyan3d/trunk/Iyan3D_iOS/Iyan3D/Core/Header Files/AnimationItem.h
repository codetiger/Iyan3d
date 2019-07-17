//
//  AnimationItem.h
//  Iyan3D
//
//  Created by karthik on 09/07/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#ifndef Iyan3D_AnimationItem_h
#define Iyan3D_AnimationItem_h

#import <Foundation/Foundation.h>

@interface AnimationItem : NSObject {
}

@property (atomic, assign) int     assetId;
@property (atomic, assign) int     type;
@property (atomic, assign) int     boneCount;
@property (atomic, assign) int     published;
@property (atomic, assign) int     rating;
@property (atomic, assign) int     downloads;
@property (atomic, assign) int     isViewerRated;
@property (atomic, assign) int     featuredindex;
@property (atomic, copy) NSString* userId;
@property (atomic, copy) NSString* assetName;
@property (atomic, copy) NSString* modifiedDate;
@property (atomic, copy) NSString* keywords;
@property (atomic, copy) NSString* userName;

@end

#endif
