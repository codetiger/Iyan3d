//
//  PopUpViewController.h
//  Iyan3D
//
//  Created by Sankar on 29/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol PopUpViewControllerDelegate
- (void) animationBtnDelegateAction:(int)indexValue;
- (void) importBtnDelegateAction:(int)indexValue;
- (void) exportBtnDelegateAction:(int)indexValue;
- (void) viewBtnDelegateAction:(int)indexValue;
- (void) infoBtnDelegateAction:(int)indexValue;
- (void) addFrameBtnDelegateAction:(int)indexValue;
- (void) myObjectsBtnDelegateAction:(int)indexValue;
- (void) loginBtnAction;
@end

@interface PopUpViewController : UIViewController <UITableViewDataSource,UITableViewDelegate>{
     NSArray *tableData;
    NSString *clickedBtn;
    
}

@property (weak, nonatomic) IBOutlet UIView *topBar;
@property (weak, nonatomic) IBOutlet UITableView *popoverBtns;
@property (weak, nonatomic) IBOutlet UIImageView *loginImage;
@property (weak, nonatomic) IBOutlet UIButton *loginBtn;
- (IBAction)loginBtnAction:(id)sender;
@property (weak, nonatomic) id <PopUpViewControllerDelegate> delegate;
- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil clickedButton:(NSString*)buttonValue;
@end
