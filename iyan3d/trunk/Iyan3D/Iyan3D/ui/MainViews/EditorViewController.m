//
//  EditorViewController.m
//  Iyan3D
//
//  Created by Sankar on 18/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "EditorViewController.h"
#import "FrameCellNew.h"

@implementation EditorViewController

#define EXPORT_POPUP 1
#define ANIMATION_POPUP 2
#define IMPORT_POPUP 3
#define INFO_POPUP 4
#define VIEW_POPUP 5

    
- (void)viewDidLoad
{
    [super viewDidLoad];
    totalFrames = 24;
    [self.framesCollectionView registerNib:[UINib nibWithNibName:@"FrameCellNew" bundle:nil] forCellWithReuseIdentifier:@"FRAMECELL"];
    tableData = [NSMutableArray arrayWithObjects:@"Camera",@"Light",@"Fatman",@"Skeleton",@"Dragon",@"Smackall Boy",nil];
    [self.objectList reloadData];
}

#pragma mark - Frames Collection View Deligates

- (NSInteger)collectionView:(UICollectionView*)collectionView numberOfItemsInSection:(NSInteger)section
{
    return totalFrames;
}

- (FrameCellNew*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    FrameCellNew* cell = [self.framesCollectionView dequeueReusableCellWithReuseIdentifier:@"FRAMECELL" forIndexPath:indexPath];
    cell.backgroundColor = [UIColor colorWithRed:61.0f / 255.0f
                                                   green:62.0f / 255.0f
                                                    blue:63.0f / 255.0f
                                                   alpha:1.0f];
    cell.layer.borderColor = [UIColor colorWithRed:61.0f / 255.0f
                                                     green:62.0f / 255.0f
                                                      blue:63.0f / 255.0f
                                                     alpha:1.0f].CGColor;
    cell.layer.borderWidth = 2.0f;
    cell.framesLabel.text = [NSString stringWithFormat:@"%d", (int)indexPath.row + 1];
    cell.framesLabel.adjustsFontSizeToFitWidth = YES;
    cell.framesLabel.minimumFontSize = 3.0;
    return cell;
}

- (void)collectionView:(UICollectionView*)collectionView didSelectItemAtIndexPath:(NSIndexPath*)indexPath
{
    NSIndexPath* toPath2 = [NSIndexPath indexPathForItem:indexPath.row inSection:0];
    UICollectionViewCell* todatasetCell2 = [self.framesCollectionView cellForItemAtIndexPath:toPath2];
    todatasetCell2.layer.borderColor = [UIColor colorWithRed:156.0f / 255.0f
                                                       green:156.0f / 255.0f
                                                        blue:156.0f / 255.0f
                                                       alpha:1.0f].CGColor;
    todatasetCell2.layer.borderWidth = 2.0f;
}

#pragma mark - Object Selection Table View Deligates

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [tableData count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSLog(@"Table View Created");
    static NSString *simpleTableIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];
    
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:simpleTableIdentifier];
    }
    
    cell.textLabel.text = [tableData objectAtIndex:indexPath.row];
    return cell;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        [tableData removeObjectAtIndex:indexPath.row];
        [tableView reloadData];
    }
}

- (UITableViewCellEditingStyle)tableView:(UITableView *)tableView editingStyleForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return UITableViewCellEditingStyleDelete;
}

