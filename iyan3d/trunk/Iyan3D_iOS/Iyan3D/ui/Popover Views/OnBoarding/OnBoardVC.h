//
//  OnBoardVC.h
//  Iyan3D
//
//  Created by Karthik on 22/06/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol OnBoardDelegate

- (void)closingOnBoard;

@end

@interface OnBoardVC : UIViewController {
    NSArray* titlesArray;
    NSArray* descArray;
    BOOL     pageControlBeingUsed;
    int      totalPages;
}
@property (weak, nonatomic) IBOutlet UIView* stepView;
@property (weak, nonatomic) IBOutlet UIImageView* stepImgView;
@property (weak, nonatomic) IBOutlet UILabel* stepTitle;
@property (weak, nonatomic) IBOutlet UILabel* stepBody;
@property (weak, nonatomic) IBOutlet UIImageView* divider;
@property (weak, nonatomic) id<OnBoardDelegate>   delegate;
@property (weak, nonatomic) IBOutlet UIView* beginView;
@property (weak, nonatomic) IBOutlet UIButton* startBtn;
@property (weak, nonatomic) IBOutlet UIScrollView* scrollView;
@property (weak, nonatomic) IBOutlet UIPageControl* pageControl;
- (IBAction)startBtnAction:(id)sender;
- (IBAction)skipBtnAction:(id)sender;
- (IBAction)closeBtnAction:(id)sender;
- (IBAction)changePage;

@end
