//
//  AppHelper.m
//  Iyan3D
//
//  Created by Karthik on 18/02/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//


#import "JDFTooltips.h"

#import <sys/utsname.h>
#import "AppHelper.h"
#import "Constants.h"
#import "DownloadTask.h"
#import "AFHTTPRequestOperation.h"
#import "AFHTTPClient.h"


#define FIVE_THOUSAND_CREDITS @"basicrecharge"
#define TWENTY_THOUSAND_CREDITS @"mediumrecharge"
#define FIFTY_THOUSAND_CREDITS @"megarecharge"

@implementation AppHelper

@synthesize productsRequest = _productsRequest;

+ (AppHelper*)getAppHelper
{
    static AppHelper* theAppHelper;
    @synchronized(self)
    {
        if (!theAppHelper)
            theAppHelper = [[self alloc] init];
    }
    return theAppHelper;
}

- (void)downloadJsonData
{
    [self initHelper];
    if(self.delegate)
        [self.delegate performLocalTasks];
}

- (void)initializeFontListArray
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    fontDirPath = [NSString stringWithFormat:@"%@/Resources/Fonts", docDirPath];
    NSArray* fontExtensions = [NSArray arrayWithObjects:@"ttf", @"otf", nil];
    NSArray* filesGathered;

    if (![[NSFileManager defaultManager] fileExistsAtPath:fontDirPath]) {
        [[NSFileManager defaultManager] createDirectoryAtPath:fontDirPath withIntermediateDirectories:YES attributes:Nil error:Nil];
    }
    [self copyFontFilesFromDirectory:docDirPath ToDirectory:fontDirPath withExtensions:fontExtensions];
    filesGathered = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:fontDirPath error:Nil];
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

- (void)initHelper
{
    cache = [CacheSystem cacheSystem];
}

- (void)setIdentifierForVendor
{
    if (![[AppHelper getAppHelper] userDefaultsForKey:@"identifierForVendor"]) {
        if ([UIDevice instancesRespondToSelector:@selector(identifierForVendor)]) {
            NSString* uniqueIdentifier = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
            [[AppHelper getAppHelper] saveToUserDefaults:uniqueIdentifier withKey:@"identifierForVendor"];
        }
    }
}

- (void)setAssetsDetails
{
    [self setAssetsDetails:SCENE_SELECTION];
}

- (void)writeDataToFile:(NSData*)data FileName:(NSString*)fileName
{
    NSMutableDictionary* dictionary = [[NSMutableDictionary alloc] init];
    [dictionary setObject:data forKey:@"data"];
    [dictionary setObject:fileName forKey:@"file"];
    [self performSelectorOnMainThread:@selector(writeDataToFile:) withObject:dictionary waitUntilDone:YES];
    while (![[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        [NSThread sleepForTimeInterval:50.0];
    }
}

- (void)writeDataToFile:(NSMutableDictionary*)dictionary
{
    NSData* data = [dictionary objectForKey:@"data"];
    NSString* fileName = [dictionary objectForKey:@"file"];
    if (data)
        [data writeToFile:fileName atomically:YES];
}

- (BOOL)userDefaultsBoolForKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    BOOL val = NO;
    if (standardUserDefaults)
        val = [standardUserDefaults boolForKey:key];
    return val;
}
- (id)userDefaultsForKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    NSString* val = nil;

    if (standardUserDefaults)
        val = [standardUserDefaults objectForKey:key];

    return val;
}

- (NSMutableDictionary*)dictionaryForKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    NSMutableDictionary* val = nil;

    if (standardUserDefaults)
        val = [standardUserDefaults objectForKey:key];

    return val;
}

- (void)removeFromUserDefaultsWithKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    [standardUserDefaults removeObjectForKey:key];
    [standardUserDefaults synchronize];
}

- (void)saveToUserDefaults:(id)value withKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];

    if (standardUserDefaults) {
        [standardUserDefaults setObject:value forKey:key];
        [standardUserDefaults synchronize];
    }
}

- (void)saveBoolUserDefaults:(BOOL)value withKey:(NSString*)key
{
    NSUserDefaults* standardUserDefaults = [NSUserDefaults standardUserDefaults];
    if (standardUserDefaults) {
        [standardUserDefaults setBool:value forKey:key];
        [standardUserDefaults synchronize];
    }
}

