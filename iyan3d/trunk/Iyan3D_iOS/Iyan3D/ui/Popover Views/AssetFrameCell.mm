//
//  AssetFrameCell.m
//  Iyan3D
//
//  Created by sabish on 22/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "AssetFrameCell.h"

@implementation AssetFrameCell

- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
    }
    return self;
}

- (IBAction)propsAction:(id)sender {
    NSString* btnStr = @"assetProps";

    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:btnStr];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController                    = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(180.0, 39 * 3);
    self.popoverController.animationType      = WEPopoverAnimationTypeCrossFade;
    self.popUpVc.delegate                     = self;
    CGRect rect                               = _propsBtn.frame;
    rect                                      = [self convertRect:rect fromView:_propsBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self
                          permittedArrowDirections:UIPopoverArrowDirectionAny
                                          animated:YES];
}

- (void)propertiesBtnDelegate:(int)indexValue {
    if (indexValue == 0) {
        [self.delegate cloneAssetAtIndex:_cellIndex];
    } else if (indexValue == 1) {
        [self.delegate deleteAssetAtIndex:_cellIndex];
    } else if (indexValue == 2) {
        [self.delegate renameAssetAtIndex:_cellIndex];
    }

    [self.popoverController dismissPopoverAnimated:YES];
}

@end
