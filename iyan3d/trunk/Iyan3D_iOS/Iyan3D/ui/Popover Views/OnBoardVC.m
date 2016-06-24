//
//  OnBoardVC.m
//  Iyan3D
//
//  Created by Karthik on 22/06/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "OnBoardVC.h"

@interface OnBoardVC ()

@end

@implementation OnBoardVC

#define IMAGE_COUNT 4
#define SCROLL_COUNT 5
int totalPages = 0;

- (void)viewDidLoad {
    [super viewDidLoad];
    self.preferredContentSize = CGSizeMake(768, 576);
    // Do any additional setup after loading the view from its nib.
}

- (void) setupScrollView
{
    self.scrollView.contentSize = CGSizeMake(self.scrollView.frame.size.width * (SCROLL_COUNT), self.scrollView.frame.size.height);
    self.scrollView.pagingEnabled = YES;
    self.pageControl.currentPage = 0;
    self.pageControl.numberOfPages = SCROLL_COUNT;
    
    for (int i = totalPages; i < SCROLL_COUNT; i++) {
        [self.scrollView addSubview:[self viewForPageInScrollViewAtIndex:i]];
        totalPages++;
    }
}

- (UIView*) viewForPageInScrollViewAtIndex:(int) index
{
    float xOrigin = (index * [self.scrollView bounds].size.width);
    CGRect frame = self.view.frame;
    frame.origin.x = xOrigin;

    if(index == 0) {
        UIImageView *imgView = [[UIImageView alloc] initWithFrame:frame];
        return imgView;
    } else if(index <= IMAGE_COUNT) {
        UIImageView * imgView = [[UIImageView alloc] initWithFrame:frame];
        [imgView setImage:[UIImage imageNamed:[NSString stringWithFormat:@"help%d.png", index]]];
        return imgView;
    }
}

- (void) scrollViewDidScroll:(UIScrollView *)sender
{
        // Update the page when more than 50% of the previous/next page is visible
        CGFloat pageWidth = self.scrollView.frame.size.width;
        int page = floor((self.scrollView.contentOffset.x - pageWidth / 2) / pageWidth) + 1;
        self.pageControl.currentPage = page;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (IBAction)startBtnAction:(id)sender {
    [self.beginView setHidden:YES];
    [self setupScrollView];
    [self.scrollView bringSubviewToFront:self.pageControl];
}

- (IBAction)skipBtnAction:(id)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (IBAction)closeBtnAction:(id)sender {
    [self dismissViewControllerAnimated:YES completion:nil];
}
@end
