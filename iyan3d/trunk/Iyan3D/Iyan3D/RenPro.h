//
//  RenPro.h
//  Iyan3D
//
//  Created by vigneshkumar on 19/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "GAI.h"
#import "RenderCell.h"
#include "CacheSystem.h"
@interface RenPro : UIViewController <UITableViewDelegate,UITableViewDataSource>{
    CacheSystem *cache;
    NSMutableArray *nameArray1;
}

@property (retain, nonatomic) IBOutlet UILabel *titleLabel;

@property(weak,nonatomic) IBOutlet UITableView *rennderTasksTable;
@property (nonatomic) float progressValue;



@end
