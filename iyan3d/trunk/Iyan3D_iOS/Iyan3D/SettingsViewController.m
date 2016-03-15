//
//  SettingsViewController.m
//  Iyan3D
//
//  Created by Sankar on 06/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "SettingsViewController.h"
#import "Utility.h"

#define FRAME_COUNT 0
#define FRAME_DURATION 1

#define PREVIEW_LEFTBOTTOM 0
#define PREVIEW_LEFTTOP 1
#define PREVIEW_RIGHTBOTTOM 2
#define PREVIEW_RIGHTTOP 3

#define TOOLBAR_RIGHT 0
#define TOOLBAR_LEFT 1

#define SCENE_SELECTION 0
#define EDITOR_VIEW_CONTROL 1




@interface SettingsViewController ()

@end

@implementation SettingsViewController

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {

    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.screenName = @"SettingsViewVC iOS";
    [self setupImageTap];
    self.doneBtn.layer.cornerRadius=CORNER_RADIUS;
    self.restoreBtn.layer.cornerRadius=CORNER_RADIUS;
    [self readUserSettings];
    [_restorePurchaseProgress setHidden:YES];
    
}



- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

-(void)setupImageTap{
    self.toolbarLeft.userInteractionEnabled = YES;
    UITapGestureRecognizer *tapToolBarLeft = [[UITapGestureRecognizer alloc]
                                              initWithTarget:self action:@selector(toolbarLeftTap:)];
    self.toolbarRight.userInteractionEnabled = YES;
    UITapGestureRecognizer *tapToolBarRight = [[UITapGestureRecognizer alloc]
                                               initWithTarget:self action:@selector(toolbarRightTap:)];
    self.renderPreviewSizeSmall.userInteractionEnabled = YES;
    UITapGestureRecognizer *taprenderPreviewSizeSmall = [[UITapGestureRecognizer alloc]
                                                         initWithTarget:self action:@selector(renderPreviewSizeSmallTap:)];
    self.renderPreviewSizeLarge.userInteractionEnabled = YES;
    UITapGestureRecognizer *taprenderPreviewSizeLarge = [[UITapGestureRecognizer alloc]
                                                         initWithTarget:self action:@selector(renderPreviewSizeLargeTap:)];
    self.framesDisplayCount.userInteractionEnabled = YES;
    UITapGestureRecognizer *tapframesDisplayCount = [[UITapGestureRecognizer alloc]
                                                         initWithTarget:self action:@selector(framesDisplayCountTap:)];
    self.framesDisplayDuration.userInteractionEnabled = YES;
    UITapGestureRecognizer *tapframesDisplayDuration = [[UITapGestureRecognizer alloc]
                                                         initWithTarget:self action:@selector(framesDisplayDurationTap:)];
    
    self.previewPositionRightBottom.userInteractionEnabled = YES;
    UITapGestureRecognizer *tappreviewPositionRightBottom = [[UITapGestureRecognizer alloc]
                                                        initWithTarget:self action:@selector(previewPositionRightBottomTap:)];
    self.previewPositionRightTop.userInteractionEnabled = YES;
    UITapGestureRecognizer *tappreviewPositionRightTop = [[UITapGestureRecognizer alloc]
                                                             initWithTarget:self action:@selector(previewPositionRightTopTap:)];
    self.previewPositionLeftBottom.userInteractionEnabled = YES;
    UITapGestureRecognizer *tappreviewPositionLeftBottom = [[UITapGestureRecognizer alloc]
                                                          initWithTarget:self action:@selector(previewPositionLeftBottomTap:)];
    self.previewPositionLeftTop.userInteractionEnabled = YES;
    UITapGestureRecognizer *tappreviewPositionLeftTop = [[UITapGestureRecognizer alloc]
                                                          initWithTarget:self action:@selector(previewPositionLeftTopTap:)];
    
    tapToolBarLeft.delegate = self;
    tapToolBarRight.delegate = self;
    taprenderPreviewSizeSmall.delegate=self;
    taprenderPreviewSizeLarge.delegate=self;
    tapframesDisplayCount.delegate=self;
    tapframesDisplayDuration.delegate=self;
    tappreviewPositionRightBottom.delegate=self;
    tappreviewPositionRightTop.delegate=self;
    tappreviewPositionLeftBottom.delegate=self;
    tappreviewPositionLeftTop.delegate=self;
    
    
    [self.toolbarLeft addGestureRecognizer:tapToolBarLeft];
    [self.toolbarRight addGestureRecognizer:tapToolBarRight];
    [self.renderPreviewSizeSmall addGestureRecognizer:taprenderPreviewSizeSmall];
    [self.renderPreviewSizeLarge addGestureRecognizer:taprenderPreviewSizeLarge];
    [self.framesDisplayCount addGestureRecognizer:tapframesDisplayCount];
    [self.framesDisplayDuration addGestureRecognizer:tapframesDisplayDuration];
    [self.previewPositionRightBottom addGestureRecognizer:tappreviewPositionRightBottom];
    [self.previewPositionRightTop addGestureRecognizer:tappreviewPositionRightTop];
    [self.previewPositionLeftBottom addGestureRecognizer:tappreviewPositionLeftBottom];
    [self.previewPositionLeftTop addGestureRecognizer:tappreviewPositionLeftTop];
    
    
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (IBAction)toolBarPositionChanged:(id)sender {
    NSLog(@"Tool Bar position Changed");
    [self.delegate toolbarPosition:(int)self.toolbarPosition.selectedSegmentIndex];
}

- (IBAction)renderPreviewSizeChanged:(id)sender {
    NSLog(@"Render preview size Changed");
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithFloat:(int)self.renderPreviewSize.selectedSegmentIndex] withKey:@"cameraPreviewSize"];
    [self.delegate cameraPreviewSize];
}

