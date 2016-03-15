//
//  ImportImageNew.h
//  Iyan3D
//
//  Created by Sankar on 20/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import <MobileCoreServices/MobileCoreServices.h>


@protocol ImageImportNewDelgate
-(void)pickedImageWithInfo:(NSDictionary*)info type:(BOOL)isTempNode;
-(void)loadingViewStatus:(BOOL)status;
- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;
-(void)pickedVideoWithInfo:(NSString*)path FileName:(NSString*)fileName IsTemp:(BOOL)isTemp;
- (void) removeTempNodeFromScene;
@end

@interface ImportImageNew : GAITrackedViewController <UINavigationControllerDelegate,UIImagePickerControllerDelegate>{
    NSDictionary* imageInfo;
    
    int viewType;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil Type:(int)type;

@property (weak, nonatomic) IBOutlet UIButton *addBtn;
@property (weak, nonatomic) IBOutlet UIButton *cancelBtn;
@property (weak, nonatomic) IBOutlet UIView *imagesView;
@property (weak, nonatomic) id <ImageImportNewDelgate> delegate;

- (IBAction)addBtnAction:(id)sender;
- (IBAction)cancelBtnAction:(id)sender;

@end
