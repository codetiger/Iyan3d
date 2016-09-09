//
//  PopUpViewController.h
//  Iyan3D
//
//  Created by Sankar on 29/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import "Constants.h"

@protocol PopUpViewControllerDelegate
- (void) animationBtnDelegateAction:(int)indexValue;
- (void) importBtnDelegateAction:(int)indexValue;
- (void) exportBtnDelegateAction:(int)indexValue;
- (void) viewBtnDelegateAction:(int)indexValue;
- (void) infoBtnDelegateAction:(int)indexValue;
- (void) addFrameBtnDelegateAction:(int)indexValue;
- (void) myObjectsBtnDelegateAction:(int)indexValue;
- (void)highlightObjectList;
- (void) propertiesBtnDelegate:(int)indexValue;
- (void) optionBtnDelegate:(int)indexValue;
- (NODE_TYPE) getNodeType:(int)nodeId;
@end

@interface PopUpViewController : UIViewController <UITableViewDataSource,UITableViewDelegate>{
     NSMutableArray *tableData;
    NSString *clickedBtn;
}

@property (weak, nonatomic) IBOutlet UIView *topBar;
@property (weak, nonatomic) IBOutlet UITableView *popoverBtns;
@property (weak, nonatomic) IBOutlet UIImageView *loginImage;
@property (weak, nonatomic) IBOutlet UIButton *loginBtn;

@property (weak, nonatomic) id <PopUpViewControllerDelegate> delegate;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil clickedButton:(NSString *)buttonValue;
- (void *)updateDescelect:(NSIndexPath *)indexPath;
- (void *)updateSelection:(NSIndexPath *)indexPath ScrollPosition:(int)scrolPosition;
-(void *)allowMultipleSelection :(BOOL)isMultipleselectionEnabled;
-(void)UpdateObjectList:(NSArray*) objectList;
@end
