//
//  RenderingViewController.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 15/02/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.

#import "RenderingViewController.h"
#import "AVFoundation/AVAssetWriterInput.h"
#import "AVFoundation/AVAssetWriter.h"
#import "AVFoundation/AVFoundation.h"
#import "QuartzCore/QuartzCore.h"
#import "Utility.h"
#import "ZipArchive.h"
#import "UploadController.h"
#import "Utils.h"
#import "ShaderCell.h"
#import "VideoUploadViewController.h"
#import <ImageIO/ImageIO.h>
#import <MobileCoreServices/MobileCoreServices.h>
#import "AFHTTPRequestOperation.h"
#import "AFHTTPClient.h"
#if !(TARGET_IPHONE_SIMULATOR)
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#endif

#define RENDER_IMAGE 0
#define RENDER_VIDEO 1
#define RENDER_GIF 2
#define FULL_HD_GIF_RANGE 100
#define HD_GIF_RANGE 200
#define DVD_GIF_RANGE 300
#define FULL_HD 0
#define HD 1
#define DVD 2

#define SHADER_DEFAULT 0
#define SHADER_TOON 6
#define SHADER_CLOUD 12

enum constants {
    UPLOAD_ALERT_VIEW = 3,
    YOUTUBE_BUTTON_TAG = 2,
    LOGOUT_BUTTON_INDEX = 1,
    UPLOAD_BUTTON_INDEX = 2,
    CANCEL_BUTTON_INDEX = 0,
    OK_BUTTON_INDEX = 1
};
@implementation RenderingViewController

