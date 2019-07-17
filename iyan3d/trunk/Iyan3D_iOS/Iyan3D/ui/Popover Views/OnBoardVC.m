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
    if ([Utility IsPadDevice])
        self.preferredContentSize = CGSizeMake(768, 576);
    [self.beginView setHidden:YES];
    [self setArrayValues];
    [self setupScrollView];
    [self.scrollView bringSubviewToFront:self.pageControl];
    [self.skipBtn setHidden:YES];
    // Do any additional setup after loading the view from its nib.
}

- (void)setArrayValues {
    titlesArray = [[NSArray alloc] initWithObjects:@"Create a Scene", @"Animate Character", @"Export Video", @"Share your video", nil];
    descArray   = [[NSArray alloc] initWithObjects:@"Add characters, environments, props from our library of over 300 assets or import an OBJ file from an external source.", @"Bring your character to life with our pre-defined motions, apply physics simulation or make one of your own actions.", @"Render the final video with the desired quality and style.", @"That's it. Send your 3D movie and showoff your creative skills to your friends.", nil];
}

- (void)setupScrollView {
    self.scrollView.contentSize    = CGSizeMake(self.scrollView.frame.size.width * (SCROLL_COUNT), self.scrollView.frame.size.height);
    self.scrollView.pagingEnabled  = YES;
    self.pageControl.currentPage   = 0;
    self.pageControl.numberOfPages = SCROLL_COUNT;

    for (int i = totalPages; i < SCROLL_COUNT; i++) {
        [self.scrollView addSubview:[self viewForPageInScrollViewAtIndex:i]];
        totalPages++;
    }
}

- (UIView*)viewForPageInScrollViewAtIndex:(int)index {
    float  xOrigin = (index * [self.scrollView bounds].size.width);
    CGRect frame   = self.view.frame;
    frame.origin.x = xOrigin;

    if (index == 0) {
        NSString* nibName;
        if ([Utility IsPadDevice])
            nibName = @"BeginView";
        else if ([self iPhone6Plus])
            nibName = @"BeginViewPhone@2x";
        else
            nibName = @"BeginViewPhone";

        UIView* begView = [[[NSBundle mainBundle] loadNibNamed:nibName owner:self options:nil] objectAtIndex:0];
        return begView;
    } else {
        if ([Utility IsPadDevice] || [self iPhone6Plus]) {
            UIView*      v       = [[UIView alloc] initWithFrame:frame];
            UIImageView* imgView = [[UIImageView alloc] initWithFrame:self.stepImgView.frame];
            UIImageView* div     = [[UIImageView alloc] initWithFrame:self.divider.frame];
            UILabel*     titLbl  = [[UILabel alloc] initWithFrame:self.stepTitle.frame];
            UILabel*     bdyLbl  = [[UILabel alloc] initWithFrame:self.stepBody.frame];

            [imgView addConstraints:self.stepImgView.constraints];
            [div addConstraints:self.divider.constraints];
            [titLbl addConstraints:self.stepTitle.constraints];
            [bdyLbl addConstraints:self.stepBody.constraints];
            imgView.autoresizingMask = self.stepImgView.autoresizingMask;
            div.autoresizingMask     = self.divider.autoresizingMask;
            titLbl.autoresizingMask  = self.stepTitle.autoresizingMask;
            bdyLbl.autoresizingMask  = self.stepBody.autoresizingMask;

            [titLbl setText:[titlesArray objectAtIndex:index - 1]];
            [bdyLbl setText:[descArray objectAtIndex:index - 1]];
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
            if (![Utility IsPadDevice])
                extension = @"_phone.png";

            [imgView setImage:[UIImage imageNamed:[NSString stringWithFormat:@"Step-%d_-img%@", index, extension]]];
            return v;
        } else {
            UIView*   someView = [[[NSBundle mainBundle] loadNibNamed:@"StepView" owner:self options:nil] objectAtIndex:0];
            StepView* stepView = (StepView*)someView;
            [stepView setFrame:frame];
            [stepView.stepTitle setText:[titlesArray objectAtIndex:index - 1]];
            [stepView.stepDec setText:[descArray objectAtIndex:index - 1]];

            [stepView.divider setImage:self.divider.image];
            NSString* extension = @".png";
            if (![Utility IsPadDevice])
                extension = @"_phone.png";

            [stepView.stepImgView setImage:[UIImage imageNamed:[NSString stringWithFormat:@"Step-%d_-img%@", index, extension]]];
            return stepView;
        }
    }
}

- (BOOL)iPhone6Plus {
    if (([UIScreen mainScreen].scale > 2.0))
        return YES;
    return NO;
}

- (void)scrollViewDidScroll:(UIScrollView*)sender {
    // Update the page when more than 50% of the previous/next page is visible
    if (!pageControlBeingUsed) {
        CGFloat pageWidth            = self.scrollView.frame.size.width;
        int     page                 = floor((self.scrollView.contentOffset.x - pageWidth / 2) / pageWidth) + 1;
        self.pageControl.currentPage = page;
    }

    [self updateUI];
}

- (void)updateUI {
    if (self.pageControl.currentPage == SCROLL_COUNT - 1) {
        [self.skipBtn setHidden:NO];
        [self.startBtn setHidden:YES];
    } else {
        [self.skipBtn setHidden:YES];
        [self.startBtn setHidden:NO];
        if (self.pageControl.currentPage > 0)
            [self.startBtn setTitle:@"Next" forState:UIControlStateNormal];
        else
            [self.startBtn setTitle:@"Let's get started" forState:UIControlStateNormal];
    }
}

- (IBAction)changePage {
    CGRect frame;
    frame.origin.x = self.scrollView.frame.size.width * self.pageControl.currentPage;
    frame.origin.y = 0;
    frame.size     = self.scrollView.frame.size;
    [self.scrollView scrollRectToVisible:frame animated:YES];

    pageControlBeingUsed = YES;
}
- (void)scrollViewWillBeginDragging:(UIScrollView*)scrollView {
    pageControlBeingUsed = NO;
}
- (void)scrollViewDidEndDecelerating:(UIScrollView*)scrollView {
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
    if (self.pageControl.currentPage < SCROLL_COUNT - 1) {
        self.pageControl.currentPage += 1;
        [self changePage];
    } else {
        [self dismissViewControllerAnimated:YES
                                 completion:^{
                                     [self.delegate closingOnBoard];
                                 }];
    }
}

- (IBAction)skipBtnAction:(id)sender {
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://www.iyan3dapp.com/tutorial-videos/"]];
}

- (IBAction)closeBtnAction:(id)sender {
    [self dismissViewControllerAnimated:YES
                             completion:^{
                                 [self.delegate closingOnBoard];
                             }];
}

- (void)dealloc {
}

@end
