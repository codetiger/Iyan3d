//
//  SceneSelectionViewControllerPad.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 03/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "SceneSelectionViewControllerPad.h"
#import "AssetSelectionViewControllerPad.h"
#import "AppDelegate.h"
#import "Constants.h"
#import "AutoRigViewController.h"
#import <sys/utsname.h> 

#define COMESFROMSCENESELECTION 9
#define HELP_FROM_SCENESELECTION 4
#define SPACING_RATIO 1.5f

enum SceneSelectionViewConstants {
    IMG_VIEW_TAG = 1,
    NAME_LABEL_TAG = 2,
    DATE_LABEL_TAG = 3,
    ADD_BUTTON_TAG = 1,
    CLONE_BUTTON_TAG = 2,
    DELETE_BUTTON_TAG = 3,
    CANCEL_BUTTON_INDEX = 0,
    OK_BUTTON_INDEX = 1,
    feedBackAlertViewTag = 9
};

@implementation SceneSelectionViewControllerPad

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil SceneNo:(int)sceneNum isAppFirstTime:(BOOL)FirstTime
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cache = [CacheSystem cacheSystem];
        carouselIndexitem = sceneNum;
        isAppFirstTime = FirstTime;
        self.sceneItemArray = [cache GetSceneList];
        dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy/MM/dd HH:mm:ss"];
        // Custom initialization
        isRigbuttonPressed = false;
        _priceFormatter = [[NSNumberFormatter alloc] init];
        [_priceFormatter setFormatterBehavior:NSNumberFormatterBehavior10_4];
        ispurchaseCalled = false;
        [_priceFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];

    }
    return self;
}
- (void) viewDidAppear:(BOOL)animated
{
    isViewAppeared = true;
    [super viewDidAppear:animated];
    self.screenName = @"SceneSelectionView";
    [self updateUIStates];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(showPriceForObjImport) name:@"ProductPriceSet" object:nil];

    if (isAppFirstTime) {
        [self helpButtonAction:nil];
        isAppFirstTime = false;
    }
    [_carousel scrollToItemAtIndex:carouselIndexitem duration:0.0f];
}
- (void) viewDidDisappear:(BOOL)animated{
    isViewAppeared = false;
}
- (void) viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"ProductPriceSet" object:nil];
}
- (void) viewDidLoad
{
    isViewAppeared = false;
    [super viewDidLoad];
    self.screenName = @"SceneSelectionView";
    [self.activityViewForOBJImporter setHidden:YES];
    [self showPriceForObjImport];
    [[AppHelper getAppHelper] moveFilesFromInboxDirectory:cache];
    deviceNames = [[AppHelper getAppHelper] parseJsonFileWithName:@"deviceCodes"];
    self.addBTiconView.layer.cornerRadius = 7.0;
    self.feedbackBTiconView.layer.cornerRadius = 7.0;
    self.cloneBTiconView.layer.cornerRadius = 7.0;
    self.deleteBTiconView.layer.cornerRadius = 7.0;
    self.rateBTiconView.layer.cornerRadius = 7.0;
    if (![[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]){
        [self.activityViewForOBJImporter setHidden:YES];
    }else{
        if([[AppHelper getAppHelper] checkInternetConnected]) {
            [self.activityViewForOBJImporter setHidden:NO];
            [self.activityViewForOBJImporter startAnimating];
            
            [[AppHelper getAppHelper] initHelper];
            [[AppHelper getAppHelper] setAssetsDetails:SCENE_SELECTION];
        } else
            [self showPriceForObjImport];
    }
    
    [[UITextField appearanceWhenContainedIn:[UISearchBar class], nil] setLeftViewMode:UITextFieldViewModeAlways];
    self.carousel.type = iCarouselTypeCoverFlow2;
    if([self.sceneItemArray count] == 0){
        [self.cloneSceneButton setEnabled:NO];
        [self.deleteSceneButton setEnabled:NO];
    }
    [self.loadingView setHidden:YES];
}
-(void) showPriceForObjImport
{
    if (![[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]){
        self.amountLabel.text = @"Upgrade To Premium";
    } else{
        self.amountLabel.text = @"";
    }
    [[AppHelper getAppHelper] resetAppHelper];
}
- (void) dealloc
{
    self.carousel.dataSource = nil;
    self.carousel.delegate = nil;
    
    self.sceneItemArray = nil;
    self.fileBeginsWith = nil;
    self.carousel = nil;
    
    cache = nil;
    dateFormatter = nil;
    _loadingView = nil;
    restoreIdArr = nil;
    _priceFormatter = nil;
    self.priceLocale = nil;
    
    _addBTView = nil;
    _addBTiconView = nil;
    _cloneBTView = nil;
    _cloneBTiconView = nil;
    _deleteBTView = nil;
    _deleteBTiconView = nil;
    _feedbackBTView = nil;
    _feedbackBTiconView = nil;
    _moviePlayer = nil;

}
- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}
- (NSUInteger) numberOfItemsInCarousel:(iCarousel *)carousel
{
    return [self.sceneItemArray count];
}
- (UIView *) carousel:(iCarousel *)carousel viewForItemAtIndex:(NSUInteger)index reusingView:(UIView *)view
{
    SceneItem* scene = [self.sceneItemArray objectAtIndex:index];
    UILabel *nameLabel = nil;
    UILabel *dateLabel = nil;
    UIImageView *imgView = nil;
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* srcFilePath = [NSString stringWithFormat:@"%@/Projects",documentsDirectory];
    NSString *imageFilePath = [NSString stringWithFormat:@"%@/%@.png", srcFilePath, scene.sceneFile];
    UIImage* image = [UIImage imageWithContentsOfFile:imageFilePath];
    if (!image) {
        image= [UIImage imageNamed:@"bgImageforall.png"];
    }
    if (view == nil) {
        if([Utility IsPadDevice]){
            view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, 330 * [Utility GetDisplaceRatio], 345 * [Utility GetDisplaceRatio])];
            view.backgroundColor = [UIColor clearColor];
            imgView = [[UIImageView alloc] initWithImage:image];
            [imgView setFrame:CGRectMake(-5 * [Utility GetDisplaceRatio], 0 * [Utility GetDisplaceRatio], 380 * [Utility GetDisplaceRatio], 280 * [Utility GetDisplaceRatio])];
            nameLabel = [[UILabel alloc] initWithFrame:CGRectMake(35, 300, 300 * [Utility GetDisplaceRatio], 21)];
            dateLabel = [[UILabel alloc] initWithFrame:CGRectMake(55, 320 * [Utility GetDisplaceRatio], 270 * [Utility GetDisplaceRatio], 21)];
            nameLabel.font = [UIFont boldSystemFontOfSize:18];
            dateLabel.font = [UIFont systemFontOfSize:10];
        }
        else{
            view = [[UIView alloc] initWithFrame:CGRectMake(0, 0, (0.50 * SCREENWIDTH) * [Utility GetDisplaceRatio], (0.60 *SCREENHEIGHT) * [Utility GetDisplaceRatio])];
            view.backgroundColor = [UIColor clearColor];
            imgView = [[UIImageView alloc] initWithImage:image];
            if(SCREENWIDTH <= 480)
                [imgView setFrame:CGRectMake(0, 10,  0.33 * SCREENWIDTH, 0.33 * SCREENHEIGHT)];
            else
                [imgView setFrame:CGRectMake(0, 10,  0.33 * SCREENWIDTH, 0.40 * SCREENHEIGHT)];
            nameLabel = [[UILabel alloc] initWithFrame:CGRectMake(0.001 * SCREENWIDTH,(imgView.frame.origin.y - (0.07 * SCREENHEIGHT)), imgView.frame.size.width, 0.080 * SCREENHEIGHT)];
            dateLabel = [[UILabel alloc] initWithFrame:CGRectMake(0.02 * SCREENWIDTH,(imgView.frame.size.height - (-0.02 * SCREENHEIGHT)), 0.29 * SCREENWIDTH, 0.070 * SCREENHEIGHT)];
            nameLabel.font = [UIFont boldSystemFontOfSize:(23/(SCREENWIDTH/SCREENHEIGHT))];
            dateLabel.font = [UIFont systemFontOfSize:8];
        }
        imgView.tag = IMG_VIEW_TAG;
        [imgView setBackgroundColor:[UIColor lightGrayColor]];
        [imgView.layer setCornerRadius:30.0f * [Utility GetDisplaceRatio]];
        [imgView.layer setMasksToBounds:YES];
        [imgView.layer setBorderColor:[UIColor whiteColor].CGColor];
        [imgView.layer setBorderWidth:2.0f * [Utility GetDisplaceRatio]];
        
        [view addSubview:imgView];
        
        nameLabel.backgroundColor = [UIColor clearColor];
        nameLabel.textColor = [UIColor whiteColor];
        nameLabel.textAlignment = NSTextAlignmentCenter;
        nameLabel.tag = NAME_LABEL_TAG;
        [view addSubview:nameLabel];
        
        dateLabel.backgroundColor = [UIColor clearColor];
        dateLabel.textColor = [UIColor whiteColor];
        dateLabel.textAlignment = NSTextAlignmentCenter;
        dateLabel.tag = DATE_LABEL_TAG;
        [view addSubview:dateLabel];
    } else {
        imgView = (UIImageView*)[view viewWithTag:IMG_VIEW_TAG];
        nameLabel = (UILabel *)[view viewWithTag:NAME_LABEL_TAG];
        dateLabel = (UILabel *)[view viewWithTag:DATE_LABEL_TAG];
    }
    
    nameLabel.text = scene.name;
    dateLabel.text = scene.createdDate;
    [imgView setImage:image];
    
    return view;
}

