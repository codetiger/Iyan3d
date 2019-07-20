//
//  GetPixelDemo.h
//  ImageBitmapRep
//
//  Created by Alex Nichol on 7/29/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ANImageBitmapRep.h"

@protocol GetPixelDemoDelegate <NSObject>

- (void)pixelDemoGotPixel:(BMPixel)pixel;
- (void)pixelDemoTouchEnded:(BMPixel)pixel;

@end

@interface GetPixelDemo : UIView {
#if __has_feature(objc_arc) == 1
    __unsafe_unretained id<GetPixelDemoDelegate> delegate;
    __strong ANImageBitmapRep* image;
    __strong UIImageView* pointerView;
    BMPixel               pixel;

#else
    ANImageBitmapRep*        image;
    id<GetPixelDemoDelegate> delegate;
    UIView*                  pointerView;
#endif
}

@property (nonatomic, assign) id<GetPixelDemoDelegate> delegate;

- (id)initWithFrame:(CGRect)frame image:(ANImageBitmapRep*)theImage;

@end
