//
//  ImageImportViewController.h
//  Iyan3D
//
//  Created by karthik on 11/05/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Utility.h"
#import "GAI.h"

@protocol ImageImportViewControllerDelgate
-(void)pickedImageWithInfo:(NSDictionary*)info;
-(void)loadingViewStatus:(BOOL)status;
@end

@interface ImageImportViewController : GAITrackedViewController <UINavigationControllerDelegate,UIImagePickerControllerDelegate>
{
    NSDictionary* imageInfo;
}

@property (weak, nonatomic) IBOutlet UIView *imagesView;
- (IBAction)cancelButtonAction:(id)sender;
- (IBAction)addButtonAction:(id)sender;
@property (weak, nonatomic) IBOutlet UIImageView *displayImageView;
@property (weak, nonatomic) IBOutlet UIButton *addButton;
@property (weak, nonatomic) IBOutlet UIButton *cancelButton;

@property (weak, nonatomic) id <ImageImportViewControllerDelgate> delegate;

@end
