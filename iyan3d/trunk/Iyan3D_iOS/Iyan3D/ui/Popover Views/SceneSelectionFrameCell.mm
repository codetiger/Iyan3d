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
    NSString* clickedBtn = @"propertiesBtn";
    bool isi3dFileExists = [self.delegate Isi3dExists:_SelectedindexValue];
    if(isi3dFileExists)
        clickedBtn = @"propertiesBtn1";
    
    _popUpVc = [[PopUpViewController alloc] initWithNibName:@"PopUpViewController" bundle:nil clickedButton:clickedBtn];
    [_popUpVc.view setClipsToBounds:YES];
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_popUpVc];
    self.popoverController.popoverContentSize = CGSizeMake(180.0, (isi3dFileExists) ? 39*4 : 39*3);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    self.popUpVc.delegate=self;
    CGRect rect = _propertiesBtn.frame;
    rect = [self convertRect:rect fromView:_propertiesBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self
                          permittedArrowDirections:UIPopoverArrowDirectionAny
                                          animated:YES];

}

- (void) propertiesBtnDelegate:(int)indexValue{
    if(indexValue==0){
        NSLog(@"Duplicate Delegate %ld",(long)_SelectedindexValue);
        [self.delegate duplicateScene:(int)_SelectedindexValue];
        [self.popoverController dismissPopoverAnimated:YES];
    }
    else if(indexValue==1){
        [self.delegate deleteScene:(int)_SelectedindexValue];
        [self.popoverController dismissPopoverAnimated:YES];
    }
    else if(indexValue==2){
        [self.delegate renameScene:(int)_SelectedindexValue];
        [self.popoverController dismissPopoverAnimated:YES];
    } else if (indexValue == 3) {
        [self.delegate shareScene:[NSNumber numberWithInt:(int)_SelectedindexValue]];
        [self.popoverController dismissPopoverAnimated:YES];
    }
}

- (void) animationBtnDelegateAction:(int)indexValue
{
    
}
- (void) importBtnDelegateAction:(int)indexValue
{
    
}
- (void) exportBtnDelegateAction:(int)indexValue
{
    
}
- (void) viewBtnDelegateAction:(int)indexValue
{
    
}
- (void) infoBtnDelegateAction:(int)indexValue
{
    
}
- (void) addFrameBtnDelegateAction:(int)indexValue
{
    
}
- (void) myObjectsBtnDelegateAction:(int)indexValue
{
    
}
- (void)highlightObjectList
{
    
}
- (void) loginBtnAction
{
    
}
- (void) optionBtnDelegate:(int)indexValue
{
    
}
- (NODE_TYPE) getNodeType:(int)nodeId
{
    
}

@end