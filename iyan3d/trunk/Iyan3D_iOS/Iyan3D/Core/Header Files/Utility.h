//
//  Utility.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 04/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CommonCrypto/CommonCryptor.h>
#import <CommonCrypto/CommonKeyDerivation.h>

#import "CacheSystem.h"

#define CORNER_RADIUS 8.0

@interface Utility : NSObject {
    CacheSystem* cache;
    
}



+ (NSMutableData*)decryptData:(NSData*)data Password:(NSString*)pwd;
+ (NSMutableData*)decryptData1:(NSData*)data Password:(NSString*)pwd;
+ (NSString*)getMD5ForString:(NSString*)key;

+ (void)AppendInt:(NSMutableData*)data Value:(int)value;
+ (void)AppendFloat:(NSMutableData*)data Value:(float)value;
+ (void)AppendBool:(NSMutableData*)data Value:(BOOL)value;
+ (void)AppendString:(NSMutableData*)data Value:(NSString*)value;

+ (int)ReadInt:(NSData*)data at:(int*)location;
+ (BOOL)ReadBool:(NSData*)data at:(int*)location;
+ (float)ReadFloat:(NSData*)data at:(int*)location;
+ (NSString*)ReadString:(NSData*)data at:(int*)location;
+ (NSString*)encryptString:(NSString*)dataStr password:(NSString*)pwd;

+ (bool)IsPadDevice;
+ (float)GetDisplaceRatio;

+ (NSString*)stringWithHexBytes:(NSMutableData*)_data;
+ (NSData*)decodeFromHexidecimal:(NSString*)str;
+ (NSString*)decryptString:(NSString*)str password:(NSString*)pwd;
@end
