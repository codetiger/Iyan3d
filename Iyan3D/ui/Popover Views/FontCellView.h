//
//  UIViewController+FontCellView.h
//  Iyan3D
//
//  Created by karthik on 09/06/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface FontCellView : UICollectionViewCell

@property (weak, nonatomic) IBOutlet UILabel* assetNameLabel;
@property (weak, nonatomic) IBOutlet UILabel* freeTagLabel;
@property (weak, nonatomic) IBOutlet UILabel* displayText;
@property (weak, nonatomic) IBOutlet UIActivityIndicatorView* loadingView;

@end