@synthesize youtubeService;
@synthesize videoFilePath;

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil StartFrame:(int)startFrame EndFrame:(int)endFrame renderOutput:(int)exportType caMresolution:(int)resolution
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cache = [CacheSystem cacheSystem];
        renderingStartFrame = startFrame;
        renderingEndFrame = endFrame;
        renderingFrame = renderingStartFrame;
        isCanceled = false;
        renderingExportImage = exportType;
        resolutionType = resolution;
        shaderType = SHADER_DEFAULT;
    }
    return self;
}
- (void) viewDidLoad
{
    [super viewDidLoad];
    
    cancelPressed = NO;
    selectedIndex = 0;
    isAppInBg = false;
    self.resolutionSegment.selectedSegmentIndex = resolutionType;
    self.renderingTypes.layer.cornerRadius = 5.0;
    self.renderingTypes.layer.borderWidth = 2.0f;
    self.renderingTypes.layer.borderColor=[UIColor clearColor].CGColor;
    self.nextButton.layer.cornerRadius = 5.0;
    self.cancelButton.layer.cornerRadius = 5.0;
    [self.activityIndicatorView setHidden:false];
    [self.shareActivityIndicator setHidden:true];
    [self.cancelActivityIndicator setHidden:true];
    [self.watermarkSwitch setOn:YES];
    [self beginViewHideAndShow:true];
    shaderArray = [[NSMutableArray alloc] init];
    shaderTypesDict = [[NSMutableDictionary alloc] init];
    [shaderArray addObject:[NSNumber numberWithInt:SHADER_DEFAULT]];
    [shaderArray addObject:[NSNumber numberWithInt:SHADER_TOON ]];
    [shaderArray addObject:[NSNumber numberWithInt:SHADER_CLOUD ]];
    //[shaderArray addObject:[NSNumber numberWithInt:SHADER_PHOTO ]];
    [shaderTypesDict setObject:@"Normal Shader" forKey:[NSNumber numberWithInt:SHADER_DEFAULT]];
    [shaderTypesDict setObject:@"Toon Shader" forKey:[NSNumber numberWithInt:SHADER_TOON ]];
    [shaderTypesDict setObject:@"Cloud Rendering" forKey:[NSNumber numberWithInt:SHADER_CLOUD]];
    //[shaderTypesDict setObject:@"Photo Realistic" forKey:[NSNumber numberWithInt:SHADER_PHOTO ]];
    if([Utility IsPadDevice])
        [self.renderingTypes registerNib:[UINib nibWithNibName:@"ShaderCell" bundle:nil] forCellWithReuseIdentifier:@"TYPE"];
    else
        [self.renderingTypes registerNib:[UINib nibWithNibName:@"ShaderCellPhone" bundle:nil] forCellWithReuseIdentifier:@"TYPE"];

    if(renderingExportImage == RENDER_IMAGE)
    {
        self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d",renderingStartFrame + 1];
        [self.youtubeButton setHidden:true];
        [self.rateButtonImage setEnabled:YES];
        [self.rateButtonText setEnabled:YES];
    }
    else if(renderingExportImage == RENDER_VIDEO)
    {
        self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d/%d",1,(int)(_trimControl.rightValue-_trimControl.leftValue)];
        [self.rateButtonImage setEnabled:NO];
        [self.rateButtonText setEnabled:NO];
    }
    else if(renderingExportImage == RENDER_GIF)
    {
        
        self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d/%d",1,(int)(_trimControl.rightValue-_trimControl.leftValue)];
        [self.youtubeButton setHidden:true];
        [self.rateButtonImage setEnabled:NO];
        [self.rateButtonText setEnabled:NO];
    }
    self.renderingProgressBar.progress = ((float)(renderingFrame - renderingStartFrame))/(1 + renderingEndFrame - renderingStartFrame);
    [self.makeVideoLoading setHidden:YES];
    [self.exportButton setHidden:true];
    [self.youtubeButton setHidden:true];
    if(renderingExportImage != RENDER_IMAGE){
        if([Utility IsPadDevice]){
            _trimControl = [[RETrimControl alloc] initWithFrame:CGRectMake(35,515, 470, 28)];
            _trimControl.length = renderingEndFrame; // 200 seconds
            _trimControl.delegate = self;
            [self.view addSubview:_trimControl];
        }
        else if(iOSVersion >= 8.0 && SCREENWIDTH == 667){
            
            _trimControl = [[RETrimControl alloc] initWithFrame:CGRectMake(210,335, 240, 28)];
            _trimControl.length = renderingEndFrame; // 200 seconds
            _trimControl.delegate = self;
            [self.view addSubview:_trimControl];
        }
        else if(iOSVersion >= 8.0 && SCREENWIDTH == 736){
            _trimControl = [[RETrimControl alloc] initWithFrame:CGRectMake(110,358, 190, 28)];
            _trimControl.length = renderingEndFrame; // 200 seconds
            _trimControl.delegate = self;
            [self.view addSubview:_trimControl];
            
        }
        else{
            _trimControl = [[RETrimControl alloc] initWithFrame:CGRectMake(165,285, 190, 28)];
            _trimControl.length = renderingEndFrame; // 200 seconds
            _trimControl.delegate = self;
            [self.view addSubview:_trimControl];
        }
        
        
    }
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{
    
}

- (UIEdgeInsets)collectionView:(UICollectionView*)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section {
    if([Utility IsPadDevice])
        return UIEdgeInsetsMake(20, 20, 20, 20);
    else {
        if(iOSVersion >= 8.0 && SCREENWIDTH == 667)
        {
            return UIEdgeInsetsMake(12, 170, 25, 70);
        }
        else if(iOSVersion >= 8.0 && SCREENWIDTH <= 640)
        {
            return UIEdgeInsetsMake(12, 90, 25, 75);
        }
        else
            return UIEdgeInsetsMake(20, 50, 25, 75);
    }

     // top, left, bottom, right
}

//- (CGFloat)collectionView:(UICollectionView *)collectionView layout:(UICollectionViewLayout*)collectionViewLayout minimumInteritemSpacingForSectionAtIndex:(NSInteger)section {
//    if([Utility IsPadDevice])
//        return 10.0;
//    else {
//        if(iOSVersion >= 8.0 && SCREENWIDTH > 667)
//            return 80.0;
//        else
//            return 80.0;
//    }
//}

-(void) beginViewHideAndShow :(BOOL)isHidden
{
    [self.renderingProgressLabel setHidden:isHidden];
    [self.renderingProgressBar setHidden:isHidden];
    if(isHidden == false){
        isHidden = true;
        [self.renderingStyleLabel setHidden:isHidden];
        [self.resolutionSegment setHidden:isHidden];
        [self.resolutionType1 setHidden:isHidden];
        [self.resolutionType2 setHidden:isHidden];
        [self.resolutionType3 setHidden:isHidden];
        [self.resolutionTypeLabel setHidden:isHidden];
        [self.startButtonText setHidden:isHidden];
        [self.waterMarkLabel setHidden:isHidden];
        [self.watermarkSwitch setHidden:isHidden];
        [self.shaderStyle setHidden:isHidden];
        [self.renderDesc setHidden:isHidden];
        [self.backgroundColorLable setHidden:isHidden];
        [self.colorPickerBtn setHidden:isHidden];
        [self.transparentLable setHidden:isHidden];
        [self.transparentBackgroundbtn setHidden:isHidden];
    }
}

-(void) appEntersBG{
    isAppInBg = true;
}

-(void) appEntersFG{
    isAppInBg = false;
}

- (void) uploadFilesToRender
{
    NSLog(@"Written");
}

- (IBAction)startButtonAction:(id)sender
{
    if([shaderArray[tempSelectedIndex] intValue] == SHADER_CLOUD){
        [self.delegate saveScene];
        [self shaderPhotoAction];
    }
    else{
        if(renderingExportImage != RENDER_IMAGE) {
            renderingFrame = _trimControl.leftValue;
            [self.trimControl setHidden:YES];
        }
        
        if(renderingExportImage == RENDER_GIF){
            if (resolutionType == FULL_HD) {
                if (renderingEndFrame <= FULL_HD_GIF_RANGE)
                    [self renderBeginFunction];
                else
                {
                    UIAlertView *closeAlert = [[UIAlertView alloc]initWithTitle:@"Warning" message:@"Creating a GIF at Full HD resolution supports only upto 100 frames." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
                    [closeAlert show];
                }
            }else if (resolutionType == HD){
                if (renderingEndFrame <= HD_GIF_RANGE)
                    [self renderBeginFunction];
                else
                {
                    UIAlertView *closeAlert = [[UIAlertView alloc]initWithTitle:@"Warning" message:@"Creating a GIF at HD resolution supports only upto 200 frames." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
                    [closeAlert show];
                }
            }else if (resolutionType == DVD){
                if (renderingEndFrame <= DVD_GIF_RANGE)
                    [self renderBeginFunction];
                else
                {
                    UIAlertView *closeAlert = [[UIAlertView alloc]initWithTitle:@"Warning" message:@"Creating a GIF at DVD resolution supports only upto 300 frames." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
                    [closeAlert show];
                }
            }
        }else{
            [self renderBeginFunction];
        }
    }
}

-(NSMutableArray*) getFileteredFilePathsFrom:(NSMutableArray*) filePaths
{
    NSMutableArray * filtFilePaths = [[NSMutableArray alloc] init];
    NSString *docDirPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *cacheDirPath = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *objDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Objs"];
    NSString *rigDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Rigs"];
    
    NSFileManager * fm = [NSFileManager defaultManager];
    
    for(int i = 0; i < [filePaths count]; i++) {
        if([[filePaths objectAtIndex:i] isEqualToString:@""])
            continue;
        NSString *filePath1 = [NSString stringWithFormat:@"%@/%@",cacheDirPath,[filePaths objectAtIndex:i]];
        NSString *filePath2 = [NSString stringWithFormat:@"%@/%@",objDirPath,[filePaths objectAtIndex:i]];
        NSString *filePath3 = [NSString stringWithFormat:@"%@/%@",rigDirPath,[filePaths objectAtIndex:i]];
        
        if([fm fileExistsAtPath:filePath1]) {
            if(![filtFilePaths containsObject:filePath1])
                [filtFilePaths addObject:filePath1];
        } else if([fm fileExistsAtPath:filePath2]){
            if(![filtFilePaths containsObject:filePath2])
                [filtFilePaths addObject:filePath2];
        } else if([fm fileExistsAtPath:filePath3]){
            if(![filtFilePaths containsObject:filePath3])
                [filtFilePaths addObject:filePath3];
        }
    }    
    return filtFilePaths;
}


-(void) shaderPhotoAction{
    
    NSFileManager  *manager = [NSFileManager defaultManager];
    NSArray *docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSMutableString *docDirectory = [docPaths objectAtIndex:0];
    
    if([[AppHelper getAppHelper] checkInternetConnected]){
        [self.nextButton setHidden:YES];
        [self.makeVideoLoading setHidden:NO];
        [self.makeVideoLoading startAnimating];
        NSMutableArray *fileNames = [self.delegate exportSGFDsWith:(int)_trimControl.leftValue EndFrame:(int)_trimControl.rightValue];
        CGPoint camResolution = [self.delegate getCameraResolution];
        NSLog(@"\n width %f height %f ",camResolution.x , camResolution.y);
        NSString* zipfile = [docDirectory stringByAppendingPathComponent:@"test2.zip"] ;
        NSArray *dirFiles1 = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirectory error:nil];
        sgfdFiles = [dirFiles1 filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.sgfd'"]];
        
        NSMutableArray *textureFiles = [self getFileteredFilePathsFrom:fileNames];
        ZipArchive* zip = [[ZipArchive alloc] init];
        BOOL ret = [zip CreateZipFile2:zipfile];
        
        for (int i = 0; i < [sgfdFiles count]; i++) {
            NSString* filesZip = [docDirectory stringByAppendingPathComponent:sgfdFiles[i]] ;
            ret = [zip addFileToZip:filesZip newname:sgfdFiles[i]];//zip
            NSLog(@" SGFD File %@ " , sgfdFiles[i]);
        }
        
        for(int i = 0; i < [textureFiles count]; i++) {
            ret = [zip addFileToZip:[textureFiles objectAtIndex:i] newname:[[textureFiles objectAtIndex:i] lastPathComponent]];
        }
        
        if( ![zip CloseZipFile2] )
        {
            zipfile = @"";
        }
        NSLog(@"The file has been zipped");
        
        
        NSString *userId= @"sankarsmackall";
        NSString *totFrames = [NSString stringWithFormat:@"%d",((int)_trimControl.rightValue-(int)_trimControl.leftValue)+1];
        NSString *resolutionWidth = [NSString stringWithFormat:@"%f",camResolution.x];
        NSString *resolutionHeight = [NSString stringWithFormat:@"%f",camResolution.y];
        
        NSLog(@"Document Directory %@",docDirectory);
        NSString *fileUrl= [docDirectory stringByAppendingString:@"/test2.zip"];
        NSLog(@"File Url: %@",fileUrl);
        NSURL *url = [NSURL URLWithString:@"https://www.iyan3dapp.com/appapi/rendertask.php"];
        NSString *postPath = @"https://www.iyan3dapp.com/appapi/rendertask.php";
        NSData *dataZip = [NSData dataWithContentsOfFile:fileUrl];
        //NSLog(@"%@ Data Items:", dataZip);
        if(dataZip==NULL){
            UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:@"Failure" message:@"The Requested Data doest not exist!!" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
            [userNameAlert show];
            [self.nextButton setHidden:NO];
            [self.makeVideoLoading setHidden:YES];
        }
        else{
            AFHTTPClient *httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
            NSMutableURLRequest *request = [httpClient multipartFormRequestWithMethod:@"POST" path:postPath parameters:nil constructingBodyWithBlock:^(id <AFMultipartFormData>formData) {
                
                [formData appendPartWithFileData:dataZip  name:@"renderFile" fileName:[NSString stringWithFormat:@"test.zip"] mimeType:@"image/png"];
                
                [formData appendPartWithFormData:[userId dataUsingEncoding:NSUTF8StringEncoding] name:@"userid"];
                [formData appendPartWithFormData:[totFrames dataUsingEncoding:NSUTF8StringEncoding] name:@"totalFrames"];
                [formData appendPartWithFormData:[resolutionWidth dataUsingEncoding:NSUTF8StringEncoding] name:@"width"];
                [formData appendPartWithFormData:[resolutionHeight dataUsingEncoding:NSUTF8StringEncoding] name:@"height"];
            }];
            NSLog(@"Request initiated");
            
            AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
            
            [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
                NSLog(@"Request Successfull");
                publishId=[[operation responseString] intValue];
                NSLog(@"publish Id %d",publishId);
                
                NSError *error = nil;
                float estimatedTime=15.40;
                for (NSString *sqliteFile in sgfdFiles)
                {
                    [manager removeItemAtPath:[docDirectory stringByAppendingPathComponent:sqliteFile] error:&error];
                }
                [manager removeItemAtPath:[docDirectory stringByAppendingPathComponent:@"test.zip"] error:&error];
                
                [self.makeVideoLoading setHidden:YES];
                [cache addRenderTaskData:publishId estTime:estimatedTime proName:self.projectName];
                
            }
             
                                             failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                                 NSLog(@"Failure: %@", error);
                                                 [self.view setUserInteractionEnabled:YES];
                                                 [self.makeVideoLoading setHidden:YES];
                                                 UIAlertView *userNameAlert = [[UIAlertView alloc]initWithTitle:@"Failure Error" message:@"Check your net connection it was slow. So animation cannot be uploaded." delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
                                                 [userNameAlert show];
                                                 
                                             }];
            [operation start];
        }
    }else{
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Network Error" message:@"No Network Available" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil , nil];
        [alertView show];
    }
}
- (void) viewWillAppear:(BOOL)animated
{
    if([Utility IsPadDevice]) {
        [self.view.layer setMasksToBounds:YES];
    }
}
- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [self removeSGFDFilesIfAny];
    self.screenName = @"RenderingView";
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersBG) name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersFG) name:@"applicationDidBecomeActive" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(uploadFilesToRender) name:@"FileWriteCompleted" object:nil];

    isAppInBg = false;
}
- (void) viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"applicationDidBecomeActive" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"FileWriteCompleted" object:nil];
}

