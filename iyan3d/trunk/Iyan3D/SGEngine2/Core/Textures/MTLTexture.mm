//
//  MTLTexture.mm
//  SGEngine2
//
//  Created by Vivek on 29/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#import "MTLTexture.h"
#import "MetalHandler.h"


MTLTexture::MTLTexture(){
    texelFormat = TEXTURE_RGBA8;
}
MTLTexture::~MTLTexture(){
    
}
void* initMTLTexture(){
    return new MTLTexture();
}
bool MTLTexture::loadTexture(string name,string texturePath,TEXTURE_DATA_FORMAT format,TEXTURE_DATA_TYPE texelType){
    
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


