//
//  LoggedInViewController.m
//  Iyan3D
//
//  Created by Sankar on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "LoggedInViewController.h"
#import "RenderTableViewCell.h"
#include "Utility.h"
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
#import <TwitterKit/TwitterKit.h>

#define GOOGLE_SIGNIN 0
#define FACEBOOK_SIGNIN 1
#define TWITTER_SIGNIN 2

@interface LoggedInViewController ()

@end

@implementation LoggedInViewController

#define FIVE_HUNDERED_CREDITS @"fivehundredcredits"
#define TWO_THOUSAND_CREDITS @"twothousandcredits"
#define FIVE_THOUSAND_CREDITS @"fivethousandcredits"

- (void)viewDidLoad {
    [super viewDidLoad];
    
    [self.creditsLoading setHidden:NO];
    [self.creditsLoading startAnimating];
    [self.creditsLoading setHidesWhenStopped:YES];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(setUserCredits_stopLoading) name:@"creditsupdate" object:nil];
    
    if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"]) {
        NSString* uniqueId = [[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"];
        NSString* userName = [[AppHelper getAppHelper] userDefaultsForKey:@"name"];
        NSString* email = [[AppHelper getAppHelper] userDefaultsForKey:@"email"];
        int signinType = [[[AppHelper getAppHelper] userDefaultsForKey:@"signintype"] intValue];
        [[AppHelper getAppHelper] getCreditsForUniqueId:uniqueId Name:userName Email:email SignInType:signinType];
    }
    NSNumber *credits = [[AppHelper getAppHelper] userDefaultsForKey:@"credits"];
    self.creditsLabel.text = [NSString stringWithFormat:@"%@", credits];

    [GIDSignIn sharedInstance].uiDelegate = self;
    [GIDSignIn sharedInstance].delegate = self;
    renderData = @{@"In Progress" : @[@"My Scene 1", @"My Scene 2", @"My Scene 3",@"My Scene 4",@"My Scene 5"],
                   @"Completed" : @[@"My Scene 1", @"My Scene 2",@"My Scene 3",@"My Scene 4"]};
    [self.renderStatus registerClass:[RenderTableViewCell class] forCellReuseIdentifier:@"Cell"];
    renderSectionTitles = [[renderData allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
    self.creditsView.layer.cornerRadius = 10;
    self.creditsView.layer.masksToBounds = YES;
    
    // Do any additional setup after loading the view from its nib.
}

- (void) setUserCredits_stopLoading
{
    NSNumber *credits = [[AppHelper getAppHelper] userDefaultsForKey:@"credits"];
    self.creditsLabel.text = [NSString stringWithFormat:@"%@", credits];
    [self.creditsLoading stopAnimating];
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

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return [renderSectionTitles count];
}
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return [renderSectionTitles objectAtIndex:section];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    NSString *sectionTitle = [renderSectionTitles objectAtIndex:section];
    NSArray *sectionAnimals = [renderData objectForKey:sectionTitle];
    return [sectionAnimals count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSString *renderCellStr = ([Utility IsPadDevice]) ? @"RenderTableViewCell" : @"RenderTableViewPhone";
    RenderTableViewCell *cell = (RenderTableViewCell *)[tableView dequeueReusableCellWithIdentifier:renderCellStr];
    if (cell == nil)
    {
        NSArray *nib = [[NSBundle mainBundle] loadNibNamed:renderCellStr owner:self options:nil];
        cell = [nib objectAtIndex:0];
    }
    // Configure the cell...
    NSString *sectionTitle = [renderSectionTitles objectAtIndex:indexPath.section];
    NSArray *sectionData = [renderData objectForKey:sectionTitle];
    NSString *render = [sectionData objectAtIndex:indexPath.row];
    cell.renderlabel.text = render;
    return cell;
}

- (IBAction)signOutBtn:(id)sender
{
    NSLog(@"Signout Clicked");
    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"]){
        
        int signinType = [[[AppHelper getAppHelper] userDefaultsForKey:@"signintype"] intValue];
        if(signinType == GOOGLE_SIGNIN)
            [[GIDSignIn sharedInstance] disconnect];
        else if (signinType == FACEBOOK_SIGNIN) {
            FBSDKLoginManager *loginManager = [[FBSDKLoginManager alloc] init];
            [loginManager logOut];
        } else if (signinType == TWITTER_SIGNIN) {
            [[Twitter sharedInstance]logOut];
        }

        [self reportAuthStatus];
        [self.delegare dismissView];
    }
}

- (void)signIn:(GIDSignIn *)signIn didDisconnectWithUser:(GIDGoogleUser *)user withError:(NSError *)error {
    // Perform any operations when the user disconnects from app here.
    if(error){
        NSLog(@"Error");
    }
    else {
        NSLog(@"Success");
         [self reportAuthStatus];
    }
}

- (void)reportAuthStatus
{
    [[AppHelper getAppHelper] saveBoolUserDefaults:NO withKey:@"signedin"];
    [[AppHelper getAppHelper] removeFromUserDefaultsWithKey:@"email"];
    [[AppHelper getAppHelper] removeFromUserDefaultsWithKey:@"uniqueid"];
    [[AppHelper getAppHelper] removeFromUserDefaultsWithKey:@"username"];
    [[AppHelper getAppHelper] removeFromUserDefaultsWithKey:@"signintype"];
    [[AppHelper getAppHelper] removeFromUserDefaultsWithKey:@"credits"];
}

- (void)dealloc
{
//    [[NSNotificationCenter defaultCenter] removeObserver:self forKeyPath:@"creditsupdate"];
}

- (IBAction)add500Credits:(id)sender
{
    [AppHelper getAppHelper].delegate = self;
    [[AppHelper getAppHelper] addTransactionObserver];
    [[AppHelper getAppHelper] callPaymentGateWayForProduct:FIVE_HUNDERED_CREDITS];
}

- (IBAction)add2KCredits:(id)sender
{
    [AppHelper getAppHelper].delegate = self;
    [[AppHelper getAppHelper] addTransactionObserver];
    [[AppHelper getAppHelper] callPaymentGateWayForProduct:TWO_THOUSAND_CREDITS];
}

- (IBAction)add5KCredits:(id)sender
{
    [AppHelper getAppHelper].delegate = self;
    [[AppHelper getAppHelper] addTransactionObserver];
    [[AppHelper getAppHelper] callPaymentGateWayForProduct:FIVE_THOUSAND_CREDITS];
}

-(void)statusForOBJImport:(NSNumber*)status
{
    if([status intValue] != 0) {
        NSString* uniqueId = [[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"];
        [[AppHelper getAppHelper] useOrRechargeCredits:uniqueId credits:[status intValue] For:0];
    }
}

- (void)transactionCancelled
{
    [[AppHelper getAppHelper] removeTransactionObserver];
    [AppHelper getAppHelper].delegate = nil;
}

@end
