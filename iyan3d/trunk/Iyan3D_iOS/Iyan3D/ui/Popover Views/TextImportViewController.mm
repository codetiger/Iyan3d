//
//  TextImportViewController.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 08/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import <iostream>
#import "TextImportViewController.h"
#import "Utility.h"
#import "PreviewHelper.h"
#import "SGPreviewScene.h"
#import "DownloadTask.h"

#define CANCEL_PREMIUM_NOT_PURCHASE 50
#define CANCEL_BUTTON_INDEX 0
#define OK_BUTTON_ACTION 1
#define FONT_STORE 0
#define MY_FONT 1

PreviewHelper* textPreviewHelper;
SGPreviewScene* textPreviewScene;

@implementation TextImportViewController

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    cache = [CacheSystem cacheSystem];
    downloadQueue = [[NSOperationQueue alloc] init];
    [downloadQueue setMaxConcurrentOperationCount:3];

    return self;
}
- (void)viewDidLoad
{
    [super viewDidLoad];
    pendingLoading = false;
    firstFrame = true;
    fontArray = [cache GetAssetList:FONT Search:@""];
    [self.loadingView setHidden:YES];
    [self.collectionviewEmpty setHidden:YES];
    [self initializeFontListArray];
    [self setUpView];
    typedText = [NSString stringWithFormat:@"Text"];

    cache = [CacheSystem cacheSystem];

    if ([[AppHelper getAppHelper] userDefaultsForKey:@"fontDetails"]) {
        NSDictionary* fontDetails = [[AppHelper getAppHelper] userDefaultsForKey:@"fontDetails"];
        fontSize = [[fontDetails objectForKey:@"size"] intValue];
        fontFileName = [fontDetails objectForKey:@"font"];
        red = [[fontDetails objectForKey:@"red"] floatValue];
        green = [[fontDetails objectForKey:@"green"] floatValue];
        blue = [[fontDetails objectForKey:@"blue"] floatValue];
        alpha = [[fontDetails objectForKey:@"alpha"] floatValue];
        bevelRadius = [[fontDetails objectForKey:@"bevel"] floatValue];
        tabValue = [[fontDetails objectForKey:@"tabValue"] intValue];

        [self.fontTab setSelectedSegmentIndex:[[fontDetails objectForKey:@"tabValue"] integerValue]];
        if (tabValue == MY_FONT) {
            [[AppHelper getAppHelper] saveToUserDefaults:fontFileName withKey:@"My_Font_Array"];
            if ([fontArray count] > 0) {
                AssetItem* assetItem = [fontArray objectAtIndex:0];
                [[AppHelper getAppHelper] saveToUserDefaults:assetItem.name withKey:@"Font_Store_Array"];
            }
        }
        else {
            [[AppHelper getAppHelper] saveToUserDefaults:fontFileName withKey:@"Font_Store_Array"];
            if ([fontListArray count] != 0)
                [[AppHelper getAppHelper] saveToUserDefaults:[fontListArray objectAtIndex:0] withKey:@"My_Font_Array"];
        }
    }
    else {
        if ([fontArray count] > 0) {
            AssetItem* assetItem = [fontArray objectAtIndex:0];
            fontSize = 20;
            bevelRadius = 0;
            fontFileName = [NSString stringWithFormat:@"%@", assetItem.name];
            tabValue = FONT_STORE;
            red = green = blue = alpha = 0.5;
            [[AppHelper getAppHelper] saveToUserDefaults:fontFileName withKey:@"Font_Store_Array"];
        }
    }
    self.bevelSlider.value = bevelRadius;
    self.bevelValueDisplay.text = [NSString stringWithFormat:@"%d", bevelRadius];
    hideableSubViews = [[NSMutableArray alloc] init];
    [hideableSubViews addObject:self.sizePickerView];
    [hideableSubViews addObject:self.colorView];
    UITapGestureRecognizer* tapRecognizer = [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapGesture:)];
    tapRecognizer.delegate = self;
    [self.renderView addGestureRecognizer:tapRecognizer];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersBG) name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(appEntersFG) name:@"applicationDidBecomeActive" object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(assetsSet) name:@"AssetsSet" object:nil];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AppGoneBackground" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"applicationDidBecomeActive" object:nil];
    [[NSNotificationCenter defaultCenter] removeObserver:self name:@"AssetsSet" object:nil];
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event
{

}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    self.screenName = @"TextImportView";
    [self.fontList reloadData];
    [self.loadingView setHidden:NO];
    if ([fontArray count] == 0) {
        if ([[AppHelper getAppHelper] checkInternetConnected]) {
            [[AppHelper getAppHelper] initHelper];
            [[AppHelper getAppHelper] performReadingJsonInQueue:downloadQueue ForPage:TEXT_VIEW];
        }
        else {
            [self showInternetError:NO_INTERNET];
        }
    }
    else {
        [self loadAllFonts];
    }

    self.fontSizeDisplay.text = [NSString stringWithFormat:@"%d", fontSize];
}

