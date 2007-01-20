//
//  RenPro.m
//  Iyan3D
//
//  Created by vigneshkumar on 19/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import "RenPro.h"
#import "RenderItem.h"
#import "AFNetworking.h"
#import "AFHTTPRequestOperation.h"



@interface RenPro ()

@end

@implementation RenPro{
    
    NSMutableArray *nameArray,*frameArray,*progressArray,*renArray;
    
    RenderItem *renderItem;
}

- (void)viewDidLoad {
    
    [super viewDidLoad];
    
    // Do any additional setup after loading the view from its nib.
    // nameArray = [[NSMutableArray alloc] initWithObjects:@"Project 1",@"Project 2",@"Project 3",@"project 4",@"Project 5", nil];
    //frameArray = [[NSMutableArray alloc] initWithObjects:@"Frame:30/150",@"Frame:22/150",@"Frame:50/150",@"Frame:/44/150",@"Frame:/44/150",@"Frame:/44/150", nil];
    
    cache = [CacheSystem cacheSystem];
    //[cache OpenDatabase];
    
    nameArray=[[NSMutableArray alloc]init];
    frameArray=[[NSMutableArray alloc]init];
    progressArray = [[NSMutableArray alloc] init];
    
    renArray=[cache getRenderTask];
    [self updateUIForRenderTasks];
    
    for(int i = 0; i < [renArray count]; i++) {
        renderItem = [renArray objectAtIndex:i];
        [self getRenderTaskProgress:renderItem.taskId];
    }
}

- (void) updateUIForRenderTasks
{
    if(renArray.count>0){
        NSLog(@"Array Contains Some data");
        for (int i = 0; i <[renArray count]; i++){
            renderItem = [renArray objectAtIndex:i];
            NSLog(@"Valuse %d",renderItem.taskId);
            NSString *renderTaskName = [NSString stringWithFormat:@"%@",renderItem.taskName];
            NSLog(@" new Value: %@",renderTaskName );
            [nameArray addObject:renderTaskName];
            NSString *renderTaskId = [NSString stringWithFormat:@"Progress: %d",renderItem.taskProgress];
            NSLog(@" new Value: %@",renderTaskId );
            [frameArray addObject:renderTaskId];
            [progressArray addObject:[NSNumber numberWithInt:renderItem.taskProgress]];
            NSLog(@" Namearray Value: %@",nameArray[i] );
        }
    }
    [self.rennderTasksTable reloadData];
}

- (void) getRenderTaskProgress:(int)taskId
{
    NSURL* url = [NSURL URLWithString:@"https://www.iyan3dapp.com/appapi/checkprogress.php"];
    NSString* postPath = @"https://www.iyan3dapp.com/appapi/checkprogress.php";
    
    AFHTTPClient* httpClient = [[AFHTTPClient alloc] initWithBaseURL:url];
    NSMutableURLRequest *request = [httpClient requestWithMethod:@"GET" path:postPath parameters:[NSDictionary dictionaryWithObjectsAndKeys:[NSString stringWithFormat:@"%d",taskId], @"taskid", nil]];
    
    AFHTTPRequestOperation* operation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    __block BOOL complete = NO;
    [operation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation* operation, id responseObject) {
        //ret = [self handle:data];
        complete = YES;
        int progress = [[operation responseString] intValue];
        if(progress != -1)
            [cache updateRenderTask:taskId WithProgress:progress];
        else
            [cache deleteRenderTaskData:taskId];
        
        renArray = [cache getRenderTask];
        [self performSelectorOnMainThread:@selector(updateUIForRenderTasks) withObject:nil waitUntilDone:YES];
    } failure:^(AFHTTPRequestOperation* operation, NSError* error) {
        NSLog(@" Request failed with error %@ ", error.localizedDescription);
    }];
    
    [operation start];
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

#pragma - mark UITableView Methods

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section

{
    
    return [nameArray count];
    
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath

{
    
    return 65;
    
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath

{
    static NSString *simpleTableIdentifier = @"RenderCell";
    
    RenderCell *cell = (RenderCell *)[tableView dequeueReusableCellWithIdentifier:simpleTableIdentifier];

    if (cell == nil)
        
    {
        
        NSArray *nibArray = [[NSBundle mainBundle] loadNibNamed:@"RenderCell" owner:self options:nil];
        
        cell = [nibArray objectAtIndex:0];
        
    }

    if(indexPath.row < [progressArray count]) {
        
        
        cell.nameLabel.text = [nameArray objectAtIndex:indexPath.row];
        
        
        cell.frameLabel.text = [frameArray objectAtIndex:indexPath.row];
        
        [cell.renProgress setProgress:[[progressArray objectAtIndex:indexPath.row] intValue]];
        
    }
    
    return cell;

}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath

{
    
    UIAlertView *selectedAlert = [[UIAlertView alloc]
                                  
                                  initWithTitle:[NSString stringWithFormat:@"%@ Selected", [nameArray objectAtIndex:indexPath.row]] message:[NSString stringWithFormat:@"Frames value %@",[frameArray objectAtIndex:indexPath.row]] delegate:nil cancelButtonTitle:@"Got It" otherButtonTitles:nil];
    [selectedAlert show];
    
}

-(void) dealloc
{
    nameArray = nil;
    frameArray = nil;
    progressArray = nil;
    
}

@end
