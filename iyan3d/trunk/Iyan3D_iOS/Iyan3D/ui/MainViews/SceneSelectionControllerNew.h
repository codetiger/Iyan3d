//
//  SceneSelectionControllerNew.h
//  Iyan3D
//
//  Created by harishankarn on 16/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#ifndef SceneSelectionControllerNew_h
#define SceneSelectionControllerNew_h


#import <UIKit/UIKit.h>
#import <MessageUI/MFMailComposeViewController.h>
#import <MediaPlayer/MediaPlayer.h>
#import "Utility.h"
#import "HelpViewController.h"
#import "AppHelper.h"
#import "AnimationEditorViewControllerPad.h"
#import "GAI.h"
#import "SceneSelectionFrameCell.h"
#import "EditorViewController.h"
#import "WEPopoverController.h"
#import "SettingsViewController.h"


@interface SceneSelectionControllerNew : UIViewController<UICollectionViewDelegate,UICollectionViewDataSource,MFMailComposeViewControllerDelegate,ScenePropertiesDelegate,UIAlertViewDelegate,PopUpViewControllerDelegate ,WEPopoverControllerDelegate,SettingsViewControllerDelegate>{
    NSMutableArray *scenesArray;
    CacheSystem* cache;
    NSDateFormatter *dateFormatter;
    int currentSelectedScene;
    CGPoint cell_center;
    UITapGestureRecognizer *tapGesture;
    NSDictionary *deviceNames;
    SceneSelectionFrameCell *cell;
    int sceneToBeRenamed;
    CGFloat screenHeight;
    SettingsViewController *settingsVc;
}

@property (weak, nonatomic) IBOutlet UIView *topBar;
@property (nonatomic, strong) NSString *fileBeginsWith;
@property (weak, nonatomic) IBOutlet UIButton *cloneSceneBtn;
@property (weak, nonatomic) IBOutlet UIButton *deleteSceneBtn;
@property (weak, nonatomic) IBOutlet UIButton *mailBtn;
@property (weak, nonatomic) IBOutlet UIButton *addSceneBtn;
@property (weak, nonatomic) IBOutlet UIView *sceneView;
@property (weak, nonatomic) IBOutlet UIImageView *scenePreview;
@property (weak, nonatomic) IBOutlet UITextField *sceneTitle;
@property (weak, nonatomic) IBOutlet UILabel *sceneDate;
@property (weak, nonatomic) IBOutlet UIButton *scenePreviewCloseBtn;
@property (weak, nonatomic) IBOutlet UIButton *openScene;
@property (weak, nonatomic) IBOutlet UIButton *infoBtn;

@property (weak, nonatomic) IBOutlet UICollectionView *scenesCollectionView;
@property (nonatomic, strong) PopUpViewController *popUpVc;
@property (nonatomic, strong) WEPopoverController *popoverController;


- (IBAction)openSceneAction:(id)sender;

@end

#endif