- (CGFloat) carousel:(iCarousel *)_carousel valueForOption:(iCarouselOption)option withDefault:(CGFloat)value
{
    switch (option) {
        case iCarouselOptionSpacing: {
            return value * SPACING_RATIO;
        }
            //        case iCarouselOptionWrap: {
            //            return YES;
            //        }
        case iCarouselOptionFadeMin: {
            return -0.1f;
        }
        case iCarouselOptionFadeMax: {
            return 0.1f;
        }
        case iCarouselOptionFadeRange: {
            return 3;
        }
        default: {
            return value;
        }
    }
}
- (void) carousel:(iCarousel *)carousel didSelectItemAtIndex:(NSInteger)index
{
    if(!isViewAppeared)
        return;
    SceneItem *scene = (self.sceneItemArray)[index];
    [[AppHelper getAppHelper] resetAppHelper];
    if([Utility IsPadDevice]) {
        AnimationEditorViewControllerPad* animationEditor = [[AnimationEditorViewControllerPad alloc] initWithNibName:@"AnimationView" bundle:nil SceneItem:scene carouselIndex:(int)index];
        animationEditor.priceLocale = self.priceLocale;
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    } else {
        AnimationEditorViewControllerPad* animationEditor = [[AnimationEditorViewControllerPad alloc] initWithNibName:@"AnimationEditorViewControllerPhone" bundle:nil SceneItem:scene carouselIndex:(int)index];
        animationEditor.priceLocale = self.priceLocale;
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    }
    [self removeFromParentViewController];
}

