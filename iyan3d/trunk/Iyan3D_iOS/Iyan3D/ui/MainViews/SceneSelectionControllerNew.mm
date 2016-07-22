//
//  SceneSelectionControllerNew.m
//  Iyan3D
//
//  Created by harishankarn on 16/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//


#import <Crashlytics/Answers.h>

#import "MediaPreviewVC.h"
#import "AppDelegate.h"
#import "SceneSelectionControllerNew.h"
#import "SceneItem.h"
#import <sys/utsname.h>
#import "ZipArchive.h"
#import "AFHTTPRequestOperation.h"
#import "AFHTTPClient.h"

#define SCENE_NAME_ALERT 0
#define LOGOUT_ALERT 1
#define CANCEL 0
#define OK 1

#define TUTORIAL 0
#define SETTINGS 1
#define CONTACT_US 4
#define FOLLOW_US 3
#define RATE_US 2

#define CAMERA_PREVIEW_SMALL 0

#define FRAME_COUNT 0
#define FRAME_DURATION 1

#define RESTORE_PURCHASH_ALERT 2

#define ALERT_TYPE 1
#define IMAGE_TYPE 2
#define URL_TYPE 3

@implementation SceneSelectionControllerNew

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil IsFirstTimeOpen:(BOOL)value
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cache = [CacheSystem cacheSystem];
        scenesArray = [cache GetSceneList];
        dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy/MM/dd HH:mm:ss"];
        currentSelectedScene = -1;
        isFirstTime = value;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.sceneTitleLabel.text = NSLocalizedString(@"Scenes", nil);
    
    self.screenName = @"SceneSelectionView iOS";
    [self.feedCount setHidden:YES];
    
    self.feedCount.layer.cornerRadius = 20.0;
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    screenHeight = screenRect.size.height;
    if([Utility IsPadDevice]){
        [self.scenesCollectionView registerNib:[UINib nibWithNibName:@"SceneSelectionFrameCell" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    else{
        [[UIApplication sharedApplication] setStatusBarHidden:NO];
        [self.scenesCollectionView registerNib:[UINib nibWithNibName:@"SceneSelectionFrameCellPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    [self.sceneView setHidden:YES];
    
    if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"] && ![[AppHelper getAppHelper] userDefaultsBoolForKey:@"hasRestored"] && isFirstTime) {
        UIAlertView* infoAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Information", nil) message:NSLocalizedString(@"Already_upgraded_Premium_Please_Restore_Purchase", nil) delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [infoAlert setTag:RESTORE_PURCHASH_ALERT];
        [infoAlert show];
    }
    
    isFirstTimeUser = false;
    [self loadNewsFeedData];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    restClient = [[DBRestClient alloc] initWithSession:[DBSession sharedSession]];
    restClient.delegate = self;

    if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"openrenderTasks"]) {
        [self openLoggedInView];
        [[AppHelper getAppHelper] saveBoolUserDefaults:NO withKey:@"openrenderTasks"];
    }
    [self.centerLoading setHidden:YES];
    
    [self checkAndOpeni3d];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(shareScene:) name:@"DBlinked" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(openLoggedInView) name:@"renderCompleted" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(checkAndOpeni3d) name:@"i3dopen" object:nil];

    [[AppHelper getAppHelper] moveFilesFromInboxDirectory:cache];
    
    UITapGestureRecognizer* tapGest = [[UITapGestureRecognizer alloc] initWithTarget:self action:nil];
    tapGest.delegate = self;
    [self.view addGestureRecognizer:tapGest];
    
    if(![[AppHelper getAppHelper] userDefaultsForKey:@"onBoardShown"]) {
        [self presentOnBoard];
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithBool:YES] withKey:@"onBoardShown"];
    }
}

- (void) loadNewsFeedData
{
    
    NSURL *url = [NSURL URLWithString:@"https://www.iyan3dapp.com/appapi/newsfeed.php"];
    NSString *postPath = @"https://www.iyan3dapp.com/appapi/newsfeed.php";
    
    NSString* lastid = @"0";
    if([[AppHelper getAppHelper] userDefaultsForKey:@"lastfeedid"])
        lastid = [[AppHelper getAppHelper] userDefaultsForKey:@"lastfeedid"];
    
    AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:lastid, @"lastid", nil]];
    
    AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        NSError * jsonError;
        NSArray* jsonArray = [[NSArray alloc] init];
        jsonArray = [NSJSONSerialization JSONObjectWithData:responseObject options:NSJSONReadingAllowFragments error:&jsonError];
        if(!jsonError && [jsonArray count] > 0) {
            for(int i = 0; i < [jsonArray count]; i++) {
                FeedItem *f = [[FeedItem alloc] init];
                f.itemId = [[[jsonArray objectAtIndex:i] objectForKey:@"id"] intValue];
                f.type = [[[jsonArray objectAtIndex:i] objectForKey:@"type"] intValue];
                f.title = [[jsonArray objectAtIndex:i] objectForKey:@"title"];
                f.message = [[jsonArray objectAtIndex:i] objectForKey:@"message"];
                f.url = [[jsonArray objectAtIndex:i] objectForKey:@"url"];
                f.thumbImage = [[jsonArray objectAtIndex:i] objectForKey:@"thumbnailurl"];
                f.isRead = false;
                [cache addNewsFeed:f];
                
                if(i == 0) {
                    [[AppHelper getAppHelper] saveToUserDefaults:[NSString stringWithFormat:@"%d", f.itemId] withKey:@"lastfeedid"];
                }
            }
            
            NSMutableArray * feedItems = [cache getNewsFeedsFromLocal];
            [self performSelectorOnMainThread:@selector(setFeedsCount:) withObject:feedItems waitUntilDone:NO];
        } else {
            NSMutableArray * feedItems = [cache getNewsFeedsFromLocal];
            [self performSelectorOnMainThread:@selector(setFeedsCount:) withObject:feedItems waitUntilDone:NO];
        }
    }
                                     failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                         NSLog(@"Failure: %@", error.localizedDescription);
                                         NSMutableArray * feedItems = [cache getNewsFeedsFromLocal];
                                         [self performSelectorOnMainThread:@selector(setFeedsCount:) withObject:feedItems waitUntilDone:NO];
                                     }];
    [operation start];
    
}

