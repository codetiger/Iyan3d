//
//  HelpViewController.h
//  Iyan3D
//
//  Created by crazycubes on 19/08/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#define MORE_HELP_FROM_ANIMATION 2
#define MORE_HELP_FROM_AUTORIG 1

@protocol HelpViewControllerDelegate

@end

@interface HelpViewController :  GAITrackedViewController< HelpViewControllerDelegate>
{
    BOOL pageControlBeingUsed;
    NSInteger ValueType;
    int scrollviewCount;
    NSString *imageName,*displayfilePathforGetImage,*currentLinkValue;
    int playBtnHeight , playBtnWidth , imageYOrigin , xOffset;
    int imageCount;
    NSMutableArray *nameList;
    BOOL isInternetAvailable;
    NSArray* responseDict;
    int addPage;
    int totalPages;
    NSString *extension;
    int currentPage;
    int j;
    float buttonPosition;
    NSMutableArray *linkArray,*imageArray;
    UIActivityIndicatorView *activityIndicator;
}

@property (nonatomic, strong) NSMutableData *responseData;
@property (nonatomic, strong) NSMutableArray *linksArray;
@property (nonatomic, retain) IBOutlet UIScrollView* scrollView;
@property (nonatomic, retain) IBOutlet UIPageControl* pageControl;
@property (nonatomic, retain) IBOutlet UIButton *doneButton;
@property (nonatomic, retain) IBOutlet UIButton *moreInfo;
@property (weak, nonatomic) id <HelpViewControllerDelegate> delegate;
@property (weak,nonatomic) IBOutlet UIActivityIndicatorView *loadingView;

- (IBAction)changePage;
- (IBAction)doneButtonAction:(id)sender;
- (IBAction)moreInfoButtonAction:(id)sender;
- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil CalledFrom:(NSInteger) callerId;
- (void)setImageInfo:(NSString*)name;
- (void)playButtonPressed:(id)sender;
@end