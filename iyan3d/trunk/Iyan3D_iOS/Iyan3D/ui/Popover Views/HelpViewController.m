//
//  HelpViewController.m
//  Iyan3D
//
//  Created by crazycubes on 19/08/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "HelpViewController.h"
#import "Utility.h"
#define MAX_RESOLUTION 2048
#define MIN_RESOLUTION 670
#define FROM_AUTO_RIG 1
#define FROM_OBJ_FILE_VIEW 6
#define SCROLL_COUNT 7
#define IMAGE_COUNT 7
#define IMAGE_HELP 100
#define VIDEO_HELP 101

@implementation HelpViewController

@synthesize scrollView, pageControl;

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil CalledFrom:(NSInteger)callerId ScreenWidth:(int)screenWidth ScreenHeight:(int)screenHeight
{
    ScreenWidth= screenWidth;
    ScreenHeight = screenHeight;
    ValueType= callerId;
    currentPage = 0;
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
//    [self.loadingView setHidden:NO];
//    [self.loadingView startAnimating];
    imageArray= [[NSMutableArray alloc] init];
    linkArray= [[NSMutableArray alloc] init];
    self.responseData = [NSMutableData data];
    xOffset = 0;
    if(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
    {
        if(ScreenWidth == MAX_RESOLUTION)
        {
            playBtnWidth = 100;
            playBtnHeight = 101;
        }
        else {
            playBtnWidth = 50;
            playBtnHeight = 51;
        }
        extension = @"-pad";
        buttonPosition = 0.245;
        imageYOrigin = -10;
    }
    else
    {
        if(ScreenWidth <= MIN_RESOLUTION) {
            playBtnWidth = 60;
            playBtnHeight = 61;
            buttonPosition = 0.385;
        } else if(ScreenWidth >= 768) {
            playBtnWidth = 60;
            playBtnHeight = 61;
            buttonPosition = 0.385;
        }
        else{
            playBtnWidth = 30;
            playBtnHeight = 31;
            buttonPosition = 0.220;
        }
        extension = @"";
        imageYOrigin = 10;
    }
    if (self) {
       
        // Custom initialization
    }
    return self;
}
- (void)viewDidLoad
{
    [super viewDidLoad];
    self.screenName = @"HelpView iOS";

    if([Utility IsPadDevice]){
        self.view.layer.borderWidth = 2.0f;
        self.view.layer.borderColor = [UIColor grayColor].CGColor;
    }
    self.doneButton.layer.cornerRadius = 5.0;
    self.moreInfo.layer.cornerRadius = 5.0;
    totalPages = 0;
    if(ValueType == FROM_AUTO_RIG || ValueType == FROM_OBJ_FILE_VIEW){
        scrollviewCount = SCROLL_COUNT;
        imageName = @"AutoRigHelp";
        imageCount = IMAGE_COUNT;
        addPage = 0;
        //[self showImage];
        [self setDataForScrollView];
        [self performSelectorInBackground:@selector(getVideoHelpURL:) withObject:[NSNumber numberWithInt:1]];
    }
    else{
        scrollviewCount = SCROLL_COUNT;
        imageCount = IMAGE_COUNT;
        imageName = @"helpimage";
        addPage = 0;
        //[self showImage];
        [self setDataForScrollView];
        [self performSelectorInBackground:@selector(getVideoHelpURL:) withObject:[NSNumber numberWithInt:2]];
    }
}

-(void) getVideoHelpURL:(NSNumber*) urlId
{
    NSString *videoUrl =[NSString stringWithFormat:@"https://www.smackall.com/app/anim3/videolink.php?viewFrom=%@",urlId];
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:videoUrl]];
    dispatch_async(dispatch_get_main_queue(), ^{
    NSURLConnection *connection = [[NSURLConnection alloc] initWithRequest:request delegate:self];
    [connection start];
    });

}

- (void) viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
}
- (void) viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    if([Utility IsPadDevice]) {
        //[self.view.layer setCornerRadius:20.0f];
        [self.view.layer setMasksToBounds:YES];
    }
}
- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
    [self.responseData setLength:0];
}
- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    [self.responseData appendData:data];
}
- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
}
- (void)connectionDidFinishLoading:(NSURLConnection *)connection {

    [self performSelectorInBackground:@selector(reloadScrollView) withObject:nil];
    [connection cancel];
    connection = nil;
}

-(void) reloadScrollView
{
    NSError *errorJson=nil;
    responseDict = [NSJSONSerialization JSONObjectWithData:self.responseData options:kNilOptions error:&errorJson];
    addPage = (int)[responseDict count];
    for (int i =0; i< [responseDict count]; i++) {
        NSDictionary *dict = responseDict[i];
        [imageArray addObject:[dict valueForKey:@"image"]];
        [linkArray addObject:[dict valueForKey:@"links"]];
        scrollviewCount++;
    }
    //[self showImage];
    if(addPage > 0) {
        if(UI_USER_INTERFACE_IDIOM() != UIUserInterfaceIdiomPad)
        {
            if(ScreenWidth >= 768)
                xOffset = 30;
        }

        totalPages = 0;
        [self setDataForScrollView];
    }

}

