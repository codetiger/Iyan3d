//
//  AssetItem.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 03/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface AssetItem : NSObject {
}

@property (atomic, assign) int assetId;
@property (atomic, assign) int type;
@property (atomic, assign) int boneCount;
@property (atomic, copy) NSString* name;
@property (atomic, copy) NSString* iap;
@property (atomic, copy) NSString* hash;
@property (atomic, copy) NSString* modifiedDate;
@property (atomic, copy) NSString* keywords;
@property (atomic, copy) NSString* price;

@end
