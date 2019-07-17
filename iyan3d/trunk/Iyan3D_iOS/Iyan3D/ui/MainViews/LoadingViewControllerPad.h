//
//  LoadingViewControllerPad.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 04/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "StoreKit/StoreKit.h"
#import "CacheSystem.h"
#import "AssetItem.h"
#import "Utility.h"
#import "GAI.h"
#import "AppHelper.h"

@interface LoadingViewControllerPad : GAITrackedViewController <AppHelperDelegate> {
    NSArray*     jsonArr;
    CacheSystem* cache;
    BOOL         isAppFirstTime;
    BOOL         isOldUser;
    NSDate*      startTime;
}

@end