- (void)moveFilesFromInboxDirectory:(CacheSystem*)presentCache
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* inboxDirectoryPath = [docDirPath stringByAppendingPathComponent:@"Inbox"];
    NSArray* filesGathered;

    if ([[NSFileManager defaultManager] fileExistsAtPath:inboxDirectoryPath]) {
        filesGathered = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:inboxDirectoryPath error:Nil];

        for (NSString* aFile in filesGathered) {
            NSError* error;
            if (![[NSFileManager defaultManager] fileExistsAtPath:[docDirPath stringByAppendingPathComponent:aFile]]) {
                [[NSFileManager defaultManager] moveItemAtPath:[inboxDirectoryPath stringByAppendingPathComponent:aFile] toPath:[docDirPath stringByAppendingPathComponent:aFile] error:&error];
            }

            if (error)
                NSLog(@" Error copying font files %@ due to %@", error.localizedDescription, error.localizedFailureReason);
            else
                [[NSFileManager defaultManager] removeItemAtPath:[inboxDirectoryPath stringByAppendingPathComponent:aFile] error:nil];
        }
    }
}

- (void)moveFontFilesIfNeccasary
{
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* fontDirectoryPath = [docDirPath stringByAppendingPathComponent:@"/Resources/Fonts"];
    NSString* bundlePath = [[NSBundle mainBundle] bundlePath];
    NSArray* fontExtensions = [NSArray arrayWithObjects:@"ttf", @"otf", nil];
    NSArray* filesGathered;

    if ([cache checkOBJImporterPurchase]) {
        if ([[NSFileManager defaultManager] fileExistsAtPath:fontDirectoryPath]) {
            filesGathered = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:fontDirectoryPath error:Nil];
        }
        else {
            [[NSFileManager defaultManager] createDirectoryAtPath:fontDirectoryPath withIntermediateDirectories:YES attributes:Nil error:Nil];

            NSArray* fontFilesToCopy = [[[NSFileManager defaultManager] contentsOfDirectoryAtPath:bundlePath error:Nil] filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", fontExtensions]];

            for (NSString* aFile in fontFilesToCopy) {
                NSError* error;
                if (![[NSFileManager defaultManager] fileExistsAtPath:[fontDirectoryPath stringByAppendingPathComponent:aFile]]) {
                    [[NSFileManager defaultManager] copyItemAtPath:[bundlePath stringByAppendingPathComponent:aFile] toPath:[fontDirectoryPath stringByAppendingPathComponent:aFile] error:&error];
                }
                if (error)
                    NSLog(@" Error copying font files %@ due to %@", error.localizedDescription, error.localizedFailureReason);
            }
        }
    }
}

- (void)missingAlertView
{
    UIAlertView* closeAlert = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Warning", nil) message:NSLocalizedString(@"missing_resources", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"OK", nil) otherButtonTitles:nil];
    [closeAlert show];
}