- (void) setFeedsCount:(NSMutableArray*) feedItems
{
    int count = 0;
    for(int i = 0; i < [feedItems count]; i ++) {
        FeedItem* f = [feedItems objectAtIndex:i];
        if(!f.isRead)
            count++;
    }
    
    if(count > 0) {
        [self.feedCount setHidden:NO];
        self.feedCount.text = [NSString stringWithFormat:@"%d", count];
        self.feedCount.layer.cornerRadius = 7.5;
        self.feedCount.clipsToBounds = YES;
    } else
        [self.feedCount setHidden:YES];
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
    if([touch.view isDescendantOfView:self.helpBtn]) {
        return YES;
    }
    
    [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
    return NO;
}

- (void) checkAndOpeni3d
{
    if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"i3dopen"]) {
        NSString* i3dPath = [[AppHelper getAppHelper] userDefaultsForKey:@"i3dpath"];
        [self showLoading];
        [self performSelectorInBackground:@selector(unzipAndRestoreScenei3d:) withObject:i3dPath];
        [[AppHelper getAppHelper] saveBoolUserDefaults:NO withKey:@"i3dopen"];
    }
}

- (void) showLoading
{
    [self.centerLoading setHidden:NO];
    [self.centerLoading startAnimating];
}

- (void)hideLoading
{
    [self.centerLoading stopAnimating];
    [self.centerLoading setHidden:YES];
    scenesArray = [cache GetSceneList];
    [self.scenesCollectionView reloadData];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"DBlinked" object:nil];

    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"i3dopen" object:nil];

    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"renderCompleted" object:nil];
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (void) openLoggedInView
{
    [self performSelectorOnMainThread:@selector(loginBtnAction:) withObject:nil waitUntilDone:YES];
}

