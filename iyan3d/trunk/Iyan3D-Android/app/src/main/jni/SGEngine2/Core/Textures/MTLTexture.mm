//
//  MTLTexture.mm
//  SGEngine2
//
//  Created by Vivek on 29/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//


#import "MTLTexture.h"
#ifdef IOS
#import "MetalHandler.h"
#import <AVFoundation/AVFoundation.h>
#endif


MTLTexture::MTLTexture(){
    texelFormat = TEXTURE_RGBA8;
}
MTLTexture::~MTLTexture(){
    
}
void* initMTLTexture(){
    return new MTLTexture();
}
bool MTLTexture::loadTexture(string name,string texturePath,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType)
{
    
    textureName = name;
    NSString *pathToTextureFile = [NSString stringWithFormat:@"%s",texturePath.c_str()];
    UIImage *image = [UIImage imageWithContentsOfFile:pathToTextureFile];
    if (!image)
        return NO;
    width = (uint32_t)CGImageGetWidth(image.CGImage);
    height = (uint32_t)CGImageGetHeight(image.CGImage);
    int bytesPerRow = getBytesPerRow(width,format);
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate( NULL, width, height, getBitsPerCompomentForFormat(format), bytesPerRow, colorSpace, (CGBitmapInfo)kCGImageAlphaPremultipliedLast );
    CGContextDrawImage( context, CGRectMake( 0, 0, width, height ), image.CGImage );
    
    MTLTextureDescriptor *texDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:NO];
    texture = [MetalHandler::getMTLDevice() newTextureWithDescriptor:texDesc];
    if(!texture)
        return NO;
    
    [texture replaceRegion:MTLRegionMake2D(0, 0, width, height)
               mipmapLevel:0
                 withBytes:CGBitmapContextGetData(context)
               bytesPerRow:4 * width];
    
    CGColorSpaceRelease( colorSpace );
    CGContextRelease(context);
    return YES;
}

bool MTLTexture::loadTextureFromVideo(string videoFileName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType)
{
    textureName = videoFileName;
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* videoFilePath = [NSString stringWithFormat:@"%@/Resources/Videos/%@",documentsDirectory,[NSString stringWithFormat:@"%s",videoFileName.c_str()]];
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
    printf("\n Duraton %lf ", duration);
    CMTime midpoint = CMTimeMake(0, 24);
    
    CGImageRef imageRef = [gen copyCGImageAtTime:midpoint actualTime:NULL error:&err];
    
    width = (int)CGImageGetWidth(imageRef);
    height = (int)CGImageGetHeight(imageRef);
    
    printf("\n width %d ", width);

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

    MTLTextureDescriptor *texDesc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm width:width height:height mipmapped:NO];
    texture = [MetalHandler::getMTLDevice() newTextureWithDescriptor:texDesc];
    if(!texture)
        return NO;
    [texture replaceRegion:MTLRegionMake2D(0, 0, width, height)
               mipmapLevel:0
                 withBytes:CGBitmapContextGetData(spriteContext)
               bytesPerRow:4 * width];
    CGImageRelease(imageRef);
    CGContextRelease(spriteContext);
    delete textureData;
    asset = nil;
    gen = nil;
    return YES;
}

void MTLTexture::updateTexture(string fileName, int frame)
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    
    NSString* videoFilePath = [NSString stringWithFormat:@"%@/Resources/Videos/%@",documentsDirectory,[NSString stringWithFormat:@"%s",fileName.c_str()]];
    NSLog(@"Video File Path Update : %@ " , videoFilePath);
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
    
    width = (int)CGImageGetWidth(imageRef);
    height = (int)CGImageGetHeight(imageRef);
    
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
    
    [texture replaceRegion:MTLRegionMake2D(0, 0, width, height)
               mipmapLevel:0
                 withBytes:CGBitmapContextGetData(spriteContext)
               bytesPerRow:4 * width];
    
    CGContextRelease(spriteContext);
    CGImageRelease(imageRef);
    delete textureData;
    asset = nil;
    gen = nil;    
}

int MTLTexture::getBytesPerRow(int width,TEXTURE_DATA_FORMAT format){
    return width * getMTLPixelBytesForFormat(format);
}
int MTLTexture::getBytesPerRow(){
    return getBytesPerRow(width,texelFormat);
}
MTLPixelFormat MTLTexture::getMTLPixelFormat(TEXTURE_DATA_FORMAT format){
    MTLPixelFormat MTLFormat = MTLPixelFormatRGBA8Unorm;
    switch (format) {
        case TEXTURE_RG:
            MTLFormat = MTLPixelFormatRG16Unorm;
            break;
        case TEXTURE_R8:
            MTLFormat = MTLPixelFormatR8Unorm;
            break;
        default:
            break;
    }
    return MTLFormat;
}
int MTLTexture::getBitsPerCompomentForFormat(TEXTURE_DATA_FORMAT format){
    return 8;// To do
}
int MTLTexture::getMTLPixelBytesForFormat(TEXTURE_DATA_FORMAT format){
    int bytesCount = 4;
    switch (format) {
        case TEXTURE_RG:
            bytesCount = 2;
            break;
        case TEXTURE_R8:
            bytesCount = 1;
            break;
        default:
            break;
    }
    return bytesCount;
}
void MTLTexture::createRenderTargetTexture(string textureName,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType,int width,int height)
{
    
}


