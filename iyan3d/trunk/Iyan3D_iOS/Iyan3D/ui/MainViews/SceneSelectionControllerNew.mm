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
    deviceNames = [[AppHelper getAppHelper] parseJsonFileWithName:@"deviceCodes"];
    if([Utility IsPadDevice]){
        [self.scenesCollectionView registerNib:[UINib nibWithNibName:@"SceneSelectionFrameCell" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    else{
        [self.scenesCollectionView registerNib:[UINib nibWithNibName:@"SceneSelectionFrameCellPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    [self.sceneView setHidden:YES];
    
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillShow:) name:UIKeyboardWillShowNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(keyboardWillBeHidden:) name:UIKeyboardWillHideNotification object:nil];    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

#pragma Button Actions

- (IBAction)addSceneButtonAction:(id)sender {
    [self addNewScene];
}

- (IBAction)scenePreviewClosebtn:(id)sender {
    [self popZoomOut];
}

- (IBAction)feedBackButtonAction:(id)sender {
    
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

-(NSString*) deviceName
{
    struct utsname systemInfo;
    uname(&systemInfo);
    
    return [NSString stringWithCString:systemInfo.machine
                              encoding:NSUTF8StringEncoding];
}

#pragma CollectionView Delegates

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section{
    return [scenesArray count]+1;
}

- (SceneSelectionFrameCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath{
   cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
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

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    cell = (SceneSelectionFrameCell*)[collectionView cellForItemAtIndexPath:indexPath];
    
    
    
    cell_center = [self.scenesCollectionView convertPoint:cell.center fromView:cell];
    cell_center = [self.view convertPoint:cell_center fromView:self.scenesCollectionView];
    
    if(indexPath.row == [scenesArray count])
        [self addNewScene];
    else
        [self openSCene:indexPath.row];
    
    
    
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
        EditorViewController* animationEditor = [[EditorViewController alloc] initWithNibName:@"EditorViewControllerPhone" bundle:nil SceneItem:scene selectedindex:selectedScene];
        AppDelegate *appDelegate = (AppDelegate *)[[UIApplication sharedApplication] delegate];
        [appDelegate.window setRootViewController:animationEditor];
    }
    
    [self removeFromParentViewController];
    
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
    [self popUpZoomIn];
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

#pragma Keyboard handler Delegate


- (void) closeKeyBoard {
    [self.view removeGestureRecognizer:tapGesture];
    [_sceneTitle resignFirstResponder];
}

- (void)keyboardWillShow:(NSNotification*)aNotification {
    
    tapGesture = [[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(closeKeyBoard)];
    
    [self.view addGestureRecognizer:tapGesture];
    
    NSDictionary* keyboardInfo = [aNotification userInfo];
    NSValue* keyboardFrameBegin = [keyboardInfo valueForKey:UIKeyboardFrameBeginUserInfoKey];
    CGRect keyboardFrameBeginRect = [keyboardFrameBegin CGRectValue];

    [UIView animateWithDuration:0.25 animations:^
     {
         CGRect newFrame = [self.view frame];
         newFrame.origin.y -= keyboardFrameBeginRect.size.height; // tweak here to adjust the moving position
         [self.view setFrame:newFrame];
         
     }completion:^(BOOL finished)
     {
         
     }];
}

- (void)keyboardWillBeHidden:(NSNotification*)aNotification {
    NSDictionary* keyboardInfo = [aNotification userInfo];
    NSValue* keyboardFrameBegin = [keyboardInfo valueForKey:UIKeyboardFrameBeginUserInfoKey];
    CGRect keyboardFrameBeginRect = [keyboardFrameBegin CGRectValue];

    [UIView animateWithDuration:0.25 animations:^
     {
         CGRect newFrame = [self.view frame];
         newFrame.origin.y += keyboardFrameBeginRect.size.height; // tweak here to adjust the moving position
         [self.view setFrame:newFrame];
         
     }completion:^(BOOL finished)
     {
         
     }];
    
}

#pragma View Animation Delegate

- (void)popUpZoomIn {
    [_sceneView setHidden:NO];
    _sceneView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.001, 0.001);
    _sceneView.center = cell_center;
    [UIView animateWithDuration:0.5
                     animations:^{
                         _sceneView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1.0, 1.0);
                         _sceneView.center = self.view.center;
                     } completion:^(BOOL finished) {
                         
                     }];
}

- (void)popZoomOut {
    [UIView animateWithDuration:0.5
                     animations:^{
                         _sceneView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.001, 0.001);
                         _sceneView.center = cell_center;
                     } completion:^(BOOL finished) {
                         _sceneView.hidden = TRUE;
                     }];
}

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
    [renameScene setTag:0];
    [renameScene show];
//    [[renameScene textFieldAtIndex:0] becomeFirstResponder];
    [self updateSceneDB];
}

#pragma AlertView
- (void)alertView:(UIAlertView*)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    if (alertView.tag==0) {
        NSLog(@"Index : %ld",(long)buttonIndex);
        NSCharacterSet* set = [[NSCharacterSet characterSetWithCharactersInString:@"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLKMNOPQRSTUVWXYZ0123456789 "] invertedSet];
        
        if (buttonIndex == 0) {
        }
        else if (buttonIndex == 1) {
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
}

#pragma Dealloc Delegate

- (void)dealloc {
    self.fileBeginsWith = nil;
    cache = nil;
    [scenesArray removeAllObjects];
    scenesArray = nil;
    dateFormatter = nil;
    tapGesture = nil;
    deviceNames = nil;
}

@end

