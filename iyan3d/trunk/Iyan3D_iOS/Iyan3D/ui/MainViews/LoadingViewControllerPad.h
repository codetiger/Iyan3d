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

@interface LoadingViewControllerPad : GAITrackedViewController<AppHelperDelegate>
{
    NSNumberFormatter * priceFormatter;
    NSArray *jsonArr;
    CacheSystem* cache;
    NSArray *textureFile,*sceneFiles,*objFiles,*rigFiles;
    BOOL isAppFirstTime;
    BOOL isOldUser;
    NSDate *startTime;
}

@property (strong, nonatomic) NSArray *allProducts;
@property (strong, nonatomic) NSMutableDictionary *allAssets;
@property (weak, nonatomic) IBOutlet UILabel* ProgessLabel;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *activityIndicator;

@end
