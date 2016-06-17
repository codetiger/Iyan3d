//
//  MediaPreviewVC.h
//  Iyan3D
//
//  Created by Karthik on 15/06/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <MediaPlayer/MediaPlayer.h>
#import "GAI.h"

@interface MediaPreviewVC : UIViewController
{
    int mediaType;
    NSString *mediaPath;
}

@property (weak, nonatomic) IBOutlet UIView *medView;
@property (weak, nonatomic) IBOutlet UIButton *shareBtn;
@property (weak, nonatomic) IBOutlet UIButton *closeBtn;
@property (strong, nonatomic) MPMoviePlayerController *moviePlayer;


- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil mediaType:(int)medType medPath:(NSString*) medPath;
- (IBAction)closeAction:(id)sender;
- (IBAction)shareAction:(id)sender;

@end
