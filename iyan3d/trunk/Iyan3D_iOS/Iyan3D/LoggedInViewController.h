//
//  LoggedInViewController.h
//  Iyan3D
//
//  Created by Sankar on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GoogleSignIn/GoogleSignIn.h>
#import "DownloadTask.h"
#import "AppHelper.h"

@protocol LoggedinViewControllerDelegat
-(void)dismissView;
@end

@class GIDSignIn;

@interface LoggedInViewController : UIViewController<UITableViewDataSource,UITableViewDelegate,GIDSignInDelegate,GIDSignInUIDelegate, AppHelperDelegate>{
    
    CacheSystem* cache;
    NSMutableArray *renderData;
    NSMutableArray *progressingTasks;
    NSMutableArray *completedTask;
    NSArray *renderSectionTitles;
    NSOperationQueue* downloadQueue;
    
}
- (IBAction)add500Credits:(id)sender;
- (IBAction)add2KCredits:(id)sender;
- (IBAction)add5KCredits:(id)sender;

@property (weak, nonatomic) IBOutlet UIView *creditsView;
@property (weak, nonatomic) IBOutlet UITableView *renderStatus;
@property (weak, nonatomic) IBOutlet UIButton *signOutBtn;
@property (weak, nonatomic) id <LoggedinViewControllerDelegat> delegare;
@property (weak, nonatomic) IBOutlet UILabel *creditsLabel;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *creditsLoading;
- (IBAction)signOutBtn:(id)sender;
@end