- (void) removeSGFDFilesIfAny
{
    NSFileManager  *manager = [NSFileManager defaultManager];
    NSArray *docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSMutableString *docDirectory = [docPaths objectAtIndex:0];
    NSArray *dirFiles1 = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirectory error:nil];
    NSArray *filesToDelete = [dirFiles1 filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.sgfd'"]];
    for(int i = 0; i < [filesToDelete count]; i++)
        [manager removeItemAtPath:[docDirectory stringByAppendingPathComponent:filesToDelete[i]] error:nil];
}

- (void) renderingProgress
{
    finalFrame=(int)_trimControl.rightValue - (int)_trimControl.leftValue;
    
    int loopEnd = (renderingExportImage == RENDER_IMAGE) ? renderingStartFrame :(int)_trimControl.rightValue;
    
    if(isAppInBg)
        return;
    while (renderingFrame <= loopEnd && !isCanceled) {
        if(!isAppInBg){
            
            dispatch_block_t work_to_do = ^{
                [self doRenderingOnMainThread];
            };
            
            if ([NSThread isMainThread])
                work_to_do();
            else
                dispatch_sync(dispatch_get_main_queue(), work_to_do);

            if(!isAppInBg){
                if (renderingExportImage == RENDER_IMAGE || renderingExportImage == RENDER_VIDEO) {
                    renderingFrame++;
                }
                else{
                    renderingFrame = renderingFrame+3;
                }
            }
        }
    }
    [self.delegate setShaderTypeForRendering:SHADER_DEFAULT];
    if(isCanceled)
        return;
    if(renderingExportImage == RENDER_VIDEO) {
        [self performSelectorOnMainThread:@selector(doMakeVideoUIUpdate) withObject:nil waitUntilDone:YES];
        
        [self MakeVideo];
        [self.makeVideoLoading stopAnimating];
        [self.makeVideoLoading setHidden:YES];
        [self.youtubeButton setHidden:false];
        [self.youtubeButton setEnabled:true];
        [self.rateButtonImage setEnabled:YES];
        [self.rateButtonText setEnabled:YES];
    }
    if(renderingExportImage == RENDER_GIF) {
        [self performSelectorOnMainThread:@selector(doMakeGifUIUpdate) withObject:nil waitUntilDone:YES];
        [self makingGif];
        [self.makeVideoLoading stopAnimating];
        [self.makeVideoLoading setHidden:YES];
        [self.youtubeButton setHidden:true];
        [self.rateButtonImage setEnabled:YES];
        [self.rateButtonText setEnabled:YES];
    }
    [self.exportButton setHidden:false];
    [self.exportButton setEnabled:true];
    [self.renderingProgressLabel setHidden:YES];
    [self.renderingProgressBar setHidden:YES];
}
- (IBAction)youtubeButtonAction:(id)sender
{
    if(sender != nil) {
    }
    [self.view endEditing:YES];
    uploadSceneAlert = [[UIAlertView alloc]initWithTitle:@"Upload Video" message:@"Do you want to Upload the video to your Youtube?" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Ok", nil];
    [uploadSceneAlert setTag:YOUTUBE_BUTTON_TAG];
    [uploadSceneAlert show];
    
}
- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return [shaderArray count];
}

