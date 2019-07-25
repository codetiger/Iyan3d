//
//  SceneSelectionControllerNew.m
//  Iyan3D
//
//  Created by harishankarn on 16/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "MediaPreviewVC.h"
#import "AppDelegate.h"
#import "SceneSelectionControllerNew.h"
#import "SceneItem.h"
#import <sys/utsname.h>
#import "ZipArchive.h"

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

#define ALERT_TYPE 1
#define IMAGE_TYPE 2
#define URL_TYPE 3

@implementation SceneSelectionControllerNew

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cache         = [CacheSystem cacheSystem];
        scenesArray   = [cache GetSceneList];
        dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy/MM/dd HH:mm:ss"];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    self.sceneTitleLabel.text = NSLocalizedString(@"Scenes", nil);

    CGRect screenRect = [[UIScreen mainScreen] bounds];
    screenHeight      = screenRect.size.height;
    if ([Utility IsPadDevice]) {
        [self.scenesCollectionView registerNib:[UINib nibWithNibName:@"SceneSelectionFrameCell" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    } else {
        [[UIApplication sharedApplication] setStatusBarHidden:NO];
        [self.scenesCollectionView registerNib:[UINib nibWithNibName:@"SceneSelectionFrameCellPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
}

- (void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];

    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"openrenderTasks"]) {
        [[AppHelper getAppHelper] saveBoolUserDefaults:NO withKey:@"openrenderTasks"];
    }
    [self.centerLoading setHidden:YES];

    [[AppHelper getAppHelper] moveFilesFromInboxDirectory:cache];

    UITapGestureRecognizer* tapGest = [[UITapGestureRecognizer alloc] initWithTarget:self action:nil];
    tapGest.delegate                = self;
    [self.view addGestureRecognizer:tapGest];

    if (![[AppHelper getAppHelper] userDefaultsForKey:@"onBoardShown"]) {
        [self presentOnBoard];
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithBool:YES] withKey:@"onBoardShown"];
    }
}

- (BOOL)gestureRecognizer:(UIGestureRecognizer*)gestureRecognizer shouldReceiveTouch:(UITouch*)touch {
    if ([touch.view isDescendantOfView:self.helpBtn]) {
        return YES;
    }

    [[AppHelper getAppHelper] toggleHelp:nil Enable:NO];
    return NO;
}

- (void)showLoading {
    [self.centerLoading setHidden:NO];
    [self.centerLoading startAnimating];
}

