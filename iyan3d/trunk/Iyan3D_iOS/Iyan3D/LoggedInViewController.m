//
//  LoggedInViewController.m
//  Iyan3D
//
//  Created by Sankar on 31/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "LoggedInViewController.h"
#import "RenderTableViewCell.h"
@interface LoggedInViewController ()

@end

@implementation LoggedInViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    renderData = @{@"In Progress" : @[@"My Scene 1", @"My Scene 2", @"My Scene 3",@"My Scene 4",@"My Scene 5"],
                   @"Completed" : @[@"My Scene 1", @"My Scene 2",@"My Scene 3",@"My Scene 4"]};
    [self.renderStatus registerClass:[RenderTableViewCell class] forCellReuseIdentifier:@"Cell"];
    renderSectionTitles = [[renderData allKeys] sortedArrayUsingSelector:@selector(localizedCaseInsensitiveCompare:)];
    self.creditsView.layer.cornerRadius = 10;
    self.creditsView.layer.masksToBounds = YES;
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return [renderSectionTitles count];
}
- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    return [renderSectionTitles objectAtIndex:section];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    NSString *sectionTitle = [renderSectionTitles objectAtIndex:section];
    NSArray *sectionAnimals = [renderData objectForKey:sectionTitle];
    return [sectionAnimals count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    RenderTableViewCell *cell = (RenderTableViewCell *)[tableView dequeueReusableCellWithIdentifier:@"RenderTableViewCell"];
    if (cell == nil)
    {
        NSArray *nib = [[NSBundle mainBundle] loadNibNamed:@"RenderTableViewCell" owner:self options:nil];
        cell = [nib objectAtIndex:0];
    }
    // Configure the cell...
    NSString *sectionTitle = [renderSectionTitles objectAtIndex:indexPath.section];
    NSArray *sectionData = [renderData objectForKey:sectionTitle];
    NSString *render = [sectionData objectAtIndex:indexPath.row];
    cell.renderlabel.text = render;
    return cell;
}

- (IBAction)signOutBtn:(id)sender {
}
@end