- (void)assetsSet
{
    [self performSelectorOnMainThread:@selector(loadAllFonts) withObject:nil waitUntilDone:NO];
}

- (void)loadAllFonts
{
    [self resetFontsList];
    [[AppHelper getAppHelper] loadAllFontsInQueue:downloadQueue WithDelegate:self AndSelectorMethod:@selector(fontReloadData:)];
    //[self.loadingView setHidden:YES];
}

- (void)resetFontsList
{
    if (tabValue == FONT_STORE)
        fontArray = [cache GetAssetList:FONT Search:@""];

    [self.fontList reloadData];
}

- (void)appEntersBG
{
    isAppEntersBG = true;
}
- (void)appEntersFG
{
    isAppEntersBG = false;
}

- (void)showInternetError:(int)connectionError
{
    if (connectionError != SLOW_INTERNET) {
        [self.view endEditing:YES];
        UIAlertView* internetError = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to connect to the server, Please check your network settings." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [internetError performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}
- (void)dismissKeyboard
{
    [self.textInputField resignFirstResponder];
}
- (void)initializeFontListArray
{
    fontListArray = Nil;
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    fontDirectoryPath = [NSString stringWithFormat:@"%@/Resources/Fonts", docDirPath];
    NSArray* fontExtensions = [NSArray arrayWithObjects:@"ttf", @"otf", nil];
    NSArray* filesGathered;

    if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]) {
        if (![[NSFileManager defaultManager] fileExistsAtPath:fontDirectoryPath]) {
            [[NSFileManager defaultManager] createDirectoryAtPath:fontDirectoryPath withIntermediateDirectories:YES attributes:Nil error:Nil];
        }
        [self copyFontFilesFromDirectory:docDirPath ToDirectory:fontDirectoryPath withExtensions:fontExtensions];
        filesGathered = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:fontDirectoryPath error:Nil];
        fontListArray = [filesGathered filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", fontExtensions]];
    }
}
- (void)copyFontFilesFromDirectory:(NSString*)sourceDir ToDirectory:(NSString*)destinationDir withExtensions:(NSArray*)extensions
{
    NSArray* fontFilesToCopy = [[[NSFileManager defaultManager] contentsOfDirectoryAtPath:sourceDir error:Nil] filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];

    for (NSString* aFile in fontFilesToCopy) {
        NSError* error;
        if (![[NSFileManager defaultManager] fileExistsAtPath:[destinationDir stringByAppendingPathComponent:aFile]]) {
            [[NSFileManager defaultManager] copyItemAtPath:[sourceDir stringByAppendingPathComponent:aFile] toPath:[destinationDir stringByAppendingPathComponent:aFile] error:&error];
        }
        if (error)
            NSLog(@" Error copying font files %@ due to %@", error.localizedDescription, error.localizedFailureReason);
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    if ([Utility IsPadDevice]) {
        [self.view.layer setMasksToBounds:YES];
    }
}
- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}
- (BOOL)textField:(UITextField*)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString*)string
{
    NSUInteger newLength = [textField.text length] + [string length] - range.length;
    return (newLength > MAXTEXTLENGTH) ? NO : YES;
}
- (void)scrollToSelectedFont
{
    if (tabValue == MY_FONT) {
        if ([fontListArray count] > 0)
            highlightedFontId = [NSIndexPath indexPathForItem:[fontListArray indexOfObject:fontFileName] inSection:0];
        if (highlightedFontId > 0 && [fontListArray count] > highlightedFontId.row)
            [self.fontList scrollToItemAtIndexPath:highlightedFontId atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    }
    else if (tabValue == FONT_STORE) {
        for (int i = 0; i < [fontArray count]; i++) {
            AssetItem* assetItem = [fontArray objectAtIndex:i];
            if ([assetItem.name isEqualToString:fontFileName])
                highlightedFontId = [NSIndexPath indexPathForItem:i inSection:0];
        }
        if (highlightedFontId > 0 && [fontArray count] > highlightedFontId.row)
            [self.fontList scrollToItemAtIndexPath:highlightedFontId atScrollPosition:UICollectionViewScrollPositionCenteredHorizontally animated:NO];
    }
    self.fontNameDisplay.text = [fontFileName stringByDeletingPathExtension];
    if (self.isViewLoaded && self.view.window)
        [self load3DText];
}

- (void)fontReloadData:(id)returnValue
{
    NSString* assetName;
    if ([returnValue isKindOfClass:[DownloadTask class]]) {
        assetName = ((DownloadTask*)returnValue).returnObj;
    }
    else
        assetName = returnValue;

    if (!self.loadingView.isHidden && [fontFileName isEqualToString:assetName])
        [self load3DText];

    [self.fontList reloadData];

    AssetItem* asset = [cache GetAssetByName:assetName];

    if ([fontArray lastObject] == asset) {
        [self scrollToSelectedFont];
    }
}

- (void)downloadFile:(NSString*)url FileName:(NSString*)fileName
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName])
        return;

    NSData* data = [NSData dataWithContentsOfURL:[NSURL URLWithString:url]];
    if (data)
        [data writeToFile:fileName atomically:YES];
}
- (void)initializeColorWheel
{
    UIImage* theImage = [UIImage imageNamed:@"wheel2.png"];
    _demoView = [[GetPixelDemo alloc] initWithFrame:CGRectMake(10, 10, 180, 180) image:[ANImageBitmapRep imageBitmapRepWithImage:theImage]];
    _demoView.delegate = self;
    [self.colorView addSubview:_demoView];
}
- (void)pixelDemoGotPixel:(BMPixel)pixel
{
    red = pixel.red;
    green = pixel.green;
    blue = pixel.blue;
    alpha = pixel.alpha;

    [self.colorPreview setBackgroundColor:[UIColor colorWithRed:pixel.red green:pixel.green blue:pixel.blue alpha:pixel.alpha]];
    [self.fontList reloadData];
}

