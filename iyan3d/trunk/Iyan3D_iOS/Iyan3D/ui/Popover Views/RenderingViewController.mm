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


#define THOUSAND_EIGHTY_P 0
#define SEVEN_HUNDRED_TWENTY_P 1
#define FOUR_HUNDRED_EIGHTY_P 2
#define THREE_HUNDRED_SIXTY_P 3
#define TWO_HUNDRED_FOURTY_P 4


#define SHADER_DEFAULT 0
#define SHADER_TOON 6
#define SHADER_CLOUD 12

#define UPLOAD_ALERT_VIEW  3
#define YOUTUBE_BUTTON_TAG  2
#define LOGOUT_BUTTON_INDEX  1
#define UPLOAD_BUTTON_INDEX  2
#define CANCEL_BUTTON_INDEX  0
#define OK_BUTTON_INDEX  1

#define DONE 1
#define START 0

#define SIGNIN_ALERT_VIEW 4
#define CREDITS_VIEW 5



@implementation RenderingViewController

@synthesize youtubeService;
@synthesize videoFilePath;

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil StartFrame:(int)startFrame EndFrame:(int)endFrame renderOutput:(int)exportType caMresolution:(int)resolution ScreenWidth:(int)screenWidth ScreenHeight:(int)screenHeight
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
        bgColor = Vector3(0.1,0.1,0.1);
        ScreenWidth = screenWidth;
        ScreenHeight = screenHeight;
    }
    return self;
}
- (void) viewDidLoad
{
    [super viewDidLoad];
    self.screenName = @"RenderingView iOS";
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(creditsUsed:) name:@"creditsupdate" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(verifiedUsage) name:@"creditsused" object:nil];
    
    cancelPressed = resAlertShown = NO;
    selectedIndex = 0;
    isAppInBg = false;
    self.resolutionSegment.selectedSegmentIndex = resolutionType;
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
    
    [self.delegate freezeEditorRender:YES];
    
    [shaderTypesDict setObject:@"Normal Shader" forKey:[NSNumber numberWithInt:SHADER_DEFAULT]];
    [shaderTypesDict setObject:@"Toon Shader" forKey:[NSNumber numberWithInt:SHADER_TOON ]];
    [shaderTypesDict setObject:@"HQ Rendering" forKey:[NSNumber numberWithInt:SHADER_CLOUD]];
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
    self.renderingProgressBar.progress = ((float)(renderingFrame - renderingStartFrame))/(1 + renderingEndFrame - renderingStartFrame);
    [self.makeVideoLoading setHidden:YES];
    [self.exportButton setHidden:true];
    [self.youtubeButton setHidden:true];
    if(renderingExportImage != RENDER_IMAGE){
        /*
        _trimControl = [[RETrimControl alloc] initWithFrame:CGRectMake(_progressSub.frame.origin.x,_progressSub.frame.origin.y, _progressSub.frame.size.width, _progressSub.frame.size.height)];
        _trimControl.length = renderingEndFrame; // 200 seconds
        _trimControl.delegate = self;
        _trimControl.center = _progressSub.center;
        [self.view addSubview:_trimControl];
        
        
        /*
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
        
        */
    }
    _nextButton.tag = START;
    [_checkCreditProgress setHidden:YES];
    [self updateCreditLable];
}

- (void)trimControl:(RETrimControl *)trimControl didChangeLeftValue:(CGFloat)leftValue rightValue:(CGFloat)rightValue
{
    [self updateCreditLable];
}

