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

#import "ImageLoaderOBJC.h"

uint8_t* loadPNGImage(std::string filePath,int &width,int &height){
    NSString *fileName = [NSString stringWithFormat:@"%s",filePath.c_str()];
    
    CGImageRef spriteImage;
    spriteImage = [UIImage imageWithContentsOfFile:fileName].CGImage;
    
    if (!spriteImage) 
        NSLog(@"Failed to load image %@ FilePath %@", fileName,fileName);
    
    // 2
    width = CGImageGetWidth(spriteImage);
    height = CGImageGetHeight(spriteImage);
    
    GLubyte * spriteData = (GLubyte *) calloc(width*height*4, sizeof(GLubyte));
    
    CGContextRef spriteContext = CGBitmapContextCreate(spriteData, width, height, 8, width*4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
    
    // 3
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

#endif