- (IBAction)frameCountDisplayType:(id)sender {
    NSLog(@"frame display type Changed");
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:(int)self.frameCountDisplay.selectedSegmentIndex] withKey:@"indicationType"];
    [self.delegate frameCountDisplayMode];
}

- (IBAction)previewpositionChanged:(id)sender {
    [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:(int)self.renderPreviewPosition.selectedSegmentIndex] withKey:@"cameraPreviewPosition"];
    [self.delegate cameraPreviewPosition];
}

- (IBAction)doneBtnAction:(id)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (void)toolbarLeftTap:(UITapGestureRecognizer *)pinchGestureRecognizer{
    if(self.toolbarPosition.selectedSegmentIndex==0){
        self.toolbarPosition.selectedSegmentIndex=1;
        [self.delegate toolbarPosition:(int)self.toolbarPosition.selectedSegmentIndex];
    }
    else{
        
    }
    
}
- (void)toolbarRightTap:(UITapGestureRecognizer *)pinchGestureRecognizer{
    if(self.toolbarPosition.selectedSegmentIndex==0){
        
    }
    else{
        self.toolbarPosition.selectedSegmentIndex=0;
        [self.delegate toolbarPosition:(int)self.toolbarPosition.selectedSegmentIndex];
    }
    
}
- (void)renderPreviewSizeSmallTap:(UITapGestureRecognizer *)pinchGestureRecognizer{
    if(self.renderPreviewSize.selectedSegmentIndex==0){
        
    }
    else{
        self.renderPreviewSize.selectedSegmentIndex=0;
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithFloat:(int)self.renderPreviewSize.selectedSegmentIndex] withKey:@"cameraPreviewSize"];

        [self.delegate cameraPreviewSize];
    }
    
}
- (void)renderPreviewSizeLargeTap:(UITapGestureRecognizer *)pinchGestureRecognizer{
    if(self.renderPreviewSize.selectedSegmentIndex==0){
        self.renderPreviewSize.selectedSegmentIndex=1;
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithFloat:(int)self.renderPreviewSize.selectedSegmentIndex] withKey:@"cameraPreviewSize"];

        [self.delegate cameraPreviewSize];
    }
    else{
        
    }
    
}
- (void)framesDisplayDurationTap:(UITapGestureRecognizer *)pinchGestureRecognizer{
    if(self.frameCountDisplay.selectedSegmentIndex==0){
        self.frameCountDisplay.selectedSegmentIndex=1;
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:(int)self.frameCountDisplay.selectedSegmentIndex] withKey:@"indicationType"];
        [self.delegate frameCountDisplayMode];
    }
    else{
        
    }
    
}
- (void)framesDisplayCountTap:(UITapGestureRecognizer *)pinchGestureRecognizer{
    if(self.frameCountDisplay.selectedSegmentIndex==0){
        
    }
    else{
        self.frameCountDisplay.selectedSegmentIndex=0;
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:(int)self.frameCountDisplay.selectedSegmentIndex] withKey:@"indicationType"];
        [self.delegate frameCountDisplayMode];
    }
    
}
- (void)previewPositionRightBottomTap:(UITapGestureRecognizer *)pinchGestureRecognizer{
    if(self.renderPreviewPosition.selectedSegmentIndex!=0){
         self.renderPreviewPosition.selectedSegmentIndex=0;
        [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:0] withKey:@"cameraPreviewPosition"];
        [self.delegate cameraPreviewPosition];
    }
    else{
       
    }
    
}
- (void)previewPositionRightTopTap:(UITapGestureRecognizer *)pinchGestureRecognizer{
    if(self.renderPreviewPosition.selectedSegmentIndex!=1){
        self.renderPreviewPosition.selectedSegmentIndex=1;
                [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:1] withKey:@"cameraPreviewPosition"];
        [self.delegate cameraPreviewPosition];
    }
    else{
        
    }
    
}
- (void)previewPositionLeftBottomTap:(UITapGestureRecognizer *)pinchGestureRecognizer{
    if(self.renderPreviewPosition.selectedSegmentIndex!=2){
        self.renderPreviewPosition.selectedSegmentIndex=2;
                [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:2] withKey:@"cameraPreviewPosition"];
        [self.delegate cameraPreviewPosition];
    }
    else{
        
    }
    
}
- (void)previewPositionLeftTopTap:(UITapGestureRecognizer *)pinchGestureRecognizer{
    if(self.renderPreviewPosition.selectedSegmentIndex!=3){
        self.renderPreviewPosition.selectedSegmentIndex=3;
                [[AppHelper getAppHelper] saveToUserDefaults:[NSNumber numberWithInt:3] withKey:@"cameraPreviewPosition"];
        [self.delegate cameraPreviewPosition];
    }
    else{
        
    }
    
}


