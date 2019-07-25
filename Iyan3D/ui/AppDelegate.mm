//
//  AppDelegate.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 03/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "AppDelegate.h"

#include "SGEngineCommon.h"
#include "SGEngineMTL.h"
#include <SafariServices/SafariServices.h>
#include "Utility.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/machine.h>

#include <sys/time.h>

SceneManager* scenemgr;

@implementation AppDelegate

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions {
    [[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleLightContent];
    self.window           = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    loadingViewController = [[LoadingViewControllerPad alloc] initWithNibName:@"LoadingViewControllerPad" bundle:nil];

    if ([Utility IsPadDevice]) {
        [[UIApplication sharedApplication] setStatusBarHidden:NO];
    } else {
        [[UIApplication sharedApplication] setStatusBarHidden:YES];
    }

    [self.window setRootViewController:loadingViewController];
    [self.window makeKeyAndVisible];

    return YES;
}

- (void)application:(UIApplication*)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData*)deviceToken {
    NSString*       deviceTokenString    = [[[[deviceToken description]
        stringByReplacingOccurrencesOfString:@"<"
                                  withString:@""]
        stringByReplacingOccurrencesOfString:@">"
                                  withString:@""]
        stringByReplacingOccurrencesOfString:@" "
                                  withString:@""];
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];

    if (standardUserDefaults) {
        [standardUserDefaults setObject:deviceTokenString forKey:@"deviceToken"];
        [standardUserDefaults synchronize];
    }
}

- (void)application:(UIApplication*)application didFailToRegisterForRemoteNotificationsWithError:(NSError*)error {
    NSLog(@" Error %@ ", error.localizedDescription);
}

- (BOOL)initEngine:(float)width ScreenHeight:(float)height ScreenScale:(float)screenScale renderView:(UIView*)view {
    scenemgr = new SceneManager(width, height, screenScale, [[[NSBundle mainBundle] resourcePath] UTF8String], (__bridge void*)view);
    if (!scenemgr)
        return false;
    return true;
}

+ (AppDelegate*)getAppDelegate {
    return (AppDelegate*)[UIApplication sharedApplication].delegate;
}

- (void*)getSceneManager {
    return scenemgr;
}

- (void)applicationWillResignActive:(UIApplication*)application {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"AppGoneBackground" object:nil userInfo:nil];
}

- (void)applicationDidEnterBackground:(UIApplication*)application {
}

- (void)applicationWillEnterForeground:(UIApplication*)application {
}

- (void)applicationDidBecomeActive:(UIApplication*)application {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"applicationDidBecomeActive" object:nil userInfo:nil];
}

- (void)applicationWillTerminate:(UIApplication*)application {
}

- (BOOL)iPhone6Plus {
    if (([UIScreen mainScreen].scale > 2.0))
        return YES;
    return NO;
}

@end
