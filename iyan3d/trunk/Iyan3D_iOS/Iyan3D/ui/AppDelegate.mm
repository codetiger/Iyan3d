//
//  AppDelegate.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 03/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "AppDelegate.h"

#include "SGEngineCommon.h"
#include "SGEngineOGL.h"
#include "SGEngineMTL.h"
#include <SafariServices/SafariServices.h>
#include <GoogleSignIn/GoogleSignIn.h>
#include "Utility.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/machine.h>
#import <Fabric/Fabric.h>
#import <TwitterKit/TwitterKit.h>
#import <Crashlytics/Crashlytics.h>

#include <sys/time.h>
#import <GLKit/GLKMath.h>
#include <LinearMath/btMatrixX.h>

#define DB_ALERT 0

SceneManager *scenemgr;
@interface AppDelegate ()

@property(nonatomic, assign) BOOL okToWait;
@property(nonatomic, copy) void (^dispatchHandler)(GAIDispatchResult result);

@end

static NSString *const kTrackingId = @"UA-49819146-1";
static NSString *const kAllowTracking = @"allowTracking";
static NSString * const kClient = @"328259754555-buqbocp0ehq7mtflh0lk3j2p82cc4ltm.apps.googleusercontent.com";

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    
    if ([application respondsToSelector:@selector(registerUserNotificationSettings:)]) {
        [[UIApplication sharedApplication] registerUserNotificationSettings:[UIUserNotificationSettings settingsForTypes:(UIUserNotificationTypeBadge | UIUserNotificationTypeSound | UIUserNotificationTypeAlert) categories:nil]];
    } else {
        [[UIApplication sharedApplication] registerForRemoteNotificationTypes:
         (UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeSound | UIRemoteNotificationTypeAlert)];
    }
    

    [[Twitter sharedInstance] startWithConsumerKey:@"FVYtYJI6e4lZMHoZvYCt2ejao" consumerSecret:@"eiFIXzb9zjoaH0lrDZ2Jrh2ezvbmuFv6rvPJdIXLYxgkaZ7YKC"];
    
#if !(TARGET_IPHONE_SIMULATOR)
    [Fabric with:@[[Crashlytics class], [Twitter class]]];
#else
   [Fabric with:@[[Twitter class]]];
#endif
    
    [self setUpDropBox];
    
    [GIDSignIn sharedInstance].clientID = kClient;

    NSDictionary *appDefaults = @{kAllowTracking: @(YES)};
    [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
    [GAI sharedInstance].optOut = ![[NSUserDefaults standardUserDefaults] boolForKey:kAllowTracking];
    [GAI sharedInstance].dispatchInterval = 20;
    [GAI sharedInstance].trackUncaughtExceptions = YES;
    //[[GAI sharedInstance].logger setLogLevel:kGAILogLevelVerbose];
    //    self.tracker = [[GAI sharedInstance] trackerWithName:@"CuteAnimals"
    //                                              trackingId:kTrackingId];
    self.tracker = [[GAI sharedInstance] trackerWithTrackingId:kTrackingId];
    //self.tracker.allowIDFACollection = NO;

    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSLog(@"doc dir %@",docDirPath);
    [[UIApplication sharedApplication] setStatusBarStyle:UIStatusBarStyleLightContent];
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    if([Utility IsPadDevice]){
        [[UIApplication sharedApplication] setStatusBarHidden:NO];
		loadingViewController = [[LoadingViewControllerPad alloc] initWithNibName:@"LoadingViewControllerPad" bundle:nil];
    }
    else {
        [[UIApplication sharedApplication] setStatusBarHidden:YES];
        loadingViewController = [[LoadingViewControllerPad alloc] initWithNibName:([self iPhone6Plus]) ? @"LoadingViewControllerPhone@2x" : @"LoadingViewControllerPhone" bundle:nil];
    }
    [self.window setRootViewController:loadingViewController];
    [self.window makeKeyAndVisible];
    
    if (launchOptions != nil)
    {
        NSDictionary *dictionary = [launchOptions objectForKey:UIApplicationLaunchOptionsRemoteNotificationKey];
        if (dictionary != nil)
        {
            [self saveBoolToUserDefaultsWith:YES AndKey:@"openrenderTasks"];
        }
    }
    
    return YES;
}

