
//  ObjFileViewController.m
//  Iyan3D
//
//  Created by crazycubes on 19/04/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "ObjFileViewController.h"
#import "ObjCellView.h"
#import "Utility.h"
#import "AppDelegate.h"
#import "AppHelper.h"
#define OBJType 6
#define ComesFromAutoRig 7
#define ComesFromAssetSelection 8
#define ComesFromSceneSelection 9
#define COMES_FROM_OBJIMPORTER 6

@implementation ObjFileViewController
{
    NSArray *objFiles;
    NSInteger valueType;
    NSArray *textureFile;
    NSArray *temptextureFile;
    NSInteger objFileIndex,textureFileIndex;
    NSMutableData *myData;
}

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil callerId:(NSInteger) callerid objImported:(bool)value
{
    valueType= callerid;
    objAvailable=value;
    objFileIndex = -1 ,textureFileIndex = -1; //default value
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cache = [CacheSystem cacheSystem];
    }
    return self;
}
- (void) viewDidLoad
{
    [super viewDidLoad];

    if([Utility IsPadDevice]){
        [self.objFileList registerNib:[UINib nibWithNibName:@"ObjCellView" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
        [self.textureFileList registerNib:[UINib nibWithNibName:@"ObjCellView" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    else{
        [self.objFileList registerNib:[UINib nibWithNibName:@"ObjCellViewPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
        [self.textureFileList registerNib:[UINib nibWithNibName:@"ObjCellViewPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    self.cancelButton.layer.cornerRadius= 5.0;
    self.importButton.layer.cornerRadius= 5.0;
    [self.loadingView setHidden:YES];
	NSArray *extensions = [NSArray arrayWithObjects:@"png", @"jpeg", @"jpg", @"PNG", @"JPEG", nil];
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSArray* cachepaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString* cacheDirectory = [cachepaths objectAtIndex:0];
    NSString *dataPath = [cacheDirectory stringByAppendingPathComponent:@"/texureFile"];
    NSArray *dirFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirPath error:nil];
    NSArray *dirFiles1 = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:dataPath error:nil];
    objFiles = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.obj'"]];
    textureFile = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
    temptextureFile = [dirFiles1 filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
    
    [[AppHelper getAppHelper] moveFilesFromInboxDirectory:cache];
    if(([textureFile count]> [temptextureFile count]))
    {
        NSSet *set1 = [NSSet setWithArray:temptextureFile];
        NSMutableSet *set2 = [NSMutableSet setWithArray:textureFile];
        [set2 minusSet:set1];
        NSArray *missingImg = [set2 allObjects];
        for(int i = 0 ; i< [missingImg count] ; i++)
        {
            NSString* srcFilePath = [NSString stringWithFormat:@"%@/%@",docDirPath,missingImg[i]];
            NSString* desFilePathForDisplay = [NSString stringWithFormat:@"%@/%@",dataPath,missingImg[i]];
            NSData *imageDataforDisplay = [self convertAndScaleImage:[UIImage imageWithContentsOfFile:srcFilePath] size:64];
            [imageDataforDisplay writeToFile:desFilePathForDisplay atomically:YES];
        }
        dirFiles1 = nil;
        dirFiles1 = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:dataPath error:nil];
        temptextureFile = [dirFiles1 filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
    }
    else if([textureFile count]<[temptextureFile count]){
        NSSet *set1 = [NSSet setWithArray:textureFile];
        NSMutableSet *set2 = [NSMutableSet setWithArray:temptextureFile];
        [set2 minusSet:set1];
        NSArray *missingImg = [set2 allObjects];
        for(int i = 0 ; i< [missingImg count] ; i++)
        {
            NSString* desFilePathForDisplay = [NSString stringWithFormat:@"%@/%@",dataPath,missingImg[i]];
            [[NSFileManager defaultManager] removeItemAtPath:desFilePathForDisplay error:nil];
        }
        dirFiles1 = nil;
        dirFiles1 = [[NSFileManager defaultManager] subpathsOfDirectoryAtPath:dataPath error:nil];
        temptextureFile = [dirFiles1 filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
    }
    if([objFiles count])
		[self.emptyObj setHidden:YES];
    if([temptextureFile count])
		[self.emptyTexture setHidden:YES];
	
	[self.importButton setHighlighted:YES];
	[self.importButton setEnabled:NO];
    self.overview.layer.borderWidth = 2.0f;
    self.overview.layer.borderColor=[UIColor colorWithRed:67.0f/255.0f
                                                    green:68.0f/255.0f
                                                     blue:67.0f/255.0f
                                                    alpha:1.0f].CGColor;
    self.overview.backgroundColor= [UIColor colorWithRed:7.0f/255.0f
                                                   green:7.0f/255.0f
                                                    blue:7.0f/255.0f
                                                   alpha:1.0f];
    self.objFileList.layer.borderWidth = 2.0f;
    self.objFileList.layer.borderColor=[UIColor colorWithRed:67.0f/255.0f
                                                       green:68.0f/255.0f
                                                        blue:67.0f/255.0f
                                                       alpha:1.0f].CGColor;
    self.textureFileList.layer.borderWidth = 2.0f;
    self.textureFileList.layer.borderColor=[UIColor colorWithRed:67.0f/255.0f
                                                           green:68.0f/255.0f
                                                            blue:67.0f/255.0f
                                                           alpha:1.0f].CGColor;
    if(valueType ==ComesFromAutoRig){
        [self.mainLabel setHidden:YES];
        [self.alertLabel setHidden:NO];
    }
    else{
        [self.mainLabel setHidden:NO];
        [self.alertLabel setHidden:YES];
    }
}
- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    self.screenName = @"ObjFilesView";
}
- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    if(self.objFileList == collectionView)
        return [objFiles count];
    else
        return [temptextureFile count];
}
- (ObjCellView *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    if(self.objFileList == collectionView){
        ObjCellView *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
        cell.assetNameLabel.text = objFiles[indexPath.row];
        cell.layer.cornerRadius = 10.0;
        cell.layer.borderWidth = 1.0f;
        cell.layer.borderColor = [UIColor grayColor].CGColor;
        cell.assetImageView.image =[UIImage imageNamed:@"objfile.png"];
        return cell;
    }
    else{
        NSArray* cachepaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* cacheDirectory = [cachepaths objectAtIndex:0];
        NSString* srcFilePath = [NSString stringWithFormat:@"%@/texureFile/%@",cacheDirectory,temptextureFile[indexPath.row]];
        ObjCellView *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
        cell.assetNameLabel.text = temptextureFile[indexPath.row];
        cell.layer.cornerRadius = 10.0;
        cell.layer.borderWidth = 1.0f;
        cell.layer.borderColor = [UIColor grayColor].CGColor;
        cell.assetImageView.image=[UIImage imageWithContentsOfFile:srcFilePath];
        return cell;
    }
}
- (void) collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath 
{
    NSArray *indexPathArr = [collectionView indexPathsForVisibleItems];
    for(int i = 0; i < [indexPathArr count]; i++){
        NSIndexPath *indexPath = [indexPathArr objectAtIndex:i];
        UICollectionViewCell* cell = [collectionView  cellForItemAtIndexPath:indexPath];
        cell.backgroundColor = [UIColor clearColor];
    }
    if(self.objFileList == collectionView)
        objFileIndex=indexPath.row;
    else
        textureFileIndex=indexPath.row;
    
    UICollectionViewCell* cell = [collectionView  cellForItemAtIndexPath:indexPath];
    cell.backgroundColor = [UIColor colorWithRed:150.0f/255.0f
                                           green:150.0f/255.0f
                                            blue:150.0f/255.0f
                                           alpha:1.0f];
	
	if(objFileIndex + 1 && textureFileIndex + 1){
        // make positive value from the default value add 1 to the objFileIndex and textureFileIndex
		[self.importButton setHighlighted:NO];
		[self.importButton setEnabled:YES];
	}
	else{
		[self.importButton setHighlighted:YES];
		[self.importButton setEnabled:NO];
	}
}
- (IBAction) helpButtonAction:(id)sender
{
    if([Utility IsPadDevice]) {
        HelpViewController* morehelpView = [[HelpViewController alloc] initWithNibName:@"HelpViewController" bundle:nil CalledFrom:COMES_FROM_OBJIMPORTER];
        morehelpView.delegate = self;
        morehelpView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:morehelpView animated:YES completion:nil];
		CGRect rect = CGRectInset(morehelpView.view.frame, -80, -0);
		morehelpView.view.superview.backgroundColor = [UIColor clearColor];
		morehelpView.view.frame = rect;
	}
    else{
        HelpViewController* morehelpView = [[HelpViewController alloc] initWithNibName:@"HelpViewControllerPhone" bundle:nil CalledFrom:COMES_FROM_OBJIMPORTER];
        morehelpView.delegate = self;
        morehelpView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:morehelpView animated:YES completion:nil];
 	}
}
- (IBAction)importTextureAction:(id)sender {
    if(self.imagePicker == nil) {
        self.imagePicker = [[UIImagePickerController alloc] init];
        self.imagePicker.delegate = self;
        self.imagePicker.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
        [self.imagePicker setNavigationBarHidden:YES];
        [self.imagePicker setToolbarHidden:YES];
    }
    if([Utility IsPadDevice]){
        ImageImportViewController* imageImportView = [[ImageImportViewController alloc] initWithNibName:@"ImageImportViewPad" bundle:nil];
        imageImportView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:imageImportView animated:YES completion:nil];
        imageImportView.view.superview.backgroundColor = [UIColor clearColor];
        imageImportView.view.layer.borderWidth = 2.0f;
        imageImportView.view.layer.borderColor = [UIColor grayColor].CGColor;
        [imageImportView.imagesView addSubview:self.imagePicker.view];
        self.imagePicker.delegate = imageImportView;
        [self.imagePicker.view setFrame:CGRectMake(0, 0, imageImportView.imagesView.frame.size.width, imageImportView.imagesView.frame.size.height)];
        imageImportView.delegate = self;
    }
    else
    {
        ImageImportViewController* imageImportView = [[ImageImportViewController alloc] initWithNibName:@"ImageImportViewPhone" bundle:nil];
        imageImportView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:imageImportView animated:YES completion:nil];
        imageImportView.view.superview.backgroundColor = [UIColor clearColor];
        imageImportView.view.layer.borderWidth = 2.0f;
        imageImportView.view.layer.borderColor = [UIColor grayColor].CGColor;
        [imageImportView.imagesView addSubview:self.imagePicker.view];
        self.imagePicker.delegate = imageImportView;
        [self.imagePicker.view setFrame:CGRectMake(0, 0, imageImportView.imagesView.frame.size.width, imageImportView.imagesView.frame.size.height)];
        imageImportView.delegate = self;
     }
}
-(void)pickedImageWithInfo:(NSDictionary*)info
{
    UIImage *imgData = [info objectForKey:UIImagePickerControllerOriginalImage];
    NSString* imgSalt = [[info objectForKey:UIImagePickerControllerReferenceURL] absoluteString];
        
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *fileNameSalt = [Utility getMD5ForString:imgSalt];
    NSString *saveFileName = [NSString stringWithFormat:@"%@/%@.png", documentsDirectory, fileNameSalt];
    NSData* data = UIImagePNGRepresentation(imgData);
    [data writeToFile:saveFileName atomically:YES];
    [self.textureFileList reloadData];
    [self viewDidLoad];
}

-(void)loadingViewStatus:(BOOL)status
{
    
}

- (IBAction) addButtonAction:(id)sender
{
	[self.loadingView setHidden:NO];
	[self.loadingView startAnimating];
    if(valueType ==ComesFromAutoRig){
        NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* docDirPath = [srcDirPath objectAtIndex:0];
        NSString* srcObjFilePath = [NSString stringWithFormat:@"%@/%@",docDirPath,objFiles[objFileIndex]];
        
        NSString* srcTextureFilePath = [NSString stringWithFormat:@"%@/%@",docDirPath,temptextureFile[textureFileIndex]];
        [self dismissViewControllerAnimated:NO completion:^{
            [self.delegate ObjFileSelected:srcObjFilePath string2:srcTextureFilePath string3:objFiles[objFileIndex] string4:textureFile[textureFileIndex]];
        }];
    }
}
- (NSData*) convertAndScaleImage:(UIImage*)image size:(int)textureRes
{
    float target = 0;
    target = (float)textureRes;
    
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(target, target), NO, 1.0);
    [image drawInRect:CGRectMake(0, 0, target, target)];
    UIImage* nImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    NSData* data = UIImagePNGRepresentation(nImage);
    return data;
}
- (IBAction) cancelButtonAction:(id)sender
{
    [self dismissViewControllerAnimated:NO completion:^{
        [self.delegate CancelSelected:1];
    }];
}

- (void) imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    [self.popOverView dismissPopoverAnimated:YES];
    UIImage *imgData = [info objectForKey:UIImagePickerControllerOriginalImage];
    NSString* imgSalt = [[info objectForKey:UIImagePickerControllerReferenceURL] absoluteString];
    
    float imgW = imgData.size.width;
    float imgH = imgData.size.height;
    float bigSide = (imgW >= imgH) ? imgW : imgH;
    float target = 0;
    
    if(bigSide <= 128)
        target = 128;
    else if(bigSide <= 256)
        target = 256;
    else if(bigSide <= 512)
        target = 512;
    else
        target = 1024;
    
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(target, target), NO, 1.0);
    [imgData drawInRect:CGRectMake(0, 0, target, target)];
    UIImage* nImage = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    NSString *fileNameSalt = [Utility getMD5ForString:imgSalt];
    NSString *saveFileName = [NSString stringWithFormat:@"%@/%@.png", documentsDirectory, fileNameSalt];
    NSData* data = UIImagePNGRepresentation(nImage);
    [data writeToFile:saveFileName atomically:YES];
    [self.textureFileList reloadData];
    [self viewDidLoad];
}


-(void) dealloc{
    cache = nil;
    _morehelpView = nil;
    _objFileList = nil;
    _textureFileList = nil;
    _cancelButton = nil;
    _importButton = nil;
    _emptyObj = nil;
    _mainLabel = nil;
    _alertLabel = nil;
    _emptyTexture = nil;
    _loadingView = nil;
    _overview = nil;
    _helpButton = nil;
    temptextureFile = nil;
}
@end
