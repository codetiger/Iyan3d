//
//  ChangeTextureSidePanel.h
//  Iyan3D
//
//  Created by Sankar on 22/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "Vector3.h"
@protocol ChangeTextureDelegate
- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;
- (void) changeTexture:(NSString*)textureName VertexColor:(Vector3)color;

@end

@interface ChangeTextureSidePanel : UIViewController<UICollectionViewDataSource,UICollectionViewDelegate,UIImagePickerControllerDelegate,UINavigationControllerDelegate>{
   
    UIPopoverController *popover;
}
@property (weak, nonatomic) id <ChangeTextureDelegate> textureDelegate;
@property (weak, nonatomic) IBOutlet UIButton *addBtn;
@property (nonatomic, strong) UIImagePickerController *ipc;
@property (weak, nonatomic) IBOutlet UIButton *cancelbtn;
@property (weak, nonatomic) IBOutlet UIButton *importBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *texturefilesList;
- (IBAction)cancelBtnAction:(id)sender;
- (IBAction)addBtnAction:(id)sender;
- (IBAction)importBtnAction:(id)sender;
@end
