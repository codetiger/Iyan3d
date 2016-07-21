//
//  LoggedInViewController.m
//  Iyan3D
//
//  Created by Sankar on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//


#import <FBSDKCoreKit/FBSDKCoreKit.h>
#import <FBSDKLoginKit/FBSDKLoginKit.h>
#import <TwitterKit/TwitterKit.h>
#import "AFNetworking.h"
#import "AFHTTPRequestOperation.h"
#import "AFHTTPClient.h"

#import "LoggedInViewController.h"
#import "RenderTableViewCell.h"
#include "Utility.h"

#define GOOGLE_SIGNIN 0
#define FACEBOOK_SIGNIN 1
#define TWITTER_SIGNIN 2

#define IN_PROGRESS 0
#define COMPLETED 1

#define DOWNLOAD_FILE 0
#define GET_FILE_TYPE 1

#define DEFAULT_OR_IMAGE 0
#define VIDEO_FILE 1

@interface LoggedInViewController ()

@end

@implementation LoggedInViewController

#define FIVE_THOUSAND_CREDITS @"basicrecharge"
#define TWENTY_THOUSAND_CREDITS @"mediumrecharge"
#define FIFTY_THOUSAND_CREDITS @"megarecharge"

- (instancetype)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cache = [CacheSystem cacheSystem];
        downloadQueue = [[NSOperationQueue alloc] init];
        completedTask = [[NSMutableArray alloc] init];
        progressingTasks = [[NSMutableArray alloc] init];
        [self updateTableViewData];
        downloadCompletedTaskIds = 0;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.screenName = @"CreditsView iOS";
    [self.creditsLoading setHidden:NO];
    [self.creditsLoading startAnimating];
    [self.creditsLoading setHidesWhenStopped:YES];
    
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(setUserCredits_stopLoading) name:@"creditsupdate" object:nil];
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(requestCredits) name:@"creditsused" object:nil];
    
    [self requestCredits];
    NSNumber *credits = [[AppHelper getAppHelper] userDefaultsForKey:@"credits"];
    self.creditsLabel.text = [NSString stringWithFormat:@"%@", credits];
    NSString * userName = [[AppHelper getAppHelper] userDefaultsForKey:@"username"];
    if([userName length] > 1)
        self.userNameLbl.text = userName;
    
    int signinType = [[[AppHelper getAppHelper] userDefaultsForKey:@"signintype"] intValue];
    NSString* extension = [Utility IsPadDevice] ? @"_Pad.png" : @"_Phone.png";
    
    
    if(signinType == GOOGLE_SIGNIN)
       [self.loginTypeImg setImage:[UIImage imageNamed:[NSString stringWithFormat:@"gPlus%@", extension]]];
    else if(signinType == FACEBOOK_SIGNIN)
        [self.loginTypeImg setImage:[UIImage imageNamed:[NSString stringWithFormat:@"fb%@", extension]]];
    else if(signinType == TWITTER_SIGNIN)
        [self.loginTypeImg setImage:[UIImage imageNamed:[NSString stringWithFormat:@"twit%@", extension]]];

    
    
    [GIDSignIn sharedInstance].uiDelegate = self;
    [GIDSignIn sharedInstance].delegate = self;
    
    
    if(renderData != nil) {
        for(int i = 0; i < [renderData count]; i++) {
            RenderItem* renderItem = [renderData objectAtIndex:i];
            [self getRenderTaskProgress:renderItem.taskId];
        }
    }

    
//    [self.renderStatus registerClass:[RenderTableViewCell class] forCellReuseIdentifier:@"RenderTableViewCell"];
    renderSectionTitles = [NSArray arrayWithObjects:@"In Progress", @"Completed", nil];
    self.creditsView.layer.cornerRadius = 10;
    self.creditsView.layer.masksToBounds = YES;
    
    // Do any additional setup after loading the view from its nib.
}

