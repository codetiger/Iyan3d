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
        NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        docDirPath = [srcDirPath objectAtIndex:0];
        tabValue = FONT_STORE;
        red = green = blue = alpha = 1;
        
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    fontArray = [cache GetAssetList:FONT Search:@""];
    typedText = [NSString stringWithFormat:@"Text"];
    cache = [CacheSystem cacheSystem];
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    cacheDirectory = [paths objectAtIndex:0];
    //[self initializeFontListArray];
    if([Utility IsPadDevice]){
        [self.collectionView registerNib:[UINib nibWithNibName:@"TextFrameCell" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    }
    else
    {
       [self.collectionView registerNib:[UINib nibWithNibName:@"TextFrameCellPhone" bundle:nil] forCellWithReuseIdentifier:@"CELL"]; 
    }
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(assetsSet) name:@"AssetsSet" object:nil];
    self.cancelBtn.layer.cornerRadius=8.0f;
    self.addToScene.layer.cornerRadius=8.0f;
    self.bevelSlider.value = bevelRadius;
    
}

- (void)initializeFontListArray
{
    fontListArray = Nil;
    fontDirectoryPath = [NSString stringWithFormat:@"%@/Resources/Fonts", docDirPath];
    NSArray* fontExtensions = [NSArray arrayWithObjects:@"ttf", @"otf", nil];
    NSArray* filesGathered;
    [self copyFontFilesFromDirectory:docDirPath ToDirectory:fontDirectoryPath withExtensions:fontExtensions];
    filesGathered = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:fontDirectoryPath error:Nil];
    fontListArray = [filesGathered filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", fontExtensions]];
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
    if (tabValue == FONT_STORE) {
        return [fontArray count];
    }
    else {
        return [fontListArray count];
    }
}

- (TextFrameCell*)collectionView:(UICollectionView*)collectionView cellForItemAtIndexPath:(NSIndexPath*)indexPath
{
    TextFrameCell* cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
    cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
    
    if (tabValue == FONT_STORE) {
        AssetItem* assetItem = fontArray[indexPath.row];
        cell.fontName.text = [assetItem.name stringByDeletingPathExtension];
        NSString* fileName = [NSString stringWithFormat:@"%@/%@", cacheDirectory, assetItem.name];
        if (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
            [cell.displayText setHidden:YES];
            [cell.progress setHidden:NO];
            [cell.progress startAnimating];
        }
        else {
            [self loadFont:fileName withExtension:[fontFileName pathExtension]];
            cell.displayText.font = [UIFont fontWithName:customFontName size:15];
            CGFontRelease(customFont);
            cell.displayText.textColor = [UIColor colorWithRed:red green:green blue:blue alpha:alpha];
            [cell.displayText setHidden:NO];
            [cell.progress stopAnimating];
            [cell.progress setHidden:YES];
        }
    }
    else {
        cell.fontName.text = [fontListArray[indexPath.row] stringByDeletingPathExtension];
        NSString* fileName = [NSString stringWithFormat:@"%@/%@", fontDirectoryPath, fontListArray[indexPath.row]];
        if (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
            [cell.displayText setHidden:YES];
            [cell.progress setHidden:NO];
            [cell.progress startAnimating];
        }
        else {
            [self loadFont:fileName withExtension:[fontFileName pathExtension]];
            cell.displayText.font = [UIFont fontWithName:customFontName size:15];
            cell.displayText.textColor = [UIColor colorWithRed:red green:green blue:blue alpha:alpha];
            CGFontRelease(customFont);
            [cell.displayText setHidden:NO];
            [cell.progress stopAnimating];
            [cell.progress setHidden:YES];
        }
    }
    return cell;
}


- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    if(fontArray != NULL && fontArray.count != 0 && fontArray.count > indexPath.row){
        AssetItem* assetItem = fontArray[indexPath.row];
        fontFileName = assetItem.name;
        NSArray* indexPathArr = [collectionView indexPathsForVisibleItems];
        for (int i = 0; i < [indexPathArr count]; i++) {
            NSIndexPath* indexPath = [indexPathArr objectAtIndex:i];
            TextFrameCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
            cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
        }
        TextFrameCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
        cell.layer.backgroundColor = [UIColor colorWithRed:71.0/255.0 green:71.0/255.0 blue:71.0/255.0 alpha:1.0].CGColor;

        [self load3dText];
    }
}

- (IBAction)inputTextChangedAction:(id)sender {
    [self load3dText];
}

- (IBAction)bevalChangeAction:(id)sender {
    [self load3dText];
}

- (IBAction)fontStoreTapChangeAction:(id)sender {
    if ((int)self.fontTab.selectedSegmentIndex == MY_FONT) {
            [self initializeFontListArray];
            if ([fontArray count] != 0) {
                [self.fontTab setSelectedSegmentIndex:MY_FONT];
                tabValue = MY_FONT;
                [self.collectionView reloadData];
            }
            else {
                tabValue = MY_FONT;
            }
    }
        else {
            [self.fontTab setSelectedSegmentIndex:FONT_STORE];
            tabValue = FONT_STORE;
            [self loadAllFonts];
        }
}

- (IBAction)colorPickerAction:(id)sender {    
    _textColorProp = [[TextColorPicker alloc] initWithNibName:@"TextColorPicker" bundle:nil TextColor:nil];
    _textColorProp.delegate = self;
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_textColorProp];
    self.popoverController.popoverContentSize = CGSizeMake(200, 200);
    self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    [_popUpVc.view setClipsToBounds:YES];
    CGRect rect = _colorWheelbtn.frame;
    rect = [self.view convertRect:rect fromView:_colorWheelbtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self.view
                          permittedArrowDirections:UIPopoverArrowDirectionUp
                                          animated:NO];
}

- (IBAction)cancelBtnAction:(id)sender {
    [_textSelectionDelegate removeTempNodeFromScene];
    [_textSelectionDelegate showOrHideLeftView:NO withView:nil];
    [self deallocMem];
    [self.view removeFromSuperview];    
}

- (IBAction)addToSceneBtnAction:(id)sender {
    Vector4 color = Vector4(red,green,blue,1.0);
    float bevelValue = _bevelSlider.value;
    [_textSelectionDelegate load3DTex:FONT AssetId:0 TypedText:_inputText.text FontSize:10 BevelValue:bevelValue TextColor:color FontPath:fontFileName isTempNode:NO];
    [_textSelectionDelegate removeTempNodeFromScene];
    [_textSelectionDelegate showOrHideLeftView:NO withView:nil];
    [self deallocMem];
    [self.view removeFromSuperview];
}

- (void) load3dText{
    if(_inputText.text.length ==0){
         UIAlertView* loadNodeAlert = [[UIAlertView alloc] initWithTitle:@"Field Empty" message:@"Please enter some text to add." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
    [loadNodeAlert show];
        return;
    }
    if(fontFileName.length == 0){
    UIAlertView* loadNodeAlert = [[UIAlertView alloc] initWithTitle:@"Information" message:@"Please Choose Font Style." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [loadNodeAlert show];
        return;
    }
    
    Vector4 color = Vector4(red,green,blue,1.0);
    float bevelValue = _bevelSlider.value;
    [_textSelectionDelegate load3DTex:FONT AssetId:0 TypedText:_inputText.text FontSize:10 BevelValue:bevelValue TextColor:color FontPath:fontFileName isTempNode:YES];
}

- (void) changeVertexColor:(Vector3)vetexColor dragFinish:(BOOL)isDragFinish{
    red = vetexColor.x;
    green = vetexColor.y;
    blue = vetexColor.z;
    [_collectionView reloadData];
    if(isDragFinish)
        [self load3dText];
}

- (void)deallocMem
{
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