//
//  OnBoardVC.h
//  Iyan3D
//
//  Created by Karthik on 22/06/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface OnBoardVC : UIViewController
{
    NSArray* imagesArray;
}

@property (weak, nonatomic) IBOutlet UIView *beginView;
@property (weak, nonatomic) IBOutlet UIButton *startBtn;
@property (weak, nonatomic) IBOutlet UIButton *skipBtn;
@property (weak, nonatomic) IBOutlet UIScrollView *scrollView;
@property (weak, nonatomic) IBOutlet UIPageControl *pageControl;
- (IBAction)startBtnAction:(id)sender;
- (IBAction)skipBtnAction:(id)sender;
- (IBAction)closeBtnAction:(id)sender;

@end
