//
//  ImageImportViewController.m
//  Iyan3D
//
//  Created by karthik on 11/05/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import "ImageImportViewController.h"

@interface ImageImportViewController ()

@end

@implementation ImageImportViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    self.screenName = @"ImageImportView";
    if([Utility IsPadDevice]){
        self.view.layer.borderWidth = 2.0f;
        self.view.layer.cornerRadius = 5.0;
        self.view.layer.borderColor = [UIColor grayColor].CGColor;
    }
    self.cancelButton.layer.cornerRadius = 5.0;
    self.addButton.layer.cornerRadius = 5.0;
    [self.addButton setEnabled:NO];
    // Do any additional setup after loading the view.
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

- (NSUInteger) supportedInterfaceOrientations
{
    //Because your app is only landscape, your view controller for the view in your
    // popover needs to support only landscape
    return UIInterfaceOrientationMaskLandscapeLeft | UIInterfaceOrientationMaskLandscapeRight;
}

- (IBAction)cancelButtonAction:(id)sender {
    [self.delegate loadingViewStatus:NO];
    [self dismissViewControllerAnimated:YES completion:Nil];
}

- (IBAction)addButtonAction:(id)sender {
    [self.delegate pickedImageWithInfo:imageInfo];
    [self dismissViewControllerAnimated:YES completion:Nil];
}

- (void) imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    imageInfo = nil;
    imageInfo = [NSDictionary dictionaryWithDictionary:info];
    
    if(imageInfo)
        [self.addButton setEnabled:YES];
    else
        [self.addButton setEnabled:NO];
    
    UIImage *imgData = [info objectForKey:UIImagePickerControllerOriginalImage];
    [self.displayImageView setImage:imgData];
}


@end