-(void) showErrorAlertViewWithMessage:(NSString*)message
{
    UIAlertView* errorAlert = [[UIAlertView alloc] initWithTitle:@"Error" message:message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [errorAlert show];

}

- (void)statusForOBJImport:(NSNumber*)status
{
    if(self.delegate != nil)
        [self.delegate statusForOBJImport:status];
}

- (void)parseHelpJson
{
    helpStatements = [self parseJsonFileWithName:@"helpStatements"];
}

- (NSDictionary*)parseJsonFileWithName:(NSString*)jsonFileName
{
    NSError* error;
    NSString* jsonPath = [[NSBundle mainBundle] pathForResource:jsonFileName ofType:@"json"];
    if(![[NSFileManager defaultManager] fileExistsAtPath:jsonPath])
        return nil;
    
    NSString* jsonStr = [NSString stringWithContentsOfFile:jsonPath encoding:NSUTF8StringEncoding error:nil];

    jsonStr = [jsonStr stringByTrimmingCharactersInSet:[NSCharacterSet controlCharacterSet]];
    NSDictionary* outputDict = [NSJSONSerialization JSONObjectWithData:[jsonStr dataUsingEncoding:NSUTF8StringEncoding] options:kNilOptions error:&error];
    if (error)
        NSLog(@" Error %@ ", error);
    return outputDict;
}

- (NSString*)getHelpStatementForAction:(int)action
{
    // use int action to get array of statements from the parsed json
    NSArray* actionStatements = [helpStatements objectForKey:[NSString stringWithFormat:@"%d", action]];

    // use size of array to get a random number
    if (actionStatements.count) {
        long randindex = arc4random() % (int)actionStatements.count;
        // use rand index to get the final statement
        return [actionStatements objectAtIndex:randindex];
    }
    return @"";
}

- (void) toggleHelp:(UIViewController*) vc Enable:(BOOL)enable
{
    if(toolTips == nil)
        toolTips = [[NSMutableArray alloc] init];
    
    if(!enable || [toolTips count] > 0) {
        for(int i = 0; i < [toolTips count]; i++) {
            JDFTooltipView *tooltip = [toolTips objectAtIndex:i];
            [tooltip hideAnimated:YES];
            tooltip = nil;
        }
        [toolTips removeAllObjects];
        toolTips = nil;
    } else {
        [self checkToolTip:vc.view MainView:vc.view];
    }
}

- (void) showTipForView:(UIView*) subView InMainView:(UIView*)view
{
    if(toolTips == nil)
        toolTips = [[NSMutableArray alloc] init];
    
    NSString* hint = NSLocalizedString([subView accessibilityHint], nil);
    int arrowDirection = [[subView accessibilityIdentifier] intValue];
    
    if([hint length] > 8) {
        JDFTooltipView *tooltip = [[JDFTooltipView alloc] initWithTargetView:subView hostView:view tooltipText:hint arrowDirection:arrowDirection width:220.0f];
        tooltip.font = [UIFont fontWithName:tooltip.font.fontName size:([Utility IsPadDevice]) ? 12 : 9];
        [tooltip show];
        [toolTips addObject:tooltip];
    } else {
        for( int i = 0; i < [toolTips count]; i++) {
            JDFTooltipView *t = [toolTips objectAtIndex:i];
            if(t.targetView == subView) {
                [t hideAnimated:YES];
                [toolTips removeObject:t];
                t = nil;
            }
        }
    }
}

- (void) checkToolTip:(UIView*) view MainView:(UIView*) mainView
{
    for( UIView* subView in view.subviews) {
        if(![subView isHidden] && subView.frame.origin.x >= 0 && subView.frame.origin.x < mainView.frame.size.width) {
            NSString* hint = NSLocalizedString([subView accessibilityHint], nil);
            int arrowDirection = [[subView accessibilityIdentifier] intValue];
            
            if([hint length] > 8) {
                JDFTooltipView *tooltip = [[JDFTooltipView alloc] initWithTargetView:subView hostView:mainView tooltipText:hint arrowDirection:arrowDirection width:220.0f];
                tooltip.font = [UIFont fontWithName:tooltip.font.fontName size:([Utility IsPadDevice]) ? 12 : 9];
                [tooltip show];
                [toolTips addObject:tooltip];
            }
            [self checkToolTip:subView MainView:mainView];
        }
    }
}

-(NSString*) deviceName
{
    struct utsname systemInfo;
    uname(&systemInfo);
    
    return [NSString stringWithCString:systemInfo.machine
                              encoding:NSUTF8StringEncoding];
}

- (NSData*) getReceiptData
{
    NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
    NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];
    return receipt;
}

-(NSString*) stringWithwstring:(const std::wstring&)ws
{
    char* data = (char*)ws.data();
    unsigned size = (int)ws.size() * sizeof(wchar_t);
    
    NSString* result = [[NSString alloc] initWithBytes:data length:size encoding:NSUTF16StringEncoding];
    return result;
}

-(std::wstring) getwstring:(NSString*) sourceString
{
    NSData* asData = [sourceString dataUsingEncoding:NSUTF16StringEncoding];
    return std::wstring((wchar_t*)[asData bytes], [asData length] / sizeof(wchar_t));
}

-(BOOL)iPhone6Plus{
    if (([UIScreen mainScreen].scale > 2.0)) return YES;
    return NO;
}

- (void)resetAppHelper
{
    cache = nil;

    if (self.productsRequest) {
        self.productsRequest.delegate = nil;
        [self.productsRequest cancel];
        self.productsRequest = nil;
    }
}

@end