-(void) saveBoolToUserDefaultsWith:(BOOL)value AndKey:(NSString*) key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    
    if (standardUserDefaults) {
        [standardUserDefaults setBool:value forKey:key];
        [standardUserDefaults synchronize];
    }
}

-(void) saveToUserDefaultsWithValue:(id)value AndKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    
    if (standardUserDefaults) {
        [standardUserDefaults setObject:value forKey:key];
        [standardUserDefaults synchronize];
    }
}

- (void) setUpDropBox
{
    NSString* appKey = @"9crd2qv94ebj7i6";
    NSString* appSecret = @"iv95w28ddsmzf0u";
    NSString *root = kDBRootAppFolder;
    
    DBSession* session =
    [[DBSession alloc] initWithAppKey:appKey appSecret:appSecret root:root];
    session.delegate = self;
    [DBSession setSharedSession:session];
    
    [DBRequest setNetworkRequestDelegate:self];
}

- (void)sessionDidReceiveAuthorizationFailure:(DBSession *)session userId:(NSString *)userId
{
    [[[UIAlertView alloc]
       initWithTitle:@"Error" message:@"Failed Linking Dropbox" delegate:self
       cancelButtonTitle:@"Ok" otherButtonTitles:nil] show];
}

- (void)networkRequestStarted
{
    
}

- (void)networkRequestStopped
{

}

- (void)application:(UIApplication*)application didReceiveRemoteNotification:(NSDictionary*)userInfo
{
    NSLog(@"Received notification: %@", userInfo);
    [[NSNotificationCenter defaultCenter] postNotificationName:@"renderCompleted" object:nil];
}


- (void)application:(UIApplication *)application didRegisterUserNotificationSettings:(UIUserNotificationSettings *)notificationSettings
{
    [[UIApplication sharedApplication] registerForRemoteNotifications];
}

- (void)application:(UIApplication *)application didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)deviceToken
{
    NSString * deviceTokenString = [[[[deviceToken description]
                                      stringByReplacingOccurrencesOfString: @"<" withString: @""]
                                     stringByReplacingOccurrencesOfString: @">" withString: @""]
                                    stringByReplacingOccurrencesOfString: @" " withString: @""];
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    
    if (standardUserDefaults) {
        [standardUserDefaults setObject:deviceTokenString forKey:@"deviceToken"];
        [standardUserDefaults synchronize];
    }
}

- (void)application:(UIApplication *)application didFailToRegisterForRemoteNotificationsWithError:(NSError *)error
{
    NSLog(@" Error %@ ", error.localizedDescription);
}

-(void) initEngine:(int)type ScreenWidth:(float)width ScreenHeight:(float)height ScreenScale:(float)screenScale renderView:(UIView*) view
{
    scenemgr = new SceneManager(width,height,screenScale,(DEVICE_TYPE)type,[[[NSBundle mainBundle] resourcePath] UTF8String],(__bridge void*)view);
}
+(AppDelegate *)getAppDelegate
{
    return (AppDelegate*)[UIApplication sharedApplication].delegate;
}
-(void*) getSceneManager{
    return scenemgr;
}

-(bool) isMetalSupported
{
    size_t size;
    cpu_type_t type;
    cpu_subtype_t subtype;
    size = sizeof(type);
    sysctlbyname("hw.cputype", &type, &size, NULL, 0);
    
    size = sizeof(subtype);
    sysctlbyname("hw.cpusubtype", &subtype, &size, NULL, 0);
    if(subtype == CPU_SUBTYPE_ARM64_V8)
        return true;
    return false;
}
- (void)applicationWillResignActive:(UIApplication *)application {
    [[NSNotificationCenter defaultCenter] postNotificationName:@"AppGoneBackground" object:nil userInfo:nil];
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    [self sendHitsInBackground];
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    [GAI sharedInstance].optOut = ![[NSUserDefaults standardUserDefaults] boolForKey:kAllowTracking];
    [[NSNotificationCenter defaultCenter] postNotificationName:@"applicationDidBecomeActive" object:nil userInfo:nil];
}

