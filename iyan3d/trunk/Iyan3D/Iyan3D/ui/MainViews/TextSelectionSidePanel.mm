//
//  TextSelectionSidePanel.m
//  Iyan3D
//
//  Created by sabish on 19/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "TextSelectionSidePanel.h"
#import "Constants.h"
#import "DownloadTask.h"
#import "Utility.h"

#define CANCEL_PREMIUM_NOT_PURCHASE 50
#define CANCEL_BUTTON_INDEX 0
#define OK_BUTTON_ACTION 1
#define FONT_STORE 0
#define MY_FONT 1

@interface TextSelectionSidePanel ()

@end


@implementation TextSelectionSidePanel

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cache = [CacheSystem cacheSystem];
        downloadQueue = [[NSOperationQueue alloc] init];
        [downloadQueue setMaxConcurrentOperationCount:3];
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    fontArray = [cache GetAssetList:FONT Search:@""];
    [self initializeFontListArray];
    typedText = [NSString stringWithFormat:@"Text"];
    cache = [CacheSystem cacheSystem];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    cacheDirectory = [paths objectAtIndex:0];
    [self.collectionView registerNib:[UINib nibWithNibName:@"TextFrameCell" bundle:nil] forCellWithReuseIdentifier:@"CELL"];

    if ([[AppHelper getAppHelper] userDefaultsForKey:@"fontDetails"]) {
        NSDictionary* fontDetails = [[AppHelper getAppHelper] userDefaultsForKey:@"fontDetails"];
        fontSize = 10;
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
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(assetsSet) name:@"AssetsSet" object:nil];

    self.bevelSlider.value = bevelRadius;
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

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
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
}

- (void)showInternetError:(int)connectionError
{
    if (connectionError != SLOW_INTERNET) {
        [self.view endEditing:YES];
        UIAlertView* internetError = [[UIAlertView alloc] initWithTitle:@"Connection Error" message:@"Unable to connect to the server, Please check your network settings." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [internetError performSelectorOnMainThread:@selector(show) withObject:nil waitUntilDone:YES];
    }
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}


#pragma mark Fonts data handling methods

- (void)assetsSet
{
    [self performSelectorOnMainThread:@selector(loadAllFonts) withObject:nil waitUntilDone:NO];
}

- (void)loadAllFonts
{
    [self resetFontsList];
    [[AppHelper getAppHelper] loadAllFontsInQueue:downloadQueue WithDelegate:self AndSelectorMethod:@selector(fontReloadData:)];
}

- (void)resetFontsList
{
    fontArray = [cache GetAssetList:FONT Search:@""];
    [self.collectionView reloadData];
}

- (void)loadFont:(NSString*)customFontPath withExtension:(NSString*)extension
{
    CGDataProviderRef fontDataProvider = CGDataProviderCreateWithFilename([customFontPath UTF8String]);
    
    customFont = CGFontCreateWithDataProvider(fontDataProvider);
    CGDataProviderRelease(fontDataProvider);
    
    customFontName = (NSString*)CFBridgingRelease(CGFontCopyPostScriptName(customFont));
    CTFontManagerRegisterGraphicsFont(customFont, Nil);
}

#pragma mark Download delegate methods

- (void)fontReloadData:(id)returnValue
{
    NSString* assetName;
    if ([returnValue isKindOfClass:[DownloadTask class]]) {
        assetName = ((DownloadTask*)returnValue).returnObj;
    }
    else
        assetName = returnValue;
    NSLog(@" Font array %lu ", (unsigned long)[fontArray count]);
    [self.collectionView reloadData];
    
}

#pragma mark CollectionView Delegate methods

- (NSInteger)collectionView:(UICollectionView*)collectionView numberOfItemsInSection:(NSInteger)section
{
    return [fontArray count];
}

- (TextFrameCell*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    TextFrameCell* cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
    NSLog(@"TextAssets CollectionView");
    
    cell.layer.cornerRadius = 8.0;
    cell.layer.borderWidth = 1.0f;
    cell.layer.borderColor = [UIColor grayColor].CGColor;
    cell.backgroundColor = [UIColor clearColor];
    cell.displayText.text = typedText;
    
    AssetItem* assetItem = fontArray[indexPath.row];
    cell.fontName.text = [assetItem.name stringByDeletingPathExtension];
    NSString* fileName = [NSString stringWithFormat:@"%@/%@", cacheDirectory, assetItem.name];
    
    if (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        NSLog(@" Font file path %@ ", fileName);
        [cell.progress setHidden:NO];
        [cell.displayText setHidden:YES];
    }
    else {
        [self loadFont:fileName withExtension:[fontFileName pathExtension]];
        cell.displayText.font = [UIFont fontWithName:customFontName size:fontSize];
        
        CGFontRelease(customFont);
        
        cell.displayText.textColor = [UIColor colorWithRed:red green:green blue:blue alpha:alpha];
        [cell.progress setHidden:YES];
        [cell.displayText setHidden:NO];
    }
    if ([[[AppHelper getAppHelper] userDefaultsForKey:@"Font_Store_Array"] isEqualToString:assetItem.name]) {
        cell.backgroundColor = [UIColor colorWithRed:37.0f / 255.0f green:37.0f / 255.0f blue:37.0f / 255.0f alpha:1.0f];
        selectedIndex = (int)indexPath.row;
    }
    return cell;
}


- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    
}

- (IBAction)inputTextChangedAction:(id)sender {
    NSLog(@"Input Text : %@",_inputText.text);
}

- (IBAction)bevalChangeAction:(id)sender {
    NSLog(@"SilderValue %f",_bevelSlider.value);
}

- (IBAction)fontStoreTapChangeAction:(id)sender {
    //NSLog(@"Font Tap : %ld", _fontStoreTab.selectedSegmentIndex);
}

- (IBAction)colorPickerAction:(id)sender {
    NSLog(@"Color Picker");
}

- (IBAction)cancelBtnAction:(id)sender {
    NSLog(@"Cancel Button");
    [_textSelectionDelegate dismissAndHideView];
    [self deallocMem];
}

- (IBAction)addToSceneBtnAction:(id)sender {
    
}

- (void)deallocMem
{
    NSLog(@"Dealloc");
    fontListArray = nil;
    docDirPath = nil;
    fontArray = nil;
    fontDirectoryPath = nil;
    typedText = nil;
    cacheDirectory = nil;
    customFontName = nil;
    fontFileName = nil;
    cache = nil;
    [assetDownloadQueue cancelAllOperations];
    assetDownloadQueue = nil;
    [downloadQueue cancelAllOperations];
    downloadQueue = nil;
    _textSelectionDelegate = nil;
}

@end