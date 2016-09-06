//
//  AppDelegate.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 03/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "LoadingViewControllerPad.h"
#import "GAI.h"
#import "ZipArchive.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate, UIAlertViewDelegate > {
    LoadingViewControllerPad* loadingViewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property(nonatomic, strong) id<GAITracker> tracker;

+(AppDelegate *)getAppDelegate;
-(void*) getSceneManager;
-(void) initEngine:(int)type ScreenWidth:(float)width ScreenHeight:(float)height ScreenScale:(float)screenScale renderView:(UIView*)view; 
-(bool) isMetalSupported;

@end

