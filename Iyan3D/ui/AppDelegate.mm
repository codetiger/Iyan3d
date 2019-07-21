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

- (void)application:(UIApplication*)application didReceiveRemoteNotification:(NSDictionary*)userInfo {
    NSLog(@"Received notification: %@", userInfo);
    [[NSNotificationCenter defaultCenter] postNotificationName:@"renderCompleted" object:nil];
}

- (void)application:(UIApplication*)application didRegisterUserNotificationSettings:(UIUserNotificationSettings*)notificationSettings {
    [[UIApplication sharedApplication] registerForRemoteNotifications];
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

- (BOOL)application:(UIApplication*)application openURL:(NSURL*)url sourceApplication:(NSString*)sourceApplication annotation:(id)annotation {
    NSString* ext = [[[url absoluteString] pathExtension] lowercaseString];
    NSString* msg = @"There was a problem in loading the file you just imported.";

    NSArray*       paths              = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString*      documentsDirectory = [paths objectAtIndex:0];
    NSFileManager* fm                 = [NSFileManager defaultManager];

    if ([ext isEqualToString:@"zip"]) {
        ZipArchive* zip = [[ZipArchive alloc] init];
        if ([zip UnzipOpenFile:[url path]]) {
            if ([zip UnzipFileTo:documentsDirectory overWrite:YES]) {
                msg = @"Your file was imported successfully, Please use import option in the Add Menu to import the file into your scene.";
                [fm removeItemAtPath:[url path] error:nil];
            }
        }
        [zip UnzipCloseFile];

    } else if ([ext isEqualToString:@"obj"] || [ext isEqualToString:@"3ds"] || [ext isEqualToString:@"fbx"] || [ext isEqualToString:@"dae"] || [ext isEqualToString:@"png"] || [ext isEqualToString:@"jpg"] || [ext isEqualToString:@"jpeg"] || [ext isEqualToString:@"tga"] || [ext isEqualToString:@"bmp"]) {
        msg = @"Your file was imported successfully, Please use import option in the Add Menu to import the file into your scene.";
    }

    UIAlertView* message = [[UIAlertView alloc] initWithTitle:@"Information" message:msg delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
    [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];

    return YES;
}

- (void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
}

- (BOOL)iPhone6Plus {
    if (([UIScreen mainScreen].scale > 2.0))
        return YES;
    return NO;
}

@end
