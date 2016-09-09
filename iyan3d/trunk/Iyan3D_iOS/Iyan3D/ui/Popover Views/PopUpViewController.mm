//
//  PopUpViewController.m
//  Iyan3D
//
//  Created by Sankar on 29/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "PopUpViewController.h"
#import "Utility.h"
#import <AppHelper.h>
#import "TableViewCell.h"

@implementation PopUpViewController


- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil clickedButton:(NSString *)buttonValue
{
   self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if(self) {
        clickedBtn = buttonValue;
        [self setTableData:buttonValue];
        return self;
    }
    return nil;
}

- (void) viewDidLoad
{
    [super viewDidLoad];
    if ([clickedBtn isEqualToString:@"myObjectsBtn"]) {
        _popoverBtns.alwaysBounceVertical = YES;
        
        if ([[AppHelper getAppHelper]userDefaultsBoolForKey:@"multiSelectOption"] == YES)
            self.popoverBtns.allowsMultipleSelection = YES;
        else
            self.popoverBtns.allowsMultipleSelection = NO;
    } else {
        _popoverBtns.alwaysBounceVertical = NO;
        self.popoverBtns.allowsMultipleSelection = NO;
    }
}

- (void) viewDidAppear:(BOOL)animated
{
    if([clickedBtn isEqualToString:@"importBtn"] && ![Utility IsPadDevice]) {
        _popoverBtns.rowHeight = self.view.frame.size.height/8;
        [_popoverBtns reloadData];
    }
}

- (void) setTableData:(NSString *)clickedBtnName
{
    
    if([clickedBtnName isEqualToString:@"importBtn"]) {
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"ImportFile", nil),
                     NSLocalizedString(@"Text", nil),
                     NSLocalizedString(@"Images", nil),
                     NSLocalizedString(@"Videos", nil),
                     NSLocalizedString(@"Light", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"animationBtn"]) {
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"Apply Animation", nil),
                     NSLocalizedString(@"Save Animation", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"exportBtn"]) {
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"Images", nil),
                     NSLocalizedString(@"Videos", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"infoBtn"]) {
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"Tutorials", nil),
                     NSLocalizedString(@"Settings", nil),
                     NSLocalizedString(@"Rate This App", nil),
                     NSLocalizedString(@"Follow Us", nil),
                     NSLocalizedString(@"Contact Us", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"viewBtn"]) {
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"Front", nil),
                     NSLocalizedString(@"Top", nil),
                     NSLocalizedString(@"Left", nil),
                     NSLocalizedString(@"Back", nil),
                     NSLocalizedString(@"Right", nil),
                     NSLocalizedString(@"Bottom", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"addFrames"]) {
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"1 Frame", nil),
                     NSLocalizedString(@"24 Frames", nil),
                     NSLocalizedString(@"240 Frames", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"myObjectsBtn"]) {
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"Camera", nil),
                     NSLocalizedString(@"Light", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"propertiesBtn"] || [clickedBtnName isEqualToString:@"assetProps"]){
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"Clone", nil),
                     NSLocalizedString(@"Delete", nil),
                     NSLocalizedString(@"Rename", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"propertiesBtn1"]) {
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"Clone", nil),
                     NSLocalizedString(@"Delete", nil),
                     NSLocalizedString(@"Rename", nil),
                     NSLocalizedString(@"Share", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"optionsBtn"]) {
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"Move Camera", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"animProps1"]) {
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"Clone", nil),
                     NSLocalizedString(@"Delete", nil),
                     NSLocalizedString(@"Rename", nil),
                     NSLocalizedString(@"Publish", nil),
                     nil];
    } else if([clickedBtnName isEqualToString:@"objImport"]) {
        [self allowMultipleSelection:NO];
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"Delete", nil),
                     nil];
    } else {
        tableData = [NSMutableArray arrayWithObjects:
                     NSLocalizedString(@"No data", nil),
                     nil];
    }
}

- (void) didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (NSInteger) tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [tableData count];
}

