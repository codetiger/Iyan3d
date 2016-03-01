//
//  SceneSelectionFrameCell.m
//  Iyan3D
//
//  Created by harishankarn on 16/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "SceneSelectionFrameCell.h"

@implementation SceneSelectionFrameCell

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
    }
    return self;
}

- (IBAction)propertiesAction:(id)sender {
    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"propertiesBtn"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(180.0, 39*3);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popUpVc.delegate=self;
    CGRect rect = _propertiesBtn.frame;
    rect = [self convertRect:rect fromView:_propertiesBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self
                          permittedArrowDirections:UIPopoverArrowDirectionUp
                                          animated:YES];

}

- (void) propertiesBtnDelegate:(int)indexValue{
    if(indexValue==0){
        NSLog(@"Duplicate Delegate %ld",(long)_SelectedindexValue);
        [self.delegate duplicateScene:_SelectedindexValue];
        [self.popoverController dismissPopoverAnimated:YES];
    }
    else if(indexValue==1){
        [self.delegate deleteScene:_SelectedindexValue];
        [self.popoverController dismissPopoverAnimated:YES];
    }
    else if(indexValue==2){
        [self.delegate renameScene:_SelectedindexValue];
        [self.popoverController dismissPopoverAnimated:YES];
    }
}
@end