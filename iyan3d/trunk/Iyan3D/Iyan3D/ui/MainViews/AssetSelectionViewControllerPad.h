//
//  AssetSelectionViewControllerPad.h
//  Iyan3D
//
//  Created by Harishankar Narayanan on 05/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "CacheSystem.h"
#import "AssetDetailsViewControllerPad.h"
#import "StoreKit/StoreKit.h"
#import "HelpViewController.h"
#import "FPPopoverController.h"
#import "AppHelper.h"
#import "RenderingView.h"
#import "GAI.h"

@protocol AssetSelectionViewControllerPadDelegate

- (void)resetContext;
- (void)loadNodeInScene:(int)assetId;
- (int)getBoneCount;

@end

@interface AssetSelectionViewControllerPad : GAITrackedViewController <UICollectionViewDataSource, UICollectionViewDelegate, UISearchBarDelegate, AssetDetailsViewControllerPadDelegate, HelpViewControllerDelegate, SKPaymentTransactionObserver, AppHelperDelegate> {

    CacheSystem* cache;
    AssetItem* asset;

    BOOL isanimationExist, isAppEntersBG;
    BOOL isMyLibrary;
    BOOL isRestorePurchasePressed, leavingView, addingNode;
    BOOL isAnimationAssetStore, isRestoring;
    BOOL isLoadingAsset;

    int tabValue, previousTabValue;
    int storeType;
    int _assetId, selectedCell;
    int priceIterator;
    int downloadCounter, restoreCounter , transactionCount;

    float screenScale;

    NSString* docDirPath;
    NSMutableArray* assetArray;
    NSMutableArray* restoreIdArr;
    NSNumberFormatter* _priceFormatter;
    NSTimer* displayTimer;
    NSOperationQueue *downloadQueue , *assetDownloadQueue;
    
    BOOL firstFrame;
    NSThread *renderingThread;
    int pendingTaskAssetId;

    CAEAGLLayer* _eaglLayer;
    EAGLContext* _context;
    GLuint _colorRenderBuffer;
    GLuint _depthRenderBuffer;
    GLuint _frameBuffer;
}

@property (nonatomic, strong) CADisplayLink* displayLink;
@property (nonatomic, strong) NSLocale* priceLocale;
@property (weak, nonatomic) IBOutlet UICollectionView* assetCollectionView;
@property (weak, nonatomic) IBOutlet UIButton* characterButton;
@property (weak, nonatomic) IBOutlet UIButton* accessoriesButton;
@property (weak, nonatomic) IBOutlet UIButton* backgroundButton;
@property (weak, nonatomic) IBOutlet UIButton* assetLibrary;
@property (weak, nonatomic) IBOutlet UIButton* animationLibrary;
@property (weak, nonatomic) IBOutlet UIButton* animationButton;
@property (weak, nonatomic) IBOutlet UIButton* restorePurchaseButton;
@property (weak, nonatomic) IBOutlet UIButton* cancelButton;
@property (weak, nonatomic) IBOutlet UIButton* helpButton;
@property (weak, nonatomic) IBOutlet UIButton* addtoScene;
@property (weak, nonatomic) IBOutlet UISearchBar* searchBar;
@property (weak, nonatomic) IBOutlet UIButton* importObj;
@property (weak, nonatomic) IBOutlet UILabel* priceDisplay;
@property (weak, nonatomic) IBOutlet UILabel* assetNameDisplay;
@property (weak, nonatomic) IBOutlet UIView* displayImage;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView* restoreActivityView;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView* downloadAssetsActivity;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView* downloadingActivityView;
@property (weak, nonatomic) id<AssetSelectionViewControllerPadDelegate> delegate;
@property (weak, nonatomic) IBOutlet RenderingView* renderView;

@property (weak, nonatomic) IBOutlet UIView* topBarView;

- (IBAction)restorePurchaseAction:(id)sender;
- (IBAction)cancelButtonAction:(id)sender;
- (IBAction)addButtonAction:(id)sender;
- (IBAction)helpButtonAction:(id)sender;
- (IBAction)tabChangingFunction:(UIButton*)sender;

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil;
- (void)downloadAsset:(AssetItem*)asset;

@end