- (ShaderCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    ShaderCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"TYPE" forIndexPath:indexPath];
    cell.ShaderCellName.text = [NSString stringWithFormat:@"%@",[shaderTypesDict objectForKey:shaderArray[indexPath.row]]];
    cell.layer.borderWidth = 1.0f;
    cell.layer.borderColor = [UIColor clearColor].CGColor;
    if ((int)indexPath.row == 0){
        cell.ShaderImage.image =[UIImage imageNamed:@"Normal-shader_Pad.png"];
    }
    else if ((int)indexPath.row == 2){
        cell.ShaderImage.image =[UIImage imageNamed:@"Normal-Cloud-shader_Pad.png"];
    }
    else if ((int)indexPath.row == 1){
        cell.ShaderImage.image =[UIImage imageNamed:@"Toon-shader_Pad.png"];
    }
    
    if(selectedIndex == (int)indexPath.row){
        UIColor *borderColor = [UIColor purpleColor];
        [cell.ShaderImage.layer setBorderColor:borderColor.CGColor];
        [cell.ShaderImage.layer setBorderWidth:3.0];
    }
    else{
        UIColor *borderColor = [UIColor clearColor];
        [cell.ShaderImage.layer setBorderColor:borderColor.CGColor];
        [cell.ShaderImage.layer setBorderWidth:2.0];
    }
    return cell;
}
- (void) collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    if([shaderArray[indexPath.row] intValue] == SHADER_TOON){
        NSLog(@" Selcted index: %d",(int)indexPath.row);
        tempSelectedIndex = (int)indexPath.row;
        [self.nextButton setTitle:@"Next" forState:UIControlStateNormal];
        [self.renderDesc setText:[NSString stringWithFormat:@"Render Toon Shader in your device!!!"]];
        selectedIndex = (int)indexPath.row;
        [self.renderingTypes reloadData];
    }
    else if([shaderArray[indexPath.row] intValue] == SHADER_DEFAULT){
        shaderType = [shaderArray[indexPath.row] intValue];
        NSLog(@" Selcted index: %d",(int)indexPath.row);
        [self.nextButton setTitle:@"Next" forState:UIControlStateNormal];
        [self.renderDesc setText:[NSString stringWithFormat:@"Render Normal Shader in your device!!!"]];
        selectedIndex = (int)indexPath.row;
        [self.renderingTypes reloadData];
    }
    else if ([shaderArray[indexPath.row] intValue] == SHADER_CLOUD){
        tempSelectedIndex = (int)indexPath.row;
        NSLog(@" Selcted index: %d",(int)indexPath.row);
        [self.nextButton setTitle:@"Publish" forState:UIControlStateNormal];
        [self showUpgradeView:(int)indexPath.row];
        
    }
}
-(void)showUpgradeView:(int)selectedRowIndex
{
    if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]){
        if([Utility IsPadDevice]) {
            upgradeView = [[PremiumUpgardeVCViewController alloc] initWithNibName:@"PremiumUpgardeVCViewController" bundle:nil];
            upgradeView.delegate = self;
            upgradeView.modalPresentationStyle = UIModalPresentationFormSheet;
            [self presentViewController:upgradeView animated:YES completion:nil];
            upgradeView.view.superview.backgroundColor = [UIColor clearColor];
            upgradeView.view.layer.borderWidth = 2.0f;
            upgradeView.view.layer.borderColor = [UIColor grayColor].CGColor;
        }else{
            upgradeView = [[PremiumUpgardeVCViewController alloc] initWithNibName:@"PremiumUpgradeViewControllerPhone" bundle:nil];
            upgradeView.delegate = self;
            upgradeView.modalPresentationStyle = UIModalPresentationFormSheet;
            [self presentViewController:upgradeView animated:YES completion:nil];
        }
    }
    else {
        if(selectedRowIndex != -1) {
        shaderType = [shaderArray[selectedRowIndex] intValue];
        selectedIndex = selectedRowIndex;
        [self.renderingTypes reloadData];
        } else {
            [self.watermarkSwitch setOn:NO];
        }
    }
}

