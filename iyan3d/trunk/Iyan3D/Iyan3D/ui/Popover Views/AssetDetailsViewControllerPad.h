//
//  AssetDetailsViewControllerPad.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 06/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import "StoreKit/StoreKit.h"
#import "QuartzCore/QuartzCore.h"
#import "PremiumUpgardeVCViewController.h"
#import "SmartImageView.h"
#import "AssetItem.h"
#import "CacheSystem.h"

@protocol AssetDetailsViewControllerPadDelegate
- (void) assetDownloadCompleted:(int)assetId;
- (void) refreshCollectionView;
@end

@interface AssetDetailsViewControllerPad:UIViewController<SKPaymentTransactionObserver,PremiumUpgardeVCViewControllerDelegate>
{
    CacheSystem *cache;
    int _assetId;
	AssetItem *asset;
	int storeType;
    BOOL isAnimationFrom,isAnimationExist;
    PremiumUpgardeVCViewController* upgradeView;
}

@property (weak, nonatomic) IBOutlet SmartImageView *assetImageView;
@property (weak, nonatomic) IBOutlet UIButton *addToSceneButton;
@property (weak, nonatomic) IBOutlet UIButton *cancelButton;
@property (weak, nonatomic) IBOutlet UILabel *assetNameLabel;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *downloadingActivityView;
@property (weak, nonatomic) id <AssetDetailsViewControllerPadDelegate> delegate;

- (IBAction)cancelButtonAction:(id)sender;
- (IBAction)addToSceneButtonAction:(id)sender;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil asset:(int)assetId storeType:(int) type isFromAnimation:(BOOL)animation;

@end
