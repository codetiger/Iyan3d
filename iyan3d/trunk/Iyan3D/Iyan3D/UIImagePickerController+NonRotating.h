//
//  UIImagePickerController+NonRotating.h
//  Iyan3D
//
//  Created by karthik on 30/09/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface UIImagePickerController (NonRotating)

- (BOOL)shouldAutorotate;
- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation;

@end