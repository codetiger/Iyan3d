//
//  TextImportViewController.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 08/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#define MAXTEXTLENGTH 160
#import <UIKit/UIKit.h>
#import <CoreText/CoreText.h>
#import "GetPixelDemo.h"
#import "FontCellView.h"
#import "PremiumUpgardeVCViewController.h"
#import "CacheSystem.h"
#import "Constants.h"
#import "RenderingView.h"
#import "SGNode.h"
#import "GAI.h"

@protocol TextImportViewControllerDelegate
    -(void) textAddCompleted:(char*)text ColorRed:(float)red ColorGreen:(float)green ColorBlue:(float)blue;
    -(void) textAddCompleted:(NSDictionary*)textDetails;
    -(void)setPremiumurchaseStatus:(BOOL)status;
-(void) resetContext;
@end

@interface TextImportViewController : GAITrackedViewController<UIGestureRecognizerDelegate, UIPickerViewDelegate,UICollectionViewDataSource,UICollectionViewDelegate,UIPickerViewDataSource,UITextFieldDelegate,GetPixelDemoDelegate,PremiumUpgardeVCViewControllerDelegate>
{
    NSArray *fontListArray;
    NSString *fontFileName;
    NSString *cacheDirectory;
    NSMutableArray *hideableSubViews,*fontArray;
    int fontSize;
    int bevelRadius;
    CGFontRef customFont;
    NSString * customFontName;
    float red,green,blue,alpha;
    CacheSystem *cache;
    NSString* fontDirectoryPath,*typedText;
    NSString *pathForFonts;
    int tabValue,selectedIndex,tempSelectedIndex;
    PremiumUpgardeVCViewController *upgradeView;
    NSIndexPath *highlightedFontId;
    BOOL isAppEntersBG;
    
    BOOL firstFrame;
    NSThread *renderingThread;
    BOOL pendingLoading;
    
    NSOperationQueue *downloadQueue;
    NSTimer *displayTimer;
    CADisplayLink *displayLink;
}

@property (weak, nonatomic) IBOutlet UIButton *addButton;
@property (weak, nonatomic) IBOutlet UIButton *cancelButton;
@property (weak, nonatomic) IBOutlet UITextField *textInputField;
@property (weak, nonatomic) IBOutlet UICollectionView *fontList;

@property (weak, nonatomic) IBOutlet UITextField *fontSizeDisplay;
@property (weak, nonatomic) IBOutlet UIButton *colorViewShowButton;
@property (weak, nonatomic) IBOutlet UILabel *bevelValueDisplay;
@property (weak, nonatomic) IBOutlet UIView *colorView;
@property (weak, nonatomic) IBOutlet UISlider *bevelSlider;
@property (weak, nonatomic) IBOutlet UILabel *collectionviewEmpty;
@property (weak, nonatomic) IBOutlet UILabel *fontNameDisplay;
//@property (weak, nonatomic) IBOutlet UIPickerView *fontPickerView;
@property (strong, nonatomic) GetPixelDemo *demoView;

@property (weak, nonatomic) IBOutlet UIPickerView *sizePickerView;
@property (weak, nonatomic) id <TextImportViewControllerDelegate> delegate;
@property (weak, nonatomic) IBOutlet UIView *colorPreview;
@property (weak, nonatomic) IBOutlet UILabel *textLength;
@property (weak, nonatomic) IBOutlet UISegmentedControl *fontTab;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView *loadingView;
@property (weak, nonatomic) IBOutlet RenderingView *renderView;

- (IBAction) addButtonAction:(id)sender;
- (IBAction) cancelButtonAction:(id)sender;

- (IBAction) fontSizeChangeButtonAction:(id)sender;
- (IBAction) colorViewShowButtonAction:(id)sender;
- (IBAction) fontTabChanged:(id)sender;
- (IBAction) bevelSliderChanged:(id)sender;
- (IBAction) textViewTouch:(id)sender;
@end