- (void) requestCredits
{
    if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"]) {
        NSString* uniqueId = [[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"];
        NSString* userName = [[AppHelper getAppHelper] userDefaultsForKey:@"username"];
        NSString* email = [[AppHelper getAppHelper] userDefaultsForKey:@"email"];
        int signinType = [[[AppHelper getAppHelper] userDefaultsForKey:@"signintype"] intValue];
        [[AppHelper getAppHelper] getCreditsForUniqueId:uniqueId Name:userName Email:email SignInType:signinType];
    }
}

- (void) getRenderTaskProgress:(int)taskId
{
    NSURL* url = [NSURL URLWithString:@"https://www.iyan3dapp.com/appapi/checkprogress.php"];
    NSString* postPath = @"https://www.iyan3dapp.com/appapi/checkprogress.php";
    
    AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:[NSString stringWithFormat:@"%d",taskId], @"taskid", nil]];

    AFHTTPRequestOperation* operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    __block BOOL complete = NO;
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation* operation, id responseObject) {
        //ret = [self handle:data];
        complete = YES;
        int progress = [[operation responseString] intValue];
        if(progress != -1)
            [cache updateRenderTask:taskId WithProgress:progress];
        else
            [cache deleteRenderTaskData:taskId];
        
        [self performSelectorOnMainThread:@selector(updateTableViewData) withObject:nil waitUntilDone:YES];
    } failure:^(AFHTTPRequestOperation* operation, NSError* error) {
        NSLog(@" Request failed with error %@ ", error.localizedDescription);
    }];
    
    [operation start];
}

- (void) updateTableViewData
{
    renderData = [cache getRenderTask];
    [completedTask removeAllObjects];
    [progressingTasks removeAllObjects];
    
    if(renderData == nil)
        return;

    for(int i = 0; i < [renderData count]; i++) {
        RenderItem* r = [renderData objectAtIndex:i];
        if(r.taskProgress >= 100)
            [completedTask addObject:r];
        else
            [progressingTasks addObject:r];
    }
    
    [self.renderStatus reloadData];
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
    NSInteger count = 0;
    if(progressingTasks != nil && [progressingTasks count] > 0)
        count++;
    if(completedTask != nil && [completedTask count] > 0)
        count++;
    
    return count;
}
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    NSString* title = (section == IN_PROGRESS && [progressingTasks count] > 0) ? @"In Progress" :  @"Completed";
    return title;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    if((section == IN_PROGRESS && progressingTasks != nil && [progressingTasks count] > 0))
        return [progressingTasks count];
    else
        return (completedTask != nil) ? [completedTask count] : 0;
}

- (RenderTableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSString *renderCellStr = [Utility IsPadDevice] ? @"RenderTableViewCell" :  @"RenderTableViewPhone";
    RenderTableViewCell *cell = (RenderTableViewCell *)[tableView dequeueReusableCellWithIdentifier:renderCellStr];
    if (cell == nil)
    {
        NSArray *nib = [[NSBundle mainBundle] loadNibNamed:renderCellStr owner:self options:nil];
        cell = [nib objectAtIndex:0];
    }
    
    RenderItem* rItem = nil;
    if(indexPath.section == IN_PROGRESS && progressingTasks != nil && [progressingTasks count] > 0) {
        rItem = (RenderItem*)[progressingTasks objectAtIndex:indexPath.row];
    } else if(completedTask != nil) {
        rItem = (RenderItem*)[completedTask objectAtIndex:indexPath.row];
    }
    
    if(rItem == nil)
        return nil;
    
    cell.renderlabel.text = rItem.taskName;
    NSString* progressStr = (rItem.taskProgress > 0) ? [NSString stringWithFormat:@"%d%%",rItem.taskProgress] : @"In Queue";
    progressStr = (indexPath.section == IN_PROGRESS && [progressingTasks count] > 0) ? progressStr : @"Download";
    cell.renderProgressLabel.text = progressStr;
    cell.dateLabel.text = rItem.dateAdded;
    [cell.renderProgressLabel setTag:rItem.taskId];
    
    if(downloadCompletedTaskIds == rItem.taskId){
        [cell.downloadProgress stopAnimating];
        [cell.downloadProgress setHidden:YES];
        cell.renderProgressLabel.text = @"Download";
    }
    
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    if(indexPath.section == IN_PROGRESS && [progressingTasks count] > 0) return;
    RenderTableViewCell *cell = [self.renderStatus cellForRowAtIndexPath:indexPath];
    [cell.downloadProgress setHidden:NO];
    [cell.downloadProgress startAnimating];
    [self.delegare showOrHideProgress:YES];
    [_renderStatus setUserInteractionEnabled:NO];
    if(downloadCompletedTaskIds != cell.renderProgressLabel.tag) {
        [self downloadAndSaveFile:(int)cell.renderProgressLabel.tag WithAction:GET_FILE_TYPE AndType:DEFAULT_OR_IMAGE];
    }
}

