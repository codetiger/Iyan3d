//
//  LoggedInViewController.h
//  Iyan3D
//
//  Created by Sankar on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface LoggedInViewController : UIViewController<UITableViewDataSource,UITableViewDelegate>{
    NSDictionary *renderData;
    NSArray *renderSectionTitles;
    
}
@property (weak, nonatomic) IBOutlet UIView *creditsView;
@property (weak, nonatomic) IBOutlet UITableView *renderStatus;
@property (weak, nonatomic) IBOutlet UIButton *signOutBtn;

- (IBAction)signOutBtn:(id)sender;
@end