- (void)pixelDemoTouchEnded:(BMPixel)pixel
{
    [self.loadingView setHidden:NO];
    [self.loadingView startAnimating];

    red = pixel.red;
    green = pixel.green;
    blue = pixel.blue;
    alpha = pixel.alpha;
    [self.colorPreview setBackgroundColor:[UIColor colorWithRed:pixel.red green:pixel.green blue:pixel.blue alpha:pixel.alpha]];
    [self load3DText];
    [self hideAllSubViewsExcept:nil];
    [self.fontList reloadData];
}

- (void)showPremiumView
{
    if ([Utility IsPadDevice]) {
        upgradeView = [[PremiumUpgardeVCViewController alloc] initWithNibName:@"PremiumUpgardeVCViewController" bundle:nil];
        upgradeView.delegate = self;
        upgradeView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:upgradeView animated:YES completion:nil];
        upgradeView.view.superview.backgroundColor = [UIColor clearColor];
        upgradeView.view.layer.borderWidth = 2.0f;
        upgradeView.view.layer.borderColor = [UIColor grayColor].CGColor;
    }
    else {
        upgradeView = [[PremiumUpgardeVCViewController alloc] initWithNibName:@"PremiumUpgradeViewControllerPhone" bundle:nil];
        upgradeView.delegate = self;
        upgradeView.modalPresentationStyle = UIModalPresentationFormSheet;
        [self presentViewController:upgradeView animated:YES completion:nil];
    }
}

- (void)loadFont:(NSString*)customFontPath withExtension:(NSString*)extension
{
    CGDataProviderRef fontDataProvider = CGDataProviderCreateWithFilename([customFontPath UTF8String]);

    // Create the font with the data provider, then release the data provider.
    customFont = CGFontCreateWithDataProvider(fontDataProvider);
    CGDataProviderRelease(fontDataProvider);

    customFontName = (NSString*)CFBridgingRelease(CGFontCopyPostScriptName(customFont));
    CTFontManagerRegisterGraphicsFont(customFont, Nil);
}
- (IBAction)textViewTouch:(id)sender
{
    [self hideAllSubViewsExcept:nil];
}
- (IBAction)addButtonAction:(id)sender
{
    NSString* name = [self.textInputField.text stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];

    AssetItem* selectedAsset;
    if (tabValue == FONT_STORE && selectedIndex < fontArray.count) {
        selectedAsset = fontArray[selectedIndex];
    }
    if (tabValue == FONT_STORE && selectedAsset && [selectedAsset.iap isEqualToString:@"0"]) {
        [self addSelectedTextToScene:name];
    }
    else if (tabValue == MY_FONT) {
        [self addSelectedTextToScene:name];
    }
    else if (tabValue == FONT_STORE && selectedAsset && ![selectedAsset.iap isEqualToString:@"0"]) {
        if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"])
            [self addSelectedTextToScene:name];
        else {
            tempSelectedIndex = selectedIndex;
            [self showPremiumView];
        }
    }
    else
        [self addSelectedTextToScene:name];
}