- (UIEdgeInsets)collectionView:(UICollectionView*)collectionView layout:(UICollectionViewLayout *)collectionViewLayout insetForSectionAtIndex:(NSInteger)section {
    if([Utility IsPadDevice])
        return UIEdgeInsetsMake(20, 20, 20, 20);
    else {
        if(iOSVersion >= 8.0 && ScreenWidth == 667)
        {
            return UIEdgeInsetsMake(12, 170, 25, 70);
        }
        else if(iOSVersion >= 8.0 && ScreenWidth <= 640)
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
        [self.resolutionType4 setHidden:isHidden];
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
        [self.backgroundColorLable setHidden:isHidden];
        [self.transparentBackgroundbtn setHidden:isHidden];
        [self.colorPickerBtn setHidden:isHidden];
        [self.transparentBgLabel setHidden:isHidden];
        
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
    if(_nextButton.tag == DONE){
        [self cancelButtonAction:nil];
    }
    else{
        if(renderingExportImage != RENDER_IMAGE) {
            renderingFrame = _trimControl.leftValue;
            [self.trimControl setHidden:YES];
        }
            NSString *uniqueId = [[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"];
        if(resolutionType == TWO_HUNDRED_FOURTY_P && _watermarkSwitch.isOn &&    ([shaderArray[selectedIndex] intValue] != SHADER_CLOUD)){
            [self renderBeginFunction:0];
        }
         else if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"] && uniqueId.length > 5) {
             [_checkCreditProgress setHidden:NO];
             [_checkCreditProgress startAnimating];
             [_nextButton setHidden:YES];
             [[AppHelper getAppHelper] getCreditsForUniqueId:uniqueId Name:[[AppHelper getAppHelper] userDefaultsForKey:@"username"] Email:[[AppHelper getAppHelper] userDefaultsForKey:@"email"] SignInType:[[[AppHelper getAppHelper] userDefaultsForKey:@"signintype"] intValue]];
            }
            else{
                UIAlertView *signinAlert = [[UIAlertView alloc]initWithTitle:@"Information" message:@"Please SignIn to continue." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
                [signinAlert show];
                [signinAlert setTag:SIGNIN_ALERT_VIEW];
                [self.trimControl setHidden:NO];
            }
        }
}

- (void) saveDeductedCredits:(int)credits
{
    NSString *videoType = (resolutionType == THOUSAND_EIGHTY_P) ? @"1080P" : (resolutionType == SEVEN_HUNDRED_TWENTY_P) ? @"720P" : (resolutionType == FOUR_HUNDRED_EIGHTY_P) ? @"480P" : (resolutionType == THREE_HUNDRED_SIXTY_P) ? @"360P" : @"240P";
    [[AppHelper getAppHelper] useOrRechargeCredits:[[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"]  credits:credits For:videoType];
}


- (void) creditsUsed:(NSNotification*) notification
{
    NSDictionary* userInfo = notification.userInfo;
    NSLog(@" \n userinfo %@ ", userInfo);
    BOOL network = [userInfo[@"network"]boolValue];
    [_checkCreditProgress stopAnimating];
    [_checkCreditProgress setHidden:YES];
    if(network) {
        [self performSelectorOnMainThread:@selector(verifyCreditsAndRender:) withObject:userInfo waitUntilDone:YES];
    }
    else{
        [self.trimControl setHidden:NO];
        [_nextButton setHidden:NO];
    }
}

- (void) verifiedUsage
{
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
    
    [self.exportButton setHidden:YES];
    [self.exportButton setEnabled:NO];
    [self.renderingProgressLabel setHidden:YES];
    [self.renderingProgressBar setHidden:YES];
    [self performSelectorOnMainThread:@selector(renderFinishAction:) withObject:[NSNumber numberWithInt:renderingExportImage] waitUntilDone:NO];
}

- (void) verifyCreditsAndRender:(NSDictionary*)userInfo
{
    int valueForRender = 0;
    NSNumber* userCredits = userInfo[@"credits"];
    BOOL premium = [userInfo[@"premium"] boolValue];
    
    if(([shaderArray[selectedIndex] intValue] != SHADER_CLOUD))
        valueForRender = (resolutionType == THOUSAND_EIGHTY_P) ? 8 : (resolutionType == SEVEN_HUNDRED_TWENTY_P) ? 4 : (resolutionType == FOUR_HUNDRED_EIGHTY_P) ? 2 : (resolutionType == THREE_HUNDRED_SIXTY_P) ? 1 : 0;
    else
        valueForRender = (resolutionType == THOUSAND_EIGHTY_P) ? 230 : (resolutionType == SEVEN_HUNDRED_TWENTY_P) ? 100 : (resolutionType == FOUR_HUNDRED_EIGHTY_P) ? 45 : (resolutionType == THREE_HUNDRED_SIXTY_P) ? 25 : 10;
    
    int frames = (renderingExportImage == RENDER_IMAGE) ? 1 : ((int)_trimControl.rightValue - (int)_trimControl.leftValue);
    int waterMarkCredit = ((!_watermarkSwitch.isOn && ([shaderArray[selectedIndex] intValue] != SHADER_CLOUD)) ? 50 : 0);
    int creditsForFrames = frames * valueForRender;
    int credits = (creditsForFrames + waterMarkCredit)  * -1;
    
    if([userCredits intValue] >= abs(credits)) {
        [_creditLable setHidden:YES];
        if(([shaderArray[selectedIndex] intValue] == SHADER_CLOUD)){
            [self.delegate saveScene];
            [self shaderPhotoAction:credits];
        }
        else
            [self renderBeginFunction:credits];
    } else if (premium) {
        if(([shaderArray[selectedIndex] intValue] != SHADER_CLOUD)) {
            [_creditLable setHidden:YES];
            [self renderBeginFunction:credits];
        }  else{
            UIAlertView *notEnoughCredit = [[UIAlertView alloc]initWithTitle:@"Information" message:[NSString stringWithFormat:@"%@",@"You are not have enough credits please recharge your account to proceed." ] delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
            [notEnoughCredit show];
            [notEnoughCredit setTag:CREDITS_VIEW];
            [self.trimControl setHidden:NO];
            [_nextButton setHidden:NO];
        }
    } else{
        UIAlertView *notEnoughCredit = [[UIAlertView alloc]initWithTitle:@"Information" message:[NSString stringWithFormat:@"%@",@"You are not have enough credits please recharge your account to proceed." ] delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [notEnoughCredit show];
        [notEnoughCredit setTag:CREDITS_VIEW];
        [self.trimControl setHidden:NO];
        [_nextButton setHidden:NO];
    }
}

- (void) updateCreditLable
{
    if([[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"] && [[AppHelper getAppHelper] userDefaultsBoolForKey:@"hasRestored"] && ([shaderArray[selectedIndex] intValue] != SHADER_CLOUD))
        [_creditLable setHidden:YES];
    else {
        [_creditLable setHidden:NO];
        int valueForRender = 0;
        if(([shaderArray[selectedIndex] intValue] != SHADER_CLOUD))
            valueForRender = (resolutionType == THOUSAND_EIGHTY_P) ? 8 : (resolutionType == SEVEN_HUNDRED_TWENTY_P) ? 4 : (resolutionType == FOUR_HUNDRED_EIGHTY_P) ? 2 : (resolutionType == THREE_HUNDRED_SIXTY_P) ? 1 : 0;
        else
            valueForRender = (resolutionType == THOUSAND_EIGHTY_P) ? 230 : (resolutionType == SEVEN_HUNDRED_TWENTY_P) ? 100 : (resolutionType == FOUR_HUNDRED_EIGHTY_P) ? 45 : (resolutionType == THREE_HUNDRED_SIXTY_P) ? 25 : 10;

        int frames = (renderingExportImage == RENDER_IMAGE) ? 1 : ((int)_trimControl.rightValue - (int)_trimControl.leftValue);
        int waterMarkCredit = ((!_watermarkSwitch.isOn && ([shaderArray[selectedIndex] intValue] != SHADER_CLOUD)) ? 50 : 0);
        int creditsForFrames = frames * valueForRender;
        int credits = (creditsForFrames + waterMarkCredit);
        _creditLable.text = (credits == 0 ) ? @"" : [NSString stringWithFormat:@"%d Credits", credits];
    }
}

-(NSMutableArray*) getFileteredFilePathsFrom:(NSMutableArray*) filePaths
{
    NSMutableArray * filtFilePaths = [[NSMutableArray alloc] init];
    NSString *docDirPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *cacheDirPath = [NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *objDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Objs"];
    NSString *rigDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Rigs"];
    NSString *texDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Textures"];
    NSString *sgmDirPath = [docDirPath stringByAppendingPathComponent:@"Resources/Sgm"];
    
    NSFileManager * fm = [NSFileManager defaultManager];
    
    for(int i = 0; i < [filePaths count]; i++) {
        if([[filePaths objectAtIndex:i] isEqualToString:@""])
            continue;
        NSString *filePath1 = [NSString stringWithFormat:@"%@/%@",cacheDirPath,[filePaths objectAtIndex:i]];
        NSString *filePath2 = [NSString stringWithFormat:@"%@/%@",objDirPath,[filePaths objectAtIndex:i]];
        NSString *filePath3 = [NSString stringWithFormat:@"%@/%@",rigDirPath,[filePaths objectAtIndex:i]];
        NSString *filePath4 = [NSString stringWithFormat:@"%@/%@",texDirPath,[filePaths objectAtIndex:i]];
        NSString *filePath5 = [NSString stringWithFormat:@"%@/%@", sgmDirPath, [filePaths objectAtIndex:i]];
        NSString *filePath6 = [NSString stringWithFormat:@"%s/%@",constants::BundlePath.c_str(), [filePaths objectAtIndex:i]];
        
    
        if([fm fileExistsAtPath:filePath1]) {
            if(![filtFilePaths containsObject:filePath1])
                [filtFilePaths addObject:filePath1];
        } else if([fm fileExistsAtPath:filePath2]){
            if(![filtFilePaths containsObject:filePath2])
                [filtFilePaths addObject:filePath2];
        } else if([fm fileExistsAtPath:filePath3]){
            if(![filtFilePaths containsObject:filePath3])
                [filtFilePaths addObject:filePath3];
        } else if([fm fileExistsAtPath:filePath4]){
            if(![filtFilePaths containsObject:filePath4])
                [filtFilePaths addObject:filePath4];
        } else if([fm fileExistsAtPath:filePath5]){
            if(![filtFilePaths containsObject:filePath5])
                [filtFilePaths addObject:filePath5];
        } else if([fm fileExistsAtPath:filePath6]){
            if(![filtFilePaths containsObject:filePath6])
                [filtFilePaths addObject:filePath6];
        }
    }    
    return filtFilePaths;
}


-(void) shaderPhotoAction:(int)credits{
    
    NSFileManager  *manager = [NSFileManager defaultManager];
    NSArray *docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSMutableString *docDirectory = [docPaths objectAtIndex:0];
    
    if([[AppHelper getAppHelper] checkInternetConnected]){
        [self.nextButton setHidden:YES];
        [self.makeVideoLoading setHidden:NO];
        [self.makeVideoLoading startAnimating];
        NSMutableArray *fileNames = [self.delegate getFileNamesToAttach];
        if(![self.delegate canUploadToCloud]) {
            UIAlertView *errAlert = [[UIAlertView alloc]initWithTitle:@"Information" message:@"Scene contains video/praticles which is currently not supported in HQ rendering." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [errAlert show];
            [_cancelButton setHidden:YES];
            [_nextButton setHidden:NO];
            [_nextButton setEnabled:YES];
            [_youtubeButton setHidden:YES];
            [_nextButton setTitle:@"Done" forState:UIControlStateNormal];
            _nextButton.tag = DONE;
            return;
        }
        CGPoint camResolution = [self.delegate getCameraResolution];
        NSString* zipfile = [docDirectory stringByAppendingPathComponent:@"index.zip"] ;
        
        NSMutableArray *userFiles = [self getFileteredFilePathsFrom:fileNames];
        ZipArchive* zip = [[ZipArchive alloc] init];
        BOOL ret = [zip CreateZipFile2:zipfile];
        
        if([[NSFileManager defaultManager] fileExistsAtPath:self.sgbPath]) {
            ret = [zip addFileToZip:self.sgbPath newname:@"index.sgb"];
        }
        
        for(int i = 0; i < [userFiles count]; i++) {
            NSLog(@"\n User File: %@", [userFiles objectAtIndex:i]);
            ret = [zip addFileToZip:[userFiles objectAtIndex:i] newname:[[userFiles objectAtIndex:i] lastPathComponent]];
        }
        
        if( ![zip CloseZipFile2] )
        {
            zipfile = @"";
        }
        NSLog(@"The file has been zipped");
        
        
        NSString *osId = @"1";
        NSString *uniqueId= [[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"];
        NSString *deviceToken = [[AppHelper getAppHelper] userDefaultsForKey:@"deviceToken"];
        int startFrame = (renderingExportImage == RENDER_IMAGE) ? renderingStartFrame+1 : ((int)_trimControl.leftValue + 1);
        int endFrame = (renderingExportImage == RENDER_IMAGE) ? renderingStartFrame+1 : ((int)_trimControl.rightValue);
        printf("\n Frame %d %d " , startFrame , endFrame);
        NSString *sFrame = [NSString stringWithFormat:@"%d",startFrame];
        NSString *eFrame = [NSString stringWithFormat:@"%d",endFrame];
        NSString *creditsStr = [NSString stringWithFormat:@"%d",credits];
        
        NSString *resolutionWidth = [NSString stringWithFormat:@"%f",camResolution.x];
        NSString *resolutionHeight = [NSString stringWithFormat:@"%f",camResolution.y];
        
        NSString *fileUrl= [docDirectory stringByAppendingString:@"/index.zip"];
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
                [formData appendPartWithFormData:[uniqueId dataUsingEncoding:NSUTF8StringEncoding] name:@"userid"];
                [formData appendPartWithFormData:[sFrame dataUsingEncoding:NSUTF8StringEncoding] name:@"startFrame"];
                [formData appendPartWithFormData:[eFrame dataUsingEncoding:NSUTF8StringEncoding] name:@"endFrame"];
                [formData appendPartWithFormData:[resolutionWidth dataUsingEncoding:NSUTF8StringEncoding] name:@"width"];
                [formData appendPartWithFormData:[resolutionHeight dataUsingEncoding:NSUTF8StringEncoding] name:@"height"];
                [formData appendPartWithFormData:[creditsStr dataUsingEncoding:NSUTF8StringEncoding] name:@"credits"];
                [formData appendPartWithFormData:[deviceToken dataUsingEncoding:NSUTF8StringEncoding] name:@"pushid"];
                [formData appendPartWithFormData:[osId dataUsingEncoding:NSUTF8StringEncoding] name:@"os"];
            }];
            NSLog(@"Request initiated");
            
            AFHTTPRequestOperation *operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
            
            [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
                NSLog(@"Request Successfull");
                publishId=[[operation responseString] intValue];
                NSLog(@"publish Id %d",publishId);
                
                NSError *error = nil;
                for (NSString *sqliteFile in sgfdFiles)
                {
                    [manager removeItemAtPath:[docDirectory stringByAppendingPathComponent:sqliteFile] error:&error];
                }
                [manager removeItemAtPath:[docDirectory stringByAppendingPathComponent:@"index.zip"] error:&error];
                
                [self.makeVideoLoading setHidden:YES];
                
                NSDateFormatter *dateFormat = [[NSDateFormatter alloc] init];
                [dateFormat setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
                NSString* dateStr = [dateFormat stringFromDate:[NSDate date]];

                [cache addRenderTaskData:publishId estTime:renderingStartFrame+1 proName:self.projectName date:dateStr];
                
                UIAlertView *showAlert = [[UIAlertView alloc]initWithTitle:@"Information" message:@"Uploaded successfully. You can see the progress of your render in your profile view." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
                [showAlert show];

                [_cancelButton setHidden:YES];
                [_nextButton setHidden:NO];
                [_nextButton setEnabled:YES];
                [_youtubeButton setHidden:YES];
                [_nextButton setTitle:@"Done" forState:UIControlStateNormal];
                _nextButton.tag = DONE;
            }
                                             failure:^(AFHTTPRequestOperation *operation, NSError *error) {
                                                 
                                                 [_cancelButton setHidden:YES];
                                                 [_nextButton setHidden:NO];
                                                 [_nextButton setEnabled:YES];
                                                 [_youtubeButton setHidden:YES];
                                                 [_nextButton setTitle:@"Done" forState:UIControlStateNormal];
                                                 _nextButton.tag = DONE;

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

- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    if(renderingExportImage != RENDER_IMAGE){
        
        _trimControl = [[RETrimControl alloc] initWithFrame:CGRectMake(_progressSub.frame.origin.x,_progressSub.frame.origin.y, _progressSub.frame.size.width, _progressSub.frame.size.height)];
        _trimControl.length = renderingEndFrame; // 200 seconds
        _trimControl.delegate = self;
        _trimControl.center = _progressSub.center;
        [self.view addSubview:_trimControl];
    }
    
    [self removeSGFDFilesIfAny];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersBG) name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersFG) name:@"applicationDidBecomeActive" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(uploadFilesToRender) name:@"FileWriteCompleted" object:nil];

    isAppInBg = false;
    [self updateCreditLable];
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

- (void) renderingProgress:(NSNumber*)credits
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
        [self.makeVideoLoading setHidden:NO];
        [self.makeVideoLoading startAnimating];
    }
    
    if(resolutionType != TWO_HUNDRED_FOURTY_P || (resolutionType == TWO_HUNDRED_FOURTY_P && !_watermarkSwitch.isOn))
        [self saveDeductedCredits:[credits intValue]];
    else
        [self verifiedUsage];
}

- (void) renderFinishAction:(NSNumber*)object
{
    int renderingType = [object intValue];
    NSString *tempDir = NSTemporaryDirectory();
    if(renderingExportImage == RENDER_VIDEO){
        if(self.videoFilePath == nil || [self.videoFilePath isEqualToString:@""])
            return;
        NSString *filePath = [NSString stringWithFormat:@"%@/myMovie.mov", tempDir];
        UISaveVideoAtPathToSavedPhotosAlbum(filePath,nil,nil,nil);
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Video successfully saved in your gallery." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil, nil];
        [alert show];
    }
    else {
        NSString *filePath = [NSString stringWithFormat:@"%@r-%d.png", tempDir, renderingFrame - 1];
        UIImage *image = [UIImage imageWithContentsOfFile:filePath];
        UIImageWriteToSavedPhotosAlbum(image,nil,nil,nil);
        UIAlertView* alert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Image was successfully saved in your gallery." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil, nil];
        [alert show];
    }
    [_cancelButton setHidden:YES];
    [_nextButton setHidden:NO];
    [_nextButton setEnabled:YES];
    [_youtubeButton setHidden:YES];
    [_nextButton setTitle:@"Done" forState:UIControlStateNormal];
    _nextButton.tag = DONE;
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
        cell.ShaderImage.image =[UIImage imageNamed:@"render1.png"];
    }
    else if ((int)indexPath.row == 1){
        cell.ShaderImage.image =[UIImage imageNamed:@"render2.png"];
    }
    else if ((int)indexPath.row == 2){
        cell.ShaderImage.image =[UIImage imageNamed:@"render3.png"];
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
        [self.renderDesc setText:[NSString stringWithFormat:@"Render Toon Shader in your device."]];
        selectedIndex = (int)indexPath.row;
        [self.renderingTypes reloadData];
    }
    else if([shaderArray[indexPath.row] intValue] == SHADER_DEFAULT){
        shaderType = [shaderArray[indexPath.row] intValue];
        NSLog(@" Selcted index: %d",(int)indexPath.row);
        [self.nextButton setTitle:@"Next" forState:UIControlStateNormal];
        [self.renderDesc setText:[NSString stringWithFormat:@"Render Normal Shader in your device."]];
        selectedIndex = (int)indexPath.row;
        [self.renderingTypes reloadData];
    }
    else if ([shaderArray[indexPath.row] intValue] == SHADER_CLOUD){
//        tempSelectedIndex = (int)indexPath.row;
        if(!resAlertShown) {
            UIAlertView *resolutionAlert = [[UIAlertView alloc]initWithTitle:@"Information" message:@"Please try 240p or 360p resolution which are much cheaper so that you can preview the scene before final renders." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
            [resolutionAlert show];
            resAlertShown = YES;
        }
        self.resolutionSegment.selectedSegmentIndex = TWO_HUNDRED_FOURTY_P;
        [self cameraResolutionChanged:nil];
        [self.renderDesc setText:[NSString stringWithFormat:@"Render with High Quality in cloud."]];
        shaderType = [shaderArray[indexPath.row] intValue];
        NSLog(@" Selcted index: %d",(int)indexPath.row);
        [self.nextButton setTitle:@"Upload" forState:UIControlStateNormal];
        selectedIndex = (int)indexPath.row;
        [self.renderingTypes reloadData];
    }
    
    if([shaderArray[indexPath.row] intValue] == SHADER_CLOUD){
        [_watermarkSwitch setOn:NO animated:YES];
        [_watermarkSwitch setEnabled:NO];
        [_colorPickerBtn setEnabled:NO];
    }
    else{
        [_watermarkSwitch setEnabled:YES];
        [_watermarkSwitch setOn:YES animated:YES];
        [_colorPickerBtn setEnabled:YES];
    }
    [self updateCreditLable];
}
//-(void)showUpgradeView:(int)selectedRowIndex
//{
//    if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]){
//        if([Utility IsPadDevice]) {
//            upgradeView = [[PremiumUpgardeVCViewController alloc] initWithNibName:@"PremiumUpgardeVCViewController" bundle:nil];
//            upgradeView.delegate = self;
//            upgradeView.modalPresentationStyle = UIModalPresentationFormSheet;
//            [self presentViewController:upgradeView animated:YES completion:nil];
//            upgradeView.view.superview.backgroundColor = [UIColor clearColor];
//            upgradeView.view.layer.borderWidth = 2.0f;
//            upgradeView.view.layer.borderColor = [UIColor grayColor].CGColor;
//        }else{
//            upgradeView = [[PremiumUpgardeVCViewController alloc] initWithNibName:@"PremiumUpgradeViewControllerPhone" bundle:nil];
//            upgradeView.delegate = self;
//            upgradeView.modalPresentationStyle = UIModalPresentationFormSheet;
//            [self presentViewController:upgradeView animated:YES completion:nil];
//        }
//    }
//    else {
//        if(selectedRowIndex != -1) {
//        shaderType = [shaderArray[selectedRowIndex] intValue];
//        selectedIndex = selectedRowIndex;
//        [self.renderingTypes reloadData];
//        } else {
//            [self.watermarkSwitch setOn:NO];
//        }
//    }
//}

- (void) alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
    switch (alertView.tag) {
        case SIGNIN_ALERT_VIEW:
            if(buttonIndex == CANCEL_BUTTON_INDEX){
                [self dismissViewControllerAnimated:NO completion:^{
                    cancelPressed = YES;
                    [self.cancelActivityIndicator setHidden:false];
                    [self.cancelActivityIndicator startAnimating];
                    [thread cancel];
                    isCanceled = true;
                    [self.delegate clearFolder:NSTemporaryDirectory()];
                    [self.delegate resumeRenderingAnimationScene];
                    [_delegate loginBtnAction:nil];
                    [self deallocMem];
                }];
            }
            break;
            
        default:
            break;
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
    [self updateCreditLable];
}

- (IBAction)waterMarkValueChanged:(id)sender {
        [self updateCreditLable];
}

- (IBAction) cancelButtonAction:(id)sender
{
    cancelPressed = YES;
    [self.cancelActivityIndicator setHidden:false];
    [self.cancelActivityIndicator startAnimating];
    [thread cancel];
    isCanceled = true;
    [self.delegate clearFolder:NSTemporaryDirectory()];
    [self.delegate resumeRenderingAnimationScene];
    [self dismissViewControllerAnimated:YES completion:nil];
    [self deallocMem];
}

- (IBAction)colorPickerAction:(id)sender {
    _bgColorProp = [[TextColorPicker alloc] initWithNibName:@"TextColorPicker" bundle:nil TextColor:nil];
    _bgColorProp.delegate = self;
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_bgColorProp];
    self.popoverController.popoverContentSize = CGSizeMake(200, 200);
    self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    [_popUpVc.view setClipsToBounds:YES];
    CGRect rect = _colorPickerBtn.frame;
    rect = [self.view convertRect:rect fromView:_colorPickerBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self.view
                          permittedArrowDirections:([Utility IsPadDevice]) ? UIPopoverArrowDirectionUp : UIPopoverArrowDirectionRight
                                          animated:NO];

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
- (void) renderBeginFunction:(int)credits
{
    [self beginViewHideAndShow:false];
    [self.resolutionSegment setEnabled:false];
    [self.startButtonText setEnabled:false];
    resolutionType = (int)self.resolutionSegment.selectedSegmentIndex;
    if(resolutionType == THOUSAND_EIGHTY_P) {
        cameraResolutionWidth = 1920.0f;
        cameraResolutionHeight = 1080.0f;
    } else if(resolutionType == SEVEN_HUNDRED_TWENTY_P) {
        cameraResolutionWidth = 1280.0f;
        cameraResolutionHeight = 720.0f;
    } else if(resolutionType == FOUR_HUNDRED_EIGHTY_P) {
        cameraResolutionWidth = 848.0f;
        cameraResolutionHeight = 480.0f;
    } else if(resolutionType == THREE_HUNDRED_SIXTY_P){
        cameraResolutionWidth = 640.0f;
        cameraResolutionHeight = 360.0f;
    } else {
        cameraResolutionWidth = 352.0f;
        cameraResolutionHeight = 240.0f;
    }
    [[AppHelper getAppHelper] saveToUserDefaults:[NSString stringWithFormat:@"%d",resolutionType] withKey:@"cameraResolution"];
    [UIApplication sharedApplication].idleTimerDisabled = YES;
    
    self.renderingProgressLabel.text = [NSString stringWithFormat:@"%d/%d",(int)_trimControl.leftValue,(int)_trimControl.rightValue];
    thread = [[NSThread alloc] initWithTarget:self selector:@selector(renderingProgress:) object:[NSNumber numberWithInt:credits]];
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
        
        [self.delegate renderFrame:renderingFrame withType:shaderType isImage:(renderingExportImage == RENDER_IMAGE) andRemoveWatermark:(!self.watermarkSwitch.isOn)];
        
        NSString *tempDir = NSTemporaryDirectory();
        NSString *imageFilePath = [NSString stringWithFormat:@"%@/r-%d.png", tempDir, renderingFrame];
        [self.renderedImageView setImage:[UIImage imageWithContentsOfFile:imageFilePath]];
        
        [self.activityIndicatorView stopAnimating];
        [self.activityIndicatorView setHidden:true];
    }
}
- (void) MakeVideo
{
    NSLog(@"Camera Width %f Height %f " ,cameraResolutionWidth,cameraResolutionHeight);
    
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
    
    if(self.videoFilePath == nil || [self.videoFilePath isEqualToString:@""])
        return;
    
    AVAssetWriter *videoWriter = [[AVAssetWriter alloc] initWithURL:[NSURL fileURLWithPath:self.videoFilePath] fileType:AVFileTypeQuickTimeMovie error:&error];
    NSLog(@" Video Error %@ ", error.localizedDescription);
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
    /*
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
     */
    
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
    //[[AppHelper getAppHelper] callPaymentGateWayForPremiumUpgrade];
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

- (void) changeVertexColor:(Vector3)vetexColor dragFinish:(BOOL)isDragFinish{
    bgColor = vetexColor;
    if(isDragFinish){
        [self.delegate changeRenderingBgColor:Vector4(bgColor,(_transparentBgSwitch.isOn) ? 0.0 : 1.0)];
    }
}

- (void) deallocMem
{
    thread = nil;
    [self.delegate freezeEditorRender:NO];
    self.videoFilePath = nil;
    self.youtubeService = nil;
    self.uploadVideo = nil;
    self.authController = nil;
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"creditsupdate" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"creditsused" object:nil];
}

- (IBAction)transparentBgValueChanged:(id)sender {
    (_transparentBgSwitch.isOn) ? [self.delegate changeRenderingBgColor:(Vector4(bgColor,0.0))] : [self.delegate changeRenderingBgColor:(Vector4(bgColor,1.0))];
    
}

@end
