//
//  TextColorPicker.m
//  Iyan3D
//
//  Created by sabish on 06/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "TextColorPicker.h"

@interface TextColorPicker ()

@end

@implementation TextColorPicker

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil TextColor:(Vector3)textColor {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.screenName = @"TextColorPicker iOS";
    [self initializeColorWheel];
    [self.colorPreview setBackgroundColor:[UIColor colorWithRed:color.x green:color.y blue:color.z alpha:1.0]];
}

- (void)viewDidAppear:(BOOL)animated {
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)initializeColorWheel {
    UIImage* theImage  = [UIImage imageNamed:@"wheel2.png"];
    _demoView          = [[GetPixelDemo alloc] initWithFrame:CGRectMake(0, 0, 184, 141) image:[ANImageBitmapRep imageBitmapRepWithImage:theImage]];
    _demoView.delegate = self;
    [self.colorPickerView addSubview:_demoView];
}

- (void)pixelDemoGotPixel:(BMPixel)pixel {
    [self.colorPreview setBackgroundColor:[UIColor colorWithRed:pixel.red green:pixel.green blue:pixel.blue alpha:pixel.alpha]];
    color = Vector3(pixel.red, pixel.green, pixel.blue);
    [_delegate changeVertexColor:color dragFinish:NO];
}

- (void)pixelDemoTouchEnded:(BMPixel)pixel {
    [self.colorPreview setBackgroundColor:[UIColor colorWithRed:pixel.red green:pixel.green blue:pixel.blue alpha:pixel.alpha]];
    color = Vector3(pixel.red, pixel.green, pixel.blue);
    [_delegate changeVertexColor:color dragFinish:YES];
}

@end
