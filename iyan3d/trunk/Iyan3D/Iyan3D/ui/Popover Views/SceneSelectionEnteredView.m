//
//  SceneSelectionEnteredView.m
//  Iyan3D
//
//  Created by harishankarn on 18/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "SceneSelectionEnteredView.h"

@interface SceneSelectionEnteredView ()

@end

@implementation SceneSelectionEnteredView

- (void)viewDidLoad {
    
    CALayer *TopBorder = [CALayer layer];
    TopBorder.frame = CGRectMake(0.0f, 0.0f, _bottom_bar.frame.size.width, 3.0f);
    TopBorder.backgroundColor = [UIColor grayColor].CGColor;
    [_bottom_bar.layer addSublayer:TopBorder];
    
    [super viewDidLoad];
    
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}
- (IBAction)closeButtonAction:(id)sender {
    [self dismissModalViewControllerAnimated:YES];
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
