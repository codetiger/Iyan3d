//
//  SceneSelectionViewControllerPad.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 03/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MessageUI/MFMailComposeViewController.h>
#import <MediaPlayer/MediaPlayer.h>
#import "CacheSystem.h"
#import "AssetItem.h"
#import "Utility.h"
#import "iCarousel.h"
#import "SceneItem.h"
#import "FPPopoverController.h"
#import "HelpViewController.h"
#import "AppHelper.h"
#import "AnimationEditorViewControllerPad.h"
#import "PremiumUpgardeVCViewController.h"
#import "GAI.h"

@interface SceneSelectionViewControllerPad : GAITrackedViewController <iCarouselDataSource, PremiumUpgardeVCViewControllerDelegate,iCarouselDelegate, UISearchBarDelegate, UIAlertViewDelegate,MFMailComposeViewControllerDelegate,HelpViewControllerDelegate> {
    CacheSystem* cache;
    int carouselIndexitem;
    NSDateFormatter *dateFormatter;
	BOOL isRigbuttonPressed;
    NSMutableArray *restoreIdArr;
    NSNumberFormatter * _priceFormatter;
    BOOL ispurchaseCalled,isViewAppeared,isAppFirstTime;
    NSDictionary *deviceNames;
}

@property (nonatomic, strong) NSLocale *priceLocale;
@property (nonatomic, strong) NSMutableArray *sceneItemArray;
@property (nonatomic, strong) NSString *fileBeginsWith;
@property (nonatomic, strong) IBOutlet iCarousel *carousel;
@property (weak, nonatomic) IBOutlet UIButton *addSceneButton;
@property (weak, nonatomic) IBOutlet UIButton *deleteSceneButton;
@property (weak, nonatomic) IBOutlet UIView *pageTitleBG;
@property (weak, nonatomic) IBOutlet UILabel *pageTitle;
@property (weak, nonatomic) IBOutlet UILabel *amountLabel;
@property (weak, nonatomic) IBOutlet UIButton *cloneSceneButton;
@property (weak, nonatomic) IBOutlet UIButton *feedbackButton;
@property (weak, nonatomic) IBOutlet UISearchBar *searchBar;
@property (weak, nonatomic) IBOutlet UIButton *playvideo;
@property (weak, nonatomic) IBOutlet UIView *addBTView;
@property (weak, nonatomic) IBOutlet UIView *addBTiconView;
@property (weak, nonatomic) IBOutlet UIView *cloneBTView;
@property (weak, nonatomic) IBOutlet UIView *cloneBTiconView;
@property (weak, nonatomic) IBOutlet UIView *deleteBTView;
@property (weak, nonatomic) IBOutlet UIView *deleteBTiconView;
@property (weak, nonatomic) IBOutlet UIView *feedbackBTView;
@property (weak, nonatomic) IBOutlet UIView *feedbackBTiconView;
@property (weak, nonatomic) IBOutlet UIButton *helpButton;
@property (weak, nonatomic) IBOutlet UIButton *rigViewButton;
@property (strong, nonatomic) MPMoviePlayerController *moviePlayer;
@property (nonatomic,weak) IBOutlet UIActivityIndicatorView *activityViewForOBJImporter;
@property (nonatomic,weak) IBOutlet UIActivityIndicatorView *loadingView;
@property (weak, nonatomic) IBOutlet UIView *rateBTiconView;

- (IBAction)addSceneButtonAction:(id)sender;
- (IBAction)deleteSceneButtonAction:(id)sender;
- (IBAction)cloneSceneButtonAction:(id)sender;
- (IBAction)feedbackButtonAction:(id)sender;
- (IBAction)playButtonAction:(id)sender;
- (IBAction)helpButtonAction:(id)sender;
- (IBAction)rateMeButtonAction:(id)sender;
- (IBAction)rigViewButtonAction:(id)sender;
- (void)placeView:(UIView*)theView fromValue:(CGPoint)fromPoint toValue:(CGPoint)toPoint active:(BOOL)setValue;
- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil SceneNo:(int)no isAppFirstTime:(BOOL)FirstTime;

@end