- (void) downloadAndSaveFile:(int) taskId WithAction:(int)action AndType:(int) type
{
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [docPaths objectAtIndex:0];
    
    NSString *extension = (type == VIDEO_FILE) ? @"mp4" : @"png";
    NSString* outputFilePath = [NSString stringWithFormat:@"%@/%d.%@", documentsDirectory, taskId, extension];
    
    if(action == DOWNLOAD_FILE && [[NSFileManager defaultManager] fileExistsAtPath:outputFilePath]) {
        outputFile = outputFilePath;
        [self.delegare showPreview:outputFile];
        return;
    }
    
    NSURL *url = [NSURL URLWithString:@"https://www.iyan3dapp.com/appapi/download.php"];
    NSString *postPath = @"https://www.iyan3dapp.com/appapi/download.php";
    
    NSString* taskIdStr = [NSString stringWithFormat:@"%d",taskId];
    NSString* hashFormatStr = [NSString stringWithFormat:@"%d-%@",taskId, [[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"]];
    NSString* uHashStr = [Utility getMD5ForString:hashFormatStr];
    
    AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:taskIdStr, @"taskid", uHashStr, @"uhash", [NSString stringWithFormat:@"%d", action], @"action", nil]];
    
    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        if(action == GET_FILE_TYPE)
            [self downloadAndSaveFile:taskId WithAction:DOWNLOAD_FILE AndType:[[operation responseString] intValue]];
        else {
////            NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
////            NSString* documentsDirectory = [docPaths objectAtIndex:0];
////
////            NSString *extension = (type == VIDEO_FILE) ? @"mp4" : @"png";
//            NSString* outputFilePath = [NSString stringWithFormat:@"%@/%d.%@", documentsDirectory, taskId, extension];
            outputFile = outputFilePath;
            NSLog(@" \n Output File %@ ", outputFile);
            NSData *fileData = [operation responseData];
            [fileData writeToFile:outputFilePath atomically:YES];
            
            if([[NSFileManager defaultManager] fileExistsAtPath:outputFilePath]) {
                if(type == VIDEO_FILE)
                    UISaveVideoAtPathToSavedPhotosAlbum(outputFilePath, self,  @selector(video:didFinishSavingWithError:contextInfo:), nil);
                else {
                    UIImage *img = [UIImage imageWithContentsOfFile:outputFilePath];
                    UIImageWriteToSavedPhotosAlbum(img, self, @selector(image:didFinishSavingWithError:contextInfo:), nil);
                }
                    
            }
            downloadCompletedTaskIds = taskId;
        }
    }
                                     failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                         NSLog(@"Failure: %@", error.localizedDescription);
                                         UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:@"Failure Error" message:@"Cannot download file. Check your net connection." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                                         [userNameAlert show];
                                         downloadCompletedTaskIds = taskId;
                                         [_renderStatus setUserInteractionEnabled:YES];
                                         [_renderStatus reloadData];
                                     }];
    [operation start];
}

 - (void)image:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo
{
    if(error != nil)
        [self performSelectorOnMainThread:@selector(showCompletionAlert:) withObject:[NSNumber numberWithBool:NO] waitUntilDone:NO];
    else
        [self performSelectorOnMainThread:@selector(showCompletionAlert:) withObject:[NSNumber numberWithBool:YES] waitUntilDone:NO];
    [self.delegare showOrHideProgress:NO];
    [_renderStatus setUserInteractionEnabled:YES];
    [_renderStatus reloadData];
}

 - (void)video:(NSString *)videoPath didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo
{
    if(error != nil)
        [self performSelectorOnMainThread:@selector(showCompletionAlert:) withObject:[NSNumber numberWithBool:NO] waitUntilDone:NO];
    else
        [self performSelectorOnMainThread:@selector(showCompletionAlert:) withObject:[NSNumber numberWithBool:YES] waitUntilDone:NO];
    [self.delegare showOrHideProgress:NO];
    [_renderStatus setUserInteractionEnabled:YES];
    [_renderStatus reloadData];

}

