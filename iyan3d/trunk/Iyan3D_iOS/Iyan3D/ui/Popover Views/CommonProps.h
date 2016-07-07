//
//  CommonProps.h
//  Iyan3D
//
//  Created by Karthik on 05/07/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import "Options.h"

@interface CommonProps : GAITrackedViewController < UITableViewDelegate, UITableViewDataSource >
{
    NSArray * propArray;
}

@property (weak, nonatomic) IBOutlet UITableView *propTableView;


- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil WithProps:(NSArray*) props;

@end