- (IBAction)multiselectValueChanged:(id)sender {
    if(self.multiSelectSwitch.isOn){
        [self.delegate multiSelectUpdate:YES];
    }
    else{
        [self.delegate multiSelectUpdate:NO];
    }
}

-(void) readUserSettings{
    float cameraPreviewSize= [[[AppHelper getAppHelper]userDefaultsForKey:@"cameraPreviewSize"]floatValue];
    int cameraPosition = [[[AppHelper getAppHelper]userDefaultsForKey:@"cameraPreviewPosition"]intValue];
    int frameCountMode = [[[AppHelper getAppHelper] userDefaultsForKey:@"indicationType"]intValue];
    
    if ([[AppHelper getAppHelper] userDefaultsForKey:@"cameraPreviewSize"]){
        if(cameraPreviewSize==2.0){
            [self.renderPreviewSize setSelectedSegmentIndex:1];
        }
        else{
            
        }
    }
    [self.renderPreviewPosition setSelectedSegmentIndex:cameraPosition];
    [self.frameCountDisplay setSelectedSegmentIndex:frameCountMode];
    if([[[AppHelper getAppHelper]userDefaultsForKey:@"toolbarPosition"]integerValue]==TOOLBAR_LEFT)
    {
        [self.toolbarPosition setSelectedSegmentIndex:TOOLBAR_LEFT];
    }
    else
    {
        [self.toolbarPosition setSelectedSegmentIndex:TOOLBAR_RIGHT];
    }

    if ([[AppHelper getAppHelper]userDefaultsBoolForKey:@"multiSelectOption"]==YES) {
        [self.multiSelectSwitch setOn:YES];
    }
    else
    {
        [self.multiSelectSwitch setOn:NO];
    }

}

- (IBAction)restoreAction:(id)sender
{
    if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"signedin"] &&  [[[AppHelper getAppHelper] userDefaultsForKey:@"uniqueid"] length] <= 5){
        UIAlertView *signinAlert = [[UIAlertView alloc]initWithTitle:@"Information" message:@"Please SignIn to continue." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [signinAlert show];
        return;
    }
    [_restoreBtn setHidden:YES];
    [_restorePurchaseProgress setHidden:NO];
    [_restorePurchaseProgress startAnimating];
    [[AppHelper getAppHelper] addTransactionObserver];
    [AppHelper getAppHelper].delegate = self;
    [[AppHelper getAppHelper] restorePurchasedTransaction];
}

-(void)statusForRestorePurchase:(NSNumber *)object
{
    if([object boolValue]) {
        NSLog(@" Restored ");
        NSArray *restoreIds = [[AppHelper getAppHelper] getRestoreIds];
        for (int i = 0; i < [restoreIds count]; i++) {
            if ([[restoreIds objectAtIndex:i] isEqual:@"objimport"]) {
                CacheSystem* cacheSystem = [CacheSystem cacheSystem];
                [cacheSystem addOBJImporterColumn];
                [[AppHelper getAppHelper] saveBoolUserDefaults:true withKey:@"premiumUnlocked"];
                [[AppHelper getAppHelper] saveBoolUserDefaults:true withKey:@"hasRestored"];
                [[AppHelper getAppHelper] verifyRestorePurchase];
                [AppHelper getAppHelper].delegate = nil;
                [_restoreBtn setHidden:NO];
                [_restorePurchaseProgress stopAnimating];
                [_restorePurchaseProgress setHidden:YES];
                break;
            } else {
                [[AppHelper getAppHelper] saveBoolUserDefaults:false withKey:@"premiumUnlocked"];
                [[AppHelper getAppHelper] saveBoolUserDefaults:false withKey:@"hasRestored"];
                [_restoreBtn setHidden:NO];
                [_restorePurchaseProgress stopAnimating];
                [_restorePurchaseProgress setHidden:YES];
            }
        }
        
        if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"] || ![[AppHelper getAppHelper] userDefaultsBoolForKey:@"hasRestored"]) {
            UIAlertView* infoAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"There seems to be a problem with your purchase. Please make sure that you have upgraded to Premium with the current account." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
            [infoAlert show];
        }
    }
    
    
}

-(void)transactionCancelled
{
    [[AppHelper getAppHelper] removeTransactionObserver];
    [AppHelper getAppHelper].delegate = nil;
    [_restoreBtn setHidden:NO];
    [_restorePurchaseProgress stopAnimating];
    [_restorePurchaseProgress setHidden:YES];
}


@end
