//
//  UIView+RenderingView.m
//  Iyan3D
//
//  Created by vigneshkumar on 17/05/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import "RenderingView.h"

@implementation RenderingView
{
    
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if(self)
    {
        
    }
    return self;
}

+ (Class)layerClass
{
#if !(TARGET_IPHONE_SIMULATOR)
    if(MTLCreateSystemDefaultDevice())
        return [CAMetalLayer class];
    else
#endif
        return [CAEAGLLayer class];
}

@end

