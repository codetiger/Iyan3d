//
//  ObjSidePanel.h
//  Iyan3D
//
//  Created by Sankar on 08/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import "TextColorPicker.h"
#import "WEPopoverController.h"
#import "PopUpViewController.h"
#import <FPPopoverController.h>

#ifdef OPTIMGLKM
#import "Vector3GLK.h"
#else
#import "Vector3.h"
#endif

@protocol ObjSliderDelegate
- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;

- (void)importObjAndTexture:(int)indexPathOfOBJ TextureName:(NSString*)textureFileName VertexColor:(Vector3)color haveTexture:(BOOL)isHaveTexture IsTempNode:(BOOL)isTempNode;
- (void) removeTempNodeFromScene;
- (void) changeTexture:(NSString*)textureName VertexColor:(Vector3)color IsTemp:(BOOL)isTemp;
- (void) removeTempTextureAndVertex;
-(void) showOrHideProgress:(BOOL) value;
- (void) deallocSubViews;
@end


@interface ObjSidePanel : GAITrackedViewController<UICollectionViewDataSource,UICollectionViewDelegate,TextColorPickerDelegate,UIImagePickerControllerDelegate,UINavigationControllerDelegate>
{
    int indexPathOfOBJ;
    NSString* textureFileName;
    BOOL haveTexture;
    Vector3 color;
    NSArray* basicShapes;
    int viewType;
    FPPopoverController *popover;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil Type:(int)type;


@property (weak, nonatomic) IBOutlet UIButton *addBtn;
@property (weak, nonatomic) IBOutlet UIButton *cancelBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *importFilesCollectionView;
@property (weak, nonatomic) IBOutlet UILabel *viewTitle;
@property (weak, nonatomic) id <ObjSliderDelegate> objSlideDelegate;
@property (nonatomic, strong) TextColorPicker *vertexColorProp;
@property (nonatomic, strong) WEPopoverController *popoverController;
@property (nonatomic, strong) PopUpViewController *popUpVc;
@property (nonatomic, strong) UIImagePickerController *ipc;
@property (weak, nonatomic) IBOutlet UIButton *colorWheelBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;
@property (weak, nonatomic) IBOutlet UIButton *importBtn;
@property (weak, nonatomic) IBOutlet UILabel *ObjInfoLable;


- (IBAction)addBtnAction:(id)sender;
- (IBAction)cancelBtnAction:(id)sender;
@end
