//
//  SmartImageView.m
//  Iyan3D
//
//  Created by Harishankar Narayanan on 05/01/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "SmartImageView.h"
#import "Utility.h"
#import "AppHelper.h"

enum constants {
    SCENE_SELECTION = 2,
    ASSET_SELECTION =3,
    ASSET_DETAIL = 4,
    ALL_ANIMATION_VIEW = 5,
    MY_ANIMATION_VIEW = 6
    
};

@implementation SmartImageView

- (id) initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleGray];
        [activityIndicator setHidden:NO];
        [activityIndicator setHidesWhenStopped:YES];
        [activityIndicator stopAnimating];
        [activityIndicator setCenter:CGPointMake(self.frame.size.width/2, self.frame.size.height/2)];
        [self addSubview:activityIndicator];
    }
    return self;
}
- (id) initWithCoder:(NSCoder *)aDecoder {
    self = [super initWithCoder:aDecoder];
    if (self) {
        activityIndicator = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle:UIActivityIndicatorViewStyleWhite];
        [activityIndicator setHidden:NO];
        [activityIndicator setHidesWhenStopped:YES];
        [activityIndicator stopAnimating];
        [activityIndicator setCenter:self.center];
        [self addSubview:activityIndicator];
    }
    return self;
}
- (void) setImageInfo:(NSString*)assetId forView:(int)viewType {
    float x = 0.0;
    if(SCREENWIDTH == 1024)
        x = 3.8;
    else if(SCREENWIDTH <= 667)
        x = 2;
    else
        x = 3.2;
    CGRect aRect = CGRectMake(self.frame.size.width/x, self.frame.size.height/3, 30, 30);
    if (viewType == ASSET_DETAIL)
        [activityIndicator setFrame:aRect];
    [activityIndicator startAnimating];
    _assetId = assetId;
    _viewType = viewType;
    if(viewType == ASSET_SELECTION) {
        UIImage *image = [self GetImage:assetId];
        BOOL updateImage;
        if([AppHelper getAppHelper].isAssetsUpdated)
            updateImage = [[AppHelper getAppHelper] userDefaultsBoolForKey:[NSString stringWithFormat:@"updateImage%@",assetId]];
        else
            updateImage = false;
        
        if(image && !updateImage) {
            [self setImage:image];
            [activityIndicator stopAnimating];
        } else {
            [self setImage:nil];
            [self performSelectorInBackground:@selector(DownloadImage:) withObject:assetId];
        }
    }else if(viewType == MY_ANIMATION_VIEW) {
        UIImage *image = [self GetImage:assetId];
        if(image) {
            [self setImage:image];
            [activityIndicator stopAnimating];
        } else {
            [self setImage:nil];
            //[self performSelectorInBackground:@selector(DownloadImage:) withObject:assetId];
        }
    }else if(viewType == ALL_ANIMATION_VIEW){
        UIImage *image = [self GetImage:assetId];
        if(image) {
            [self setImage:image];
            [activityIndicator stopAnimating];
        } else {
            [self setImage:nil];
            [self performSelectorInBackground:@selector(DownloadAnimationImage:) withObject:assetId];
        }
    }
}
- (void) reLoadImage:(NSString*)assetId {
    UIImage *image = [self GetImage:assetId];
    if(image) {
        if([_assetId isEqualToString:assetId]) {
            [self setImage:image];
            [[AppHelper getAppHelper] saveBoolUserDefaults:NO withKey:[NSString stringWithFormat:@"updateImage%@",_assetId]];
            [activityIndicator stopAnimating];
        }
    }
}
- (void) DownloadAnimationImage:(NSString*)assetId{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cacheDirectory = [paths objectAtIndex:0];
    NSString* assetCellImageName = [NSString stringWithFormat:@"%@/%@.png", cacheDirectory, assetId];
    NSString *imageUrl128 = [NSString stringWithFormat:@"https://s3.amazonaws.com/iyan3d/animationpreview/%@.png", assetId];
    [self downloadImageFromURL:imageUrl128 SaveWithFileName:assetCellImageName];
    [self performSelectorOnMainThread:@selector(reLoadImage:) withObject:assetId waitUntilDone:NO];
}
- (void) DownloadImage:(NSString*)assetId{
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cacheDirectory = [paths objectAtIndex:0];
    NSString* assetCellImageName = [NSString stringWithFormat:@"%@/%@.png", cacheDirectory, assetId];
    NSString *imageUrl128 = [NSString stringWithFormat:@"https://s3.amazonaws.com/iyan3d/128images/%@.png", assetId];
    [self downloadImageFromURL:imageUrl128 SaveWithFileName:assetCellImageName];
    [self performSelectorOnMainThread:@selector(reLoadImage:) withObject:assetId waitUntilDone:NO];
}
-(BOOL) downloadImageFromURL:(NSString*) imageURL SaveWithFileName:(NSString*) fileName
{
    NSData *imgData = [NSData dataWithContentsOfURL:[NSURL URLWithString:imageURL]];
    if (imgData) {
    UIImage* imageforAssetView = [UIImage imageWithData:imgData];
    [UIImagePNGRepresentation(imageforAssetView) writeToFile:fileName options:NSAtomicWrite error:nil];
        return YES;
    } else
        return NO;

}
- (UIImage*) GetImage:(NSString*)assetId {
    UIImage* image = nil;
    NSString* storagePath;
    NSArray *strings = [assetId componentsSeparatedByString:@"_"];
    int localId = [strings[0] intValue];;
    NSArray *docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *docDirectory = [docPaths objectAtIndex:0];
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cacheDirectory = [paths objectAtIndex:0];
    if(_viewType != ALL_ANIMATION_VIEW){
        if ([assetId hasPrefix:@"2"] && localId >= 20000)
            storagePath = [NSString stringWithFormat:@"%@/Resources/Objs",docDirectory];
        else if ([assetId hasPrefix:@"4"]&& localId >= 40000)
            storagePath = [NSString stringWithFormat:@"%@/Resources/Rigs",docDirectory];
        else if ([assetId hasPrefix:@"8"] && localId >= 80000)
            storagePath = [NSString stringWithFormat:@"%@/Resources/Animations",docDirectory];
        else
            storagePath = cacheDirectory;
    }else{
        storagePath = cacheDirectory;
    }
    NSString* fileName = [NSString stringWithFormat:@"%@/%@.png", storagePath, assetId];
    if([[NSFileManager defaultManager] fileExistsAtPath:fileName]) {
        image = [UIImage imageWithContentsOfFile:fileName];
    }
    return image;
}
-(void) dealloc {
    [activityIndicator removeFromSuperview];
	activityIndicator = nil;
}

@end