- (void) alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    self.youtubeService = [[GTLServiceYouTube alloc] init];
    self.youtubeService.authorizer =
    [GTMOAuth2ViewControllerTouch authForGoogleFromKeychainForName:kKeychainItemName
                                                          clientID:kClientID
                                                      clientSecret:kClientSecret];
    
    self.uploadVideo = [[YouTubeUploadVideo alloc] init];
    self.uploadVideo.delegate = self;
    
    
    switch(alertView.tag)
    {
        case YOUTUBE_BUTTON_TAG:
        {
            if (buttonIndex == OK_BUTTON_INDEX) {
                if([[AppHelper getAppHelper] userDefaultsForKey:@"YouTube_Login"]) {
                    [self.view endEditing:YES];
                    logoutAlert = [[UIAlertView alloc]initWithTitle:@"YouTube" message:[NSString stringWithFormat:@"Logged in as %@",[[AppHelper getAppHelper]userDefaultsForKey:@"YouTube_Login"]] delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Logout",@"OK", nil];
                    [logoutAlert setTag:UPLOAD_ALERT_VIEW];
                    [logoutAlert show];
                }
                else {
                    [self createAuthController];
                }
            }
        }
        case UPLOAD_ALERT_VIEW:
        {
            if(buttonIndex == UPLOAD_BUTTON_INDEX) {
                NSData *fileData = [NSData dataWithContentsOfFile:self.videoFilePath];
                NSString *title = @"Iyan3D";
                NSString *description = @"I made this animation video using Iyan3D app, It's amazing";
                
                if ([title isEqualToString:@""]) {
                    NSDateFormatter *dateFormat = [[NSDateFormatter alloc] init];
                    [dateFormat setDateFormat:@"'Direct Lite Uploaded File ('EEEE MMMM d, YYYY h:mm a, zzz')"];
                    title = [dateFormat stringFromDate:[NSDate date]];
                }
                if ([description isEqualToString:@""]) {
                    description = @"Uploaded from YouTube Direct Lite iOS";
                }
                
                [self.uploadVideo uploadYouTubeVideoWithService:self.youtubeService fileData:fileData title:title description:description];
                
            } else if (buttonIndex == LOGOUT_BUTTON_INDEX) {
                [[AppHelper getAppHelper] removeFromUserDefaultsWithKey:@"YouTube_Login"];
                [self alertView:uploadSceneAlert clickedButtonAtIndex:CANCEL_BUTTON_INDEX];
            }
        }
    }
}
- (BOOL)isAuthorized {
    if([[AppHelper getAppHelper] userDefaultsForKey:@"YouTube_Login"]) {
        return [((GTMOAuth2Authentication *)self.youtubeService.authorizer) canAuthorize];
    }
    else
        return NO;
}
- (IBAction)cameraResolutionChanged:(id)sender {
    resolutionType = (int)self.resolutionSegment.selectedSegmentIndex;
    [self.delegate cameraResolutionChanged:resolutionType];
}

- (IBAction)waterMarkValueChanged:(id)sender {
    if(!self.watermarkSwitch.isOn) {
        [self.watermarkSwitch setOn:YES];
        [self showUpgradeView:-1];
    }
}

