//
//  ObjSidePanel.h
//  Iyan3D
//
//  Created by Sankar on 08/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "TextColorPicker.h"
#import "WEPopoverController.h"
#import "PopUpViewController.h"

@protocol ObjSliderDelegate
- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;

- (void)importObjAndTexture:(int)indexPathOfOBJ TextureName:(NSString*)textureFileName VertexColor:(Vector3)color haveTexture:(BOOL)isHaveTexture;
@end


@interface ObjSidePanel : UIViewController<UICollectionViewDataSource,UICollectionViewDelegate,TextColorPickerDelegate>
{
    int indexPathOfOBJ;
    NSString* textureFileName;
    BOOL haveTexture;
    Vector3 color;
    NSArray* basicShapes;
}

@property (weak, nonatomic) IBOutlet UIButton *addBtn;
@property (weak, nonatomic) IBOutlet UIButton *cancelBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *importFilesCollectionView;
@property (weak, nonatomic) IBOutlet UILabel *viewTitle;
@property (weak, nonatomic) id <ObjSliderDelegate> objSlideDelegate;
@property (nonatomic, strong) TextColorPicker *vertexColorProp;
@property (nonatomic, strong) WEPopoverController *popoverController;
@property (nonatomic, strong) PopUpViewController *popUpVc;

@property (weak, nonatomic) IBOutlet UIButton *colorWheelBtn;

- (IBAction)addBtnAction:(id)sender;
- (IBAction)cancelBtnAction:(id)sender;
@end
