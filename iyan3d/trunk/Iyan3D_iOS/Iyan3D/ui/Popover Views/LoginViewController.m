//
//  LoginViewController.m
//  Iyan3D
//
//  Created by Sankar on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "LoginViewController.h"
#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
#import <TwitterKit/TwitterKit.h>


@interface LoginViewController ()

@end

@implementation LoginViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [GIDSignIn sharedInstance].uiDelegate = self;
    [GIDSignIn sharedInstance].delegate = self;
    NSLog(@"Frame Height :%f",self.view.frame.size.height);
    // Do any additional setup after loading the view from its nib.
    

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


- (IBAction)cancelBtnAction:(id)sender {
    NSLog(@"Frame Height :%f",self.view.frame.size.height);
    [self dismissViewControllerAnimated:YES completion:nil];
    
}

- (IBAction)googleSigninAction:(id)sender {
[[GIDSignIn sharedInstance] signIn];
}

- (IBAction)fbSigninAction:(id)sender {
    
   
    FBSDKLoginManager *login = [[FBSDKLoginManager alloc] init];
    [login
     logInWithReadPermissions: @[@"email",@"public_profile", @"user_friends"]
     fromViewController:self
     handler:^(FBSDKLoginManagerLoginResult *result, NSError *error) {
         
         if (error) {
             NSLog(@"Process error");
         } else if (result.isCancelled) {
             NSLog(@"Cancelled");
         } else {
             
             [[[FBSDKGraphRequest alloc] initWithGraphPath:@"me" parameters:nil]
              startWithCompletionHandler:^(FBSDKGraphRequestConnection *connection, id result, NSError *error) {
                  
                  if (!error) {
                      NSLog(@"fetched user:%@  and Email : %@", result[@"id"],[result objectForKey:@"name"]);
                      [[AppHelper getAppHelper] saveBoolUserDefaults:YES withKey:@"facebookauthentication"];
                      [[AppHelper getAppHelper]saveToUserDefaults:result[@"id"] withKey:@"facebookid"];
                      [[AppHelper getAppHelper]saveToUserDefaults:result[@"name"] withKey:@"facebookname"];
                      [self dismissViewControllerAnimated:YES completion:nil];
                      
                  }
              }];
             NSLog(@"Logged in");
         }
     }];
}

- (IBAction)twitterSigninAction:(id)sender {
    UIActivityIndicatorView *activityView = [[UIActivityIndicatorView alloc]
                                             initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhiteLarge];
    
    activityView.center=self.view.center;
    [activityView startAnimating];
    [self.view addSubview:activityView];
    [self.cancelBtn setEnabled:false];
    [self.facebookSignin setEnabled:false];
    [self.googleSigninBtn setEnabled:false];
    [[Twitter sharedInstance] logInWithCompletion:^(TWTRSession *session, NSError *error) {
        if (session) {
            NSLog(@"signed in as %@", [session userName]);
            [[AppHelper getAppHelper] saveBoolUserDefaults:YES withKey:@"twitterauthentication"];
            [[AppHelper getAppHelper]saveToUserDefaults:[session userName] withKey:@"twittername"];
            [[AppHelper getAppHelper]saveToUserDefaults:[session userID] withKey:@"twitterid"];
            [activityView setHidden:YES];
            [self.cancelBtn setEnabled:true];
            [self.facebookSignin setEnabled:true];
            [self.googleSigninBtn setEnabled:true];
            [self.delegare dismisspopover];
        }
        else
        {
            NSLog(@"error: %@", [error localizedDescription]);
            [activityView setHidden:YES];
            [self.cancelBtn setEnabled:true];
            [self.facebookSignin setEnabled:true];
            [self.googleSigninBtn setEnabled:true];
            NSString *message = [NSString stringWithFormat:@"%@",[error localizedDescription]];
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error"
                                                                    message:message
                                                                   delegate:nil
                                                          cancelButtonTitle:@"OK"
                                                          otherButtonTitles:nil];
            [alert show];

        }
    }];
}



#pragma mark - GIDSignInDelegate

- (void)signIn:(GIDSignIn *)signIn
didSignInForUser:(GIDGoogleUser *)user
     withError:(NSError *)error {
    
    if (error) {
        return;
    }
    NSString *userId = user.userID;                  // For client-side use only!
    NSString *idToken = user.authentication.idToken; // Safe to send to the server
    NSString *name = user.profile.name;
    NSString *email = user.profile.email;
    NSLog(@"User name: %@",userId);
    [self reportAuthStatus];
    [self.delegare googleSigninDelegate];
    [self dismissViewControllerAnimated:YES completion:nil];
    
}

- (void)signIn:(GIDSignIn *)signIn didDisconnectWithUser:(GIDGoogleUser *)user withError:(NSError *)error {
    // Perform any operations when the user disconnects from app here.
    if(error){
        NSLog(@"Error");
    }
    else {
        NSLog(@"Success");
    }
    [self reportAuthStatus];
    
}
- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    return [[GIDSignIn sharedInstance] handleURL:url sourceApplication:sourceApplication
                                      annotation:annotation];
}
- (void)presentSignInViewController:(UIViewController*)viewController
{
    [[self navigationController] pushViewController:viewController animated:YES];
}


- (void)reportAuthStatus
{
    GIDGoogleUser* googleUser = [[GIDSignIn sharedInstance] currentUser];
    if (googleUser.authentication) {
        [[AppHelper getAppHelper] saveBoolUserDefaults:YES withKey:@"googleAuthentication"];
       [[AppHelper getAppHelper] saveToUserDefaults:[NSString stringWithFormat:@"%@", [GIDSignIn sharedInstance].currentUser.profile.email] withKey:@"googleEmail"];
        [[AppHelper getAppHelper] saveToUserDefaults:[NSString stringWithFormat:@"%@", [GIDSignIn sharedInstance].currentUser.userID] withKey:@"googleUserId"];
    }
    else {
        // To authenticate, use Google+ sign-in button.
        [[AppHelper getAppHelper] saveBoolUserDefaults:NO withKey:@"googleAuthentication"];
    }
    
    
}

// Implement these methods only if the GIDSignInUIDelegate is not a subclass of
// UIViewController.

#pragma mark - IBActions


- (void)toggleBasicProfile:(UISwitch*)sender
{
    [GIDSignIn sharedInstance].shouldFetchBasicProfile = sender.on;
}

- (void)toggleAllowSignInWithBrowser:(UISwitch*)sender
{
    [GIDSignIn sharedInstance].allowsSignInWithBrowser = sender.on;
}

- (void)toggleAllowSignInWithWebView:(UISwitch*)sender
{
    [GIDSignIn sharedInstance].allowsSignInWithWebView = sender.on;
}

-(void)dealloc{
    [GIDSignIn sharedInstance].uiDelegate = nil;
    [GIDSignIn sharedInstance].delegate = nil;
}

@end
