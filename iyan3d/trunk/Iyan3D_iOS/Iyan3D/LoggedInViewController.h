//
//  LoggedInViewController.h
//  Iyan3D
//
//  Created by Sankar on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GoogleSignIn/GoogleSignIn.h>
#import "AppHelper.h"

@protocol LoggedinViewControllerDelegat
-(void)dismissView;
@end

@class GIDSignIn;

@interface LoggedInViewController : UIViewController<UITableViewDataSource,UITableViewDelegate,GIDSignInDelegate,GIDSignInUIDelegate>{
    NSDictionary *renderData;
    NSArray *renderSectionTitles;
    
}
@property (weak, nonatomic) IBOutlet UIView *creditsView;
@property (weak, nonatomic) IBOutlet UITableView *renderStatus;
@property (weak, nonatomic) IBOutlet UIButton *signOutBtn;
@property (weak, nonatomic) id <LoggedinViewControllerDelegat> delegare;
- (IBAction)signOutBtn:(id)sender;
@end
