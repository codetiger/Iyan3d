//
//  ObjFileViewController.h
//  Iyan3D
//
//  Created by crazycubes on 19/04/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AssetItem.h"
#import "CacheSystem.h"
#import <CommonCrypto/CommonDigest.h>
#import "HelpViewController.h"
#import "FPPopoverController.h"
#import "ImageImportViewController.h"
#import "GAI.h"

@protocol ObjFileViewControllerDelegate
- (void) assetDownloadCompleted:(int)assetId;
- (void) ObjFileSelected:(NSString *)srcObjPath string2:(NSString *)srcTexturePath string3:(NSString *)fileName string4:(NSString *)textureFile;
- (void) CancelSelected:(BOOL)isCancelled;
- (void) returnToMainView;
@end

@interface ObjFileViewController : GAITrackedViewController < UICollectionViewDataSource, UICollectionViewDelegate,HelpViewControllerDelegate, UIImagePickerControllerDelegate,UINavigationControllerDelegate,ImageImportViewControllerDelgate>
{
    CacheSystem *cache;
	int assetId;
    NSInteger viewType;
    BOOL objAvailable;
    int ScreenWidth,ScreenHeight;
}

@property (weak, nonatomic) IBOutlet UICollectionView *objFileList;
@property (weak, nonatomic) IBOutlet UICollectionView *textureFileList;
@property (weak, nonatomic) IBOutlet UIButton *cancelButton;
@property (weak, nonatomic) IBOutlet UIButton *importButton;
@property (weak, nonatomic) id <ObjFileViewControllerDelegate> delegate;
@property (weak, nonatomic) IBOutlet UILabel* emptyObj;
@property (weak, nonatomic) IBOutlet UILabel* mainLabel;
@property (weak, nonatomic) IBOutlet UILabel* alertLabel;
@property (weak, nonatomic) IBOutlet UILabel* emptyTexture;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView* loadingView;
@property (weak, nonatomic) IBOutlet UIView* overview;
@property (weak, nonatomic) IBOutlet UIButton *helpButton;
@property (nonatomic, strong) HelpViewController *morehelpView;
@property (nonatomic, strong) FPPopoverController* popOverView;
@property (nonatomic, strong) UIImagePickerController *imagePicker;

- (IBAction)importTextureAction:(id)sender;
- (IBAction) addButtonAction:(id)sender;
- (IBAction) cancelButtonAction:(id)sender;
- (IBAction) helpButtonAction:(id)sender;
-(NSString*) getMD5ForNonReadableFile:(NSString*)path;
-(void) storeOBJinCachesDirectory:(NSString*)fileName assetId:(int)assetId;
-(void) storeOBJTextureinCachesDirectory:(NSString*)fileName assetId:(int)assetId;
- (NSData*) convertAndScaleImage:(UIImage*)image size:(int)textureRes;

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil callerId:(NSInteger) callerid objImported:(bool)value ScreenWidth:(int)screenWidth ScreenHeight:(int)screenHeight;
@end
