//
//  ObjCellView.m
//  Iyan3D
//
//  Created by crazycubes on 22/04/14.
//  Copyright (c) 2014 Smackall Games. All rights reserved.
//

#import "ObjCellView.h"

@implementation ObjCellView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
    }
    return self;
}

- (IBAction)propsAction:(id)sender
{
    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:@"objImport"];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(180.0, 39);
    self.popoverController.animationType = WEPopoverAnimationTypeCrossFade;
    self.popUpVc.delegate=self;
    CGRect rect = _propsBtn.frame;
    rect = [self convertRect:rect fromView:_propsBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self
                          permittedArrowDirections:UIPopoverArrowDirectionAny
                                          animated:YES];
}

- (void)propertiesBtnDelegate:(int)indexValue
{
    if(indexValue == 0) {
        [self.delegate deleteAssetAtIndex:_cellIndex];
    } else if(indexValue == 1) {
    } else if (indexValue == 2) {
    }
    
    [self.popoverController dismissPopoverAnimated:YES];
}

@end
