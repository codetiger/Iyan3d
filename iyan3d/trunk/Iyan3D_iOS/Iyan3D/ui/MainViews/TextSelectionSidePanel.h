//
//  TextSelectionSidePanel.h
//  Iyan3D
//
//  Created by sabish on 19/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef TextSelectionSidePanel_h
#define TextSelectionSidePanel_h


#import <UIKit/UIKit.h>
#import "CacheSystem.h"
#import "AppHelper.h"
#import "GetPixelDemo.h"
#import "TextFrameCell.h"
#import <CoreText/CoreText.h>

@protocol TextSelectionDelegate

-(void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;

@end

@interface TextSelectionSidePanel : UIViewController<UICollectionViewDelegate,UICollectionViewDataSource>{
    
    NSArray *fontListArray;
    CacheSystem* cache;
    NSString* docDirPath;
    NSMutableArray* fontArray;
    NSOperationQueue *downloadQueue , *assetDownloadQueue;
    NSString* fontDirectoryPath,*typedText;
    NSString *cacheDirectory;
    CGFontRef customFont;
    NSString * customFontName;
    int fontSize;
    int tabValue,selectedIndex,tempSelectedIndex;
    float red,green,blue,alpha;
    NSString *fontFileName;
    int bevelRadius;
    


    
}
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UISegmentedControl *fontTab;
@property (weak, nonatomic) IBOutlet UISlider *bevelSlider;
@property (weak, nonatomic) IBOutlet UITextField *inputText;
@property (weak, nonatomic) IBOutlet UISegmentedControl *fontStoreTab;
@property (weak, nonatomic) id <TextSelectionDelegate> textSelectionDelegate;

@end

#endif