//
//  UIImagePickerController+NonRotating.m
//  Iyan3D
//
//  Created by karthik on 30/09/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import "UIImagePickerController+NonRotating.h"

@implementation UIImagePickerController (NonRotating)

- (BOOL)shouldAutorotate {
    return NO;
}

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation {
    return UIInterfaceOrientationLandscapeLeft | UIInterfaceOrientationLandscapeRight;
}

@end