- (void)hideLoading {
    [self.centerLoading stopAnimating];
    [self.centerLoading setHidden:YES];
    scenesArray = [cache GetSceneList];
    [self.scenesCollectionView reloadData];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (void)unzipAndRestoreScenei3d:(NSString*)filePath {
    NSFileManager* fm                 = [NSFileManager defaultManager];
    NSArray*       paths              = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString*      documentsDirectory = [paths objectAtIndex:0];
    NSString*      projectsDir        = [NSString stringWithFormat:@"%@/Projects", documentsDirectory];
    NSString*      unzipPath          = [NSString stringWithFormat:@"%@/Unzip", documentsDirectory];

    ZipArchive* zip = [[ZipArchive alloc] init];
    if ([zip UnzipOpenFile:filePath]) {
        if ([zip UnzipFileTo:unzipPath overWrite:YES]) {
            [fm removeItemAtPath:filePath error:nil];
            NSString* sceneFile = [cache insertImportedScene];

            NSArray*  filesArr     = [fm contentsOfDirectoryAtPath:unzipPath error:nil];
            NSArray*  sgbArr       = [filesArr filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.sgb'"]];
            NSString* oldSgbPath   = [NSString stringWithFormat:@"%@/%@", unzipPath, [sgbArr objectAtIndex:0]];
            NSString* oldSgbName   = [[oldSgbPath lastPathComponent] stringByDeletingPathExtension];
            NSString* newsgbPath   = [NSString stringWithFormat:@"%@/%@.sgb", projectsDir, sceneFile];
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

- (void)moveFilesToRespectiveDirs:(NSArray*)filePaths {
    NSFileManager* fm                 = [NSFileManager defaultManager];
    NSArray*       paths              = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSArray*       cachePaths         = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString*      documentsDirectory = [paths objectAtIndex:0];
    NSString*      cachesDir          = [cachePaths objectAtIndex:0];
    NSString*      unzipPath          = [NSString stringWithFormat:@"%@/Unzip", documentsDirectory];

    for (NSString* f in filePaths) {
        NSString* fPath     = [NSString stringWithFormat:@"%@/%@", unzipPath, f];
        NSString* extension = [fPath pathExtension];
        NSString* fileName  = [fPath lastPathComponent];
        NSString* destPath  = @"";

        if ([extension isEqualToString:@"sgm"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Sgm/%@", documentsDirectory, fileName];
        else if ([extension isEqualToString:@"png"]) {
            destPath = [NSString stringWithFormat:@"%@/%@", cachesDir, fileName];
            [fm copyItemAtPath:fPath toPath:destPath error:nil];
            destPath = [NSString stringWithFormat:@"%@/Resources/Textures/%@", documentsDirectory, fileName];
        } else if ([extension isEqualToString:@"obj"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Objs/%@", documentsDirectory, fileName];
        else if ([extension isEqualToString:@"MOV"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Videos/%@", documentsDirectory, fileName];
        else if ([extension isEqualToString:@"sgr"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Rigs/%@", documentsDirectory, fileName];
        else if ([extension isEqualToString:@"png"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Textures/%@", documentsDirectory, fileName];
        else if ([extension isEqualToString:@"ttf"] || [extension isEqualToString:@"otf"])
            destPath = [NSString stringWithFormat:@"%@/Resources/Fonts/%@", documentsDirectory, fileName];
        else
            destPath = [NSString stringWithFormat:@"%@/%@", documentsDirectory, fileName];

        if (![fm fileExistsAtPath:[destPath stringByDeletingLastPathComponent]])
            [fm createDirectoryAtPath:[destPath stringByDeletingLastPathComponent] withIntermediateDirectories:YES attributes:nil error:nil];

        NSError* error;
        [fm moveItemAtPath:fPath toPath:destPath error:&error];
        if (error)
            NSLog(@" Error moving resource %@ %@", fPath, error.localizedDescription);
    }
}

#pragma Button Actions

- (IBAction)addSceneButtonAction:(id)sender {
    [self addNewScene];
}

#pragma CollectionView Delegates

- (NSInteger)collectionView:(UICollectionView*)collectionView numberOfItemsInSection:(NSInteger)section {
    return [scenesArray count] + 1;
}

- (SceneSelectionFrameCell*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath {
    SceneSelectionFrameCell* cell         = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
    cell.delegate                         = self;
    cell.propertiesBtn.layer.cornerRadius = CORNER_RADIUS;
    cell.propertiesBtn.clipsToBounds      = YES;

    if (indexPath.row < [scenesArray count]) {
        if (indexPath.row == 0) {
            [cell setAccessibilityHint:NSLocalizedString(@"Tap to open the scene.", nil)];
            [cell setAccessibilityIdentifier:@"3"];
            [cell.propertiesBtn setAccessibilityHint:NSLocalizedString(@"Tap to Clone / Delete / Share the scene.", nil)];
            [cell.propertiesBtn setAccessibilityIdentifier:@"3"];
        } else {
            [cell setAccessibilityHint:@""];
            [cell.propertiesBtn setAccessibilityHint:@""];
        }

        SceneItem* scenes            = scenesArray[indexPath.row];
        cell.name.text               = [NSString stringWithFormat:@"%@", scenes.name];
        NSArray*  paths              = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* documentsDirectory = [paths objectAtIndex:0];
        NSString* originalFilePath   = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scenes.sceneFile];
        BOOL      fileExist          = [[NSFileManager defaultManager] fileExistsAtPath:originalFilePath];
        cell.propertiesBtn.hidden    = NO;
        cell.SelectedindexValue      = indexPath.row;
        if (fileExist)
            cell.image.image = [UIImage imageWithContentsOfFile:originalFilePath];
        else
            cell.image.image = [UIImage imageNamed:@"bgImageforall.png"];
    } else {
        if (indexPath.row == 0) {
            [cell setAccessibilityHint:NSLocalizedString(@"Creates a new scene.", nil)];
            [cell setAccessibilityIdentifier:@"3"];
        }

        cell.name.text            = NSLocalizedString(@"Create Scene", nil);
        cell.SelectedindexValue   = indexPath.row;
        cell.propertiesBtn.hidden = YES;
        cell.image.image          = [UIImage imageNamed:@"New-scene.png"];
    }
    return cell;
}

- (void)collectionView:(UICollectionView*)collectionView didSelectItemAtIndexPath:(NSIndexPath*)indexPath {
    SceneSelectionFrameCell* cell = (SceneSelectionFrameCell*)[collectionView cellForItemAtIndexPath:indexPath];
    cell_center                   = [self.scenesCollectionView convertPoint:cell.center fromView:cell];
    cell_center                   = [self.view convertPoint:cell_center fromView:self.scenesCollectionView];

    if (indexPath.row == [scenesArray count]) {
        [self addNewScene];
    } else
        [self openSCene:(int)indexPath.row];
}

#pragma Scene Update Delegates

- (void)addNewScene {
    SceneItem* scene = [[SceneItem alloc] init];
    SceneItem* previousScene;
    if ([scenesArray count] != 0)
        previousScene = scenesArray[[scenesArray count] - 1];
    scene.name        = [NSString stringWithFormat:NSLocalizedString(@"My Scene %d", nil), previousScene.sceneId + 1];
    scene.createdDate = [dateFormatter stringFromDate:[NSDate date]];

    if ([cache AddScene:scene]) {
        scenesArray = [cache GetSceneList];
        [self.scenesCollectionView reloadData];
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:[scenesArray count] - 1 inSection:0];
        [self.scenesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredVertically animated:YES];
    }
}

- (void)openSCene:(int)selectedScene {
    SceneItem* scene = scenesArray[selectedScene];
    [[AppHelper getAppHelper] resetAppHelper];

    if ([Utility IsPadDevice]) {
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:@"EditorViewController" bundle:nil SceneItem:scene selectedindex:selectedScene];
        AppDelegate*          appDelegate     = (AppDelegate*)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    } else {
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:([self iPhone6Plus]) ? @"EditorViewControllerPhone@2x" : @"EditorViewControllerPhone" bundle:nil SceneItem:scene selectedindex:selectedScene];
        AppDelegate*          appDelegate     = (AppDelegate*)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    }

    [self removeFromParentViewController];
}

- (IBAction)infoBtnAction:(id)sender {
    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"infoBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController                    = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(200.0, 39 * 5);
    self.popoverController.animationType      = WEPopoverAnimationTypeCrossFade;
    self.popoverController.delegate           = self;
    self.popUpVc.delegate                     = self;
    CGRect rect                               = _infoBtn.frame;
    rect                                      = [self.view convertRect:rect fromView:_infoBtn.superview];
    [self.popoverController presentPopoverFromRect:rect inView:self.view permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}

- (void)infoBtnDelegateAction:(int)indexValue {
    if (indexValue == TUTORIAL) {
        [self.popoverController dismissPopoverAnimated:YES];
        [self presentOnBoard];

    } else if (indexValue == SETTINGS) {
        [self.popoverController dismissPopoverAnimated:YES];
        settingsVc = [[SettingsViewController alloc] initWithNibName:([Utility IsPadDevice]) ? @"SettingsViewController" : ([self iPhone6Plus]) ? @"SettingsViewControllerPhone2x" : @"SettingsViewControllerPhone" bundle:nil];
        [settingsVc.view setClipsToBounds:YES];
        settingsVc.delegate = self;
        if ([Utility IsPadDevice] || screenHeight < 400)
            settingsVc.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:settingsVc animated:YES completion:nil];
        settingsVc.view.superview.backgroundColor = [UIColor clearColor];

    } else if (indexValue == CONTACT_US) {
        [self.popoverController dismissPopoverAnimated:YES];
        if ([MFMailComposeViewController canSendMail]) {
            MFMailComposeViewController *composeViewController = [[MFMailComposeViewController alloc] initWithNibName:nil bundle:nil];
            [composeViewController setMailComposeDelegate:self];
            [composeViewController setToRecipients:@[@"codetiger42@icloud.com"]];
            [composeViewController setSubject:[NSString stringWithFormat:@"Feedback on Iyan 3D Pro (%@, iOS Version: %@)", [self deviceName], iOSVersion]];
            [self presentViewController:composeViewController animated:YES completion:nil];
        } else {
            [self.view endEditing:YES];
            UIAlertView* alert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Alert", nil) message:NSLocalizedString(@"No_Email_account_configured", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
            [alert show];
            return;
        }
    }
}

- (void)presentOnBoard {
    NSString* nibName = @"OnBoardVC";
    if ([self iPhone6Plus])
        nibName = @"OnBoardVCPhone@2x";
    else if (![Utility IsPadDevice])
        nibName = @"OnBoardVCPhone";

    OnBoardVC* ovc = [[OnBoardVC alloc] initWithNibName:nibName bundle:nil];
    if ([Utility IsPadDevice])
        ovc.modalPresentationStyle = UIModalPresentationFormSheet;
    ovc.delegate = self;
    [self presentViewController:ovc animated:YES completion:nil];
}

- (void)closingOnBoard {
    if (![[AppHelper getAppHelper] userDefaultsForKey:@"tipsShown"]) {
        [[AppHelper getAppHelper] toggleHelp:self Enable:YES];
        if ([scenesArray count] > 0)
            [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithBool:YES] withKey:@"tipsShown"];
    }
}

- (void)mailComposeController:(MFMailComposeViewController*)controller didFinishWithResult:(MFMailComposeResult)result error:(NSError*)error {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (NSString*)deviceName {
    struct utsname systemInfo;
    uname(&systemInfo);

    return [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
}

#pragma View Animation Delegate

- (IBAction)toolTipAction:(id)sender {
    [[AppHelper getAppHelper] toggleHelp:self Enable:YES];
}

- (void)showOrHideProgress:(BOOL)value {
}

#pragma mark ScenePropertiesDelegate

- (void)duplicateScene:(int)indexValue {
    if ([scenesArray count] <= 0)
        return;

    SceneItem* originalScene = scenesArray[indexValue];

    SceneItem* scene = [[SceneItem alloc] init];
    SceneItem* previousScene;
    if ([scenesArray count] != 0)
        previousScene = scenesArray[[scenesArray count] - 1];
    scene.name        = [NSString stringWithFormat:NSLocalizedString(@"My Scene %d", nil), previousScene.sceneId + 1];
    scene.createdDate = [dateFormatter stringFromDate:[NSDate date]];

    if (![cache AddScene:scene]) {
        [self.view endEditing:YES];
        UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Duplicate_Scene_Name", nil) message:NSLocalizedString(@"Duplicate_Scene_Name_message", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
        [errorAlert show];
    } else {
        NSArray*  paths              = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* documentsDirectory = [paths objectAtIndex:0];
        NSString* originalFilePath   = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, originalScene.sceneFile];
        NSString* newFilePath        = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, scene.sceneFile];
        [[NSFileManager defaultManager] copyItemAtPath:originalFilePath toPath:newFilePath error:nil];
        originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, originalScene.sceneFile];
        newFilePath      = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scene.sceneFile];
        [[NSFileManager defaultManager] copyItemAtPath:originalFilePath toPath:newFilePath error:nil];

        scenesArray = [cache GetSceneList];
        [self.scenesCollectionView reloadData];
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:[scenesArray count] - 1 inSection:0];
        [self.scenesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredVertically animated:YES];
        [self.scenesCollectionView reloadData];
    }
}

- (void)deleteScene:(int)indexValue {
    SceneItem* scene = scenesArray[indexValue];
    [[AppHelper getAppHelper] removeFromUserDefaultsWithKey:scene.name];
    NSArray*  paths              = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* filePath           = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, scene.sceneFile];
    self.fileBeginsWith          = scene.sceneFile;
    NSArray*  cachepath          = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory     = [cachepath objectAtIndex:0];
    NSArray*  dirFiles           = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:cacheDirectory error:nil];
    NSArray*  jpgFiles           = [dirFiles filteredArrayUsingPredicate:
                                      [NSPredicate predicateWithFormat:@"self BEGINSWITH[cd] %@", self.fileBeginsWith]];
    for (int i = 0; i < [jpgFiles count]; i++) {
        NSString* filePath1 = [NSString stringWithFormat:@"%@/%@-%d.png", cacheDirectory, self.fileBeginsWith, i];
        [[NSFileManager defaultManager] removeItemAtPath:filePath1 error:nil];
    }
    [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
    filePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scene.sceneFile];
    [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];

    [cache DeleteScene:scene];
    [scenesArray removeObjectAtIndex:indexValue];
    [self.scenesCollectionView reloadData];
}

- (void)renameScene:(int)indexValue {
    UIAlertView* renameScene = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Rename Scene", nil) message:NSLocalizedString(@"Please enter a new Scene name", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"Cancel", nil) otherButtonTitles:NSLocalizedString(@"Ok", nil), nil];

    sceneToBeRenamed = indexValue;
    [renameScene setAlertViewStyle:UIAlertViewStylePlainTextInput];
    [[renameScene textFieldAtIndex:0] setPlaceholder:NSLocalizedString(@"Scene Name", nil)];
    [[renameScene textFieldAtIndex:0] setKeyboardType:UIKeyboardTypeAlphabet];
    [renameScene setTag:SCENE_NAME_ALERT];
    [renameScene show];
    [[renameScene textFieldAtIndex:0] becomeFirstResponder];
}

- (void)setSelectedSceneIndex:(int)index {
    selectedSceneIndex = index;
}

- (void)shareScene:(NSNumber*)value {
    SceneItem* scene              = scenesArray[selectedSceneIndex];
    NSArray*   paths              = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString*  documentsDirectory = [paths objectAtIndex:0];
    NSString*  i3dPath            = [NSString stringWithFormat:@"%@/Projects/%@.i3d", documentsDirectory, scene.sceneFile];
    if ([[NSFileManager defaultManager] fileExistsAtPath:i3dPath]) {
        NSArray* objectsToShare;

        NSURL* sceneURL = [NSURL fileURLWithPath:i3dPath];
        objectsToShare  = [NSArray arrayWithObjects:NSLocalizedString(@"A Scene created using Iyan 3D in iOS.", nil), sceneURL, nil];

        self.docController     = [UIDocumentInteractionController interactionControllerWithURL:sceneURL];
        self.docController.UTI = @"com.smackall.i3d";
        if (self.docController != nil) {
            self.docController.delegate = self;
            CGRect sourceRect           = [self.scenesCollectionView convertRect:[self.scenesCollectionView cellForItemAtIndexPath:[NSIndexPath indexPathForRow:selectedSceneIndex inSection:0]].frame toView:self.view];
            [self.docController presentOptionsMenuFromRect:sourceRect inView:self.view animated:YES];
        }
    }
}

- (bool)Isi3dExists:(int)indexValue {
    SceneItem* scene              = scenesArray[indexValue];
    NSArray*   paths              = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString*  documentsDirectory = [paths objectAtIndex:0];
    NSString*  i3dPath            = [NSString stringWithFormat:@"%@/Projects/%@.i3d", documentsDirectory, scene.sceneFile];
    if ([[NSFileManager defaultManager] fileExistsAtPath:i3dPath])
        return true;

    return false;
}

#pragma AlertView
- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex {
    if (alertView.tag == SCENE_NAME_ALERT) {
        NSCharacterSet* set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];

        if (buttonIndex == CANCEL) {
        } else if (buttonIndex == OK) {
            NSString* name = [[alertView textFieldAtIndex:0].text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
            if ([name length] == 0) {
                [self.view endEditing:YES];
                UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"Scene_name_empty", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];

                [errorAlert show];

            } else {
                if ([name rangeOfCharacterFromSet:set].location != NSNotFound) {
                    UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"Scene_name_special_character", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
                    [errorAlert show];

                } else {
                    SceneItem* scene  = scenesArray[sceneToBeRenamed];
                    scene.name        = name;
                    scene.createdDate = scene.createdDate;
                    scene.sceneFile   = scene.sceneFile;
                    scene.sceneId     = scene.sceneId;
                    [cache UpdateScene:scene];
                    scenesArray = [cache GetSceneList];
                    [self.scenesCollectionView reloadData];
                }
            }
        }
        [alertView resignFirstResponder];
    }
}

#pragma Settings Delegates

- (void)frameCountDisplayMode {
}

- (void)cameraPreviewSize {
}

- (void)cameraPreviewPosition {
}

- (void)toolbarPosition:(int)selctedIndex {
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:selctedIndex] withKey:@"toolbarPosition"];
}

- (void)multiSelectUpdate:(BOOL)value {
    [[AppHelper getAppHelper] saveBoolUserDefaults:value withKey:@"multiSelectOption"];
}

- (BOOL)iPhone6Plus {
    if (([UIScreen mainScreen].scale > 2.0))
        return YES;
    return NO;
}

- (void)dismisspopover {
    [self.popoverController dismissPopoverAnimated:YES];
}

- (void)dismissView {
    [self.popoverController dismissPopoverAnimated:YES];
}

- (void)showPreview:(NSString*)outputPath {
    [self performSelectorOnMainThread:@selector(showPreviewInMainThread:) withObject:outputPath waitUntilDone:NO];
}

- (void)showPreviewInMainThread:(NSString*)outputPath {
    [self.centerLoading setHidden:YES];
    int mediaType = [[outputPath pathExtension] isEqualToString:@"png"] ? 0 : 1;

    if ([Utility IsPadDevice]) {
        MediaPreviewVC* medPreview = [[MediaPreviewVC alloc] initWithNibName:@"MediaPreviewVC" bundle:nil mediaType:mediaType medPath:outputPath];
        [self dismissView];
        medPreview.modalPresentationStyle = UIModalPresentationFullScreen;
        [self presentViewController:medPreview animated:YES completion:nil];
    } else {
        MediaPreviewVC* medPreview = [[MediaPreviewVC alloc] initWithNibName:[[AppHelper getAppHelper] iPhone6Plus] ? @"MediaPreviewVCPhone@2x" : @"MediaPreviewVCPhone" bundle:nil mediaType:mediaType medPath:outputPath];
        [self dismissView];
        medPreview.modalPresentationStyle = UIModalPresentationFullScreen;
        [self presentViewController:medPreview animated:YES completion:nil];
    }
}

- (void)showInternetError:(int)connectionError {
    if (connectionError != SLOW_INTERNET) {
        [self.view endEditing:YES];
        UIAlertView* internetError = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Connection Error", nil) message:NSLocalizedString(@"unable_connect_server", nil) delegate:self cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];

        [internetError performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}

#pragma Dealloc Delegate

- (void)dealloc {
    self.fileBeginsWith = nil;
    cache               = nil;
    [scenesArray removeAllObjects];
    scenesArray            = nil;
    dateFormatter          = nil;
    settingsVc             = nil;
    self.popUpVc           = nil;
    self.popoverController = nil;
    if (settingsVc) {
        settingsVc.delegate = nil;
        settingsVc          = nil;
    }
    if (_popoverController) {
        _popoverController.delegate = nil;
        _popoverController          = nil;
    }
    if (_docController) {
        _docController.delegate = nil;
        _docController          = nil;
    }
}

@end
