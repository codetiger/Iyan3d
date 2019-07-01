//
//  OnBoardVC.m
//  Iyan3D
//
//  Created by Karthik on 22/06/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "OnBoardVC.h"
#import "Utility.h"
#import "StepView.h"

@interface OnBoardVC ()

@end

@implementation OnBoardVC

#define IMAGE_COUNT 4
#define SCROLL_COUNT 5

- (void)viewDidLoad {
    [super viewDidLoad];
    
    totalPages = 0;
    if([Utility IsPadDevice])
        self.preferredContentSize = CGSizeMake(768, 576);
    [self.beginView setHidden:YES];
    [self setArrayValues];
    [self setupScrollView];
    [self.scrollView bringSubviewToFront:self.pageControl];
    // Do any additional setup after loading the view from its nib.
}

- (void) setArrayValues
{
    titlesArray = [[NSArray alloc] initWithObjects:NSLocalizedString(@"Create a Scene", nil), NSLocalizedString(@"Animate Character", nil), NSLocalizedString(@"Export Video", nil), NSLocalizedString(@"Share your video", nil), nil];
    descArray = [[NSArray alloc] initWithObjects:NSLocalizedString(@"onboarding_message_#1", nil), NSLocalizedString(@"onboarding_message_#2", nil), NSLocalizedString(@"onboarding_message_#3", nil), NSLocalizedString(@"onboarding_message_#4", nil), nil];
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
        NSString* nibName;
        if([Utility IsPadDevice])
            nibName = @"BeginView";
        else if([self iPhone6Plus])
            nibName = @"BeginViewPhone@2x";
        else
            nibName = @"BeginViewPhone";
        
        UIView *begView = [[[NSBundle mainBundle] loadNibNamed:nibName owner:self options:nil] objectAtIndex:0];
        return begView;
    } else {
        if([Utility IsPadDevice] || [self iPhone6Plus]) {
            
            UIView *v = [[UIView alloc] initWithFrame:frame];
            UIImageView *imgView = [[UIImageView alloc] initWithFrame:self.stepImgView.frame];
            UIImageView *div = [[UIImageView alloc] initWithFrame:self.divider.frame];
            UILabel *titLbl = [[UILabel alloc] initWithFrame:self.stepTitle.frame];
            UILabel *bdyLbl = [[UILabel alloc] initWithFrame:self.stepBody.frame];
            
            [imgView addConstraints:self.stepImgView.constraints];
            [div addConstraints:self.divider.constraints];
            [titLbl addConstraints:self.stepTitle.constraints];
            [bdyLbl addConstraints:self.stepBody.constraints];
            imgView.autoresizingMask = self.stepImgView.autoresizingMask;
            div.autoresizingMask = self.divider.autoresizingMask;
            titLbl.autoresizingMask = self.stepTitle.autoresizingMask;
            bdyLbl.autoresizingMask = self.stepBody.autoresizingMask;
            
            [titLbl setText:[titlesArray objectAtIndex:index-1]];
            [bdyLbl setText:[descArray objectAtIndex:index-1]];
            [titLbl setFont:[UIFont fontWithName:self.stepTitle.font.fontName size:self.stepTitle.font.pointSize]];
            [bdyLbl setFont:[UIFont fontWithName:self.stepBody.font.fontName size:self.stepBody.font.pointSize]];
            [titLbl setTextAlignment:self.stepTitle.textAlignment];
            [bdyLbl setTextAlignment:self.stepBody.textAlignment];
            
            [titLbl setTextColor:self.stepTitle.textColor];
            [bdyLbl setTextColor:self.stepBody.textColor];
            [bdyLbl setNumberOfLines:self.stepBody.numberOfLines];
            
            [v addSubview:imgView];
            [v addSubview:titLbl];
            [v addSubview:bdyLbl];
            [v addSubview:div];
            
            [div setImage:self.divider.image];
            NSString* extension = @".png";
            if(![Utility IsPadDevice])
                extension = @"_phone.png";
            
            [imgView setImage:[UIImage imageNamed:[NSString stringWithFormat:@"Step-%d_-img%@", index, extension]]];
            return v;
        }
        else {
            
            UIView *someView = [[[NSBundle mainBundle] loadNibNamed:@"StepView" owner:self options:nil] objectAtIndex:0];
            StepView* stepView = (StepView*)someView;
            [stepView setFrame:frame];
            [stepView.stepTitle setText:[titlesArray objectAtIndex:index-1]];
            [stepView.stepDec setText:[descArray objectAtIndex:index-1]];
            
            [stepView.divider setImage:self.divider.image];
            NSString* extension = @".png";
            if(![Utility IsPadDevice])
                extension = @"_phone.png";
            
            [stepView.stepImgView setImage:[UIImage imageNamed:[NSString stringWithFormat:@"Step-%d_-img%@", index, extension]]];
            return stepView;
        }
    }
}

-(BOOL)iPhone6Plus{
    if (([UIScreen mainScreen].scale > 2.0)) return YES;
    return NO;
}

- (void) scrollViewDidScroll:(UIScrollView *)sender
{
        // Update the page when more than 50% of the previous/next page is visible
    if(!pageControlBeingUsed) {
        CGFloat pageWidth = self.scrollView.frame.size.width;
        int page = floor((self.scrollView.contentOffset.x - pageWidth / 2) / pageWidth) + 1;
        self.pageControl.currentPage = page;
    }
    
    [self updateUI];
}

- (void) updateUI
{
    if(self.pageControl.currentPage == SCROLL_COUNT-1) {
        [self.startBtn setHidden:YES];
    } else {
        [self.startBtn setHidden:NO];
        if(self.pageControl.currentPage > 0)
            [self.startBtn setTitle:NSLocalizedString(@"Next", nil) forState:UIControlStateNormal];
        else
            [self.startBtn setTitle:NSLocalizedString(@"Let's get started", nil) forState:UIControlStateNormal];
    }
}

- (IBAction) changePage
{
    CGRect frame;
    frame.origin.x = self.scrollView.frame.size.width * self.pageControl.currentPage;
    frame.origin.y = 0;
    frame.size = self.scrollView.frame.size;
    [self.scrollView scrollRectToVisible:frame animated:YES];
    
    pageControlBeingUsed = YES;
    
}
- (void) scrollViewWillBeginDragging:(UIScrollView *)scrollView
{
    pageControlBeingUsed = NO;
}
- (void) scrollViewDidEndDecelerating:(UIScrollView *)scrollView
{
    pageControlBeingUsed = NO;
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
    if(self.pageControl.currentPage < SCROLL_COUNT-1) {
        self.pageControl.currentPage += 1;
        [self changePage];
    } else {
        [self dismissViewControllerAnimated:YES completion:^{
            [self.delegate closingOnBoard];
        }];
    }
}

- (IBAction)closeBtnAction:(id)sender {
    [self dismissViewControllerAnimated:YES completion:^{
        [self.delegate closingOnBoard];
    }];
}

- (void) dealloc
{
    
}

@end