- (void)addSelectedTextToScene:(NSString*)name
{
    if (name && [name length]) {
        std::string textToAdd = std::string([name UTF8String]);

        //NSString * fontFileAbsolutePath =[fontDirectoryPath stringByAppendingPathComponent:fontFileName];
        NSDictionary* fontDetails = [NSDictionary dictionaryWithObjectsAndKeys:name, @"text", [NSNumber numberWithFloat:fontSize], @"size", [NSNumber numberWithFloat:red], @"red", [NSNumber numberWithFloat:blue], @"blue", [NSNumber numberWithFloat:green], @"green", [NSNumber numberWithFloat:alpha], @"alpha", [NSNumber numberWithInt:bevelRadius], @"bevel", fontFileName, @"font", [NSNumber numberWithInt:tabValue], @"tabValue", nil];

        if ([fontDetails objectForKey:@"text"] != nil)
            [[AppHelper getAppHelper] saveToUserDefaults:fontDetails withKey:@"fontDetails"];

        //Crash Fixed
        if (tabValue == FONT_STORE && [fontArray count] > selectedIndex) {
            AssetItem* assetItem = fontArray[selectedIndex];
            [cache AddDownloadedAsset:assetItem];
        }

        [self performSelectorOnMainThread:@selector(addAndDismissView:) withObject:fontDetails waitUntilDone:YES];
    }
    else {
        [self.view endEditing:YES];
        UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Field Empty" message:@"Please enter some text to add." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [errorAlert show];
    }
}
- (void)addAndDismissView:(NSDictionary*)fontDetails
{
    [self destroyDisplayLink];
    [textPreviewHelper removeEngine];
    [self.delegate textAddCompleted:fontDetails];

    [self dismissViewControllerAnimated:YES completion:nil];
    [self dismissViewControllerAnimated:YES completion:nil];
}
- (IBAction)cancelButtonAction:(id)sender
{
    NSDictionary* fontDetails = [NSDictionary dictionaryWithObjectsAndKeys:self.textInputField.text, @"text", [NSNumber numberWithFloat:fontSize], @"size", [NSNumber numberWithFloat:red], @"red", [NSNumber numberWithFloat:blue], @"blue", [NSNumber numberWithFloat:green], @"green", [NSNumber numberWithFloat:alpha], @"alpha", [NSNumber numberWithInt:bevelRadius], @"bevel", fontFileName, @"font", [NSNumber numberWithInt:tabValue], @"tabValue", nil];
    if ([fontDetails objectForKey:@"text"] != nil)
        [[AppHelper getAppHelper] saveToUserDefaults:fontDetails withKey:@"fontDetails"];
    [self destroyDisplayLink];
    [textPreviewHelper removeEngine];
    [self.delegate resetContext];
    [self dismissViewControllerAnimated:YES completion:nil];
}
- (IBAction)fontSizeChangeButtonAction:(id)sender
{
    [self dismissKeyboard];
    [self.sizePickerView setHidden:(!self.sizePickerView.hidden)];
    [self hideAllSubViewsExcept:self.sizePickerView];
}
- (IBAction)colorViewShowButtonAction:(id)sender
{
    [self dismissKeyboard];
    [self.colorView setHidden:(!self.colorView.hidden)];
    [self hideAllSubViewsExcept:self.colorView];
    self.colorPreview.backgroundColor = [UIColor colorWithRed:red green:green blue:blue alpha:alpha];
}
- (BOOL)textViewShouldBeginEditing:(UITextView*)textView
{
    [self hideAllSubViewsExcept:textView];
    return YES;
}
- (IBAction)textFieldDidChange:(id)sender
{
    [self hideAllSubViewsExcept:self.textInputField];
    if (self.textInputField.text.length > 160) {
        return;
    }
    self.textLength.text = [NSString stringWithFormat:@"%lu", (unsigned long)self.textInputField.text.length];
    if (self.textInputField.text.length != 0)
        typedText = self.textInputField.text;
    else
        typedText = @"Text";

    [self.fontList reloadData];
}
- (void)textFieldDidEndEditing:(UITextField*)textField
{
    [self performSelectorInBackground:@selector(loadingViewUI) withObject:nil];
    if (displayTimer && textPreviewScene)
        [self load3DText];
}
- (void)loadingViewUI
{
    [self.loadingView setHidden:NO];
    [self.loadingView startAnimating];
}
- (BOOL)textFieldShouldReturn:(UITextField*)textField
{
    [textField resignFirstResponder];
    return YES;
}
- (IBAction)bevelSliderChanged:(id)sender
{
    [self.loadingView setHidden:NO];
    [self.loadingView startAnimating];
    bevelRadius = (int)self.bevelSlider.value;
    self.bevelValueDisplay.text = [NSString stringWithFormat:@"%d", bevelRadius];
}

- (void)bevelSliderChangeEnded
{
    [self load3DText];
}