- (IBAction) cancelButtonAction:(id)sender
{
    cancelPressed = YES;
    [self.cancelActivityIndicator setHidden:false];
    [self.cancelActivityIndicator startAnimating];
    [thread cancel];
    isCanceled = true;
    [self.delegate clearFolder:NSTemporaryDirectory()];
    [self dismissViewControllerAnimated:YES completion:nil];
    [self.delegate resumeRenderingAnimationScene];
}
- (void)uploadYouTubeVideo:(YouTubeUploadVideo *)uploadVideo
      didFinishWithResults:(GTLYouTubeVideo *)video {
    
    
    NSString *postString = [NSString stringWithFormat:@"id=%@",video.identifier];
    NSData *postData = [postString dataUsingEncoding:NSASCIIStringEncoding allowLossyConversion:YES];
    NSString *postLength = [NSString stringWithFormat:@"%lu",(unsigned long)[postData length]];
    NSMutableURLRequest *request = [[NSMutableURLRequest alloc] init];
    [request setURL:[NSURL URLWithString:[NSString stringWithFormat:@"https://www.smackall.com/app/anim3/youtube.php"]]];
    [request setHTTPMethod:@"POST"];
    [request setValue:postLength forHTTPHeaderField:@"Content-Length"];
    [request setValue:@"application/x-www-form-urlencoded" forHTTPHeaderField:@"Current-Type"];
    [request setHTTPBody:postData];
    NSURLConnection *conn = [[NSURLConnection alloc]initWithRequest:request delegate:self];
    [conn start];
    if(conn)
    {
    
    }
    else
    {
        NSLog(@"Connection failed");
    }
    
}

-(void)connection:(NSURLConnection *)connection didReceiveData:(NSData*)data
{

}

