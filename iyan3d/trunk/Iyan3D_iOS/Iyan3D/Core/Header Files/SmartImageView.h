//
//  SmartImageView.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 05/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SmartImageView : UIImageView {
    NSString* _assetId;
    int _viewType;
    UIActivityIndicatorView* activityIndicator;
    NSOperationQueue* queue;
}

- (void)setImageInfo:(NSString*)assetId forView:(int)viewType OperationQueue:(NSOperationQueue*)q ScreenWidth:(int)screenWidth ScreenHeight:(int)screenHeight;
@end