- (IBAction)fontTabChanged:(id)sender
{
    //tabValue = (int)self.fontTab.selectedSegmentIndex;
    if ((int)self.fontTab.selectedSegmentIndex == MY_FONT) {
        if ([[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]) {
            [self initializeFontListArray];
            if ([fontListArray count] != 0) {
                [self.loadingView setHidden:NO];
                [self.loadingView startAnimating];
                if (![[AppHelper getAppHelper] userDefaultsForKey:@"My_Font_Array"])
                    [[AppHelper getAppHelper] saveToUserDefaults:[fontListArray objectAtIndex:0] withKey:@"My_Font_Array"];
                [self.fontTab setSelectedSegmentIndex:MY_FONT];
                tabValue = MY_FONT;
                [self.fontList reloadData];
            }
            else {
                [self.collectionviewEmpty setHidden:NO];
                tabValue = MY_FONT;
                [self.fontList reloadData];
                [self performSelectorOnMainThread:@selector(remove3DText) withObject:nil waitUntilDone:YES];
            }
            fontFileName = [[AppHelper getAppHelper] userDefaultsForKey:@"My_Font_Array"];
        }
        else {
            [self.view endEditing:YES];
            UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"INFORMATIOM" message:@"To Access this option need to buy a premium offer.Do you want to buy premium" delegate:self cancelButtonTitle:@"NO" otherButtonTitles:@"YES", nil];
            [errorAlert setTag:CANCEL_PREMIUM_NOT_PURCHASE];
            [errorAlert show];
        }
    }
    else {
        [self.collectionviewEmpty setHidden:YES];
        [self.fontTab setSelectedSegmentIndex:FONT_STORE];
        tabValue = FONT_STORE;
        [self.fontList reloadData];
        fontFileName = [[AppHelper getAppHelper] userDefaultsForKey:@"Font_Store_Array"];
        [self performSelectorOnMainThread:@selector(remove3DText) withObject:nil waitUntilDone:YES];
    }
    self.fontNameDisplay.text = [fontFileName stringByDeletingPathExtension];
    [self load3DText];
}

#pragma collectionViewDelegate

