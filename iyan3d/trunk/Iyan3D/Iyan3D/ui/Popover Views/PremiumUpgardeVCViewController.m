//
//  PremiumUpgardeVCViewController.m
//  Iyan3D
//
//  Created by karthik on 29/04/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import "PremiumUpgardeVCViewController.h"

#define OBJ_IMPORT_IAP @"objimport"

@interface PremiumUpgardeVCViewController ()

@end

@implementation PremiumUpgardeVCViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.layer.borderWidth = 2.0f;
    self.view.layer.cornerRadius = 5.0;
    self.view.layer.borderColor = [UIColor grayColor].CGColor;
    self.upgradeButton.layer.cornerRadius = 5.0;
    self.cancelButton.layer.cornerRadius = 5.0;
    self.restorePurchaseButton.layer.cornerRadius = 5.0;
    processTransaction = false;
    
    [self loadingViewStatus:YES];
    [self.restoreActivityView setHidden:YES];
    cache = [CacheSystem cacheSystem];
    [self loadObjImporterPrice];
    
    [self createAndLoadInterstitial];
    // Do any additional setup after loading the view from its nib.
}
- (void)viewDidAppear:(BOOL)animated
{
    self.screenName = @"PremiumUpgradeView";
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(showPriceForPremiumUpgrade) name:@"ProductPriceSet" object:nil];
}
- (void)viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"ProductPriceSet"  object:nil];
}

- (void)createAndLoadInterstitial {
    
    [self.continueOnceBtn setHidden:YES];
    [self.activityForAd setHidden:NO];
    [self.activityForAd startAnimating];
    
    self.interstitial =
    [[GADInterstitial alloc] initWithAdUnitID:@"ca-app-pub-2186321854033194/4906272023"];
    self.interstitial.delegate = self;
    
    GADRequest *request = [GADRequest request];

//    request.testDevices = @[
//                            @"b3b75bcb2f86d7fcbc16e1885aa1457f"  // Eric's iPod Touch
//                            ];
    [self.interstitial loadRequest:request];
}

- (void) showInterstitialAd
{
    if (self.interstitial.isReady) {
        [self.interstitial presentFromRootViewController:self];
    }
}

- (void)interstitialDidReceiveAd:(GADInterstitial *)ad
{
    NSLog(@"Received ad");
    if(ad.isReady) {
        [self.continueOnceBtn setHidden:NO];
        [self.activityForAd stopAnimating];
        [self.activityForAd setHidden:YES];
    }
}

- (void)interstitial:(GADInterstitial *)interstitial
didFailToReceiveAdWithError:(GADRequestError *)error {
    NSLog(@"interstitialDidFailToReceiveAdWithError: %@", [error localizedDescription]);
}

- (void)interstitialDidDismissScreen:(GADInterstitial *)interstitial {
    NSLog(@"interstitialDidDismissScreen");
    [self.delegate premiumUnlocked];
    [self dismissViewControllerAnimated:YES completion:Nil];
    self.interstitial = nil;
}