- (void) showCompletionAlert:(id) object
{
    
    NSString * message = @"";
    if([object boolValue])
        message = @"Image/Video successfully saved to Photos.";
    else
        message = @"There is a problem downloading the image/video. Please try again later.";
    
    UIAlertView *alert=[[UIAlertView alloc] initWithTitle:@"Information" message:message delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    alert.delegate = self;
    
    [alert show];
    
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
}

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if([outputFile length] > 5) {
        [self.delegare showPreview:outputFile];
    }
}

- (IBAction)signOutBtn:(id)sender
{
    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"]){
        
        int signinType = [[[AppHelper getAppHelper] userDefaultsForKey:@"signintype"] intValue];
        if(signinType == GOOGLE_SIGNIN)
            [[GIDSignIn sharedInstance] disconnect];
        else if (signinType == FACEBOOK_SIGNIN) {
            FBSDKLoginManager *loginManager = [[FBSDKLoginManager alloc] init];
            [loginManager logOut];
        } else if (signinType == TWITTER_SIGNIN) {
            [[[Twitter sharedInstance] sessionStore] logOutUserID:[[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"]];
        }

        [self reportAuthStatus];
        [self.delegare dismissView:self];
    }
}

- (IBAction)manageAccountAction:(id)sender {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://www.iyan3dapp.com/cms"]];
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
    //[[NSNotificationCenter defaultCenter] removeObserver:self forKeyPath:@"creditsupdate"];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:(BOOL)animated];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"creditsused" object:nil];
    [renderData removeAllObjects];
    [progressingTasks removeAllObjects];
    [completedTask removeAllObjects];
    
    renderData = nil;
    progressingTasks = nil;
    completedTask = nil;
}

- (IBAction)add500Credits:(id)sender
{
    [self.creditsLoading setHidden:NO];
    [self.creditsLoading startAnimating];
    [self.creditsLoading setHidesWhenStopped:YES];

    [AppHelper getAppHelper].delegate = self;
    [[AppHelper getAppHelper] addTransactionObserver];
    [[AppHelper getAppHelper] callPaymentGateWayForProduct:FIVE_THOUSAND_CREDITS];
}

- (IBAction)add2KCredits:(id)sender
{
    [self.creditsLoading setHidden:NO];
    [self.creditsLoading startAnimating];
    [self.creditsLoading setHidesWhenStopped:YES];

    [AppHelper getAppHelper].delegate = self;
    [[AppHelper getAppHelper] addTransactionObserver];
    [[AppHelper getAppHelper] callPaymentGateWayForProduct:TWENTY_THOUSAND_CREDITS];
}

- (IBAction)add5KCredits:(id)sender
{
    [self.creditsLoading setHidden:NO];
    [self.creditsLoading startAnimating];
    [self.creditsLoading setHidesWhenStopped:YES];

    [AppHelper getAppHelper].delegate = self;
    [[AppHelper getAppHelper] addTransactionObserver];
    [[AppHelper getAppHelper] callPaymentGateWayForProduct:FIFTY_THOUSAND_CREDITS];
}

-(void)statusForOBJImport:(NSNumber*)status
{
    if([status intValue] != 0) {
        NSString* uniqueId = [[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"];
        [[AppHelper getAppHelper] useOrRechargeCredits:uniqueId credits:[status intValue] For:@"RECHARGE"];
    }
}

- (void)transactionCancelled
{
    [self.creditsLoading setHidden:YES];
    [self.creditsLoading stopAnimating];

    [[AppHelper getAppHelper] removeTransactionObserver];
    [AppHelper getAppHelper].delegate = nil;
}

-(void)loadingViewStatus:(BOOL)status
{
    
}

-(void)premiumUnlocked
{
    
}
-(void)addRestoreId:(NSString*)productIdentifier
{
    
}
-(void)statusForRestorePurchase:(NSNumber *)object
{
    
}
-(void)setAnimationData:(NSArray*)allAnimations
{
    
}


@end