- (NSInteger)collectionView:(UICollectionView*)collectionView numberOfItemsInSection:(NSInteger)section
{
    if (tabValue == FONT_STORE) {
        return [fontArray count];
    }
    else {
        if ([fontListArray count] == 0) {
            [self.collectionviewEmpty setHidden:NO];
            [self.addButton setEnabled:NO];
        }
        else
            [self.addButton setEnabled:YES];
        return [fontListArray count];
    }
}
- (FontCellView*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    FontCellView* cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
    if ([Utility IsPadDevice])
        cell.layer.cornerRadius = 10.0;
    else
        cell.layer.cornerRadius = 8.0;
    cell.layer.borderWidth = 1.0f;
    cell.layer.borderColor = [UIColor grayColor].CGColor;
    cell.assetNameLabel.textColor = [UIColor whiteColor];
    cell.displayText.text = typedText;
    cell.backgroundColor = [UIColor clearColor];
    if (tabValue == FONT_STORE) {
        AssetItem* assetItem = fontArray[indexPath.row];
        cell.assetNameLabel.text = [assetItem.name stringByDeletingPathExtension];
        NSString* fileName = [NSString stringWithFormat:@"%@/%@", cacheDirectory, assetItem.name];
        if (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
            [cell.loadingView setHidden:NO];
            [cell.displayText setHidden:YES];
        }
        else {
            [self loadFont:fileName withExtension:[fontFileName pathExtension]];
            cell.displayText.font = [UIFont fontWithName:customFontName size:fontSize];

            CGFontRelease(customFont);
            cell.displayText.textColor = [UIColor colorWithRed:red green:green blue:blue alpha:alpha];
            [cell.loadingView setHidden:YES];
            [cell.displayText setHidden:NO];
        }
        if ([assetItem.iap isEqualToString:@"0"]) {
            cell.premiumicon.hidden = YES;
        }
        else {
            cell.premiumicon.hidden = NO;
            cell.freeTagLabel.text = @"PREMIUM";
        }
        if ([[[AppHelper getAppHelper] userDefaultsForKey:@"Font_Store_Array"] isEqualToString:assetItem.name]) {
            cell.backgroundColor = [UIColor colorWithRed:37.0f / 255.0f green:37.0f / 255.0f blue:37.0f / 255.0f alpha:1.0f];
            selectedIndex = (int)indexPath.row;
            BOOL loadingViewIsHidden = cell.loadingView.isHidden;
            if (loadingViewIsHidden == false)
                [self.addButton setEnabled:NO];
            else
                [self.addButton setEnabled:YES];
        }
        [cell.freeTagLabel setHidden:YES];
        cell.freeTagLabel.textColor = [UIColor whiteColor];
    }
    else {
        cell.premiumicon.hidden = YES;
        [cell.freeTagLabel setHidden:YES];
        cell.assetNameLabel.text = [fontListArray[indexPath.row] stringByDeletingPathExtension];
        NSString* fileName = [NSString stringWithFormat:@"%@/%@", fontDirectoryPath, fontListArray[indexPath.row]];
        if (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
            [cell.loadingView setHidden:NO];
            [cell.displayText setHidden:YES];
        }
        else {
            [self loadFont:fileName withExtension:[fontFileName pathExtension]];
            cell.displayText.font = [UIFont fontWithName:customFontName size:fontSize];
            cell.displayText.textColor = [UIColor colorWithRed:red green:green blue:blue alpha:alpha];
            CGFontRelease(customFont);
            [cell.loadingView setHidden:YES];
            [cell.displayText setHidden:NO];
        }
        if ([[[AppHelper getAppHelper] userDefaultsForKey:@"My_Font_Array"] isEqualToString:fontListArray[indexPath.row]]) {
            cell.backgroundColor = [UIColor colorWithRed:37.0f / 255.0f green:37.0f / 255.0f blue:37.0f / 255.0f alpha:1.0f];
            selectedIndex = (int)indexPath.row;
            BOOL loadingViewIsHidden = cell.loadingView.isHidden;
            if (loadingViewIsHidden == false)
                [self.addButton setEnabled:NO];
            else
                [self.addButton setEnabled:YES];
        }
    }
    return cell;
}
- (void)collectionView:(UICollectionView*)collectionView didSelectItemAtIndexPath:(NSIndexPath*)indexPath
{
    [self.loadingView setHidden:NO];
    [self.loadingView startAnimating];
    bevelRadius = 0;
    self.bevelSlider.value = 0;
    self.bevelValueDisplay.text = [NSString stringWithFormat:@"%d", bevelRadius];
    UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
    if (tabValue == FONT_STORE) {
        AssetItem* assetItem = fontArray[indexPath.row];
        /*
        if (![assetItem.iap isEqual: @"0"]) {
            if(![[AppHelper getAppHelper] userDefaultsBoolForKey:@"premiumUnlocked"]){
                tempSelectedIndex = (int) indexPath.row;
                [self showPremiumView];
            }
            else{
                [self unselectAll];
                cell.backgroundColor = [UIColor colorWithRed:150.0f/255.0f green:150.0f/255.0f blue:150.0f/255.0f alpha:1.0f];
                fontFileName =assetItem.name;
                selectedIndex = (int)indexPath.row;
                [[AppHelper getAppHelper] saveToUserDefaults:fontFileName withKey:@"Font_Store_Array"];
            }
        }
         */
        [self unselectAll];
        cell.backgroundColor = [UIColor colorWithRed:37.0f / 255.0f green:37.0f / 255.0f blue:37.0f / 255.0f alpha:1.0f];
        fontFileName = assetItem.name;
        selectedIndex = (int)indexPath.row;
        self.fontNameDisplay.text = [fontFileName stringByDeletingPathExtension];
        [[AppHelper getAppHelper] saveToUserDefaults:fontFileName withKey:@"Font_Store_Array"];
    }
    else {
        [self unselectAll];
        cell.backgroundColor = [UIColor colorWithRed:37.0f / 255.0f green:37.0f / 255.0f blue:37.0f / 255.0f alpha:1.0f];
        fontFileName = [NSString stringWithFormat:@"%@", fontListArray[indexPath.row]];
        self.fontNameDisplay.text = [fontFileName stringByDeletingPathExtension];
        [[AppHelper getAppHelper] saveToUserDefaults:fontFileName withKey:@"My_Font_Array"];
    }
    if (((FontCellView*)cell).loadingView.isHidden)
        [self performSelectorOnMainThread:@selector(load3DText) withObject:nil waitUntilDone:NO];
    [self.fontList reloadData];
}
- (void)unselectAll
{
    NSArray* indexPathArr = [self.fontList indexPathsForVisibleItems];
    for (int i = 0; i < [indexPathArr count]; i++) {
        NSIndexPath* indexPathValue = [indexPathArr objectAtIndex:i];
        UICollectionViewCell* cell = [self.fontList cellForItemAtIndexPath:indexPathValue];
        cell.backgroundColor = [UIColor clearColor];
    }
}

#pragma PickerView delegate methods