-(void) setDataForScrollView
{
    pageControlBeingUsed = NO;
    
    self.scrollView.contentSize = CGSizeMake(self.scrollView.frame.size.width * (scrollviewCount), self.scrollView.frame.size.height);
    self.scrollView.pagingEnabled = YES;
    self.pageControl.currentPage = 0;
    self.pageControl.numberOfPages = scrollviewCount;
    
    for (int i = totalPages; i < scrollviewCount; i++) {
        if(totalPages < IMAGE_COUNT)
            [self.scrollView addSubview:[self viewForPageInScrollViewAtIndex:i WithPageType:IMAGE_HELP]];
        else
            [self.scrollView addSubview:[self viewForPageInScrollViewAtIndex:i WithPageType:VIDEO_HELP]];
        
        totalPages++;
    }
    [self.loadingView stopAnimating];
    [self.loadingView setHidden:YES];
    

    }

- (UIImageView*) viewForPageInScrollViewAtIndex:(int) index WithPageType:(int) pageType
{
    float xOrigin = (index * [self.scrollView bounds].size.width);
    UIImageView * imageForPage = [[UIImageView alloc] initWithFrame:CGRectMake(xOrigin, imageYOrigin,self.scrollView.frame.size.width,self.scrollView.frame.size.height)];
    if(pageType == IMAGE_HELP)
        [imageForPage setImage:[UIImage imageNamed:[NSString stringWithFormat:@"%@%d.png",imageName, index+1]]];
    else {
        UIImage *image = [self GetImage:imageArray[index-IMAGE_COUNT]];
        if(!image) {
            NSString *imageFilePath = [self DownloadImage:imageArray[index-IMAGE_COUNT]];
            image = [UIImage imageWithContentsOfFile:imageFilePath];
        }
        [imageForPage setImage:image];
        
        UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
        [imageForPage setUserInteractionEnabled:YES];
        currentLinkValue=linkArray[index-IMAGE_COUNT];
        [button setFrame:CGRectMake((imageForPage.frame.size.width/2) - playBtnWidth/2 , (imageForPage.frame.size.height/2) - playBtnHeight/2,playBtnWidth, playBtnHeight)];
        [button addTarget:self action:@selector(playButtonPressed:) forControlEvents:UIControlEventTouchUpInside];
        UIImage *buttonImageNormal =[UIImage imageNamed:[NSString stringWithFormat:@"playing%@.png",extension]];
        [button setBackgroundImage:buttonImageNormal forState:UIControlStateNormal];
        [imageForPage addSubview:button];

    }
    imageForPage.contentMode = UIViewContentModeScaleAspectFit;
//    imageForPage.autoresizingMask = UIViewAutoresizingNone;
    //[imageForPage sizeToFit];
    [imageForPage setTag:index];
    return imageForPage;
}

-(void)reAlignScrollViewContents
{
    for (int i = 0; i < totalPages; i++) {
        float xOrigin = (i * self.scrollView.frame.size.width);
        UIImageView* imageForPage = (UIImageView*)[self.scrollView viewWithTag:i];
        [imageForPage setFrame:CGRectMake(xOrigin, imageYOrigin,self.scrollView.frame.size.width,self.scrollView.frame.size.height)];

    }
}

- (void)playButtonPressed:(id)sender
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:[NSString stringWithFormat:@"%@",currentLinkValue]]];
}
- (NSString*) DownloadImage:(NSString *)nameImage{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cacheDirectory = [paths objectAtIndex:0];
    NSString* fileName = [NSString stringWithFormat:@"%@/%@", cacheDirectory, nameImage];
    NSString* url = [NSString stringWithFormat:@"https://www.smackall.com/app/anim/videohelp/%@",nameImage];
    NSData *data = [NSData dataWithContentsOfURL:[NSURL URLWithString:url]];
    UIImage* image = [UIImage imageWithData:data];
    [UIImagePNGRepresentation(image) writeToFile:fileName options:NSAtomicWrite error:nil];
    return fileName;
}
- (UIImage*) GetImage:(NSString*)name{
    UIImage* image = nil;
    
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cacheDirectory = [paths objectAtIndex:0];
    displayfilePathforGetImage = [NSString stringWithFormat:@"%@/%@", cacheDirectory, name];
    
    if([[NSFileManager defaultManager] fileExistsAtPath:displayfilePathforGetImage]) {
        image = [UIImage imageWithContentsOfFile:displayfilePathforGetImage];
    }
    return image;
}
- (void) scrollViewDidScroll:(UIScrollView *)sender
{
    if (!pageControlBeingUsed) {
        // Update the page when more than 50% of the previous/next page is visible
        CGFloat pageWidth = self.scrollView.frame.size.width;
        int page = floor((self.scrollView.contentOffset.x - pageWidth / 2) / pageWidth) + 1;
        self.pageControl.currentPage = page;
    }
}
- (IBAction) changePage
{
    CGRect frame;
    frame.origin.x = self.scrollView.frame.size.width * self.pageControl.currentPage;
    frame.origin.y = 0;
    frame.size = self.scrollView.frame.size;
    [self.scrollView scrollRectToVisible:frame animated:YES];
    
    // Keep track of when scrolls happen in response to the page control
    // value changing. If we don't do this, a noticeable "flashing" occurs
    // as the the scroll delegate will temporarily switch back the page
    // number.
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
- (void) didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}
- (IBAction) doneButtonAction:(id)sender
{
    [self dismissViewControllerAnimated:YES completion:nil];
}
- (IBAction) moreInfoButtonAction:(id)sender
{
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"https://www.smackall.com/iyan3d/"]];
}

-(void) dealloc
{
    self.scrollView = nil;
    self.pageControl = nil;
    self.doneButton = nil;
    self.moreInfo = nil;
    imageName = nil;
}

@end
