//
//  SceneSelectionControllerNew.m
//  Iyan3D
//
//  Created by harishankarn on 16/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "AppDelegate.h"
#import "SceneSelectionControllerNew.h"
#import "SceneSelectionEnteredView.h"
#import "SceneItem.h"
#import <sys/utsname.h>


#define SCENE_NAME_ALERT 0
#define CANCEL 0
#define OK 1

#define SETTINGS 3
#define CONTACT_US 4

#define CAMERA_PREVIEW_SMALL 0

#define FRAME_COUNT 0
#define FRAME_DURATION 1


@implementation SceneSelectionControllerNew

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cache = [CacheSystem cacheSystem];
        scenesArray = [cache GetSceneList];
        dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy/MM/dd HH:mm:ss"];
        currentSelectedScene = -1;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    CGRect screenRect = [[UIScreen mainScreen] bounds];
    screenHeight = screenRect.size.height;
    deviceNames = [[AppHelper getAppHelper] parseJsonFileWithName:@"deviceCodes"];
    if([Utility IsPadDevice]){
        [self.scenesCollectionView registerNib:[UINib nibWithNibName:@"SceneSelectionFrameCell" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    else{
        [self.scenesCollectionView registerNib:[UINib nibWithNibName:@"SceneSelectionFrameCellPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    [self.sceneView setHidden:YES];
    
    if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"] && ![[AppHelper getAppHelper] userDefaultsBoolForKey:@"hasRestored"]) {
        UIAlertView* infoAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"You have already upgraded to Premium. Please Signin and use 'Restore Purchase' in 'Settings' menu to verify your purchase." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [infoAlert show];
    }
    
    /*
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillBeHidden:) name:UIKeyboardWillHideNotification object:nil];   
     */
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

#pragma Button Actions

- (IBAction)addSceneButtonAction:(id)sender {
    [self addNewScene];
}

- (IBAction)scenePreviewClosebtn:(id)sender {
    //[self popZoomOut];
}

- (IBAction)feedBackButtonAction:(id)sender {
    
    NSString *currentDeviceName;
    
    if(deviceNames != nil && [deviceNames objectForKey:[[AppHelper getAppHelper] deviceName]])
        currentDeviceName = [deviceNames objectForKey:[[AppHelper getAppHelper] deviceName]];
    else
        currentDeviceName = @"Unknown Device";
    
    if ([MFMailComposeViewController canSendMail]) {
        MFMailComposeViewController *picker = [[MFMailComposeViewController alloc] init];
        picker.mailComposeDelegate = self;
        NSArray *usersTo = [NSArray arrayWithObject: @"iyan3d@smackall.com"];
        [picker setSubject:[NSString stringWithFormat:@"Feedback on Iyan 3d app (%@  , iOS Version: %.01f)",[deviceNames objectForKey:[[AppHelper getAppHelper] deviceName]],iOSVersion]];
        [picker setToRecipients:usersTo];
        [self presentModalViewController:picker animated:YES];
    }else {
        [self.view endEditing:YES];
        UIAlertView *alert=[[UIAlertView alloc] initWithTitle:@"Alert" message:@"Email account not configured." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
        return;
    }
}

#pragma CollectionView Delegates

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section{
    return [scenesArray count]+1;
}

- (SceneSelectionFrameCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
   SceneSelectionFrameCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
    cell.delegate=self;
    if(indexPath.row < [scenesArray count]){
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
        cell.name.text = @"";
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
    
    if(indexPath.row == [scenesArray count])
        [self addNewScene];
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
    scene.name = [NSString stringWithFormat:@"MyScene %d", previousScene.sceneId+1];
    scene.createdDate = [dateFormatter stringFromDate:[NSDate date]];
    
    if([cache AddScene:scene]) {
        scenesArray = [cache GetSceneList];
        [self.scenesCollectionView reloadData];
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:[scenesArray count]-1 inSection:0];
        [self.scenesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredVertically animated:YES];
    }
}

-(void)openSCene: (int)selectedScene{
    SceneItem *scene = scenesArray[selectedScene];
    [[AppHelper getAppHelper] resetAppHelper];
    if([Utility IsPadDevice]){
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:@"EditorViewController" bundle:nil SceneItem:scene selectedindex:selectedScene];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    }
    else{
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:([self iPhone6Plus]) ? @"EditorViewControllerPhone" : @"EditorViewControllerPhone" bundle:nil SceneItem:scene selectedindex:selectedScene];
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
    self.popoverController.popoverContentSize = CGSizeMake(180.0, 213.0);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popoverController.delegate =self;
    self.popUpVc.delegate=self;
    CGRect rect = _infoBtn.frame;
    rect = [self.view convertRect:rect fromView:_infoBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self.view
                          permittedArrowDirections:UIPopoverArrowDirectionUp
                                          animated:YES];
}


- (void) infoBtnDelegateAction:(int)indexValue{
    
    if(indexValue==SETTINGS){
        [self.popoverController dismissPopoverAnimated:YES];
        settingsVc = [[SettingsViewController alloc]initWithNibName:([Utility IsPadDevice]) ? @"SettingsViewController" :
                      (screenHeight>320) ? @"SettingsViewControllerPhone" :
                      @"SettingsViewControllerPhone" bundle:nil];
        [settingsVc.view setClipsToBounds:YES];
        settingsVc.delegate=self;
        if([Utility IsPadDevice] || screenHeight < 400)
            settingsVc.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:settingsVc animated:YES completion:nil];
        settingsVc.view.superview.backgroundColor = [UIColor clearColor];
    }
    else if(indexValue==CONTACT_US){
        [self.popoverController dismissPopoverAnimated:YES];
        NSString *currentDeviceName;
        if(deviceNames != nil && [deviceNames objectForKey:[self deviceName]])
            currentDeviceName = [deviceNames objectForKey:[self deviceName]];
        else
            currentDeviceName = @"Unknown Device";
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
            [alert show];
            return;
        }
    }
}

-(NSString*) deviceName
{
    struct utsname systemInfo;
    uname(&systemInfo);
    
    return [NSString stringWithCString:systemInfo.machine
                              encoding:NSUTF8StringEncoding];
}

-(void)updateSceneDB {
    NSCharacterSet * set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];
    NSString* name = [_sceneTitle.text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
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

- (IBAction)titleChangeAction:(id)sender {
    [self updateSceneDB];
}

#pragma Scene Preview Delegates

- (void) showSceneEnteredView:(NSIndexPath*)sceneIndex{
    
    [self.sceneView setHidden:NO];
    //[self popUpZoomIn];
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

- (IBAction)openSceneAction:(id)sender {
    SceneItem *scene = scenesArray[currentSelectedScene];
    [[AppHelper getAppHelper] resetAppHelper];
    if([Utility IsPadDevice]){
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:@"EditorViewController" bundle:nil SceneItem:scene selectedindex:currentSelectedScene];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    }
    else{
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:@"EditorViewControllerPhone" bundle:nil SceneItem:scene selectedindex:currentSelectedScene];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    }
    
    [self removeFromParentViewController];
}

- (IBAction)loginBtnAction:(id)sender {
    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"])
    {
        if ([Utility IsPadDevice])
        {
            _loggedInVc = [[LoggedInViewController alloc] initWithNibName:@"LoggedInViewController" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_loggedInVc];
            self.popoverController.popoverContentSize = CGSizeMake(305, 182);
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
        }
        else
        {
            _loggedInVc = [[LoggedInViewController alloc] initWithNibName:@"LoggedInViewControllerPhone" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_loggedInVc];
            self.popoverController.popoverContentSize = CGSizeMake(230.0, 93);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            _loggedInVc.delegare=self;
            [_loggedInVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];
        }   
        
    }
    else
    {
        if ([Utility IsPadDevice])
        {
            loginVc = [[LoginViewController alloc] initWithNibName:@"LoginViewController" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:loginVc];
            self.popoverController.popoverContentSize = CGSizeMake(302 , 240.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [loginVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            loginVc.delegare=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];
            
        }
        else
        {
            loginVc = [[LoginViewController alloc] initWithNibName:@"LoginViewControllerPhone" bundle:nil];
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:loginVc];
            self.popoverController.popoverContentSize = CGSizeMake(228.00, 274.0);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [loginVc.view setClipsToBounds:YES];
            self.popUpVc.delegate=self;
            loginVc.delegare=self;
            self.popoverController.delegate =self;
            [self.popoverController presentPopoverFromRect:_loginBtn.frame
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:YES];
        }
    }

}
#pragma mark ScenePropertiesDelegate

-(void)duplicateScene:(int) indexValue{
    
    if([scenesArray count] <= 0)
        return;
    
    SceneItem* originalScene = scenesArray[indexValue];
    
    NSString* sceneCount = [NSString stringWithFormat:@"%lu", (unsigned long)[scenesArray count]];
    SceneItem* scene = [[SceneItem alloc] init];
    scene.name = [NSString stringWithFormat:@"%s%@", "MyScene", sceneCount];
    scene.createdDate = [dateFormatter stringFromDate:[NSDate date]];
    
    if(![cache AddScene:scene]) {
        [self.view endEditing:YES];
        UIAlertView *errorAlert = [[UIAlertView alloc]initWithTitle:@"Duplicate Scene Name" message:@"Another Scene with the same name already exists. Please provide a different name." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
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
    UIAlertView* renameScene = [[UIAlertView alloc] initWithTitle:@"Rename Scene" message:@"Please enter a new Scene name" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
    sceneToBeRenamed=indexValue;
    [renameScene setAlertViewStyle:UIAlertViewStylePlainTextInput];
    [[renameScene textFieldAtIndex:0] setPlaceholder:@"Scene Name"];
    [[renameScene textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
    [renameScene setTag:SCENE_NAME_ALERT];
    [renameScene show];
    [[renameScene textFieldAtIndex:0] becomeFirstResponder];
}

#pragma AlertView
- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if (alertView.tag==SCENE_NAME_ALERT) {
        NSCharacterSet* set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];
        
        if (buttonIndex == CANCEL) {
        }
        else if (buttonIndex == OK) {
            NSString* name = [[alertView textFieldAtIndex:0].text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
            if ([name length] == 0) {
                [self.view endEditing:YES];
                UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"Scene name cannot be empty." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                [errorAlert show];
            }
            else
            {
                if ([name rangeOfCharacterFromSet:set].location != NSNotFound) {
                    UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"Animation Name cannot contain any special characters." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                    [errorAlert show];
                }
                else
                {
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
    }
    [alertView resignFirstResponder];
}

#pragma Settings Delegates

-(void)frameCountDisplayMode:(int)selctedIndex{
    int tag = (selctedIndex == FRAME_COUNT) ? FRAME_COUNT : FRAME_DURATION;
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithLong:tag] withKey:@"indicationType"];
}
-(void)cameraPreviewSize:(int)selctedIndex{
    if(selctedIndex==CAMERA_PREVIEW_SMALL)
    {
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithFloat:1.0] withKey:@"cameraPreviewSize"];
    }
    else
    {
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithFloat:2.0] withKey:@"cameraPreviewSize"];
    }
}
-(void)cameraPreviewPosition:(int)selctedIndex{
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:selctedIndex] withKey:@"cameraPreviewPosition"];

}
-(void)toolbarPosition:(int)selctedIndex{
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:selctedIndex] withKey:@"toolbarPosition"];

}
-(void)multiSelectUpdate:(BOOL)value{
    [[AppHelper getAppHelper] saveBoolUserDefaults:value withKey:@"multiSelectOption"];
}

-(BOOL)iPhone6Plus{
    if (([UIScreen mainScreen].scale > 2.0)) return YES;
    return NO;
}

#pragma LogIn Delegates
-(void)googleSigninDelegate{
    isLoggedin= true;
}
-(void)dismisspopover{
    [self.popoverController dismissPopoverAnimated:YES];

}

-(void)dismissView{
    [_loggedInVc dismissViewControllerAnimated:YES completion:nil];
    [self.popoverController dismissPopoverAnimated:YES];

}


#pragma Dealloc Delegate

- (void)dealloc
{
    self.fileBeginsWith = nil;
    cache = nil;
    [scenesArray removeAllObjects];
    scenesArray = nil;
    dateFormatter = nil;
    deviceNames = nil;
    settingsVc = nil;
    loginVc = nil;
    self.popUpVc = nil;
    self.popoverController = nil;
    self.loggedInVc = nil;
}

@end