- (UITableViewCell *) tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *simpleTableIdentifier = @"SimpleTableItem";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];
    
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:simpleTableIdentifier];
    }
    
    cell.textLabel.text = [tableData objectAtIndex:indexPath.row];
    cell.textLabel.textColor = [UIColor blackColor];
    [cell.textLabel setFont:[UIFont systemFontOfSize:18]];
    cell.textLabel.textAlignment = NSTextAlignmentCenter;
    
    if ([Utility IsPadDevice]) {
        if([clickedBtn isEqualToString:@"exportBtn"]) {
            
            if(indexPath.row == 0)
                cell.imageView.image = [UIImage imageNamed:@"Export-image_Pad"];
            else if(indexPath.row == 1)
                cell.imageView.image = [UIImage imageNamed:@"Export-video_Pad"];
            
        } else if([clickedBtn isEqualToString:@"importBtn"]) {
            if(indexPath.row == 0)
                cell.imageView.image = [UIImage imageNamed:@"Import-model_Pad"];
            else if(indexPath.row == 1)
                cell.imageView.image = [UIImage imageNamed:@"Import-image_Pad"];
            else if(indexPath.row == 2)
                cell.imageView.image = [UIImage imageNamed:@"Export-video_Pad"];
            else if(indexPath.row == 3)
                cell.imageView.image = [UIImage imageNamed:@"Import-text_Pad"];
            else if(indexPath.row == 4)
                cell.imageView.image = [UIImage imageNamed:@"Import-Light_Pad"];
            else if(indexPath.row == 5)
                cell.imageView.image = [UIImage imageNamed:@"Import-models_Pad"];
            else if(indexPath.row == 6)
                cell.imageView.image = [UIImage imageNamed:@"Add-Bones_Pad"];
            else if(indexPath.row == 7)
                cell.imageView.image = [UIImage imageNamed:@"Particles"];
        }
    } else {
        if([clickedBtn isEqualToString:@"exportBtn"]) {
            if(indexPath.row == 0 )
                cell.imageView.image = [UIImage imageNamed:@"Export-image_IPhone"];
            else if(indexPath.row == 1 )
                cell.imageView.image = [UIImage imageNamed:@"Export-video_IPhone"];
        } else if([clickedBtn isEqualToString:@"importBtn"]) {
            if(indexPath.row == 0)
                cell.imageView.image = [UIImage imageNamed:@"Import-model_IPhone"];
            else if(indexPath.row == 1)
                cell.imageView.image = [UIImage imageNamed:@"Import-image_IPhone"];
            else if(indexPath.row == 2)
                cell.imageView.image = [UIImage imageNamed:@"Export-video_IPhone"];
            else if(indexPath.row == 3)
                cell.imageView.image = [UIImage imageNamed:@"Import-text_IPhone"];
            else if(indexPath.row == 4)
                cell.imageView.image = [UIImage imageNamed:@"Import-Light_IPhone"];
            else if(indexPath.row == 5)
                cell.imageView.image = [UIImage imageNamed:@"Import-models_IPhone"];
            else if(indexPath.row == 6)
                cell.imageView.image = [UIImage imageNamed:@"Add-Bones_IPhone"];
            else if(indexPath.row == 7)
                cell.imageView.image = [UIImage imageNamed:@"Particle_IPhone"];

        } else if([clickedBtn isEqualToString:@"myObjectsBtn"]) {
            
            enum NODE_TYPE nodeType = [self.delegate getNodeType:(int)indexPath.row];
            
                if(nodeType == NODE_CAMERA)
                    cell.imageView.image = [UIImage imageNamed:@"My-objects-Camera_IPhone"];
                else if(nodeType == NODE_LIGHT)
                    cell.imageView.image = [UIImage imageNamed:@"My-objects-Light_IPhone"];
                else if(nodeType == NODE_ADDITIONAL_LIGHT)
                    cell.imageView.image = [UIImage imageNamed:@"My-objects-Light_IPhone"];
                else if(nodeType == NODE_TEXT_SKIN)
                    cell.imageView.image = [UIImage imageNamed:@"My-objects-Text_IPhone"];
                else if(nodeType == NODE_TEXT)
                    cell.imageView.image = [UIImage imageNamed:@"My-objects-Text_IPhone"];
                else if(nodeType == NODE_IMAGE)
                    cell.imageView.image = [UIImage imageNamed:@"My-objects-Image_IPhone"];
                else if(nodeType == NODE_PARTICLES)
                    cell.imageView.image = [UIImage imageNamed:@"My-objects-Particles"];
                else if(nodeType == NODE_VIDEO)
                    cell.imageView.image = [UIImage imageNamed:@"My-objects-Camera_IPhone"];
                else
                    cell.imageView.image = [UIImage imageNamed:@"My-objects-Models_IPhone"];

        }
    }
    
    if([clickedBtn isEqualToString:@"viewBtn"] || [clickedBtn isEqualToString:@"infoBtn"] || [clickedBtn isEqualToString:@"addFrames"] ) {
        cell.textLabel.textColor = [UIColor colorWithRed:0.0 green:0.478 blue:1.0 alpha:1.0];
        [cell.textLabel setFont:[UIFont systemFontOfSize:18]];
        cell.textLabel.textAlignment = NSTextAlignmentCenter;
    } else {
        cell.textLabel.textColor = [UIColor blackColor];
        [cell.textLabel setFont:[UIFont systemFontOfSize:18]];
        cell.textLabel.textAlignment = NSTextAlignmentCenter;
    }
    return cell;
}

