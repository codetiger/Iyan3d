//
//  ImportImageNew.m
//  Iyan3D
//
//  Created by Sankar on 20/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "ImportImageNew.h"

@interface ImportImageNew ()

@end

@implementation ImportImageNew

- (void)viewDidLoad {

    
    [super viewDidLoad];
   
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
   
}

#pragma mark Button Actions

- (IBAction)addBtnAction:(id)sender
{
}

- (IBAction)cancelBtnAction:(id)sender
{
    [self.delegate dismissAndHideView];
    [self.view removeFromSuperview];
}

#pragma mark ImportImage imagepicker Delegate

- (void) imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    
}

- (void)dealloc{
    NSLog(@"Dealloc called");
}
@end
