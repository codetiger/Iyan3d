//
//  ObjSidePanel.h
//  Iyan3D
//
//  Created by Sankar on 08/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol ObjSliderDelegate

- (void) showOrHideLeftView:(BOOL)showView withView:(UIView*)subViewToAdd;
- (void) importObj:(NSString*)objFileName TextureName:(NSString*)textureFileName;
@end


@interface ObjSidePanel : UIViewController<UICollectionViewDataSource,UICollectionViewDelegate>
{
    NSString* objFileName;
    NSString* textureFileName;
}
@property (weak, nonatomic) IBOutlet UIButton *addBtn;
@property (weak, nonatomic) IBOutlet UIButton *cancelBtn;
@property (weak, nonatomic) IBOutlet UICollectionView *importFilesCollectionView;
@property (weak, nonatomic) IBOutlet UILabel *viewTitle;
@property (weak, nonatomic) id <ObjSliderDelegate> delegate;


- (IBAction)addBtnAction:(id)sender;
- (IBAction)cancelBtnAction:(id)sender;
@end
