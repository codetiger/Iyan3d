//
//  ChangeTextureSidePanel.h
//  Iyan3D
//
//  Created by Sankar on 22/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ChangeTextureDelegate
- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;


@end

@interface ChangeTextureSidePanel : UIViewController<UICollectionViewDataSource,UICollectionViewDelegate>
@property (weak, nonatomic) id <ChangeTextureDelegate> textureDelegate;
@property (weak, nonatomic) IBOutlet UIButton *addBtn;

@property (weak, nonatomic) IBOutlet UIButton *cancelbtn;
@property (weak, nonatomic) IBOutlet UICollectionView *texturefilesList;
- (IBAction)cancelBtnAction:(id)sender;
- (IBAction)addBtnAction:(id)sender;
@end
