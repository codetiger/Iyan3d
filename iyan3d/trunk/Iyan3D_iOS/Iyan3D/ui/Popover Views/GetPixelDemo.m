//
//  GetPixelDemo.m
//  ImageBitmapRep
//
//  Created by Alex Nichol on 7/29/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "GetPixelDemo.h"


@implementation GetPixelDemo

@synthesize delegate;

- (id)initWithFrame:(CGRect)frame {
    if ((self = [super initWithFrame:frame])) {
        // Initialization code
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame image:(ANImageBitmapRep *)theImage {
    if ((self = [super initWithFrame:frame])) {
#if __has_feature(objc_arc) == 1
        image = theImage;
#else
        image = [theImage retain];
#endif
    }
    
    self.backgroundColor = [UIColor whiteColor];
    pointerView = [[UIImageView alloc] initWithImage:[UIImage imageNamed:@"round83.png"]];
    [pointerView setFrame:CGRectMake(40, 40, 7, 7)];
    pointerView.backgroundColor = [UIColor clearColor];
    [self addSubview:pointerView];
    pixel.red = pixel.green = pixel.blue = pixel.alpha = -1.0;
    return self;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    CGPoint p = [[touches anyObject] locationInView:self];
    
    int pointX = p.x;
    int pointY = p.y;
    
    p.x = round(p.x);
    p.y = round(p.y);
    
    if (p.x < 0 || p.y < 0 || p.x >= self.frame.size.width || p.y >= self.frame.size.height) {
        return;
    }
    // translate to image
    CGSize scale = CGSizeMake(1, 1);
    scale.width = (CGFloat)([image bitmapSize].x) / self.frame.size.width;
    scale.height = (CGFloat)([image bitmapSize].y) / self.frame.size.height;
    p.x *= scale.width;
    p.y *= scale.height;
    BMPoint point = BMPointMake(round(p.x), round(p.y));
    pixel = [image getPixelAtPoint:point];
    
    if(!isnan(pixel.red) && pixel.red != -1) {
        [pointerView setFrame:CGRectMake(pointX - 3, pointY - 3, 7, 7)];
        [delegate pixelDemoGotPixel:pixel];
    }
}

- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;
{
    CGPoint p = [[touches anyObject] locationInView:self];
    
    int pointX = p.x;
    int pointY = p.y;
    
    p.x = round(p.x);
    p.y = round(p.y);
    
    if (p.x < 0 || p.y < 0 || p.x >= self.frame.size.width || p.y >= self.frame.size.height) {
        return;
    }
    // translate to image
    CGSize scale = CGSizeMake(1, 1);
    scale.width = (CGFloat)([image bitmapSize].x) / self.frame.size.width;
    scale.height = (CGFloat)([image bitmapSize].y) / self.frame.size.height;
    p.x *= scale.width;
    p.y *= scale.height;
    BMPoint point = BMPointMake(round(p.x), round(p.y));
    pixel = [image getPixelAtPoint:point];
    
    if(!isnan(pixel.red) && pixel.red != -1) {
        [pointerView setFrame:CGRectMake(pointX - 3, pointY - 3, 7, 7)];
        [delegate pixelDemoTouchEnded:pixel];
    }

}

//-(void)tapped:(UITapGestureRecognizer *)rec
//{
//    CGPoint p = [rec locationInView:self];
//    p.x = round(p.x);
//    p.y = round(p.y);
//    if (p.x < 0 || p.y < 0 || p.x >= self.frame.size.width || p.y >= self.frame.size.height) {
//        return;
//    }
//    // translate to image
//    CGSize scale = CGSizeMake(1, 1);
//    scale.width = (CGFloat)([image bitmapSize].x) / self.frame.size.width;
//    scale.height = (CGFloat)([image bitmapSize].y) / self.frame.size.height;
//    p.x *= scale.width;
//    p.y *= scale.height;
//    BMPoint point = BMPointMake(round(p.x), round(p.y));
//    pixel = [image getPixelAtPoint:point];
//    [delegate pixelDemoGotPixel:pixel];
//}

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
    [[image image] drawInRect:rect];
}

#if __has_feature(objc_arc) != 1
- (void)dealloc {
    [image release];
    [super dealloc];
    
}
#endif

@end