-(void)loadingViewStatus:(BOOL)status
{
//    if(status){
//        [self.loadingView setHidden:NO];
//        [self.loadingView startAnimating];
//    } else {
//        [self.loadingView stopAnimating];
//        [self.loadingView setHidden:YES];
//    }
}

-(void)statusForOBJImport:(NSNumber *)object
{
    if([object boolValue]){
        [self.view setUserInteractionEnabled:NO];
        [self.rigViewButton setHidden:YES];
        [self.activityViewForOBJImporter setHidden:NO];
        [self.activityViewForOBJImporter startAnimating];
    }
    else{
        [self.view setUserInteractionEnabled:YES];
        [self.rigViewButton setHidden:NO];
        [self.activityViewForOBJImporter setHidden:YES];
        [self.activityViewForOBJImporter stopAnimating];
    }
}
-(void)addRestoreId:(NSString*)productIdentifier
{
    [restoreIdArr addObject:productIdentifier];
}
-(IBAction)helpButtonAction:(id)sender
{
    if([Utility IsPadDevice]) {
        HelpViewController* morehelpView = [[HelpViewController alloc] initWithNibName:@"HelpViewController" bundle:nil CalledFrom:HELP_FROM_SCENESELECTION];
        morehelpView.delegate = self;
        morehelpView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:morehelpView animated:YES completion:nil];
        CGRect rect = CGRectInset(morehelpView.view.frame, -80, -0);
        morehelpView.view.superview.backgroundColor = [UIColor clearColor];
        morehelpView.view.frame = rect;
    }
    else{
        HelpViewController* morehelpView = [[HelpViewController alloc] initWithNibName:@"HelpViewControllerPhone" bundle:nil CalledFrom:HELP_FROM_SCENESELECTION];
        morehelpView.delegate = self;
        morehelpView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:morehelpView animated:YES completion:nil];
    }
}
- (BOOL)searchBarShouldBeginEditing:(UISearchBar *)searchBar
{
    UITextField *searchBarTextField = nil;
    for (UIView *subView in self.searchBar.subviews)
    {
        for (UIView *sendSubView in subView.subviews)
        {
            if ([sendSubView isKindOfClass:[UITextField class]])
            {
                searchBarTextField = (UITextField *)sendSubView;
                break;
            }
        }
    }
    searchBarTextField.enablesReturnKeyAutomatically = NO;
    return YES;
}
- (void)searchBarSearchButtonClicked:(UISearchBar *)searchBar
{
    [self.view endEditing:YES];
}
- (void)searchBar:(UISearchBar *)searchBar textDidChange:(NSString *)searchText
{
    self.sceneItemArray = [cache GetSceneList:searchText];
    [self.carousel reloadData];
}
- (IBAction) addSceneButtonAction:(id)sender
{
    self.addBTiconView.backgroundColor = [UIColor blackColor];
    [self.searchBar resignFirstResponder];
    [self.view endEditing:YES];
    UIAlertView *addSceneAlert = [[UIAlertView alloc]initWithTitle:@"New Scene" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
    [addSceneAlert setAlertViewStyle:UIAlertViewStylePlainTextInput];
    [[addSceneAlert textFieldAtIndex:0] setPlaceholder:@"Scene Name"];
    [[addSceneAlert textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
    [addSceneAlert setTag:ADD_BUTTON_TAG];
    [addSceneAlert show];
    [[addSceneAlert textFieldAtIndex:0] becomeFirstResponder];
    
}
- (IBAction) deleteSceneButtonAction:(id)sender
{
    self.deleteBTiconView.backgroundColor = [UIColor blackColor];
    [self.searchBar resignFirstResponder];
    self.searchBar.text = @"";
    if (self.carousel.numberOfItems > 0) {
        [self.view endEditing:YES];
        UIAlertView *deleteSceneAlert = [[UIAlertView alloc]initWithTitle:@"Delete Scene" message:@"Do you want to delete the scene?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Yes", nil];
        [deleteSceneAlert setTag:DELETE_BUTTON_TAG];
        [deleteSceneAlert show];
    }
}

- (void) updateUIStates
{
    BOOL sceneExists = ([self.sceneItemArray count] > 0);
    
    [self.deleteBTView setHidden:!sceneExists];
    [self.deleteBTiconView setHidden:!sceneExists];
    [self.cloneBTView setHidden:!sceneExists];
    [self.cloneBTiconView setHidden:!sceneExists];
}

- (IBAction) cloneSceneButtonAction:(id)sender
{
    self.cloneBTiconView.backgroundColor = [UIColor blackColor];
    [self.searchBar resignFirstResponder];
    self.searchBar.text = @"";
    [self.view endEditing:YES];
    UIAlertView *cloneSceneAlert = [[UIAlertView alloc]initWithTitle:@"Clone Scene" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
    [cloneSceneAlert setAlertViewStyle:UIAlertViewStylePlainTextInput];
    [[cloneSceneAlert textFieldAtIndex:0] setPlaceholder:@"Scene Name"];
    [[cloneSceneAlert textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
    [cloneSceneAlert setTag:CLONE_BUTTON_TAG];
    [cloneSceneAlert show];
    [[cloneSceneAlert textFieldAtIndex:0] becomeFirstResponder];
}
- (IBAction)rateMeButtonAction:(id)sender
{
    NSString *templateReviewURLiOS7 = @"https://itunes.apple.com/app/id640516535?mt=8";
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:templateReviewURLiOS7]];
}
- (IBAction)feedbackButtonAction:(id)sender
{
    NSString *currentDeviceName;
    
    if(deviceNames != nil && [deviceNames objectForKey:[self deviceName]])
        currentDeviceName = [deviceNames objectForKey:[self deviceName]];
    else
        currentDeviceName = @"Unknown Device";
    
    self.searchBar.text = @"";
    self.feedbackBTiconView.backgroundColor = [UIColor blackColor];
    if ([MFMailComposeViewController canSendMail]) {
        MFMailComposeViewController *picker = [[MFMailComposeViewController alloc] init];
        picker.mailComposeDelegate = self;
        NSArray *usersTo = [NSArray arrayWithObject: @"iyan3d@smackall.com"];
        [picker setSubject:[NSString stringWithFormat:@"Feedback on Iyan 3d app (%@  , iOS Version: %.01f)",[deviceNames objectForKey:[self deviceName]],iOSVersion]];
        [picker setToRecipients:usersTo];
        [self presentModalViewController:picker animated:YES];
    }else {
        [self.view endEditing:YES];
        UIAlertView *alert=[[UIAlertView alloc] initWithTitle:@"Alert" message:@"Email account not configured." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert setTag:feedBackAlertViewTag];
        [alert show];
        return;
    }
}

-(NSString*) deviceName
{
    struct utsname systemInfo;
    uname(&systemInfo);
    
    return [NSString stringWithCString:systemInfo.machine
                              encoding:NSUTF8StringEncoding];
}

- (IBAction)rigViewButtonAction:(id)sender
{
    if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"])
        [self showUpgradeView];
    else
        [self premiumUnlocked];
}
-(void)showUpgradeView
{
    if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]){
        PremiumUpgardeVCViewController* upgradeView;
        if([Utility IsPadDevice]) {
            upgradeView = [[PremiumUpgardeVCViewController alloc] initWithNibName:@"PremiumUpgardeVCViewController" bundle:nil];
            upgradeView.delegate = self;
            upgradeView.modalPresentationStyle = UIModalPresentationFormSheet;
            [self presentViewController:upgradeView animated:YES completion:nil];
            upgradeView.view.superview.backgroundColor = [UIColor clearColor];
            upgradeView.view.layer.borderWidth = 2.0f;
            upgradeView.view.layer.borderColor = [UIColor grayColor].CGColor;
        }else{
            upgradeView = [[PremiumUpgardeVCViewController alloc] initWithNibName:@"PremiumUpgradeViewControllerPhone" bundle:nil];
            upgradeView.delegate = self;
            upgradeView.modalPresentationStyle = UIModalPresentationFormSheet;
            [self presentViewController:upgradeView animated:YES completion:nil];
        }
    }
    else
        [self premiumUnlocked];
}
-(void)premiumUnlocked
{
    [[UIApplication sharedApplication] endIgnoringInteractionEvents];
    [self performSelectorInBackground:@selector(statusForOBJImport:) withObject:[NSNumber numberWithBool:NO]];
    if([Utility IsPadDevice]){
        //[self dismissViewControllerAnimated:TRUE completion:Nil];
        AutoRigViewController* autoRig = [[AutoRigViewController alloc] initWithNibName:@"AutoRigViewController" bundle:nil];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:autoRig];
    }else{
        //[self dismissViewControllerAnimated:TRUE completion:Nil];
        AutoRigViewController* autoRig = [[AutoRigViewController alloc] initWithNibName:@"AutoRigViewControllerPhone" bundle:nil ];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:autoRig];
    }
}
-(void)premiumUnlockedCancelled
{
    
}
-(void)changeAllButtonBG
{
    [self.addBTiconView setBackgroundColor:[UIColor colorWithRed:55.0f/255.0f green:55.0f/255.0f blue:58.0f/255.0f alpha:1]];
    [self.cloneBTiconView setBackgroundColor:[UIColor colorWithRed:55.0f/255.0f green:55.0f/255.0f blue:58.0f/255.0f alpha:1]];
    [self.feedbackBTiconView setBackgroundColor:[UIColor colorWithRed:55.0f/255.0f green:55.0f/255.0f blue:58.0f/255.0f alpha:1]];
    [self.deleteBTiconView setBackgroundColor:[UIColor colorWithRed:55.0f/255.0f green:55.0f/255.0f blue:58.0f/255.0f alpha:1]];
}
- (void)mailComposeController:(MFMailComposeViewController*)controller
          didFinishWithResult:(MFMailComposeResult)result
                        error:(NSError*)error;
{
    [self changeAllButtonBG];
    [self dismissModalViewControllerAnimated:YES];
}
- (void) alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    NSCharacterSet * set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];
    
    switch(alertView.tag)
    {
        case feedBackAlertViewTag:
        {
            [self changeAllButtonBG];
        }
            break;
        case ADD_BUTTON_TAG:
        {
            [self changeAllButtonBG];
            if (buttonIndex == CANCEL_BUTTON_INDEX) {
            } else if (buttonIndex == OK_BUTTON_INDEX) {
                NSString* name = [[alertView textFieldAtIndex:0].text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
                if([name length] <= 0) {
                    [self.view endEditing:YES];
                    UIAlertView *errorAlert = [[UIAlertView alloc]initWithTitle:@"Warning" message:@"Scene name cannot be empty." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                    [errorAlert show];
                } else {
                    [self.view endEditing:YES];
                    if([name rangeOfCharacterFromSet:set].location != NSNotFound){
                        UIAlertView *errorAlert = [[UIAlertView alloc]initWithTitle:@"Warning" message:@"Scene Name cannot contain any special characters." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                        [errorAlert show];
                    }
                    else{
                        SceneItem* scene = [[SceneItem alloc] init];
                        scene.name = [alertView textFieldAtIndex:0].text;
                        scene.createdDate = [dateFormatter stringFromDate:[NSDate date]];
                        if(![cache AddScene:scene]) {
                            [self.view endEditing:YES];
                            UIAlertView *errorAlert = [[UIAlertView alloc]initWithTitle:@"Duplicate Scene Name" message:@"Another Scene with the same name already exists. Please provide a different name." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                            [errorAlert show];
                        } else {
                            NSInteger index = self.sceneItemArray.count;
                            [self.sceneItemArray insertObject:scene atIndex:index];
                            [self.carousel insertItemAtIndex:index animated:YES];
                            [self.carousel scrollToItemAtIndex:index animated:YES];
                            [self.cloneSceneButton setEnabled:YES];
                            [self.deleteSceneButton setEnabled:YES];
                        }
                    }
                }
                // TODO: Add code to copy default scene file and screenshot image from bundle to file system
                [self updateUIStates];
            }
        }
            break;
        case CLONE_BUTTON_TAG:
        {
            [self changeAllButtonBG];
            if (buttonIndex == CANCEL_BUTTON_INDEX) {
            } else if (buttonIndex == OK_BUTTON_INDEX) {
                NSString* name = [[alertView textFieldAtIndex:0].text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
                if([name length] == 0) {
                    [self.view endEditing:YES];
                    UIAlertView *errorAlert = [[UIAlertView alloc]initWithTitle:@"Warning" message:@"Scene name cannot be empty." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                    [errorAlert show];
                } else {
                    [self.view endEditing:YES];
                    if([name rangeOfCharacterFromSet:set].location != NSNotFound){
                        UIAlertView *errorAlert = [[UIAlertView alloc]initWithTitle:@"Warning" message:@"Scene Name cannot contain any special characters." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                        [errorAlert show];
                    }
                    else{
                        if([self.sceneItemArray count] > self.carousel.currentItemIndex) {
                        SceneItem* scene = self.sceneItemArray[self.carousel.currentItemIndex];
                        
                        // TODO: Add code to copy scene file and screenshot image in file system
                        SceneItem* newScene = [[SceneItem alloc] init];
                        newScene.name = [alertView textFieldAtIndex:0].text;
                        newScene.createdDate = [dateFormatter stringFromDate:[NSDate date]];
                        if(![cache AddScene:newScene]) {
                            [self.view endEditing:YES];
                            UIAlertView *errorAlert = [[UIAlertView alloc]initWithTitle:@"Duplicate Scene Name" message:@"Another Scene with the same name already exists. Please provide a different name." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                            [errorAlert show];
                        } else {
                            NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
                            NSString* documentsDirectory = [paths objectAtIndex:0];
                            NSString *originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, scene.sceneFile];
                            NSString *newFilePath = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, newScene.sceneFile];
                            [[NSFileManager defaultManager] copyItemAtPath:originalFilePath toPath:newFilePath error:nil];
                            originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scene.sceneFile];
                            newFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, newScene.sceneFile];
                            [[NSFileManager defaultManager] copyItemAtPath:originalFilePath toPath:newFilePath error:nil];
                            
                            NSInteger index = self.sceneItemArray.count;
                            [self.sceneItemArray insertObject:newScene atIndex:index];
                            [self.carousel insertItemAtIndex:index animated:YES];
                            [self.carousel scrollToItemAtIndex:index animated:YES];
                        }
                    }
                }
                }
            }
        }
            break;
        case DELETE_BUTTON_TAG:
        {
            [self changeAllButtonBG];
            if (buttonIndex == CANCEL_BUTTON_INDEX) {
            } else if (buttonIndex == OK_BUTTON_INDEX) {
                NSInteger index = self.carousel.currentItemIndex;
                if([self.sceneItemArray count] > index) {
                SceneItem* scene = self.sceneItemArray[index];
                
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
                [self.sceneItemArray removeObjectAtIndex:index];
                [self.carousel removeItemAtIndex:index animated:YES];
                [self updateUIStates];
                if([self.sceneItemArray count] == 0){
                    [self.cloneSceneButton setEnabled:NO];
                    [self.deleteSceneButton setEnabled:NO];
                }
                // TODO: Add code to delete scene file and screenshot image     in file system
            }
            }
        }
            break;
    }
}

@end
