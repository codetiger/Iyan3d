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

#import "LoggedInViewController.h"
#import "RenderTableViewCell.h"
#include "Utility.h"

#define GOOGLE_SIGNIN 0
#define FACEBOOK_SIGNIN 1
#define TWITTER_SIGNIN 2

#define IN_PROGRESS 0
#define COMPLETED 1

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
        NSLog(@" \n resp str %@ , resp object %@ ", [operation responseString], responseObject);
        int progress = [[operation responseString] intValue];
        printf(" \n Task ID %d Progress %d \n", taskId, progress);
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
    [cell.renderProgressLabel setHidden:(indexPath.section == COMPLETED || (progressingTasks != nil && [progressingTasks count] == 0))];
    NSString* progressStr = (rItem.taskProgress > 0) ? [NSString stringWithFormat:@"%d%%",rItem.taskProgress] : @"In Queue";
    cell.renderProgressLabel.text = progressStr;
    cell.dateLabel.text = rItem.dateAdded;
    [cell.downloadBtn setHidden:(indexPath.section == IN_PROGRESS && [progressingTasks count] > 0)];
    
    [cell.downloadBtn setTag:rItem.taskId];
    
    if(downloadCompletedTaskIds == rItem.taskId){
        [cell.downloadProgress stopAnimating];
        [cell.downloadProgress setHidden:YES];
        [cell.downloadBtn setHidden:NO];
    }
    

    //[cell.downloadBtn addTarget:self action:@selector(downloadOutputVideo:) forControlEvents:UIControlEventTouchUpInside];
    
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    if(indexPath.section == IN_PROGRESS && [progressingTasks count] > 0) return;
    RenderTableViewCell *cell = [self.renderStatus cellForRowAtIndexPath:indexPath];
    [cell.downloadProgress setHidden:NO];
    [cell.downloadProgress startAnimating];
    [cell.downloadBtn setHidden:YES];
    if(downloadCompletedTaskIds != cell.downloadBtn.tag)
        [self downloadOutputVideo:(int)cell.downloadBtn.tag];
}

- (void) downloadOutputVideo:(int)taskId
{
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [docPaths objectAtIndex:0];
    
    //int taskId = (int)((UIButton*)sender).tag;
    NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/renderFiles/%d/%d.mp4",taskId, taskId];
    NSString* outputFilePath = [NSString stringWithFormat:@"%@/%d.mp4", documentsDirectory, taskId];
    
    DownloadTask* task = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(donwloadCompleted:) returnObject:[NSNumber numberWithInt:taskId] outputFilePath:outputFilePath andURL:url];
    task.taskType = DOWNLOAD_AND_WRITE;
    task.queuePriority = NSOperationQueuePriorityHigh;
    [downloadQueue addOperation:task];
}

- (void) downloadImage:(int)taskId frames:(int)frames
{
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [docPaths objectAtIndex:0];
    
    NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/renderFiles/%d/%d.png", taskId, taskId];
    NSString* outputFilePath = [NSString stringWithFormat:@"%@/%d.png", documentsDirectory, taskId];
    
    NSLog(@" \n URL %@ ", url);
    NSLog(@" \n path %@ ", outputFilePath);
    DownloadTask* task = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(donwloadCompleted:) returnObject:[NSNumber numberWithInt:taskId] outputFilePath:outputFilePath andURL:url];
    task.taskType = DOWNLOAD_AND_WRITE;
    task.queuePriority = NSOperationQueuePriorityHigh;
    [downloadQueue addOperation:task];
    
}

- (void) donwloadCompleted:(id)object
{
    DownloadTask* t = (DownloadTask*)object;
    NSString* extension = [t.outputPath pathExtension];
    int taskId = [t.returnObj intValue];
    RenderItem *r = [cache getRenderTaskByTaskId:taskId];
    if ([extension isEqualToString:@"png"]) {
        NSLog(@" \n output %@ ", t.outputPath);
        if([[NSFileManager defaultManager] fileExistsAtPath:t.outputPath]) {
            UIImage *img = [UIImage imageWithContentsOfFile:t.outputPath];
            UIImageWriteToSavedPhotosAlbum(img, self, @selector(image:didFinishSavingWithError:contextInfo:), nil);
        } else {
            [self showCompletionAlert:[NSNumber numberWithBool:NO]];
        }
    } else if ([extension isEqualToString:@"mp4"]) {
        if(![[NSFileManager defaultManager] fileExistsAtPath:t.outputPath])
            [self downloadImage:taskId frames:r.taskFrames];
        else {
            UISaveVideoAtPathToSavedPhotosAlbum(t.outputPath, self,  @selector(video:didFinishSavingWithError:contextInfo:), nil);
        }
    }
    downloadCompletedTaskIds = taskId;
    [_renderStatus reloadData];
}

 - (void)image:(UIImage *)image didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo
{
    if(error != nil)
        [self performSelectorOnMainThread:@selector(showCompletionAlert:) withObject:[NSNumber numberWithBool:NO] waitUntilDone:NO];
    else
        [self performSelectorOnMainThread:@selector(showCompletionAlert:) withObject:[NSNumber numberWithBool:YES] waitUntilDone:NO];
}

 - (void)video:(NSString *)videoPath didFinishSavingWithError:(NSError *)error contextInfo:(void *)contextInfo
{
    if(error != nil)
        [self performSelectorOnMainThread:@selector(showCompletionAlert:) withObject:[NSNumber numberWithBool:NO] waitUntilDone:NO];
    else
        [self performSelectorOnMainThread:@selector(showCompletionAlert:) withObject:[NSNumber numberWithBool:YES] waitUntilDone:NO];

}

- (void) showCompletionAlert:(id) object
{
    
    NSString * message = @"";
    if([object boolValue])
        message = @"Image/Video successfully saved to Photos.";
    else
        message = @"There is a problem downloading the image/video. Please try again later.";
    
    UIAlertView *alert=[[UIAlertView alloc] initWithTitle:@"Information" message:message delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alert show];
    
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

@end
