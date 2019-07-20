//
//  ObjSidePanel.h
//  Iyan3D
//
//  Created by Sankar on 08/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import "ObjCellView.h"
#import "TextColorPicker.h"
#import "WEPopoverController.h"
#import "PopUpViewController.h"
#import <FPPopoverController.h>
#import "Vector3.h"

@protocol ObjSliderDelegate
- (void)showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;

- (void)importObjAndTexture:(int)indexPathOfOBJ TextureName:(NSString*)textureFileName VertexColor:(Vector3)color haveTexture:(BOOL)isHaveTexture IsTempNode:(BOOL)isTempNode;
- (void)importObjWithIndexPath:(int)indexPath TextureName:(NSString*)textureFileName MeshColor:(Vector3)color HasTexture:(BOOL)hasTexture IsTempNode:(BOOL)isTempNode;
- (BOOL)addTempNodeToScene;
- (void)removeTempNodeFromScene;
- (void)changeTexture:(NSString*)textureName VertexColor:(Vector3)color IsTemp:(BOOL)isTemp AtIndex:(PROP_INDEX)pIndex;
- (void)removeTempTextureAndVertex:(PROP_INDEX)pIndex;
- (void)showOrHideProgress:(BOOL)value;
- (void)deallocSubViews;
@end

@interface ObjSidePanel : GAITrackedViewController <UICollectionViewDataSource, UICollectionViewDelegate, TextColorPickerDelegate, UIImagePickerControllerDelegate, UINavigationControllerDelegate, UIGestureRecognizerDelegate, ObjCellViewDelegate> {
    int                  indexPathOfOBJ;
    NSString*            textureFileName;
    BOOL                 haveTexture;
    Vector3              color;
    NSArray*             basicShapes;
    int                  viewType;
    FPPopoverController* popover;
    PROP_INDEX           propIndex;

    NSArray*  srcDirPath;
    NSString* docDirPath;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil Type:(int)type AndPropIndex:(PROP_INDEX)pIndex;

@property (weak, nonatomic) IBOutlet UIButton* addBtn;
@property (weak, nonatomic) IBOutlet UIButton* cancelBtn;
@property (weak, nonatomic) IBOutlet UICollectionView* importFilesCollectionView;
@property (weak, nonatomic) IBOutlet UILabel*          viewTitle;
@property (weak, nonatomic) id<ObjSliderDelegate>      objSlideDelegate;
@property (nonatomic, strong) TextColorPicker*         vertexColorProp;
@property (nonatomic, strong) WEPopoverController*     popoverController;
@property (nonatomic, strong) PopUpViewController*     popUpVc;
@property (nonatomic, strong) UIImagePickerController* ipc;
@property (weak, nonatomic) IBOutlet UIButton* colorWheelBtn;
@property (weak, nonatomic) IBOutlet UICollectionView* collectionView;
@property (weak, nonatomic) IBOutlet UIButton* importBtn;
@property (weak, nonatomic) IBOutlet UILabel* ObjInfoLable;

- (IBAction)addBtnAction:(id)sender;
- (IBAction)cancelBtnAction:(id)sender;
@end
