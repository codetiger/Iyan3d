//
//  AssetDetailsViewControllerPad.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 06/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "AssetDetailsViewControllerPad.h"
#import "AssetSelectionViewControllerPad.h"
#import "AssetItem.h"
#import "AnimationEditorViewControllerPad.h"
#import "Utility.h"
#import "Constants.h"
#define BUTTON_TAG 3
#define CANCEL_BUTTON_INDEX 0
#define OK_BUTTON_INDEX 1
#define CLOSE_CURRENT_VIEW 10

@implementation AssetDetailsViewControllerPad

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil asset:(int)assetId storeType:(int) type isFromAnimation:(BOOL)animation
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        isAnimationFrom = animation;
        cache = [CacheSystem cacheSystem];
        _assetId = assetId;
	    asset = [cache GetAsset:_assetId];
		storeType = type;
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
	}
    return self;
}
- (void)viewDidLoad
{
    [super viewDidLoad];
    if([Utility IsPadDevice]){
        self.view.layer.borderWidth = 2.0f;
        self.view.layer.cornerRadius = 5.0;
        self.view.layer.borderColor = [UIColor grayColor].CGColor;
    }
    self.cancelButton.layer.cornerRadius = 5.0;
    self.addToSceneButton.layer.cornerRadius = 5.0;
    [self.downloadingActivityView setHidden:YES];
    AssetItem* assetItem = [cache GetAsset:_assetId];
    self.assetNameLabel.text = assetItem.name;
    //[self.assetImageView setImageInfo:[NSString stringWithFormat:@"%d",_assetId] forView:ASSET_DETAIL OperationQueue];
    self.view.backgroundColor = [UIColor blackColor];
}
- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}
- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
	if([Utility IsPadDevice]) {
		[self.view.layer setMasksToBounds:YES];
	}
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}
- (IBAction)cancelButtonAction:(id)sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
}
- (IBAction)addToSceneButtonAction:(id)sender
{
        [self addanimationToScene];
}
- (void) alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    switch(alertView.tag)
    {
        case BUTTON_TAG:
        {
            if (buttonIndex == CANCEL_BUTTON_INDEX) {
                
            } else if (buttonIndex == OK_BUTTON_INDEX) {
                [self addanimationToScene];
            }
            break;
        }
        case CLOSE_CURRENT_VIEW:
        {
            if (buttonIndex == CANCEL_BUTTON_INDEX){
                [self dismissViewControllerAnimated:YES completion:^{
                    [self.delegate refreshCollectionView];
                }];;
            }
            break;
        }
    }
}
- (void) addanimationToScene{
    [self.downloadingActivityView setHidden:NO];
    [self.addToSceneButton setHidden:YES];
    [self.downloadingActivityView startAnimating];
    NSInteger length = asset.iap.length;
    // length < 2 item are free assets
    if(length > 2 && storeType >= CHARACTERS && storeType <= ACCESSORIES && ![cache checkDownloadedAsset:_assetId]){
        [self purchaseModel];
    }
    else {
        [self performSelectorInBackground:@selector(downloadAsset) withObject:nil];
    }
}
- (void) addAssetToScene
{
    [self dismissViewControllerAnimated:NO completion:^{
        [self.delegate assetDownloadCompleted:_assetId];
    }];
    
    AssetItem* assetItem = [cache GetAsset:_assetId];
    [cache AddDownloadedAsset:assetItem];
}
- (void) downloadAsset
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cacheDirectory = [paths objectAtIndex:0];
    NSArray *docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docDirectory = [docPaths objectAtIndex:0];
    AssetItem* assetItem = [cache GetAsset:_assetId];
    NSString *fileName, *url;
    
    if(assetItem.type == CHARACTERS) {
        if (![[NSString stringWithFormat:@"%d",_assetId] hasPrefix:@"4"])
        {
            fileName = [NSString stringWithFormat:@"%@/%d.sgr", cacheDirectory, _assetId];
            url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/mesh/%d.sgr", _assetId];
            [self downloadFile:url FileName:fileName];
            
            fileName = [NSString stringWithFormat:@"%@/%d-cm.png", cacheDirectory, _assetId];
            url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/meshtexture/%d.png", _assetId];
            [self downloadFile:url FileName:fileName];
            [self showConnectionError:fileName];
        }else {
            fileName = [NSString stringWithFormat:@"%@/Resources/Rigs/%d.sgr", docDirectory,_assetId];
            if(![[NSFileManager defaultManager] fileExistsAtPath:fileName])
            {
                [cache DeleteTableData:_assetId];
                [self.view endEditing:YES];
                UIAlertView *message = [[UIAlertView alloc] initWithTitle:@"Missing Files" message:@"Some resources are missing. Cannot able to import the model into scene." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
                [message setTag:CLOSE_CURRENT_VIEW];
                [message show];
            }else
                [self showConnectionError:fileName];
        }
    } else if(assetItem.type == BACKGROUNDS || assetItem.type == ACCESSORIES) {
        fileName = [NSString stringWithFormat:@"%@/%d.sgm", cacheDirectory, _assetId];
        url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/mesh/%d.sgm", _assetId];
        [self downloadFile:url FileName:fileName];

        fileName = [NSString stringWithFormat:@"%@/%d-cm.png", cacheDirectory, _assetId];
        url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/meshtexture/%d.png", _assetId];
        [self downloadFile:url FileName:fileName];
        [self showConnectionError:fileName];
    } else if(assetItem.type == ANIMATIONS) {
        fileName = [NSString stringWithFormat:@"%@/%d.sga", cacheDirectory, _assetId];
        url = [NSString stringWithFormat:@"http://iyan3dapp.com/appapi/mesh/%d.sga", _assetId];
        [self downloadFile:url FileName:fileName];
        [self showConnectionError:fileName];
    }
    else if(assetItem.type == OBJFile) {
        fileName = [NSString stringWithFormat:@"%@/Resources/Objs/%d.obj", docDirectory, _assetId];
        if(![[NSFileManager defaultManager] fileExistsAtPath:fileName])
        {
            [cache DeleteTableData:_assetId];
            [self.view endEditing:YES];
            UIAlertView *message = [[UIAlertView alloc] initWithTitle:@"Missing Files" message:@"Some resources are missing. Cannot able to import the model into scene." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [message setTag:CLOSE_CURRENT_VIEW];
            [message show];
        }else
            [self showConnectionError:fileName];
    }
}
- (void) showConnectionError: (NSString *)fileName
{
    if([[NSFileManager defaultManager] fileExistsAtPath:fileName]){
        [self performSelectorOnMainThread:@selector(addAssetToScene) withObject:nil waitUntilDone:NO];
    }
    else{
        [self.view endEditing:YES];
        UIAlertView *message = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to connect to the server, Please check your network settings." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}
- (void) purchaseModel
{
    NSString* iapStr = asset.iap;
    if ([SKPaymentQueue canMakePayments]) {
        [[UIApplication sharedApplication] beginIgnoringInteractionEvents];
        SKPayment *payment = [SKPayment paymentWithProductIdentifier:iapStr];
		[[SKPaymentQueue defaultQueue] addPayment:payment];
	} else {
        [self.view endEditing:YES];
		UIAlertView *message = [[UIAlertView alloc] initWithTitle:@"Warning" message:@"In-App Purchase is Disabled. Check your settings to enable Purchases." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
		
		[message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
		[self.downloadingActivityView setHidden:YES];
		[self.addToSceneButton setHidden:NO];
		[self.downloadingActivityView stopAnimating];
	}
}
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions
{
	for (SKPaymentTransaction *transaction in transactions) {
        switch (transaction.transactionState)
		{
			case SKPaymentTransactionStatePurchased:
                [self performSelectorInBackground:@selector(downloadAsset) withObject:nil];
				[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                [[UIApplication sharedApplication] endIgnoringInteractionEvents];
            break;
			
	 		case SKPaymentTransactionStateFailed:
                [[UIApplication sharedApplication] endIgnoringInteractionEvents];
                if (transaction.error.code != SKErrorPaymentCancelled) {
                    [self.view endEditing:YES];
					UIAlertView *message = [[UIAlertView alloc] initWithTitle:@"Error" message:transaction.error.localizedDescription delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
					[message performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
				}
				[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
				[self.downloadingActivityView setHidden:YES];
				[self.addToSceneButton setHidden:NO];
				[self.downloadingActivityView stopAnimating];
			
                break;
			
			case SKPaymentTransactionStateRestored:
                // [vc PurchaseResponse:2 InAppPurchase:transaction.payment.productIdentifier];
				[[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                break;
            default:
			break;
		}
	}
}
- (void) downloadFile:(NSString*)url FileName:(NSString*)fileName
{
    if([[NSFileManager defaultManager] fileExistsAtPath:fileName])
        return;
    
    NSData *data = [NSData dataWithContentsOfURL:[NSURL URLWithString:url]];
	if(data)
		[data writeToFile:fileName atomically:YES];
}
- (void) dealloc
{
	cache = nil;
	asset = nil;
	[[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
}

@end
