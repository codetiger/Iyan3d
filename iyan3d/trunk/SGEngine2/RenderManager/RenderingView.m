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
    if(MTLCreateSystemDefaultDevice())
        return [CAMetalLayer class];
    else
        return [CAEAGLLayer class];
}

@end