- (BOOL)tableView:(UITableView *)tableview shouldIndentWhileEditingRowAtIndexPath:(NSIndexPath *)indexPath
{
    return YES;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    if(indexPath.row > 1)
        return YES;
    
    return NO;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

#pragma mark - Button Actions

- (IBAction)editFunction:(id)sender
{
    if([self.objectList isEditing]) {
        [self.editobjectBtn setTitle:@"Edit" forState:UIControlStateNormal];
        [self.objectList setEditing:NO animated:YES];
    } else {
        [self.editobjectBtn setTitle:@"Done" forState:UIControlStateNormal];
        [self.objectList setEditing:YES animated:YES];
    }
    
}

- (IBAction)addFrames:(id)sender
{
    totalFrames++;
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:totalFrames-1 inSection:0];
    [self.framesCollectionView reloadData];
    [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
}

- (IBAction)exportAction:(id)sender
{
    UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:nil
                                                             delegate:self
                                                    cancelButtonTitle:nil
                                               destructiveButtonTitle:nil
                                                    otherButtonTitles:@"Images", @"Video", nil];
    
    CGRect rect = [self.view convertRect:self.exportBtn.frame fromView:self.exportBtn.superview];
    [actionSheet setTag:EXPORT_POPUP];
    [actionSheet showFromRect:rect inView:self.view animated:YES];
}

- (IBAction)animationBtnAction:(id)sender
{
    UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:nil
                                                             delegate:self
                                                    cancelButtonTitle:nil
                                               destructiveButtonTitle:nil
                                                    otherButtonTitles:@"Apply Animation", @"Save Animation", nil];
    
    
    CGRect rect = [self.view convertRect:self.animationBtn.frame fromView:self.animationBtn.superview];
    [actionSheet setTag: ANIMATION_POPUP];
    [actionSheet showFromRect:rect inView:self.view animated:YES];
}

- (IBAction)importBtnAction:(id)sender
{
    UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:nil
                                                             delegate:self
                                                    cancelButtonTitle:nil
                                               destructiveButtonTitle:nil
                                                    otherButtonTitles:@"Models", @"Images",@"Text",@"Light",@"OBJ File",@"Add Bone", nil];
    
    
    CGRect rect = [self.view convertRect:self.importBtn.frame fromView:self.importBtn.superview];
    [actionSheet setTag:IMPORT_POPUP];
    [actionSheet showFromRect:rect inView:self.view animated:YES];
}
- (IBAction)infoBtnAction:(id)sender {
    UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:nil
                                                             delegate:self
                                                    cancelButtonTitle:nil
                                               destructiveButtonTitle:nil
                                                    otherButtonTitles:@"About", @"Help",@"Tutorials",@"Settings",@"Contact Us", nil];
    
    
    CGRect rect = [self.view convertRect:self.infoBtn.frame fromView:self.infoBtn.superview];
    [actionSheet setTag:INFO_POPUP];
    [actionSheet showFromRect:rect inView:self.view animated:YES];
}

- (IBAction)viewBtn:(id)sender {
    UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:nil
                                                             delegate:self
                                                    cancelButtonTitle:nil
                                               destructiveButtonTitle:nil
                                                    otherButtonTitles:@"Front", @"Top",@"Left",@"Back",@"Right",@"Bottom", nil];
    
    
    CGRect rect = [self.view convertRect:self.viewBtn.frame fromView:self.infoBtn.superview];
    [actionSheet setTag:VIEW_POPUP];
    [actionSheet showFromRect:rect inView:self.view animated:YES];
}

- (IBAction)lastFrameBtnAction:(id)sender {
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:totalFrames-1 inSection:0];
    [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:YES];
}

- (IBAction)firstFrameBtnAction:(id)sender {
    NSIndexPath* toPath = [NSIndexPath indexPathForItem:0 inSection:0];
    [self.framesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:YES];
}

- (IBAction)optionsBtnAction:(id)sender
{
}

- (IBAction)moveBtnAction:(id)sender
{
}

- (IBAction)rotateBtnAction:(id)sender
{
}

- (IBAction)scaleBtnAction:(id)sender
{
}

- (IBAction)undoBtnAction:(id)sender
{
}

- (IBAction)redoBtnAction:(id)sender
{
}




#pragma mark - ActionSheet Delegate Functions