- (void) tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    if([clickedBtn isEqualToString:@"importBtn"]) {
        [self.delegate importBtnDelegateAction:(int)indexPath.row];
    } else if([clickedBtn isEqualToString:@"animationBtn"]) {
        [self.delegate animationBtnDelegateAction:(int)indexPath.row];
    } else if([clickedBtn isEqualToString:@"exportBtn"]) {
         [self.delegate exportBtnDelegateAction:(int)indexPath.row];
    } else if([clickedBtn isEqualToString:@"infoBtn"]) {
         [self.delegate infoBtnDelegateAction:(int)indexPath.row];
    } else if([clickedBtn isEqualToString:@"viewBtn"]) {
         [self.delegate viewBtnDelegateAction:(int)indexPath.row];
    } else if([clickedBtn isEqualToString:@"addFrames"]) {
        [self.delegate addFrameBtnDelegateAction:(int)indexPath.row];
    } else if([clickedBtn isEqualToString:@"myObjectsBtn"]) {
        [self.delegate myObjectsBtnDelegateAction:(int)indexPath.row];
    } else if([clickedBtn isEqualToString:@"propertiesBtn"] || [clickedBtn isEqualToString:@"propertiesBtn1"]){
        [self.delegate propertiesBtnDelegate:(int)indexPath.row];
    } else if([clickedBtn isEqualToString:@"optionsBtn"]) {
        [self.delegate optionBtnDelegate:(int)indexPath.row];
    } else if ([clickedBtn isEqualToString:@"animProps1"] || [clickedBtn isEqualToString:@"assetProps"] || [clickedBtn isEqualToString:@"objImport"]) {
        [self.delegate propertiesBtnDelegate:(int)indexPath.row];
    }
}

- (void) UpdateObjectList:(NSArray*) objectList
{
    [tableData removeAllObjects];
    [tableData addObjectsFromArray:objectList];
    [self.popoverBtns reloadData];
    [self.delegate highlightObjectList];
}

- (void *) updateSelection:(NSIndexPath *)indexPath ScrollPosition:(int)scrolPosition
{
//   [self.popoverBtns selectRowAtIndexPath:indexPath animated:YES scrollPosition:scrolPosition];
}

- (void *) updateDescelect:(NSIndexPath *)indexPath
{
    [self.popoverBtns deselectRowAtIndexPath:indexPath animated:YES];
}

- (void *) allowMultipleSelection :(BOOL)isMultipleselectionEnabled
{
    self.popoverBtns.allowsMultipleSelection=isMultipleselectionEnabled;
}

@end
