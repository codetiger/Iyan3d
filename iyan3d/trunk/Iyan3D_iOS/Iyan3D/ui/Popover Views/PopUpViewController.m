//
//  PopUpViewController.m
//  Iyan3D
//
//  Created by Sankar on 29/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "PopUpViewController.h"
#import "Utility.h"
@implementation PopUpViewController


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil clickedButton:(NSString *)buttonValue{
   self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if(self) {
        clickedBtn=buttonValue;
        [self setTableData:buttonValue];
        return self;
    }
    return nil;    
}
- (void)viewDidLoad {
    
    [super viewDidLoad];
    if([clickedBtn isEqualToString:@"loginBtn"]){
        if ([Utility IsPadDevice]){
            [self.topBar setHidden:NO];
            self.popoverBtns.hidden=YES;
            [self.loginBtn setHidden:NO];
            [self.loginImage setHidden:NO];
 
        }
        else{
            [self.topBar setHidden:NO];
            self.popoverBtns.hidden=YES;
            [self.loginBtn setHidden:NO];
            [self.loginImage setHidden:NO];
            UIImage *image = [UIImage imageNamed: @"Login-Icon_IPhone"];
            [self.loginImage setImage:image];
        }
    }
    
}

- (void) setTableData:(NSString *)clickedBtnName{
    
    if([clickedBtnName isEqualToString:@"importBtn"]){
        tableData = [NSMutableArray arrayWithObjects:@"Models", @"Images", @"Text", @"Light", @"OBJ File", @"Add Bone",nil];
    }
    else if([clickedBtnName isEqualToString:@"animationBtn"]){
        tableData = [NSMutableArray arrayWithObjects:@"Apply Animation", @"Save Animation",nil];
    }
    else if([clickedBtnName isEqualToString:@"exportBtn"]){
        tableData = [NSMutableArray arrayWithObjects:@"Images", @"Videos",nil];
    }
    else if([clickedBtnName isEqualToString:@"infoBtn"]){
        tableData = [NSMutableArray arrayWithObjects:@"About", @"Help",@"Tutorials",@"Settings",@"Contact Us",nil];
    }
    else if([clickedBtnName isEqualToString:@"viewBtn"]){
        tableData = [NSMutableArray arrayWithObjects:@"Front", @"Top",@"Left",@"Back",@"Right",@"Bottom", nil];
    }
    else if([clickedBtnName isEqualToString:@"addFrames"]){
        tableData = [NSMutableArray arrayWithObjects:@"1 Frame", @"24 Frames",@"240 Frames", nil];
    }
    else if([clickedBtnName isEqualToString:@"myObjectsBtn"]){
        tableData = [NSMutableArray arrayWithObjects:@"Camera", @"Light", nil];
    }
    else if([clickedBtnName isEqualToString:@"propertiesBtn"]){
        tableData = [NSMutableArray arrayWithObjects:@"Duplicate", @"Delete",@"Rename", nil];
    }
    else if([clickedBtnName isEqualToString:@"optionsBtn"]){
        tableData = [NSMutableArray arrayWithObjects:@"Move Camera", nil];
    }
    else if([clickedBtnName isEqualToString:@"loginBtn"]){
        self.popoverBtns.hidden=YES;
        [self.loginBtn setHidden:NO];
        [self.loginImage setHidden:NO];
    }
    else {
        tableData = [NSMutableArray arrayWithObjects:@"No data",nil];
    }
 
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [tableData count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *simpleTableIdentifier = @"SimpleTableItem";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];
    
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:simpleTableIdentifier];
    }
    
    cell.textLabel.text = [tableData objectAtIndex:indexPath.row];
    if ([Utility IsPadDevice]){
        if([clickedBtn isEqualToString:@"exportBtn"]){
            cell.textLabel.textColor = [UIColor blackColor];
            [cell.textLabel setFont:[UIFont systemFontOfSize:18]];
            cell.textLabel.textAlignment = NSTextAlignmentCenter;
            if(indexPath.row==0)
                cell.imageView.image = [UIImage imageNamed:@"Export-image_Pad.png"];
            if(indexPath.row==1)
                cell.imageView.image = [UIImage imageNamed:@"Export-video_Pad.png"];
        }
        if([clickedBtn isEqualToString:@"importBtn"]){
            cell.textLabel.textColor = [UIColor blackColor];
            [cell.textLabel setFont:[UIFont systemFontOfSize:18]];
            cell.textLabel.textAlignment = NSTextAlignmentCenter;
            if(indexPath.row==0)
                cell.imageView.image = [UIImage imageNamed:@"Import-model_Pad.png"];
            if(indexPath.row==1)
                cell.imageView.image = [UIImage imageNamed:@"Import-image_Pad.png"];
            if(indexPath.row==2)
                cell.imageView.image = [UIImage imageNamed:@"Import-text_Pad.png"];
            if(indexPath.row==3)
                cell.imageView.image = [UIImage imageNamed:@"Import-Light_Pad.png"];
            if(indexPath.row==4)
                cell.imageView.image = [UIImage imageNamed:@"Import-models_Pad.png"];
            if(indexPath.row==5)
                cell.imageView.image = [UIImage imageNamed:@"Add-Bones_Pad.png"];
        }
    }
    else
    {
        if([clickedBtn isEqualToString:@"exportBtn"]){
            if(indexPath.row==0)
                cell.imageView.image = [UIImage imageNamed:@"Export-image_IPhone.png"];
            if(indexPath.row==1)
                cell.imageView.image = [UIImage imageNamed:@"Export-video_IPhone.png"];
        }
        if([clickedBtn isEqualToString:@"importBtn"]){
            if(indexPath.row==0)
                cell.imageView.image = [UIImage imageNamed:@"Import-model_IPhone.png"];
            if(indexPath.row==1)
                cell.imageView.image = [UIImage imageNamed:@"Import-image_IPhone.png"];
            if(indexPath.row==2)
                cell.imageView.image = [UIImage imageNamed:@"Import-text_IPhone.png"];
            if(indexPath.row==3)
                cell.imageView.image = [UIImage imageNamed:@"Import-Light_IPhone.png"];
            if(indexPath.row==4)
                cell.imageView.image = [UIImage imageNamed:@"Import-models_IPhone.png"];
            if(indexPath.row==5)
                cell.imageView.image = [UIImage imageNamed:@"Add-Bones_IPhone.png"];
        }
        if([clickedBtn isEqualToString:@"myObjectsBtn"]){
            if([cell.textLabel.text isEqualToString:@"CAMERA"])
                cell.imageView.image = [UIImage imageNamed:@"My-objects-Camera_Pad.png"];
            else if([cell.textLabel.text isEqualToString:@"LIGHT"])
                cell.imageView.image = [UIImage imageNamed:@"My-objects-Light_Pad.png"];
            else if([cell.textLabel.text hasPrefix:@"TEXT"])
                cell.imageView.image = [UIImage imageNamed:@"My-objects-Text_Pad"];
            else if([cell.textLabel.text hasPrefix:@"IMAGE"])
                cell.imageView.image = [UIImage imageNamed:@"My-objects-Image_Pad"];
            else if([cell.textLabel.text hasPrefix:@"LIGHT"])
                cell.imageView.image = [UIImage imageNamed:@"My-objects-Light_Pad.png"];
            else
                cell.imageView.image = [UIImage imageNamed:@"My-objects-Models_Pad.png"];

        }
     }
    
    if([clickedBtn isEqualToString:@"viewBtn"] || [clickedBtn isEqualToString:@"infoBtn"] || [clickedBtn isEqualToString:@"addFrames"] ){
        cell.textLabel.textColor = [UIColor colorWithRed:0.0 green:0.478 blue:1.0 alpha:1.0];
        [cell.textLabel setFont:[UIFont systemFontOfSize:18]];
        cell.textLabel.textAlignment = NSTextAlignmentCenter;
    }
    else{
        cell.textLabel.textColor = [UIColor blackColor];
        [cell.textLabel setFont:[UIFont systemFontOfSize:18]];
        cell.textLabel.textAlignment = NSTextAlignmentCenter;
    }
   
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    if([clickedBtn isEqualToString:@"importBtn"]){
        [self.delegate importBtnDelegateAction:(int)indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"animationBtn"]){
        [self.delegate animationBtnDelegateAction:(int)indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"exportBtn"]){
         [self.delegate exportBtnDelegateAction:(int)indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"infoBtn"]){
         [self.delegate infoBtnDelegateAction:(int)indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"viewBtn"]){
         [self.delegate viewBtnDelegateAction:(int)indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"addFrames"]){
        [self.delegate addFrameBtnDelegateAction:(int)indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"myObjectsBtn"]){
        [self.delegate myObjectsBtnDelegateAction:(int)indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"propertiesBtn"]){
        [self.delegate propertiesBtnDelegate:(int)indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"optionsBtn"]){
        [self.delegate optionBtnDelegate:(int)indexPath.row];
    }

    else {
        
    }
    
}
- (IBAction)loginBtnAction:(id)sender {
    [self.delegate loginBtnAction];
}


-(void)UpdateObjectList:(NSArray*) objectList{
    [tableData removeAllObjects];
    [tableData addObjectsFromArray:objectList];
    [self.popoverBtns reloadData];
    [self.delegate highlightObjectList];
    
}

- (void *)updateSelection:(NSIndexPath *)indexPath ScrollPosition:(int)scrolPosition{
   [self.popoverBtns selectRowAtIndexPath:indexPath animated:YES scrollPosition:scrolPosition];
}
- (void *)updateDescelect:(NSIndexPath *)indexPath {
    [self.popoverBtns deselectRowAtIndexPath:indexPath animated:YES];
}


@end