-(void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    NSLog(@"Faliled with error %@",error.description);
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{

}

- (void)createAuthController
{
    _authController = [[GTMOAuth2ViewControllerTouch alloc] initWithScope:kGTLAuthScopeYouTube
                                                                 clientID:kClientID
                                                             clientSecret:kClientSecret
                                                         keychainItemName:kKeychainItemName
                                                                 delegate:self
                                                         finishedSelector:@selector(viewController:finishedWithAuth:error:)];
    
    // Optional: display some html briefly before the sign-in page loads
    NSString *html = @"<html><body bgcolor=silver><div align=center>Loading sign-in page...</div></body></html>";
    _authController.initialHTMLString = html;
    
    [self presentModalViewController:_authController animated:YES];
    
    //return authController;
}
- (void)viewController:(GTMOAuth2ViewControllerTouch *)viewController
      finishedWithAuth:(GTMOAuth2Authentication *)authResult
                 error:(NSError *)error {
    if (error != nil) {
        [Utils showAlert:@"Authentication Error" message:error.localizedDescription];
        self.youtubeService.authorizer = nil;
    } else {
        self.youtubeService.authorizer = authResult;
        [self uploadToYouTube:[authResult userEmail]];
        [_authController dismissViewControllerAnimated:YES completion:Nil];
    }
}
-(void) uploadToYouTube:(NSString*)userEmail
{
    [[AppHelper getAppHelper] saveToUserDefaults:userEmail withKey:@"YouTube_Login"];
    [self alertView:logoutAlert clickedButtonAtIndex:UPLOAD_BUTTON_INDEX];
}
- (void) renderBeginFunction
{
    [self beginViewHideAndShow:false];
    [self.resolutionSegment setEnabled:false];
    [self.startButtonText setEnabled:false];
    resolutionType = (int)self.resolutionSegment.selectedSegmentIndex;
    if(resolutionType == 0) {
        cameraResolutionWidth = 1920.0f;
        cameraResolutionHeight = 1080.0f;
    } else if(resolutionType == 1) {
        cameraResolutionWidth = 1280.0f;
        cameraResolutionHeight = 720.0f;
    } else {
        cameraResolutionWidth = 720.0f;
        cameraResolutionHeight = 480.0f;
    }
    [[AppHelper getAppHelper] saveToUserDefaults:[NSString stringWithFormat:@"%d",resolutionType] withKey:@"cameraResolution"];
    [UIApplication sharedApplication].idleTimerDisabled = YES;
    
    self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d/%d",(int)_trimControl.leftValue,(int)_trimControl.rightValue];
    thread = [[NSThread alloc] initWithTarget:self selector:@selector(renderingProgress) object:nil];
    [thread start];
}
- (void) doRenderingOnMainThread
{
    if(isAppInBg)
        return;
    
    if (!cancelPressed) {
        
        [self.activityIndicatorView setHidden:false];
        [self.activityIndicatorView startAnimating];
        if(renderingExportImage == RENDER_IMAGE){
            self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d",renderingStartFrame + 1];
        }
        else if(renderingExportImage == RENDER_VIDEO) {
            NSLog(@"progress label %d / %d ", 1 + (renderingFrame - (int)_trimControl.leftValue),(int)(_trimControl.rightValue - _trimControl.leftValue)+1);
            self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d/%d", renderingFrame ,(int)_trimControl.rightValue];
        }
        else if(renderingExportImage == RENDER_GIF) {
            self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d/%d",  renderingFrame,(int)_trimControl.rightValue];
        }
        self.renderingProgressBar.progress = ((float)(renderingFrame - (int)_trimControl.leftValue))/(((int)_trimControl.rightValue - (int)_trimControl.leftValue));
        
        
        if(selectedIndex < [shaderArray count])
            shaderType = [shaderArray[selectedIndex] intValue];
        
        [self.delegate renderFrame:renderingFrame withType:shaderType andRemoveWatermark:(!self.watermarkSwitch.isOn)];
        
        NSString *tempDir = NSTemporaryDirectory();
        NSString *imageFilePath = [NSString stringWithFormat:@"%@/r-%d.png", tempDir, renderingFrame];
        [self.renderedImageView setImage:[UIImage imageWithContentsOfFile:imageFilePath]];
        
        [self.activityIndicatorView stopAnimating];
        [self.activityIndicatorView setHidden:true];
    }
}
- (void) MakeVideo
{
    imagesArray = [[NSMutableArray alloc] init];
    NSString *tempDir = NSTemporaryDirectory();
    
    for (int i = _trimControl.leftValue ; i <= _trimControl.rightValue ; i++) {
        NSString* file = [tempDir stringByAppendingPathComponent:[NSString stringWithFormat:@"r-%d.png", i]];
        NSLog(@"Rendering Frame: %d",i);
        [imagesArray addObject:file];
    }
    
    NSString *fileName = @"myMovie.mov";
    self.videoFilePath = [tempDir stringByAppendingPathComponent:fileName];
    
    NSError *error = nil;
    NSFileManager *fileMgr = [[NSFileManager alloc] init];
    [fileMgr removeItemAtPath:self.videoFilePath error:&error];
    
    AVAssetWriter *videoWriter = [[AVAssetWriter alloc] initWithURL:[NSURL fileURLWithPath:self.videoFilePath] fileType:AVFileTypeQuickTimeMovie error:&error];
    NSDictionary *videoSettings = [NSDictionary dictionaryWithObjectsAndKeys:AVVideoCodecH264, AVVideoCodecKey, [NSNumber numberWithInt:cameraResolutionWidth], AVVideoWidthKey, [NSNumber numberWithInt:cameraResolutionHeight], AVVideoHeightKey, nil];
    
    AVAssetWriterInput* videoWriterInput = [AVAssetWriterInput
                                            assetWriterInputWithMediaType:AVMediaTypeVideo
                                            outputSettings:videoSettings];
    
    AVAssetWriterInputPixelBufferAdaptor *adaptor = [AVAssetWriterInputPixelBufferAdaptor
                                                     assetWriterInputPixelBufferAdaptorWithAssetWriterInput:videoWriterInput
                                                     sourcePixelBufferAttributes:nil];
    
    videoWriterInput.expectsMediaDataInRealTime = YES;
    [videoWriter addInput:videoWriterInput];
    
    [videoWriter startWriting];
    [videoWriter startSessionAtSourceTime:kCMTimeZero];
    
    CVPixelBufferRef buffer = NULL;
    
    for(int i = 0; i < [imagesArray count]; i++) {
        @autoreleasepool {
            UIImage *img = [UIImage imageWithContentsOfFile:[imagesArray objectAtIndex:i]];
            buffer = pixelBufferFromCGImage([img CGImage],CGSizeMake(cameraResolutionWidth,cameraResolutionHeight));
            
            BOOL append_ok = NO;
            int j = 0;
            while (!append_ok && j < 30) {
                if (adaptor.assetWriterInput.readyForMoreMediaData)  {
                    CMTime frameTime = CMTimeMake(i, (int32_t)24);
                    append_ok = [adaptor appendPixelBuffer:buffer withPresentationTime:frameTime];
                    if(!append_ok){
                        NSError *error = videoWriter.error;
                        if(error!=nil) {
                        }
                    }
                    else
                        CVPixelBufferRelease(buffer);
                } else {
                    [NSThread sleepForTimeInterval:0.1];
                }
                j++;
            }
            if (!append_ok) {
                //printf("error appending image %d times %d\n, with error.", i, j);
            }
        }
    }
    //Finish the session:
    [videoWriterInput markAsFinished];
    [videoWriter finishWriting];
}
CVPixelBufferRef pixelBufferFromCGImage(CGImageRef image, CGSize imageSize)
{
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:
                             [NSNumber numberWithBool:YES],   kCVPixelBufferCGImageCompatibilityKey,
                             [NSNumber numberWithBool:YES], kCVPixelBufferCGBitmapContextCompatibilityKey,
                             nil];
    CVPixelBufferRef pxbuffer = NULL;
    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault, imageSize.width, imageSize.height, kCVPixelFormatType_32ARGB, (__bridge CFDictionaryRef)options, &pxbuffer);
    if(status != kCVReturnSuccess && pxbuffer == NULL)
        return 0;
    
    CVPixelBufferLockBaseAddress(pxbuffer, 0);
    void *pxdata = CVPixelBufferGetBaseAddress(pxbuffer);
    if(!pxdata)
        return 0;
    
    CGColorSpaceRef rgbColorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(pxdata, imageSize.width, imageSize.height, 8, 4*imageSize.width, rgbColorSpace, (CGBitmapInfo)kCGImageAlphaNoneSkipFirst);
    if(!context)
        return 0;
    CGAffineTransform trans = CGAffineTransformIdentity;
    CGContextConcatCTM(context, trans);
    CGContextDrawImage(context, CGRectMake(0, 0, CGImageGetWidth(image),
                                           CGImageGetHeight(image)), image);
    CGColorSpaceRelease(rgbColorSpace);
    CGContextRelease(context);
    
    CVPixelBufferUnlockBaseAddress(pxbuffer, 0);
    
    return pxbuffer;
}
- (void) doMakeVideoUIUpdate
{
    self.renderingProgressLabel.text = @"MAKING VIDEO";
    [self.makeVideoLoading setHidden:NO];
    [self.makeVideoLoading startAnimating];
}
- (void) doMakeGifUIUpdate
{
    self.renderingProgressLabel.text = @"MAKING GIF";
    [self.makeVideoLoading setHidden:NO];
    [self.makeVideoLoading startAnimating];
}
- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}
- (void) makingGif
{
    imagesArray = [[NSMutableArray alloc] init];
    float fps = 8.0;
    NSString* tempDir = NSTemporaryDirectory();
    for (int i = _trimControl.leftValue; i <= _trimControl.rightValue; i+=3) {
        NSString* file = [tempDir stringByAppendingPathComponent:[NSString stringWithFormat:@"r-%d.png", i]];
        [imagesArray addObject:file];
    }
    NSDictionary *fileProperties = @{(__bridge id)kCGImagePropertyGIFDictionary: @{(__bridge id)kCGImagePropertyGIFLoopCount: @0, // 0 means loop forever
                                                                                   }};
    NSDictionary *frameProperties = @{(__bridge id)kCGImagePropertyGIFDictionary: @{(__bridge id)kCGImagePropertyGIFDelayTime: @(1.0/fps), // a float (not double!) in seconds, rounded to centiseconds in the GIF data
                                                                                    }};
    NSURL *documentsDirectoryURL = [NSURL URLWithString:[NSString stringWithFormat:@"file://%@",tempDir]];
    NSURL *fileURL = [documentsDirectoryURL URLByAppendingPathComponent:@"animated.gif"];
    CGImageDestinationRef destination = CGImageDestinationCreateWithURL((__bridge CFURLRef)fileURL, kUTTypeGIF, [imagesArray count], NULL);
    CGImageDestinationSetProperties(destination, (__bridge CFDictionaryRef)fileProperties);
    for (int i = 0; i < [imagesArray count]; i++) {
        @autoreleasepool {
            NSString* file = [NSString stringWithFormat:@"%@r-%d.png", tempDir, i*3];
            UIImage *shacho = [UIImage imageWithContentsOfFile:file];
            CGImageDestinationAddImage(destination, shacho.CGImage, (CFDictionaryRef)frameProperties);
        }
    }
    if (!CGImageDestinationFinalize(destination)) {
        NSLog(@"failed to finalize image destination");
    }
    CFRelease(destination);
    
}
- (IBAction) exportButtonAction:(id)sender
{
    [self.exportButton setHidden:true];
    [self.shareActivityIndicator setHidden:false];
    [self.shareActivityIndicator startAnimating];
    
    NSArray *objectsToShare;
    NSString *tempDir = NSTemporaryDirectory();
    if(renderingExportImage == RENDER_IMAGE) {
        NSString *filePath = [NSString stringWithFormat:@"%@r-%d.png", tempDir, renderingFrame - 1];
        objectsToShare = [NSArray arrayWithObjects:@"A 3D Scene, I created using Iyan 3D on my iPad", [UIImage imageWithContentsOfFile:filePath], nil];
    } else if(renderingExportImage == RENDER_VIDEO){
        NSString *filePath = [NSString stringWithFormat:@"%@/myMovie.mov", tempDir];
        NSURL *videoPath = [NSURL fileURLWithPath:filePath];
        objectsToShare = [NSArray arrayWithObjects:@"An Animation video, I created using Iyan 3D on my iPad", videoPath, nil];
    } else if(renderingExportImage == RENDER_GIF){
        NSString *filePath = [NSString stringWithFormat:@"%@/animated.gif", tempDir];
        NSURL *videoPath = [NSURL fileURLWithPath:filePath];
        objectsToShare = [NSArray arrayWithObjects:@"An Animated Gif, I created using Iyan 3D on my iPad", videoPath, nil];
    }
    
    UIActivityViewController *controller = [[UIActivityViewController alloc] initWithActivityItems:objectsToShare applicationActivities:nil];
    
    
    if([[UIDevice currentDevice].systemVersion floatValue] >= 8.0) {
        if(!controller.popoverPresentationController.barButtonItem) {
            controller.popoverPresentationController.sourceView = self.view;
            controller.popoverPresentationController.sourceRect = self.exportButton.frame;
        }
    }
    [controller setCompletionHandler:^(NSString *activityType, BOOL completed) {
        [self.shareActivityIndicator stopAnimating];
        [self.shareActivityIndicator setHidden:true];
        [self.exportButton setHidden:false];
        if([Utility IsPadDevice]) {
            //[self.view.layer setCornerRadius:20.0f];
            [self.view.layer setMasksToBounds:YES];
        }
    }];
    
    [self presentViewController:controller animated:YES completion:nil];
}
- (IBAction) rateButtonAction:(id)sender
{
    //    NSString *templateReviewURL = @"itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=640516535";
    
    NSString *templateReviewURLiOS7 = @"https://itunes.apple.com/app/id640516535?mt=8";
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:templateReviewURLiOS7]];
}

-(void) upgradeButtonPressed
{
    //[self.delegate upgradeButtonPressed];
    [[AppHelper getAppHelper] callPaymentGateWayForPremiumUpgrade];
}
-(void)loadingViewStatus:(BOOL)status
{
    
}
-(void)premiumUnlocked
{
    if(tempSelectedIndex == 0 || tempSelectedIndex == -1) {
        if(self.watermarkSwitch.isOn)
            [self.watermarkSwitch setOn:NO];
            [self.renderingTypes reloadData];
    }else{
        selectedIndex = tempSelectedIndex;
        [self.renderingTypes reloadData];
        
        if(selectedIndex < [shaderArray count])
            shaderType = [shaderArray[selectedIndex] intValue];
    }
    
    [[UIApplication sharedApplication] endIgnoringInteractionEvents];
}
-(void)premiumUnlockedCancelled
{
    
}
-(void)statusForOBJImport:(NSNumber*)status
{
    
}
- (void) dealloc
{
    thread = nil;
    self.videoFilePath = nil;
    self.youtubeService = nil;
    self.uploadVideo = nil;
    self.authController = nil;
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

- (IBAction)transparentBgValueChanged:(id)sender {
}
@end
