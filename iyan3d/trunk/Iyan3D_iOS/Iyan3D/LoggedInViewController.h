//
//  LoggedInViewController.h
//  Iyan3D
//
//  Created by Sankar on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import <GoogleSignIn/GoogleSignIn.h>
#import "DownloadTask.h"
#import "AppHelper.h"

@protocol LoggedinViewControllerDelegat
- (void)dismissView:(UIViewController*) VC;
- (void) showOrHideProgress:(BOOL) value;
- (void) showPreview:(NSString*) outputPath;
@end

@class GIDSignIn;

@interface LoggedInViewController : GAITrackedViewController<UITableViewDataSource,UITableViewDelegate,GIDSignInDelegate,GIDSignInUIDelegate, AppHelperDelegate, UIAlertViewDelegate>{
    
    CacheSystem* cache;
    NSMutableArray *renderData;
    NSMutableArray *progressingTasks;
    NSMutableArray *completedTask;
    NSArray *renderSectionTitles;
    NSOperationQueue* downloadQueue;
    int downloadCompletedTaskIds;
    NSString* outputFile;
    
}
- (IBAction)add500Credits:(id)sender;
- (IBAction)add2KCredits:(id)sender;
- (IBAction)add5KCredits:(id)sender;

@property (weak, nonatomic) IBOutlet UIImageView *loginTypeImg;
@property (weak, nonatomic) IBOutlet UILabel *userNameLbl;

@property (weak, nonatomic) IBOutlet UIView *creditsView;
@property (weak, nonatomic) IBOutlet UITableView *renderStatus;
@property (weak, nonatomic) id <LoggedinViewControllerDelegat> delegare;
@property (weak, nonatomic) IBOutlet UILabel *creditsLabel;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *creditsLoading;

@property (weak, nonatomic) IBOutlet UILabel *creditsTitleLabel;
@property (weak, nonatomic) IBOutlet UILabel *addCreditsLabel;
@property (weak, nonatomic) IBOutlet UIButton *add5kCreditsButton;
@property (weak, nonatomic) IBOutlet UIButton *add20kCreditsButton;
@property (weak, nonatomic) IBOutlet UIButton *add50kCreditsButton;

@property (weak, nonatomic) IBOutlet UILabel *hqProgressTitleLabel;

@property (weak, nonatomic) IBOutlet UIButton *manageAccountButton;
@property (weak, nonatomic) IBOutlet UIButton *signOutBtn;

- (IBAction)signOutBtn:(id)sender;
- (IBAction)manageAccountAction:(id)sender;

@end