- (void)loadObjImporterPrice
{
    _priceFormatter = [[NSNumberFormatter alloc] init];
    [_priceFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
    [_priceFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];

    
    if([[AppHelper getAppHelper] checkInternetConnected]) {
        [[AppHelper getAppHelper] initHelper];
        [[AppHelper getAppHelper] setAssetsDetails];
        if(![[cache getAssetPrice:OBJ_IMPORT_IAP] isEqualToString:@"BUY"])
        [self showPriceForPremiumUpgrade];
    } else
        [self showPriceForPremiumUpgrade];
}

-(void)showPriceForPremiumUpgrade
{
    if(cache == nil)
    cache = [CacheSystem cacheSystem];
    
    if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]){
        [_priceFormatter setLocale:[[AppHelper getAppHelper] getPriceLocale]];
        NSString *priceForObjImport = [cache getAssetPrice:OBJ_IMPORT_IAP];
        if ([priceForObjImport isEqualToString:@"BUY"]) {
            //To show alert.
            [self.view endEditing:YES];
            UIAlertView *internetAlert = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to connect to the server, Please check your network settings." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [internetAlert show];
        } else {
            [self.loadingView setHidden:YES];
            [self.loadingView stopAnimating];
            NSNumber *priceInNumber = [NSNumber numberWithFloat:[priceForObjImport floatValue]];
            self.priceLabel.text = [_priceFormatter stringFromNumber:priceInNumber];
        }
        [[AppHelper getAppHelper] resetAppHelper];
    }
    else {
        self.priceLabel.text = @"";
    }

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (IBAction)upgradeButtonPressed:(id)sender {
    if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]){
    //if([cache checkOBJImporterPurchase]){
        [self premiumUnlocked];
    }
    else {
        processTransaction = true;
        [self loadingViewStatus:YES];
        [[UIApplication sharedApplication] beginIgnoringInteractionEvents];
        [AppHelper getAppHelper].delegate = self;
        [[AppHelper getAppHelper] addTransactionObserver];
        [[AppHelper getAppHelper] callPaymentGateWayForPremiumUpgrade];
    }
}

- (IBAction)cancelPressed:(id)sender {
    processTransaction = false;
    [self.delegate premiumUnlockedCancelled];
    [[AppHelper getAppHelper] removeTransactionObserver];
    [self dismissViewControllerAnimated:TRUE completion:Nil];
}
- (IBAction)moreInfoButtonAction:(id)sender{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"http://www.iyan3dapp.com/features-in-the-premium-version/"]];
}
-(void)loadingViewStatus:(BOOL)status
{
    if(status) {
        [self.loadingView setHidden:NO];
        [self.loadingView startAnimating];
    } else {
        [self.loadingView stopAnimating];
        [self.loadingView setHidden:YES];
    }
    [self.delegate loadingViewStatus:status];
}

-(void)statusForOBJImport:(NSNumber*)status
{
    [self loadingViewStatus:!status];
    [self.delegate statusForOBJImport:status];
}
-(void)premiumUnlocked
{
    [self loadingViewStatus:NO];
    [[AppHelper getAppHelper] saveBoolUserDefaults:YES withKey:@"premiumUnlocked"];
    processTransaction = false;
    [[AppHelper getAppHelper] removeTransactionObserver];
    [self.delegate premiumUnlocked];
    [self dismissViewControllerAnimated:YES completion:Nil];
    [[UIApplication sharedApplication] endIgnoringInteractionEvents];
}

-(void) dealloc
{
    [AppHelper getAppHelper].delegate = nil;
    [[AppHelper getAppHelper]resetAppHelper];
    cache = nil;
    _priceFormatter = nil;
}
- (IBAction)restorePurchasePressed:(id)sender {
    [AppHelper getAppHelper].delegate = self;
    [[AppHelper getAppHelper] addTransactionObserver];
    [[AppHelper getAppHelper] restorePurchasedTransaction];
}

- (IBAction)continueOncePressed:(id)sender {
    [self showInterstitialAd];
}

-(void)statusForRestorePurchase:(NSNumber *)object
{
    BOOL restoreCompleted = [object boolValue];
    //[self.upgradeButton setHidden:!restoreCompleted];
    [self.restorePurchaseButton setHidden:!restoreCompleted];
    [self.restoreActivityView setHidden:restoreCompleted];
    restoreCompleted ? [self.restoreActivityView stopAnimating] : [self.restoreActivityView startAnimating];
    if(restoreCompleted) {
        [[AppHelper getAppHelper] saveBoolUserDefaults:restoreCompleted withKey:@"isPurchaseRestored"];
        if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"])
            [self premiumUnlocked];
    }
}

-(void)transactionCancelled
{
    [self.restorePurchaseButton setHidden:NO];
    [self.restoreActivityView setHidden:YES];
    [self.restoreActivityView stopAnimating];
}

# pragma payment delegate


@end