- (void)pickerView:(UIPickerView*)pickerView didSelectRow:(NSInteger)row inComponent:(NSInteger)component
{

    if (pickerView == self.sizePickerView) {
        fontSize = 10 + (int)(2 * row);
        self.fontSizeDisplay.text = [NSString stringWithFormat:@"%d", fontSize];
    }
    [self performSelectorOnMainThread:@selector(load3DText) withObject:nil waitUntilDone:NO];
    [self hideAllSubViewsExcept:nil];
    [self.fontList reloadData];
}
// tell the picker how many rows are available for a given component
- (NSInteger)pickerView:(UIPickerView*)pickerView numberOfRowsInComponent:(NSInteger)component
{
    if (pickerView == self.sizePickerView)
        return 20;
}
// tell the picker how many components it will have
- (NSInteger)numberOfComponentsInPickerView:(UIPickerView*)pickerView
{
    return 1;
}
// tell the picker the title for a given component
- (UIView*)pickerView:(UIPickerView*)pickerView viewForRow:(NSInteger)row forComponent:(NSInteger)component reusingView:(UIView*)view
{
    NSString* title;
    //title = [@"" stringByAppendingFormat:@"%lu",row];
    if (pickerView == self.sizePickerView) {
        int fontSizeInt = 10 + (int)(2 * row);
        title = [NSString stringWithFormat:@"%d", fontSizeInt];
    }

    UILabel* tView = (UILabel*)view;
    if (!tView) {
        tView = [[UILabel alloc] init];
        [tView setTextAlignment:UITextAlignmentCenter];
        tView.numberOfLines = 3;
    }
    // Fill the label text here
    tView.text = title;
    return tView;
}
- (void)hideAllSubViewsExcept:(UIView*)viewToShow
{
    //    if(viewToShow != _textInputView)
    //        [_textInputView resignFirstResponder];

    for (UIView* subView in hideableSubViews) {
        if (subView != viewToShow)
            [subView setHidden:YES];
    }
}

#pragma PremiumUpgrade delegate methods
- (void)upgradeButtonPressed
{
}
- (void)statusForOBJImport:(NSNumber*)object
{
}
- (void)loadingViewStatus:(BOOL)status
{
}
- (void)premiumUnlocked
{
    [self performSelectorOnMainThread:@selector(remove3DText) withObject:nil waitUntilDone:YES];

    if (tabValue == MY_FONT) {
        [self.fontTab setSelectedSegmentIndex:MY_FONT];
        [self initializeFontListArray];
        [self.fontList reloadData];
        if ([fontListArray count] != 0) {
            fontFileName = [fontListArray objectAtIndex:0];
            [[AppHelper getAppHelper] saveToUserDefaults:[fontListArray objectAtIndex:0] withKey:@"My_Font_Array"];
        }
    }
    else {
        selectedIndex = tempSelectedIndex;
        AssetItem* assetItem = fontArray[selectedIndex];
        fontFileName = assetItem.name;
        [[AppHelper getAppHelper] saveToUserDefaults:fontFileName withKey:@"Font_Store_Array"];
        //[self.fontList reloadData];
        [self addButtonAction:nil];
    }
}
- (void)premiumUnlockedCancelled
{
    if (tabValue == MY_FONT) {
        tabValue = FONT_STORE;
        [self.collectionviewEmpty setHidden:YES];
        [self.fontTab setSelectedSegmentIndex:FONT_STORE];
    }
    [self.fontList reloadData];
}
#pragma end

#pragma Alertview Delegate

- (void)alertView:(UIAlertView*)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    switch (alertView.tag) {
    case CANCEL_PREMIUM_NOT_PURCHASE: {
        if (buttonIndex == CANCEL_BUTTON_INDEX) {
            [self.collectionviewEmpty setHidden:YES];
            [self.fontTab setSelectedSegmentIndex:FONT_STORE];
            tabValue = FONT_STORE;
            [self.fontList reloadData];
        }
        else if (buttonIndex == OK_BUTTON_ACTION) {
            tabValue = MY_FONT;
            [self showPremiumView];
        }
    }
    }
}

#pragma end

#pragma gestureAction

- (void)tapGesture:(UITapGestureRecognizer*)rec
{
    [self hideAllSubViewsExcept:nil];
    [self dismissKeyboard];
}

#pragma UIView related

