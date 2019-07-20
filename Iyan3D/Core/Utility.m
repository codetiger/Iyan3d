//
//  Utility.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 04/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "Utility.h"

@implementation Utility

+ (NSData*)getDataFromURL:(NSURL*)url {
    NSURLRequest* request = [NSURLRequest requestWithURL:url];
    if ([NSURLConnection connectionWithRequest:request delegate:self]) {
        NSError* error = nil;
        NSData*  res   = [NSData dataWithContentsOfURL:url options:0 error:&error];
        if (!error)
            return res;
        else {
            NSLog(@"Error Connecting URL: %@\n", error.debugDescription);
        }
    }
    return nil;
}

+ (NSMutableData*)decryptData:(NSData*)data Password:(NSString*)pwd {
    if (data == nil)
        return nil;

    const CCAlgorithm kAlgorithm          = kCCAlgorithmAES128;
    const NSUInteger  kAlgorithmBlockSize = kCCBlockSizeAES128;

    NSString* password = [self getMD5ForString:pwd];
    NSData*   iv       = [[NSData alloc] init];

    CCOperation operation = kCCDecrypt;
    NSData*     key       = [password dataUsingEncoding:NSUTF8StringEncoding];

    size_t         outLength;
    NSMutableData* cipherData = [NSMutableData dataWithLength:data.length + kAlgorithmBlockSize];

    CCCryptorStatus result = CCCrypt(operation, kAlgorithm, kCCOptionPKCS7Padding, key.bytes, key.length, iv.bytes, data.bytes, data.length + kAlgorithmBlockSize, cipherData.mutableBytes, cipherData.length, &outLength);

    if (result == kCCSuccess) {
        return cipherData;
    } else {
        return nil;
    }
}

+ (NSMutableData*)decryptData1:(NSData*)data Password:(NSString*)pwd {
    if (data == nil) {
        return nil;
    }
    const CCAlgorithm kAlgorithm          = kCCAlgorithmAES128;
    const NSUInteger  kAlgorithmBlockSize = kCCBlockSizeAES128;

    NSString* password = [self getMD5ForString:pwd];
    NSData*   iv       = NULL;

    CCOperation operation = kCCDecrypt;
    NSData*     key       = [password dataUsingEncoding:NSUTF8StringEncoding];

    size_t         outLength;
    NSMutableData* cipherData = [NSMutableData dataWithLength:data.length + kAlgorithmBlockSize];

    CCCryptorStatus result = CCCrypt(operation, kAlgorithm, 0, key.bytes, key.length, iv.bytes, data.bytes, data.length, cipherData.mutableBytes, cipherData.length, &outLength);

    if (result == kCCSuccess) {
        return cipherData;
    } else {
        return nil;
    }
}

+ (NSString*)encryptString:(NSString*)Str password:(NSString*)pwd {
    const CCAlgorithm kAlgorithm          = kCCAlgorithmAES128;
    const NSUInteger  kAlgorithmBlockSize = kCCBlockSizeAES128;

    NSString* password = [self getMD5ForString:pwd];
    //	NSData *iv = [[NSData alloc] init];

    CCOperation operation = kCCEncrypt;
    NSData*     key       = [password dataUsingEncoding:NSUTF8StringEncoding];
    NSData*     data      = [Str dataUsingEncoding:NSUTF8StringEncoding];

    NSData*        iv = NULL;
    size_t         outLength;
    NSMutableData* cipherData = [NSMutableData dataWithLength:data.length + kAlgorithmBlockSize];

    CCCryptorStatus result = CCCrypt(operation, kAlgorithm, kCCOptionPKCS7Padding, key.bytes, key.length, iv.bytes, data.bytes, data.length, cipherData.mutableBytes, cipherData.length, &outLength);

    //	 NSString* jsonStr = [NSString stringWithUTF8String:(const char*)[cipherData bytes]];
    if (result == kCCSuccess) {
        cipherData.length      = outLength;
        NSString* encryptedStr = [self stringWithHexBytes:cipherData];
        return encryptedStr;
    } else {
        return nil;
    }
}