- (void)sendHitsInBackground {
    self.okToWait = YES;
    __weak AppDelegate *weakSelf = self;
    __block UIBackgroundTaskIdentifier backgroundTaskId =
    [[UIApplication sharedApplication] beginBackgroundTaskWithExpirationHandler:^{
        weakSelf.okToWait = NO;
    }];
    
    if (backgroundTaskId == UIBackgroundTaskInvalid) {
        return;
    }
    
    self.dispatchHandler = ^(GAIDispatchResult result) {
        // If the last dispatch succeeded, and we're still OK to stay in the background then kick off
        // again.
        if (result == kGAIDispatchGood && weakSelf.okToWait ) {
            [[GAI sharedInstance] dispatchWithCompletionHandler:weakSelf.dispatchHandler];
        } else {
            [[UIApplication sharedApplication] endBackgroundTask:backgroundTaskId];
        }
    };
    [[GAI sharedInstance] dispatchWithCompletionHandler:self.dispatchHandler];
}

- (void)applicationWillTerminate:(UIApplication *)application {
}
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url sourceApplication:(NSString *)sourceApplication annotation:(id)annotation{
    NSString* ext = [[url absoluteString] pathExtension];
    NSString* msg;
    
    
    if([ext isEqualToString:@"ttf"] || [ext isEqualToString:@"otf"])
        msg = [NSString stringWithFormat:@"To use your %@ font file, use 'Text' option in your Import Menu. You should have upgraded to the Premium version for using this feature.", [ext uppercaseString]];
    else if([ext isEqualToString:@"obj"] || [ext isEqualToString:@"png"]|| [ext isEqualToString:@"jpeg"]) {
        msg = [NSString stringWithFormat:@"To import your %@ file, use 'Import And Rig Models' button in Home Page", [ext uppercaseString]];
    }
    else if ([ext isEqualToString:@"i3d"]) {
        if(![self.window.rootViewController isKindOfClass:[LoadingViewControllerPad class]]) {
            
        }
        if([[NSFileManager defaultManager] fileExistsAtPath:url.path]) {
            [self saveBoolToUserDefaultsWith:YES AndKey:@"i3dopen"];
            [self saveToUserDefaultsWithValue:url.path AndKey:@"i3dpath"];
            [[NSNotificationCenter defaultCenter] postNotificationName:@"i3dopen" object:nil];
        }
    }
    else if ([[url absoluteString] containsString:@"google"]) {
        return [[GIDSignIn sharedInstance] handleURL:url
                                   sourceApplication:sourceApplication
                                          annotation:annotation];
    }
    else if ([[url absoluteString] containsString:@"fb"]) {
        return [[FBSDKApplicationDelegate sharedInstance] application:application
                                                              openURL:url
                                                    sourceApplication:sourceApplication
                                                           annotation:annotation];
    }
    else {
       
        if ([[DBSession sharedSession] handleOpenURL:url]) {
            if([[DBSession sharedSession] isLinked]) {
                [[NSNotificationCenter defaultCenter] postNotificationName:@"DBlinked" object:nil];
//                UIAlertView *dbAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"App successfully linked to DropBox. You can now backup your scene by pressing 'BackUp' in scene options." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
//                [dbAlert setTag:DB_ALERT];
//                [dbAlert show];
            }
            return YES;
        }

    }
    
    [self.window endEditing:YES];
//	UIAlertView *message = [[UIAlertView alloc]initWithTitle:@"Information" message:msg delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
	//[message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];

	return YES;
}

 - (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    switch (alertView.tag) {
        case DB_ALERT:
            
            break;
        default:
            break;
    }
    
}

-(BOOL)iPhone6Plus{
    if (([UIScreen mainScreen].scale > 2.0)) return YES;
    return NO;
}

@end