- (void)setUpView
{
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    cacheDirectory = [paths objectAtIndex:0];

    if ([Utility IsPadDevice]) {
        [self.fontList registerNib:[UINib nibWithNibName:@"FontCellView" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    else {
        [self.fontList registerNib:[UINib nibWithNibName:@"FontCellViewPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }

    self.sizePickerView.layer.cornerRadius = 5.0;
    self.sizePickerView.layer.borderWidth = 2.0f;
    self.sizePickerView.layer.borderColor = [UIColor colorWithRed:67.0f / 255.0f green:68.0f / 255.0f blue:67.0f / 255.0f alpha:1.0f].CGColor;
    self.colorView.layer.cornerRadius = 5.0;
    self.colorView.layer.borderWidth = 2.0f;
    self.colorView.layer.borderColor = [UIColor colorWithRed:67.0f / 255.0f green:68.0f / 255.0f blue:67.0f / 255.0f alpha:1.0f].CGColor;
    //    self.fontList.layer.cornerRadius = 5.0;
    //    self.fontList.layer.borderWidth = 2.0f;
    //    self.fontList.layer.borderColor=[UIColor colorWithRed:67.0f/255.0f green:68.0f/255.0f blue:67.0f/255.0f alpha:1.0f].CGColor;
    self.addButton.layer.cornerRadius = 5.0;
    self.cancelButton.layer.cornerRadius = 5.0;
    [self initializeColorWheel];
    [self.sizePickerView setHidden:YES];
    [self.colorView setHidden:YES];
    [self.bevelSlider addTarget:self action:@selector(bevelSliderChangeEnded) forControlEvents:UIControlEventTouchUpInside | UIControlEventTouchUpOutside];

    textPreviewHelper = [[PreviewHelper alloc] init];
    textPreviewScene = [textPreviewHelper initScene:self.renderView viewType:ASSET_SELECTION];
    [self createDisplayLink];
    [textPreviewHelper addCameraLight];
}

- (void)createDisplayLink
{
    displayTimer = [NSTimer scheduledTimerWithTimeInterval:1.0f / 24.0f target:self selector:@selector(updateRenderer) userInfo:nil repeats:YES];
    [[NSRunLoop mainRunLoop] addTimer:displayTimer forMode:NSDefaultRunLoopMode];
    /*
    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(updateRenderer)];
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
     */
}
- (void)destroyDisplayLink
{
    if (displayTimer) {
        [displayTimer invalidate];
        displayTimer = nil;
    }
}
- (void)updateRenderer
{
    if (isAppEntersBG || !self.isViewLoaded)
        return;
    @synchronized(textPreviewHelper)
    {
        @autoreleasepool
        {
            textPreviewScene->renderAll();
            
            if(firstFrame) {
                renderingThread = [NSThread currentThread];
                firstFrame = false;
            }
            
            if(pendingLoading) {
                [self load3DText];
                pendingLoading = false;
            }
        }
        [textPreviewHelper presentRenderBuffer];
    }
}

- (void)load3DText
{
    if([NSThread currentThread] == renderingThread)
        [self load3DTextInRenderingThread];
    else if (renderingThread)
        [self performSelector:@selector(load3DTextInRenderingThread) onThread:renderingThread withObject:nil waitUntilDone:YES];
    else
        pendingLoading = true;
    
    [self.loadingView setHidden:YES];
}

- (void) load3DTextInRenderingThread
{
    if([NSThread currentThread] != renderingThread)
        NSLog(@"loading in different thread");
    
    if (self.isViewLoaded && self.view.window) {
        if (fontFileName != nil) {
            [self remove3DText];
            @synchronized(textPreviewHelper)
            {
                [textPreviewHelper loadNodeInScene:FONT AssetId:0 AssetName:[self getwstring:typedText] FontSize:fontSize BevelValue:bevelRadius TextColor:Vector4(red, green, blue, alpha) FontPath:[fontFileName UTF8String]];
            }
        }
    }
}

- (void)remove3DText
{
    if([NSThread currentThread] == renderingThread)
        [self remove3DTextInRenderingThread];
    else if (renderingThread)
        [self performSelector:@selector(remove3DTextInRenderingThread) onThread:renderingThread withObject:nil waitUntilDone:YES];
}

- (void) remove3DTextInRenderingThread
{
    if([NSThread currentThread] != renderingThread)
        NSLog(@"Removing in different thread");

    @synchronized(textPreviewHelper)
    {
        if (textPreviewScene)
            textPreviewScene->removeObject((textPreviewScene->nodes.size() - 1), false);
    }
}

#if TARGET_RT_BIG_ENDIAN
const NSStringEncoding kEncoding_wchar_t =
CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32BE);
#else
const NSStringEncoding kEncoding_wchar_t =
CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingUTF32LE);
#endif


-(NSString*) stringWithwstring:(const std::wstring&)ws
{
    char* data = (char*)ws.data();
    unsigned size = (int)ws.size() * sizeof(wchar_t);
    
    NSString* result = [[NSString alloc] initWithBytes:data length:size encoding:kEncoding_wchar_t];
    return result;
}

-(std::wstring) getwstring:(NSString*) sourceString
{
    NSData* asData = [sourceString dataUsingEncoding:kEncoding_wchar_t];
    return std::wstring((wchar_t*)[asData bytes], [asData length] / sizeof(wchar_t));
}


#pragma end

- (void)dealloc
{
    [downloadQueue cancelAllOperations];
    for (DownloadTask *task in [downloadQueue operations]) {
            [task cancel];
        }
    
    downloadQueue = nil;
    [[AppHelper getAppHelper] resetAppHelper];
    self.fontList = nil;
    self.addButton = nil;
    self.cancelButton = nil;
    self.textInputField = nil;
    self.fontList = nil;
    self.fontSizeDisplay = nil;
    self.colorViewShowButton = nil;
    self.bevelValueDisplay = nil;
    self.colorView = nil;
    self.bevelSlider = nil;
    self.collectionviewEmpty = nil;
    self.demoView = nil;
    self.sizePickerView = nil;
    self.colorPreview = nil;
    self.textLength = nil;
    self.fontTab = nil;
    self.loadingView = nil;
    cache = nil;
    customFont = nil;
    upgradeView = nil;
}

@end