+ (NSString*)stringWithHexBytes:(NSMutableData*)_data {
    NSMutableString*     stringBuffer = [NSMutableString stringWithCapacity:([_data length] * 2)];
    const unsigned char* dataBuffer   = (const unsigned char*)[_data bytes];
    int                  i;
    for (i = 0; i < [_data length]; ++i) {
        [stringBuffer appendFormat:@"%02lX", (unsigned long)dataBuffer[i]];
    }
    return [stringBuffer copy];
}

+ (NSData*)decodeFromHexidecimal:(NSString*)str {
    NSString* command            = [NSString stringWithString:str];
    command                      = [command stringByReplacingOccurrencesOfString:@" " withString:@""];
    NSMutableData* commandToSend = [[NSMutableData alloc] init];
    unsigned char  whole_byte;
    char           byte_chars[3] = { '\0', '\0', '\0' };
    int            i;
    for (i = 0; i < [command length] / 2; i++) {
        byte_chars[0] = [command characterAtIndex:i * 2];
        byte_chars[1] = [command characterAtIndex:i * 2 + 1];
        whole_byte    = strtol(byte_chars, NULL, 16);
        [commandToSend appendBytes:&whole_byte length:1];
    }
    return commandToSend;
}

+ (NSString*)decryptString:(NSString*)str password:(NSString*)pwd {
    NSData* inputData = [self decodeFromHexidecimal:str];
    //	NSData *inputData = [str dataUsingEncoding:NSUTF8StringEncoding];
    NSMutableData* outputData = [self decryptData1:inputData Password:pwd];
    return [[NSString alloc] initWithData:outputData encoding:NSUTF8StringEncoding];
}

+ (NSString*)getMD5ForString:(NSString*)key {
    const char*   cStr = [key UTF8String];
    unsigned char result[CC_MD5_DIGEST_LENGTH];
    CC_MD5(cStr, strlen(cStr), result);
    NSMutableString* hash = [NSMutableString stringWithCapacity:CC_MD5_DIGEST_LENGTH * 2];
    for (int i = 0; i < CC_MD5_DIGEST_LENGTH; ++i) {
        [hash appendFormat:@"%02x", result[i]];
    }
    return [NSString stringWithString:hash];
}

+ (int)ReadInt:(NSData*)data at:(int*)location {
    int     v;
    NSRange r = NSMakeRange(*location, sizeof(int));
    (*location) += r.length;
    [data getBytes:&v range:r];
    return v;
}

+ (BOOL)ReadBool:(NSData*)data at:(int*)location {
    BOOL    v;
    NSRange r = NSMakeRange(*location, sizeof(BOOL));
    (*location) += r.length;
    [data getBytes:&v range:r];
    return v;
}

+ (float)ReadFloat:(NSData*)data at:(int*)location {
    float   v;
    NSRange r = NSMakeRange(*location, sizeof(float));
    (*location) += r.length;
    [data getBytes:&v range:r];
    return v;
}

+ (NSString*)ReadString:(NSData*)data at:(int*)location {
    char    v[128];
    NSRange r = NSMakeRange(*location, 128);
    (*location) += r.length;
    [data getBytes:v range:r];
    return [NSString stringWithFormat:@"%s", v];
}

+ (void)AppendInt:(NSMutableData*)data Value:(int)value {
    int v = value;
    [data appendBytes:&v length:sizeof(int)];
}

+ (void)AppendFloat:(NSMutableData*)data Value:(float)value {
    float v = value;
    [data appendBytes:&v length:sizeof(float)];
}

+ (void)AppendBool:(NSMutableData*)data Value:(BOOL)value {
    BOOL v = value;
    [data appendBytes:&v length:sizeof(BOOL)];
}

+ (void)AppendString:(NSMutableData*)data Value:(NSString*)value {
    char  v[128];
    char* n = (char*)[value UTF8String];
    sprintf(v, "%s", n);
    [data appendBytes:&v length:128];
}

+ (bool)IsPadDevice {
    return (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad);
}

+ (float)GetDisplaceRatio {
    if ([self IsPadDevice]) {
        return 1.0f;
    } else {
        return 0.65f;
    }
}

- (void)dealloc {
    cache = nil;
}

@end
