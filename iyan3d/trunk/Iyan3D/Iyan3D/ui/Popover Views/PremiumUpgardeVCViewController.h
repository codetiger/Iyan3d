//
//  PremiumUpgardeVCViewController.h
//  Iyan3D
//
//  Created by karthik on 29/04/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppHelper.h"
#import "GAI.h"
#import <GoogleMobileAds/GoogleMobileAds.h>

@protocol PremiumUpgardeVCViewControllerDelegate
-(void)upgradeButtonPressed;
-(void)premiumUnlocked;
-(void)premiumUnlockedCancelled;
-(void)statusForOBJImport:(NSNumber *)object;
-(void)loadingViewStatus:(BOOL)status;
@end

@interface PremiumUpgardeVCViewController : GAITrackedViewController <AppHelperDelegate, GADInterstitialDelegate>
{
    CacheSystem *cache;
    BOOL isRigButtonPressed;
    BOOL processTransaction;
    NSNumberFormatter * _priceFormatter;
}
- (IBAction)upgradeButtonPressed:(id)sender;
- (IBAction)moreInfoButtonAction:(id)sender;
- (IBAction)cancelPressed:(id)sender;

@property (weak, nonatomic) IBOutlet UIButton *continueOnceBtn;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *activityForAd;

@property(nonatomic, strong) GADInterstitial *interstitial;

@property (weak, nonatomic) IBOutlet UIButton *restorePurchaseButton;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *restoreActivityView;
@property (weak, nonatomic) id <PremiumUpgardeVCViewControllerDelegate> delegate;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *loadingView;
@property (weak, nonatomic) IBOutlet UIButton *upgradeButton;
@property (weak, nonatomic) IBOutlet UIButton *cancelButton;
@property (weak, nonatomic) IBOutlet UIButton *moreInfoButton;
@property (weak, nonatomic) IBOutlet UILabel *priceLabel;
- (IBAction)restorePurchasePressed:(id)sender;

- (IBAction)continueOncePressed:(id)sender;

@end

