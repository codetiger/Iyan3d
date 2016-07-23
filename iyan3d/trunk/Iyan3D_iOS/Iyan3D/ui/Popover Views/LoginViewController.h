//
//  LoginViewController.h
//  Iyan3D
//
//  Created by Sankar on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import <GoogleSignIn/GoogleSignIn.h>
#import "AppHelper.h"
@protocol LoginViewControllerDelegate
-(void)googleSigninDelegate;
-(void)dismisspopover;
- (IBAction)loginBtnAction:(id)sender;
@end

@class GIDSignInButton;


@interface LoginViewController : GAITrackedViewController<GIDSignInDelegate, GIDSignInUIDelegate,AppHelperDelegate>{
    
}

@property (weak, nonatomic) IBOutlet UIButton *twitterSignin;
@property (weak, nonatomic) IBOutlet UIButton *facebookSignin;
@property (weak, nonatomic) IBOutlet UIButton *googleSigninBtn;

@property (weak, nonatomic) IBOutlet UILabel *infoLabel;
@property (weak, nonatomic) IBOutlet UILabel *signinLabel;

@property (weak, nonatomic) id <LoginViewControllerDelegate> delegare;

- (IBAction)googleSigninAction:(id)sender;
- (IBAction)fbSigninAction:(id)sender;
- (IBAction)twitterSigninAction:(id)sender;
@end
