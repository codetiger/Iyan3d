//
//  EditorViewController.h
//  Iyan3D
//
//  Created by Sankar on 18/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ImportImageNew.h"
#import "AnimationSelectionSlider.h"
#import "TextSelectionSidePanel.h"
#import "AssetSelectionSidePanel.h"

@interface EditorViewController : UIViewController <UITableViewDelegate, UITableViewDataSource, UIActionSheetDelegate, ImageImportNewDelgate,SliderDelegate,TextSelectionDelegate,AssetSelectionDelegate,UIAlertViewDelegate>{
    int totalFrames;
    NSMutableArray *tableData;
    ImportImageNew *importImageViewVC;
    AnimationSelectionSlider *animationsliderVC;
    TextSelectionSidePanel *textSelectionSlider;
    AssetSelectionSidePanel *assetSelectionSlider;
}

@property (weak, nonatomic) IBOutlet UIButton *playBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *framesCollectionView;
@property (weak, nonatomic) IBOutlet UIButton *addFrameBtn;
@property (weak, nonatomic) IBOutlet UIButton *extrenderBtn;
@property (weak, nonatomic) IBOutlet UIButton *viewBtn;
@property (weak, nonatomic) IBOutlet UIButton *infoBtn;
@property (weak, nonatomic) IBOutlet UIButton *editobjectBtn;
@property (weak, nonatomic) IBOutlet UITableView *objectList;
@property (weak, nonatomic) IBOutlet UIButton *exportBtn;
@property (weak, nonatomic) IBOutlet UIButton *animationBtn;
@property (weak, nonatomic) IBOutlet UIButton *importBtn;
@property (weak, nonatomic) IBOutlet UIButton *optionsBtn;
@property (weak, nonatomic) IBOutlet UIButton *moveBtn;
@property (weak, nonatomic) IBOutlet UIButton *rotateBtn;
@property (weak, nonatomic) IBOutlet UIButton *scaleBtn;
@property (weak, nonatomic) IBOutlet UIButton *undoBtn;
@property (weak, nonatomic) IBOutlet UIButton *redoBtn;
@property (weak, nonatomic) IBOutlet UIButton *lastFrameBtn;
@property (weak, nonatomic) IBOutlet UIButton *firstFrameBtn;
@property (weak, nonatomic) IBOutlet UIView *leftView;
@property (nonatomic, strong) UIImagePickerController *imagePicker;

- (IBAction)editFunction:(id)sender;
- (IBAction)addFrames:(id)sender;
- (IBAction)exportAction:(id)sender;
- (IBAction)animationBtnAction:(id)sender;
- (IBAction)importBtnAction:(id)sender;
- (IBAction)optionsBtnAction:(id)sender;
- (IBAction)moveBtnAction:(id)sender;
- (IBAction)rotateBtnAction:(id)sender;
- (IBAction)scaleBtnAction:(id)sender;
- (IBAction)undoBtnAction:(id)sender;
- (IBAction)redoBtnAction:(id)sender;
- (IBAction)infoBtnAction:(id)sender;
- (IBAction)viewBtn:(id)sender;
- (IBAction)lastFrameBtnAction:(id)sender;
- (IBAction)firstFrameBtnAction:(id)sender;


@end
