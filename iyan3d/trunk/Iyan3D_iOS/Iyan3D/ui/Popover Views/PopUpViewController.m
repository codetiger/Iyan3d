//
//  PopUpViewController.m
//  Iyan3D
//
//  Created by Sankar on 29/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "PopUpViewController.h"

@implementation PopUpViewController


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil clickedButton:(NSString *)buttonValue{
   self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if(self) {
        NSLog(@"Clicked Btn %@",buttonValue);
        clickedBtn=buttonValue;
        [self setTableData:buttonValue];
        return self;
    }
    return nil;    
}
- (void)viewDidLoad {
    
    [super viewDidLoad];
    if([clickedBtn isEqualToString:@"loginBtn"]){
        [self.topBar setHidden:NO];
        self.popoverBtns.hidden=YES;
        [self.loginBtn setHidden:NO];
        [self.loginImage setHidden:NO];
    }
}

- (void) setTableData:(NSString *)clickedBtnName{
    
    if([clickedBtnName isEqualToString:@"importBtn"]){
        tableData = [NSArray arrayWithObjects:@"Models", @"Images", @"Text", @"Light", @"OBJ File", @"Add Bone",nil];
    }
    else if([clickedBtnName isEqualToString:@"animationBtn"]){
        tableData = [NSArray arrayWithObjects:@"Apply Animation", @"Save Animation",nil];
    }
    else if([clickedBtnName isEqualToString:@"exportBtn"]){
        tableData = [NSArray arrayWithObjects:@"Images", @"Videos",nil];
    }
    else if([clickedBtnName isEqualToString:@"infoBtn"]){
        tableData = [NSArray arrayWithObjects:@"About", @"Help",@"Tutorials",@"Settings",@"Contact Us",nil];
    }
    else if([clickedBtnName isEqualToString:@"viewBtn"]){
        tableData = [NSArray arrayWithObjects:@"Front", @"Top",@"Left",@"Back",@"Right",@"Bottom", nil];
    }
    else if([clickedBtnName isEqualToString:@"addFrames"]){
        tableData = [NSArray arrayWithObjects:@"1 Frame", @"24 Frames",@"240 Frames", nil];
    }
    else if([clickedBtnName isEqualToString:@"loginBtn"]){
        self.popoverBtns.hidden=YES;
        [self.loginBtn setHidden:NO];
        [self.loginImage setHidden:NO];
    }
    else {
        tableData = [NSArray arrayWithObjects:@"No data",nil];
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
    if([clickedBtn isEqualToString:@"exportBtn"]){
        if(indexPath.row==0)
            cell.imageView.image = [UIImage imageNamed:@"Export-image_Pad.png"];
        if(indexPath.row==1)
            cell.imageView.image = [UIImage imageNamed:@"Export-video_Pad.png"];
    }
    if([clickedBtn isEqualToString:@"importBtn"]){
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
    cell.textLabel.textColor = [UIColor colorWithRed:0.22 green:0.33 blue:0.53 alpha:1.0];
    [cell.textLabel setFont:[UIFont systemFontOfSize:18]];
    cell.textLabel.textAlignment = NSTextAlignmentCenter;
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    NSLog(@"Clicked BTbn %@",clickedBtn);
    if([clickedBtn isEqualToString:@"importBtn"]){
        [self.delegate importBtnDelegateAction:indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"animationBtn"]){
        [self.delegate animationBtnDelegateAction:indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"exportBtn"]){
         [self.delegate exportBtnDelegateAction:indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"infoBtn"]){
         [self.delegate infoBtnDelegateAction:indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"viewBtn"]){
         [self.delegate viewBtnDelegateAction:indexPath.row];
    }
    else if([clickedBtn isEqualToString:@"addFrames"]){
        [self.delegate addFrameBtnDelegateAction:indexPath.row];
    }
    else {
        
    }
    
}
- (IBAction)loginBtnAction:(id)sender {
    [self.delegate loginBtnAction];
}
@end
