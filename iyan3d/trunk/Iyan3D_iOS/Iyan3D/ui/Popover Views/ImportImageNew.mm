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
    [self.addBtn setEnabled:NO];
   
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
   
}

#pragma mark Button Actions

- (IBAction)addBtnAction:(id)sender
{
    [self.delegate pickedImageWithInfo:imageInfo type:NO];
    [self.delegate showOrHideLeftView:NO withView:nil];
    [self.view removeFromSuperview];
}

- (IBAction)cancelBtnAction:(id)sender
{
    [self.delegate showOrHideLeftView:NO withView:nil];
    [self.view removeFromSuperview];
}

#pragma mark ImportImage imagepicker Delegate

- (void) imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    imageInfo = nil;
    imageInfo = [NSDictionary dictionaryWithDictionary:info];
    
    if(imageInfo){
        [self.delegate pickedImageWithInfo:imageInfo type:YES];
        [self.addBtn setEnabled:YES];
    }
    else
        [self.addBtn setEnabled:NO];
    
    }

- (void)dealloc{
    
}
@end