- (void) unzipAndRestoreScenei3d:(NSString*) filePath
{
    NSFileManager *fm = [NSFileManager defaultManager];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* projectsDir = [NSString stringWithFormat:@"%@/Projects", documentsDirectory];
    NSString *unzipPath = [NSString stringWithFormat:@"%@/Unzip",documentsDirectory];

    ZipArchive *zip = [[ZipArchive alloc] init];
    if([zip UnzipOpenFile:filePath]) {
        if([zip UnzipFileTo:unzipPath overWrite:YES]) {
            [fm removeItemAtPath:filePath error:nil];
            NSString *sceneFile = [cache insertImportedScene];
    
            NSArray * filesArr = [fm contentsOfDirectoryAtPath:unzipPath error:nil];
            NSArray *sgbArr = [filesArr filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.sgb'"]];
            NSString* oldSgbPath = [NSString stringWithFormat:@"%@/%@", unzipPath, [sgbArr objectAtIndex:0]];
            NSString* oldSgbName = [[oldSgbPath lastPathComponent] stringByDeletingPathExtension];
            NSString* newsgbPath = [NSString stringWithFormat:@"%@/%@.sgb", projectsDir, sceneFile];
            NSString* oldThumbPath = [NSString stringWithFormat:@"%@/%@.png", unzipPath, oldSgbName];
            NSString* newThumbPath = [NSString stringWithFormat:@"%@/%@.png", projectsDir, sceneFile];
            
            [fm moveItemAtPath:oldSgbPath toPath:newsgbPath error:nil];
            [fm moveItemAtPath:oldThumbPath toPath:newThumbPath error:nil];
            
            filesArr = [fm contentsOfDirectoryAtPath:unzipPath error:nil];
            [self moveFilesToRespectiveDirs:filesArr];
        }
    }
    [zip UnzipCloseFile];
    [fm removeItemAtPath:filePath error:nil];
    [self performSelectorOnMainThread:@selector(hideLoading) withObject:nil waitUntilDone:YES];
}

- (void) moveFilesToRespectiveDirs:(NSArray*) filePaths
{
    NSFileManager *fm = [NSFileManager defaultManager];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSArray* cachePaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* cachesDir = [cachePaths objectAtIndex:0];
    NSString *unzipPath = [NSString stringWithFormat:@"%@/Unzip",documentsDirectory];

    for(NSString * f in filePaths) {
        NSString* fPath = [NSString stringWithFormat:@"%@/%@", unzipPath, f];
        NSString* extension = [fPath pathExtension];
        NSString* fileName = [fPath lastPathComponent];
        NSString* destPath= @"";
        
        if([extension isEqualToString:@"sgm"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Sgm/%@",documentsDirectory, fileName];
        else if ([extension isEqualToString:@"png"]) {
            destPath = [NSString stringWithFormat:@"%@/%@", cachesDir, fileName];
            [fm copyItemAtPath:fPath toPath:destPath error:nil];
            destPath = [NSString stringWithFormat:@"%@/Resources/Textures/%@",documentsDirectory, fileName];
        }
        else if ([extension isEqualToString:@"obj"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Objs/%@",documentsDirectory, fileName];
        else if ([extension isEqualToString:@"MOV"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Videos/%@",documentsDirectory, fileName];
        else if ([extension isEqualToString:@"sgr"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Rigs/%@",documentsDirectory, fileName];
        else if ([extension isEqualToString:@"png"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Textures/%@",documentsDirectory, fileName];
        else if ([extension isEqualToString:@"ttf"] || [extension isEqualToString:@"otf"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Fonts/%@",documentsDirectory, fileName];
        else
            destPath = [NSString stringWithFormat:@"%@/%@",documentsDirectory, fileName];
        
        if(![fm fileExistsAtPath:[destPath stringByDeletingLastPathComponent]])
            [fm createDirectoryAtPath:[destPath stringByDeletingLastPathComponent] withIntermediateDirectories:YES attributes:nil error:nil];
        
        NSError *error;
        [fm moveItemAtPath:fPath toPath:destPath error:&error];
        if(error)
            NSLog(@" Error moving resource %@ %@", fPath, error.localizedDescription);
    }
}

#pragma Button Actions

- (IBAction)addSceneButtonAction:(id)sender
{
    [Answers logCustomEventWithName:@"CreateNewSceneInTopLeft" customAttributes:@{}];
    [self addNewScene];
}

- (IBAction)scenePreviewClosebtn:(id)sender
{
    //[self popZoomOut];
}

- (IBAction)feedBackButtonAction:(id)sender
{
    
    if ([MFMailComposeViewController canSendMail]) {
        MFMailComposeViewController *picker = [[MFMailComposeViewController alloc] init];
        picker.mailComposeDelegate = self;
        NSArray *usersTo = [NSArray arrayWithObject: @"iyan3d@smackall.com"];
        [picker setSubject:[NSString stringWithFormat:@"Feedback on Iyan 3d app (%@, iOS Version: %.01f)", [self deviceName], iOSVersion]];
        [picker setToRecipients:usersTo];
        [self presentModalViewController:picker animated:YES];
    } else {
        [self.view endEditing:YES];
        UIAlertView *alert=[[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Alert", nil) message:NSLocalizedString(@"No_Email_account_configured", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
        [alert show];
        return;
    }
}

#pragma CollectionView Delegates

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return [scenesArray count]+1;
}

- (SceneSelectionFrameCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
   SceneSelectionFrameCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
    cell.delegate=self;
    if(indexPath.row < [scenesArray count]) {
        if(indexPath.row == 0) {
            [cell setAccessibilityHint:@"Tap to open the scene."];
            [cell setAccessibilityIdentifier:@"3"];
            [cell.propertiesBtn setAccessibilityHint:@"Tap to Clone / Delete / Share the scene."];
            [cell.propertiesBtn setAccessibilityIdentifier:@"3"];
        } else {
            [cell setAccessibilityHint:@""];
            [cell.propertiesBtn setAccessibilityHint:@""];
        }
        
        SceneItem* scenes = scenesArray[indexPath.row];
        cell.name.text = [NSString stringWithFormat:@"%@",scenes.name];
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* documentsDirectory = [paths objectAtIndex:0];
        NSString* originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scenes.sceneFile];
        BOOL fileExist = [[NSFileManager defaultManager] fileExistsAtPath:originalFilePath];
        cell.propertiesBtn.hidden=NO;
        cell.SelectedindexValue= indexPath.row;
    if(fileExist)
        cell.image.image = [UIImage imageWithContentsOfFile:originalFilePath];
    else
        cell.image.image= [UIImage imageNamed:@"bgImageforall.png"];
    }
    else{
        if(indexPath.row == 0) {
            [cell setAccessibilityHint:@"Creates a new scene."];
            [cell setAccessibilityIdentifier:@"3"];
         }

        cell.name.text = @"Create Scene";
        cell.SelectedindexValue= indexPath.row;
        cell.propertiesBtn.hidden=YES;
        cell.image.image = [UIImage imageNamed:@"New-scene.png"];
    }
    return cell;
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    SceneSelectionFrameCell *cell = (SceneSelectionFrameCell*)[collectionView cellForItemAtIndexPath:indexPath];
    cell_center = [self.scenesCollectionView convertPoint:cell.center fromView:cell];
    cell_center = [self.view convertPoint:cell_center fromView:self.scenesCollectionView];
    
    if(indexPath.row == [scenesArray count]) {
        [Answers logCustomEventWithName:@"CreateNewSceneInCollectionView" customAttributes:@{}];
        [self addNewScene];
    }
    else
        [self openSCene:(int)indexPath.row];
}

#pragma Scene Update Delegates

- (void) addNewScene
{
    SceneItem* scene = [[SceneItem alloc] init];
    SceneItem *previousScene;
    if([scenesArray count] != 0)
        previousScene = scenesArray[[scenesArray count]-1];
    scene.name = [NSString stringWithFormat:NSLocalizedString(@"My Scene %d", nil), previousScene.sceneId+1];
    scene.createdDate = [dateFormatter stringFromDate:[NSDate date]];
    
    if([cache AddScene:scene]) {
        scenesArray = [cache GetSceneList];
        [self.scenesCollectionView reloadData];
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:[scenesArray count]-1 inSection:0];
        [self.scenesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredVertically animated:YES];
    }
}

-(void)openSCene: (int)selectedScene
{
    if(_fromLoadingView) {
        NSMutableDictionary * attributes = [[NSMutableDictionary alloc] init];
        if(_isAppFirstTime)
            [attributes setObject:@"YES" forKey:@"FirstTimeUser"];
        [Answers logCustomEventWithName:@"SceneOpenedFirstTime" customAttributes:attributes];
    }
    
    SceneItem *scene = scenesArray[selectedScene];
    [[AppHelper getAppHelper] resetAppHelper];
    
    if([Utility IsPadDevice]) {
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:@"EditorViewController" bundle:nil SceneItem:scene selectedindex:selectedScene];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    } else {
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:([self iPhone6Plus]) ? @"EditorViewControllerPhone@2x" : @"EditorViewControllerPhone" bundle:nil SceneItem:scene selectedindex:selectedScene];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    }
    
    [self removeFromParentViewController];
}

- (IBAction)infoBtnAction:(id)sender
{
    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"infoBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(200.0, 39*5);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popoverController.delegate =self;
    self.popUpVc.delegate=self;
    CGRect rect = _infoBtn.frame;
    rect = [self.view convertRect:rect fromView:_infoBtn.superview];
    [self.popoverController presentPopoverFromRect:rect inView:self.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}

- (void) infoBtnDelegateAction:(int)indexValue
{
    if(indexValue == TUTORIAL) {
        [self.popoverController dismissPopoverAnimated:YES];
        [self presentOnBoard];

    } else if(indexValue==SETTINGS) {
        [self.popoverController dismissPopoverAnimated:YES];
        settingsVc = [[SettingsViewController alloc]initWithNibName:([Utility IsPadDevice]) ? @"SettingsViewController" :
                      ([self iPhone6Plus]) ? @"SettingsViewControllerPhone2x" : @"SettingsViewControllerPhone" bundle:nil];
        [settingsVc.view setClipsToBounds:YES];
        settingsVc.delegate=self;
        if([Utility IsPadDevice] || screenHeight < 400)
            settingsVc.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:settingsVc animated:YES completion:nil];
        settingsVc.view.superview.backgroundColor = [UIColor clearColor];

    } else if(indexValue==CONTACT_US) {
        [self.popoverController dismissPopoverAnimated:YES];
        if ([MFMailComposeViewController canSendMail]) {
            MFMailComposeViewController *picker = [[MFMailComposeViewController alloc] init];
            picker.mailComposeDelegate = self;
            NSArray *usersTo = [NSArray arrayWithObject: @"iyan3d@smackall.com"];
            [picker setSubject:[NSString stringWithFormat:@"Feedback on Iyan 3d app (%@  , iOS Version: %.01f)", [self deviceName],iOSVersion]];
            [picker setToRecipients:usersTo];
            [self presentModalViewController:picker animated:YES];
        } else {
            [self.view endEditing:YES];
            UIAlertView *alert=[[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Alert", nil) message:NSLocalizedString(@"No_Email_account_configured", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
            [alert show];
            return;
        }
        
    } else if (indexValue == RATE_US) {
        [self.popoverController dismissPopoverAnimated:YES];
        NSString *templateReviewURLiOS7 = @"https://itunes.apple.com/app/id640516535?mt=8";
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:templateReviewURLiOS7]];
        
    } else if (indexValue == FOLLOW_US) {
        
        [self.popoverController dismissPopoverAnimated:YES];
        if([Utility IsPadDevice]) {
            if(_followUsVC == nil)
                _followUsVC = [[FollowUsVC alloc] initWithNibName:@"FollowUsVC" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_followUsVC];
            self.popoverController.popoverContentSize = CGSizeMake(425.0 , 325.0);
        } else {
            if(_followUsVC == nil)
                _followUsVC = [[FollowUsVC alloc] initWithNibName:@"FollowUsVCPhone" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_followUsVC];
            self.popoverController.popoverContentSize = CGSizeMake(312.0 , 213.0);
        }
        
        self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
        self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
        [_followUsVC.view setClipsToBounds:YES];
        self.popUpVc.delegate=self;
        self.popoverController.delegate =self;
        [self.popoverController presentPopoverFromRect:_infoBtn.frame inView:self.view permittedArrowDirections:UIPopoverArrowDirectionUp animated:YES];
    }
}

- (void) presentOnBoard
{
    NSString *nibName = @"OnBoardVC";
    if([self iPhone6Plus])
        nibName = @"OnBoardVCPhone@2x";
    else if(![Utility IsPadDevice])
        nibName = @"OnBoardVCPhone";
    
    OnBoardVC* ovc = [[OnBoardVC alloc] initWithNibName:nibName bundle:nil];
    if([Utility IsPadDevice])
        ovc.modalPresentationStyle = UIModalPresentationFormSheet;
    ovc.delegate = self;
    [self presentViewController:ovc animated:YES completion:nil];
}

- (void) closingOnBoard
{
    if(![[AppHelper getAppHelper] userDefaultsForKey:@"tipsShown"]) {
        [[AppHelper getAppHelper] toggleHelp:self Enable:YES];
        if([scenesArray count] > 0)
            [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithBool:YES] withKey:@"tipsShown"];
    }
}

- (void)mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error
{
    switch (result)
    {
        case MFMailComposeResultCancelled:
            [self dismissViewControllerAnimated:YES completion:nil];
            break;
        case MFMailComposeResultSaved:
            [self dismissViewControllerAnimated:YES completion:nil];
            break;
        case MFMailComposeResultSent:
            [self dismissViewControllerAnimated:YES completion:nil];
            break;
        case MFMailComposeResultFailed:
            [self dismissViewControllerAnimated:YES completion:nil];
            break;
        default:
            [self dismissViewControllerAnimated:YES completion:nil];
            break;
    }
}

-(NSString*) deviceName
{
    struct utsname systemInfo;
    uname(&systemInfo);
    
    return [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
}

-(void)updateSceneDB
{
    NSCharacterSet * set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];
    NSString* name = [_sceneTitle.text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
    
    if([name length] == 0) {
        [self.view endEditing:YES];
        UIAlertView *errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"Scene_name_empty", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
        [errorAlert show];
        
    } else {
        [self.view endEditing:YES];
        if([name rangeOfCharacterFromSet:set].location != NSNotFound) {
            UIAlertView *errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"Scene_name_special_character", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
            [errorAlert show];
            
        } else {
            if(currentSelectedScene < [scenesArray count]) {
                SceneItem* scene = scenesArray[currentSelectedScene];
                scene.name = _sceneTitle.text;
                scene.createdDate = scene.createdDate;
                scene.sceneFile = scene.sceneFile;
                scene.sceneId = scene.sceneId;
                [cache UpdateScene:scene];
                scenesArray = [cache GetSceneList];
                [self.scenesCollectionView reloadData];
            }
        }
    }
}

- (IBAction)titleChangeAction:(id)sender
{
    [self updateSceneDB];
}

#pragma Scene Preview Delegates

- (void) showSceneEnteredView:(NSIndexPath*)sceneIndex
{
    [self.sceneView setHidden:NO];
    currentSelectedScene = sceneIndex.row;
    if(sceneIndex.row < [scenesArray count]) {
        SceneItem* scenes = scenesArray[sceneIndex.row];
    
        _sceneTitle.text = scenes.name;
        _sceneDate.text = scenes.createdDate;
    
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* documentsDirectory = [paths objectAtIndex:0];
        NSString* originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scenes.sceneFile];
        BOOL fileExit = [[NSFileManager defaultManager] fileExistsAtPath:originalFilePath];
        if(fileExit)
            _scenePreview.image = [UIImage imageWithContentsOfFile:originalFilePath];
        else
            _scenePreview.image = [UIImage imageNamed:@"bgImageforall.png"];
    }
}

#pragma View Animation Delegate

- (IBAction)toolTipAction:(id)sender
{
    [[AppHelper getAppHelper] toggleHelp:self Enable:YES];
}

- (IBAction)openSceneAction:(id)sender
{
    SceneItem *scene = scenesArray[currentSelectedScene];
    [[AppHelper getAppHelper] resetAppHelper];

    if([Utility IsPadDevice]) {
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:@"EditorViewController" bundle:nil SceneItem:scene selectedindex:currentSelectedScene];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];

    } else {
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:([self iPhone6Plus]) ? @"EditorViewControllerPhone@2x" : @"EditorViewControllerPhone" bundle:nil SceneItem:scene selectedindex:currentSelectedScene];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    }
    
    [self removeFromParentViewController];
}

- (IBAction)loginBtnAction:(id)sender
{
    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"]) {
        if ([Utility IsPadDevice]) {
            _loggedInVc = [[LoggedInViewController alloc] initWithNibName:@"LoggedInViewController" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_loggedInVc];
            self.popoverController.popoverContentSize = CGSizeMake(305, 495);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [_loggedInVc.view setClipsToBounds:YES];
            _loggedInVc.delegare=self;
            self.popUpVc.delegate=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];
        } else {
            _loggedInVc = [[LoggedInViewController alloc] initWithNibName:@"LoggedInViewControllerPhone" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_loggedInVc];
            self.popoverController.popoverContentSize = CGSizeMake(230.0, 250.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            _loggedInVc.delegare=self;
            [_loggedInVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionAny
                                                  animated:YES];
        }   
        
    } else {
        if ([Utility IsPadDevice]) {
            loginVc = [[LoginViewController alloc] initWithNibName:@"LoginViewController" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:loginVc];
            self.popoverController.popoverContentSize = CGSizeMake(302 , 240.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [loginVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            loginVc.delegare=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame inView:self.view permittedArrowDirections:UIPopoverArrowDirectionUp animated:YES];
            
        } else {
            loginVc = [[LoginViewController alloc] initWithNibName:@"LoginViewControllerPhone" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:loginVc];
            self.popoverController.popoverContentSize = CGSizeMake(228.00, 208.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [loginVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            loginVc.delegare=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame inView:self.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
        }
    }

}

- (IBAction)feedBtnAction:(id)sender
{
    
    [self.feedCount setHidden:YES];
    NSString* nibName = @"NewsFeedVC";
    CGSize sizeOfView = CGSizeMake(305, 495);
    if(![Utility IsPadDevice]) {
        nibName = @"NewsFeedVCPhone";
        sizeOfView = CGSizeMake(305, 253);
    }
    
    NewsFeedVC *newsVC = [[NewsFeedVC alloc] initWithNibName:nibName bundle:nil];
    newsVC.delegate = self;
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:newsVC];
    self.popoverController.popoverContentSize = sizeOfView;
    self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    [newsVC.view setClipsToBounds:YES];
    self.popUpVc.delegate=self;
    self.popoverController.delegate =self;
    [self.popoverController presentPopoverFromRect:_feedBtn.frame inView:self.view permittedArrowDirections:UIPopoverArrowDirectionUp animated:YES];
}

- (void) showOrHideProgress:(BOOL) value
{
    
}

#pragma mark ScenePropertiesDelegate

-(void)duplicateScene:(int) indexValue
{
    if([scenesArray count] <= 0)
        return;
    
    SceneItem* originalScene = scenesArray[indexValue];
    
    SceneItem* scene = [[SceneItem alloc] init];
    SceneItem *previousScene;
    if([scenesArray count] != 0)
        previousScene = scenesArray[[scenesArray count]-1];
    scene.name = [NSString stringWithFormat:NSLocalizedString(@"My Scene %d", nil), previousScene.sceneId+1];
    scene.createdDate = [dateFormatter stringFromDate:[NSDate date]];
    
    if(![cache AddScene:scene]) {
        [self.view endEditing:YES];
        UIAlertView *errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Duplicate_Scene_Name", nil) message:NSLocalizedString(@"Duplicate_Scene_Name_message", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
        [errorAlert show];
    } else {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* documentsDirectory = [paths objectAtIndex:0];
        NSString *originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, originalScene.sceneFile];
        NSString *newFilePath = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, scene.sceneFile];
        [[NSFileManager defaultManager] copyItemAtPath:originalFilePath toPath:newFilePath error:nil];
        originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, originalScene.sceneFile];
        newFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scene.sceneFile];
        [[NSFileManager defaultManager] copyItemAtPath:originalFilePath toPath:newFilePath error:nil];
        
        scenesArray = [cache GetSceneList];
        [self.scenesCollectionView reloadData];
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:[scenesArray count]-1 inSection:0];
        [self.scenesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredVertically animated:YES];
        [self.scenesCollectionView reloadData];
        
    }
}

-(void)deleteScene:(int)indexValue
{
    [_sceneTitle resignFirstResponder];
    SceneItem* scene = scenesArray[indexValue];
    [[AppHelper getAppHelper] removeFromUserDefaultsWithKey:scene.name];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString *filePath = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, scene.sceneFile];
    self.fileBeginsWith=scene.sceneFile;
    NSArray* cachepath = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [cachepath objectAtIndex:0];
    NSArray *dirFiles = [[NSFileManager defaultManager]     contentsOfDirectoryAtPath:cacheDirectory error:nil];
    NSArray *jpgFiles = [dirFiles filteredArrayUsingPredicate:
                         [NSPredicate predicateWithFormat:@"self BEGINSWITH[cd] %@",self.fileBeginsWith]];
    for (int i=0; i<[jpgFiles count]; i++) {
        NSString *filePath1 = [NSString stringWithFormat:@"%@/%@-%d.png", cacheDirectory,self.fileBeginsWith,i];
        [[NSFileManager defaultManager] removeItemAtPath:filePath1 error:nil];
    }
    [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
    filePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scene.sceneFile];
    [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
    
    [cache DeleteScene:scene];
    [scenesArray removeObjectAtIndex:indexValue];
    [self.scenesCollectionView reloadData];

}

-(void)renameScene:(int)indexValue
{
    UIAlertView *renameScene = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Rename Scene", nil) message:NSLocalizedString(@"Duplicate_Scene_Name_message", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"Ok", nil), nil];

    sceneToBeRenamed=indexValue;
    [renameScene setAlertViewStyle:UIAlertViewStylePlainTextInput];
    [[renameScene textFieldAtIndex:0] setPlaceholder:@"Scene Name"];
    [[renameScene textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
    [renameScene setTag:SCENE_NAME_ALERT];
    [renameScene show];
    [[renameScene textFieldAtIndex:0] becomeFirstResponder];
}

-(void)setSelectedSceneIndex:(int)index
{
    selectedSceneIndex = index;
}

-(void)shareScene:(NSNumber*)value
{
    
    SceneItem * scene = scenesArray[selectedSceneIndex];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString *i3dPath = [NSString stringWithFormat:@"%@/Projects/%@.i3d", documentsDirectory, scene.sceneFile];
    if([[NSFileManager defaultManager] fileExistsAtPath:i3dPath]) {

    
        NSArray *objectsToShare;
        
        NSURL *sceneURL = [NSURL fileURLWithPath:i3dPath];
        objectsToShare = [NSArray arrayWithObjects:NSLocalizedString(@"A Scene created using Iyan 3D in iOS.", nil), sceneURL, nil];
        
        self.docController = [UIDocumentInteractionController interactionControllerWithURL:sceneURL];
        self.docController.UTI = @"com.smackall.i3d";
            if (self.docController != nil) {
                self.docController.delegate = self;
                CGRect sourceRect = [self.scenesCollectionView convertRect:[self.scenesCollectionView cellForItemAtIndexPath:[NSIndexPath indexPathForRow:selectedSceneIndex inSection:0]].frame toView:self.view];
                [self.docController presentOptionsMenuFromRect:sourceRect inView:self.view animated:YES];
            }
    }
}

-(bool) Isi3dExists:(int)indexValue
{
    SceneItem * scene = scenesArray[indexValue];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString *i3dPath = [NSString stringWithFormat:@"%@/Projects/%@.i3d", documentsDirectory, scene.sceneFile];
    if([[NSFileManager defaultManager] fileExistsAtPath:i3dPath])
        return true;

    return false;
}

- (void)restClient:(DBRestClient*)client uploadProgress:(CGFloat)progress
{
    NSLog(@" Upload Progress %f ", (float)progress);
}

- (void)restClient:(DBRestClient*)client uploadedFile:(NSString*)destPath from:(NSString*)srcPath
          metadata:(DBMetadata*)metadata
{
    [self performSelectorOnMainThread:@selector(hideLoading) withObject:nil waitUntilDone:YES];
    [self.view setUserInteractionEnabled:YES];
    [[[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Success", nil) message:NSLocalizedString(@"file_successfully_saved_dropbox", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"Ok", nil) otherButtonTitles:nil] show];
}

- (void)restClient:(DBRestClient*)client uploadFileFailedWithError:(NSError*)error
{
    [self performSelectorOnMainThread:@selector(hideLoading) withObject:nil waitUntilDone:YES];
    [self.view setUserInteractionEnabled:YES];
    [[[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Failure", nil) message:NSLocalizedString(@"Error_uploading_file", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil] show];
}

- (void)restClient:(DBRestClient*)client loadAccountInfoFailedWithError:(NSError*)error
{
    NSLog(@" Error acc %@ ", error.localizedDescription);
}

- (void)restClient:(DBRestClient*)client loadThumbnailFailedWithError:(NSError*)error
{
    NSLog(@" Error thumb %@ ", error.localizedDescription);
}

#pragma AlertView
- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if (alertView.tag==SCENE_NAME_ALERT) {
        
        NSCharacterSet* set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];
        
        if (buttonIndex == CANCEL) {
        } else if (buttonIndex == OK) {
            NSString* name = [[alertView textFieldAtIndex:0].text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
            if ([name length] == 0) {
                [self.view endEditing:YES];
                UIAlertView *errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"Scene_name_empty", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];

                [errorAlert show];
                
            } else {
                if ([name rangeOfCharacterFromSet:set].location != NSNotFound) {
                    UIAlertView *errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"Scene_name_special_character", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
                    [errorAlert show];
                    
                } else {
                    SceneItem* scene = scenesArray[sceneToBeRenamed];
                    scene.name = name;
                    scene.createdDate = scene.createdDate;
                    scene.sceneFile = scene.sceneFile;
                    scene.sceneId = scene.sceneId;
                    [cache UpdateScene:scene];
                    scenesArray = [cache GetSceneList];
                    [self.scenesCollectionView reloadData];
                    
                }
            }
        }
        [alertView resignFirstResponder];
        
    } else if (alertView.tag == LOGOUT_ALERT) {
        if (buttonIndex == CANCEL) {
            [[DBSession sharedSession] unlinkAll];
            [self shareScene:[NSNumber numberWithInt:selectedSceneIndex]];
        } else if (buttonIndex == OK) {
            [self uploadSceneToDropBox];
        }
        
    } else if (alertView.tag == RESTORE_PURCHASH_ALERT){
        
        if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"]){
            [self infoBtnDelegateAction:SETTINGS];
        }
        else
            [self loginBtnAction:nil];
        
    }
}

- (void) uploadSceneToDropBox
{
    SceneItem * scene = scenesArray[selectedSceneIndex];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString *i3dPath = [NSString stringWithFormat:@"%@/Projects/%@.i3d", documentsDirectory, scene.sceneFile];
    if([[NSFileManager defaultManager] fileExistsAtPath:i3dPath]) {
        [self.view setUserInteractionEnabled:NO];
        [self performSelectorOnMainThread:@selector(showLoading) withObject:nil waitUntilDone:YES];
        NSString *destDir = @"/";
        [restClient uploadFile:[i3dPath lastPathComponent] toPath:destDir withParentRev:nil fromPath:i3dPath];
    }
}

#pragma Settings Delegates

-(void)frameCountDisplayMode
{
    
}

-(void)cameraPreviewSize
{
   
}

-(void)cameraPreviewPosition
{

}

-(void)toolbarPosition:(int)selctedIndex
{
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:selctedIndex] withKey:@"toolbarPosition"];

}

-(void)multiSelectUpdate:(BOOL)value
{
    [[AppHelper getAppHelper] saveBoolUserDefaults:value withKey:@"multiSelectOption"];
}

-(BOOL)iPhone6Plus
{
    if (([UIScreen mainScreen].scale > 2.0)) return YES;
    return NO;
}

#pragma LogIn Delegates
-(void)googleSigninDelegate
{
    isLoggedin= true;
}

-(void)dismisspopover
{
    [self.popoverController dismissPopoverAnimated:YES];

}

-(void)dismissView
{
    [_loggedInVc dismissViewControllerAnimated:YES completion:nil];
    [self.popoverController dismissPopoverAnimated:YES];

}

- (void) showPreview:(NSString*) outputPath
{
    [self performSelectorOnMainThread:@selector(showPreviewInMainThread:) withObject:outputPath waitUntilDone:NO];
}

- (void) showPreviewInMainThread:(NSString*) outputPath
{
    [self.centerLoading setHidden:YES];
    int mediaType = [[outputPath pathExtension] isEqualToString:@"png"] ? 0 : 1;
    
    if([Utility IsPadDevice]) {
        MediaPreviewVC *medPreview = [[MediaPreviewVC alloc] initWithNibName:@"MediaPreviewVC" bundle:nil mediaType:mediaType medPath:outputPath];
        [self dismissView];
        medPreview.modalPresentationStyle = UIModalPresentationFullScreen;
        [self presentViewController:medPreview animated:YES completion:nil];
    } else {
        MediaPreviewVC *medPreview = [[MediaPreviewVC alloc] initWithNibName:[[AppHelper getAppHelper] iPhone6Plus] ? @"MediaPreviewVCPhone@2x" : @"MediaPreviewVCPhone" bundle:nil mediaType:mediaType medPath:outputPath];
        [self dismissView];
        medPreview.modalPresentationStyle = UIModalPresentationFullScreen;
        [self presentViewController:medPreview animated:YES completion:nil];
    }
}

- (void) performActionForSelectedFeed:(FeedItem*) feed
{
    [self.popoverController dismissPopoverAnimated:YES];
    
    if(feed.type == ALERT_TYPE) {
        UIAlertView * infoAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:feed.message delegate:nil cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [infoAlert show];
    } else if(feed.type == IMAGE_TYPE) {
        [self.centerLoading setHidden:NO];
        [self.centerLoading startAnimating];
        NSString* tempDir = NSTemporaryDirectory();
        NSString* imagePath = [NSString stringWithFormat:@"%@/feedImage.png", tempDir];
        if(![[AppHelper getAppHelper] checkInternetConnected])  {
            [self showInternetError:SLOW_INTERNET];
        } else {
            DownloadTask *t = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(showPreviewInMainThread:) returnObject:imagePath outputFilePath:imagePath andURL:feed.url];
            t.taskType = DOWNLOAD_AND_WRITE_IMAGE;
            t.queuePriority = NSOperationQueuePriorityHigh;
            NSOperationQueue *queue = [[NSOperationQueue alloc] init];
            [queue addOperation:t];
        }
    } else if(feed.type == URL_TYPE) {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:feed.url]];
    }
}

- (void)showInternetError:(int)connectionError
{
    if (connectionError != SLOW_INTERNET) {
        [self.view endEditing:YES];
        UIAlertView *internetError = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Connection Error", nil) message:NSLocalizedString(@"unable_connect_server", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];

        [internetError performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}

#pragma Dealloc Delegate

- (void)dealloc
{
    _followUsVC = nil;
    restClient.delegate = nil;
    restClient = nil;
    self.fileBeginsWith = nil;
    cache = nil;
    [scenesArray removeAllObjects];
    scenesArray = nil;
    dateFormatter = nil;
    settingsVc = nil;
    loginVc = nil;
    self.popUpVc = nil;
    self.popoverController = nil;
    self.loggedInVc = nil;
}

@end

