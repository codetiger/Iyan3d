//
//  OGLES2RenderManager.cpp
//  SGEngine2
//
//  Created by Harishankar on 15/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifdef IOS


#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/gl.h>
#import <AVFoundation/AVFoundation.h>

#import "ImageLoaderOBJC.h"

uint8_t* loadPNGImage(std::string filePath, int &width, int &height){
    NSString *fileName = [NSString stringWithFormat:@"%s",filePath.c_str()];
    CGImageRef spriteImage = [UIImage imageWithContentsOfFile:fileName].CGImage;
    
    if (!spriteImage) 
        NSLog(@"Failed to load image %@ FilePath %@", fileName,fileName);
    
    width = CGImageGetWidth(spriteImage);
    height = CGImageGetHeight(spriteImage);
    
    GLubyte * spriteData = (GLubyte *) calloc(width*height*4, sizeof(GLubyte));
    CGContextRef spriteContext = CGBitmapContextCreate(spriteData, width, height, 8, width*4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
    CGContextDrawImage(spriteContext, CGRectMake(0, 0, width, height), spriteImage);
    CGContextRelease(spriteContext);

    return spriteData;
}

void writePNGImage(uint8_t *imageData , int width , int height , char * filePath)
{
    
    size_t imageSize = 4 * size_t(width) * size_t(height);
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, imageData, imageSize, NULL);
    
    // prep the ingredients
    int bitsPerComponent = 8;
    int bitsPerPixel = 32;
    int bytesPerRow = 4 * width;
    CGColorSpaceRef colorSpaceRef = CGColorSpaceCreateDeviceRGB();
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
    CGColorRenderingIntent renderingIntent = kCGRenderingIntentDefault;
    
    // make the cgimage
    CGImageRef imageRef = CGImageCreate(width, height, bitsPerComponent, bitsPerPixel, bytesPerRow, colorSpaceRef, bitmapInfo, provider, NULL, NO, renderingIntent);
    
    // then make the uiimage from that
    UIImage *myImage = [UIImage imageWithCGImage:imageRef];

    [UIImagePNGRepresentation(myImage) writeToFile:[NSString stringWithUTF8String:filePath] atomically:YES];
    
        CGImageRelease(imageRef);
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(colorSpaceRef);
}

uint8_t* getImageDataFromVideo(string fileName, int frame, int &width, int &height)
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];

    NSString *videoFilePath = [documentsDirectory stringByAppendingPathComponent:[NSString stringWithFormat:@"%@%s",@"Resources/Videos/",fileName.c_str()]];
    if([[NSFileManager defaultManager] fileExistsAtPath:videoFilePath]) {
        NSURL *videoURL = [NSURL fileURLWithPath:videoFilePath];
        AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:videoURL options:nil];
        AVAssetImageGenerator *gen = [[AVAssetImageGenerator alloc] initWithAsset:asset];
        if(!gen)
            return NULL;
            
        gen.appliesPreferredTrackTransform = YES;
        gen.requestedTimeToleranceAfter =  kCMTimeZero;
        gen.requestedTimeToleranceBefore =  kCMTimeZero;

        NSError *err = NULL;
        double duration = [asset duration].value;
        int videoFrames = (int)(duration/24.0);
        int extraFrames = (videoFrames/24 > 0) ? videoFrames - ((videoFrames/24) * 24) : 24 - videoFrames;
        videoFrames = videoFrames - extraFrames;
        int x = frame/ videoFrames;
        int difference = frame  - (x * videoFrames);
        
        CMTime midpoint = CMTimeMake(difference, 24);

        CGImageRef imageRef = [gen copyCGImageAtTime:midpoint actualTime:NULL error:&err];
        
        width = CGImageGetWidth(imageRef);
        height = CGImageGetHeight(imageRef);

        float bigSide = (width >= height) ? width : height;
        float target = 0;
        
        if (bigSide <= 128)
            target = 128;
        else if (bigSide <= 256)
            target = 256;
        else if (bigSide <= 512)
            target = 512;
        else
            target = 1024;

        width = height = target;
        GLubyte* textureData = (GLubyte *)calloc(target * target * 4, sizeof(GLubyte));
        CGContextRef spriteContext = CGBitmapContextCreate(textureData, target, target, 8, target * 4, CGImageGetColorSpace(imageRef), kCGImageAlphaPremultipliedLast);
        CGContextDrawImage(spriteContext, CGRectMake(0, 0, target, target), imageRef);
        CGContextRelease(spriteContext);
        CGImageRelease(imageRef);
        return textureData;
    } else {
        NSLog(@"Failed to load Video %@ ", videoFilePath);
        return NULL;
    }
}

void* getImageContextFromVideo(string fileName, int frame, int &width, int &height)
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    
    NSString *videoFilePath = [documentsDirectory stringByAppendingPathComponent:[NSString stringWithFormat:@"%s",fileName.c_str()]];
    if([[NSFileManager defaultManager] fileExistsAtPath:videoFilePath]) {
        NSURL *videoURL = [NSURL fileURLWithPath:videoFilePath];
        AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:videoURL options:nil];
        AVAssetImageGenerator *gen = [[AVAssetImageGenerator alloc] initWithAsset:asset];
        if(!gen)
            return NULL;
        
        gen.appliesPreferredTrackTransform = YES;
        gen.requestedTimeToleranceAfter =  kCMTimeZero;
        gen.requestedTimeToleranceBefore =  kCMTimeZero;
        
        NSError *err = NULL;
        double duration = [asset duration].value;
        
        if(frame > duration/24.0) {
            
        }
        
        CMTime midpoint = CMTimeMake(frame, 24);
        
        CGImageRef imageRef = [gen copyCGImageAtTime:midpoint actualTime:NULL error:&err];
        
        width = CGImageGetWidth(imageRef);
        height = CGImageGetHeight(imageRef);
        
        float bigSide = (width >= height) ? width : height;
        float target = 0;
        
        if (bigSide <= 128)
            target = 128;
        else if (bigSide <= 256)
            target = 256;
        else if (bigSide <= 512)
            target = 512;
        else
            target = 1024;
        
        width = height = target;
        CGContextRef spriteContext = CGBitmapContextCreate(NULL, target, target, 8, target * 4, CGImageGetColorSpace(imageRef), kCGImageAlphaPremultipliedLast);
        CGContextDrawImage(spriteContext, CGRectMake(0, 0, target, target), imageRef);
        return spriteContext;
    } else {
        NSLog(@"Failed to load Video %@ ", videoFilePath);
        return NULL;
    }
}

#endif