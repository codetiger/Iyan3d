//
//  AnimationSelectionCollectionViewCell.m
//  Iyan3D
//
//  Created by Sankar on 21/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "AnimationSelectionCollectionViewCell.h"

@implementation AnimationSelectionCollectionViewCell

- (void)awakeFromNib {
    // Initialization code
}

- (IBAction)propsAction:(id)sender {
    NSString* btnStr = @"animProps";
    if (_category == 7)
        btnStr = @"animProps1";

    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:btnStr];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController                    = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(180.0, (_category == 7) ? 39 * 4 : 39 * 3);
    self.popoverController.animationType      = WEPopoverAnimationTypeCrossFade;
    self.popUpVc.delegate                     = self;
    CGRect rect                               = _propsBtn.frame;
    rect                                      = [self convertRect:rect fromView:_propsBtn.superview];
    [self.popoverController presentPopoverFromRect:rect inView:self permittedArrowDirections:UIPopoverArrowDirectionAny animated:YES];
}

- (void)propertiesBtnDelegate:(int)indexValue {
    self.delegate = _parentVC;

    if (indexValue == 0) {
        [self.delegate cloneAnimation:_selectedIndex];
    } else if (indexValue == 1) {
        [self.delegate deleteAnimationAtIndex:_selectedIndex];
    } else if (indexValue == 2) {
        [self.delegate renameAnimation:_selectedIndex];
    } else if (indexValue == 3) {
        [self.delegate setSelectedAnimationAtIndex:_selectedIndex];
    }

    [self.popoverController dismissPopoverAnimated:YES];
}

@end
