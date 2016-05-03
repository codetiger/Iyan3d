//
//  TextColorPicker.h
//  Iyan3D
//
//  Created by sabish on 06/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import "GetPixelDemo.h"
#import "Constants.h"

#ifdef OPTIMGLKM
#import "Vector3GLK.h"
#else
#import "Vector3.h"
#endif

@protocol TextColorPickerDelegate
- (void) changeVertexColor:(Vector3)vetexColor dragFinish:(BOOL)isDragFinish;
@end

@interface TextColorPicker : GAITrackedViewController<GetPixelDemoDelegate>{
    Vector3 color;    
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil TextColor:(Vector3)textColor;

@property (weak, nonatomic) IBOutlet UIView *colorPickerView;
@property (weak, nonatomic) IBOutlet UIView *colorPreview;
@property (strong, nonatomic) GetPixelDemo *demoView;
@property (strong, nonatomic) id <TextColorPickerDelegate> delegate;


@end