-(void) actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex{
    switch ( actionSheet.tag )
    {
        case EXPORT_POPUP:
        {
            switch ( buttonIndex )
            {
                case 0:
                   
                    NSLog(@"Images Clicked");
                    break;
                case 1:
                    NSLog(@"Videos Clicked");
                    break;
            }
        }
            break;
        case ANIMATION_POPUP:
        {
            switch ( buttonIndex )
            {
                case 0:{
                    animationsliderVC =[[AnimationSelectionSlider alloc] initWithNibName:@"AnimationSelectionSlider" bundle:Nil];
                    [self.leftView addSubview:animationsliderVC.view];
                    [self.leftView setHidden:NO];
                    CATransition *transition = [CATransition animation];
                    transition.duration = 0.5;
                    transition.type = kCATransitionPush;
                    transition.subtype = kCATransitionFromLeft;
                    [transition setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
                    [self.leftView.layer addAnimation:transition forKey:nil];

                    NSLog(@"Apply Animation Clicked");
                    break;
                }
                case 1:
                    NSLog(@"Save Animation Clicked");
                    break;
            }
        }
            break;
        case IMPORT_POPUP:
        {
            switch ( buttonIndex )
            {
                case 0:
                    NSLog(@"Models Clicked");
                    break;
                case 1:{
                    
                    self.imagePicker = [[UIImagePickerController alloc] init];
                    self.imagePicker.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
                    [self.imagePicker setNavigationBarHidden:NO];
                    [self.imagePicker setToolbarHidden:NO];
                    
                    importImageViewVC = [[ImportImageNew alloc] initWithNibName:@"ImportImageNew" bundle:nil];
                    
                    [self.leftView addSubview:importImageViewVC.view];
                    [self.imagePicker.view setFrame:CGRectMake(0, 0, importImageViewVC.imagesView.frame.size.width, importImageViewVC.imagesView.frame.size.height)];
                    
                    self.imagePicker.delegate=importImageViewVC;
                    importImageViewVC.delegate = self;
                    [importImageViewVC.imagesView addSubview:self.imagePicker.view];

                    [self.leftView setHidden:NO];
                    CATransition *transition = [CATransition animation];
                    transition.duration = 0.5;
                    transition.type = kCATransitionPush;
                    transition.subtype = kCATransitionFromLeft;
                    [transition setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
                    [self.leftView.layer addAnimation:transition forKey:nil];
                    
                    
                    NSLog(@"Images Clicked");
                    break;
                }
                case 2:
                    NSLog(@"Text Clicked");
                    break;
                case 3:
                    NSLog(@"Light Clicked");
                    break;
                case 4:
                    NSLog(@"OBJ File Clicked");
                    break;
                case 5:
                    NSLog(@"Add Bone Clicked");
                    break;
            }
        }
            break;
        case INFO_POPUP:
        {
            switch ( buttonIndex )
            {
                case 0:
                    NSLog(@"About Clicked");
                    break;
                case 1:
                    NSLog(@"Help Clicked");
                    break;
                case 2:
                    NSLog(@"Tutorials Clicked");
                    break;
                case 3:
                    NSLog(@"Settings Clicked");
                    break;
                case 4:
                    NSLog(@"Contact Us Clicked");
                    break;
            }
        }
            break;
        case VIEW_POPUP:
        {
            switch ( buttonIndex )
            {
                case 0:
                    NSLog(@"Front View Clicked");
                    break;
                case 1:
                    NSLog(@"Top View Clicked");
                    break;
                case 2:
                    NSLog(@"Left View Clicked");
                    break;
                case 3:
                    NSLog(@"Back View Clicked");
                    break;
                case 4:
                    NSLog(@"Right View Clicked");
                    break;
                case 5:
                    NSLog(@"Bottom View Clicked");
                    break;
            }
        }
            break;
    }
    
}

#pragma mark - Other Delegate Functions

-(void) dismissAndHideView{
    NSLog(@"Delegate Function Called");
        CATransition* transition = [CATransition animation];
        transition.duration = 0.5;
        transition.type = kCATransitionPush;
        transition.subtype = kCATransitionFromRight;
        [transition setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];
        [self.leftView.layer addAnimation:transition forKey:kCATransition];
        [self.leftView setHidden:YES];    
}

- (void)dealloc
{
    
}

@end
