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


@implementation RenPro{
    
    NSMutableArray *nameArray,*frameArray,*progressArray,*renArray;
    
    RenderItem *renderItem;
}

- (void)viewDidLoad
{
    
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
    
    downloadQueue = [[NSOperationQueue alloc] init];
}

- (void) updateUIForRenderTasks
{
    [nameArray removeAllObjects];
    [frameArray removeAllObjects];
    [progressArray removeAllObjects];
    
    if(renArray.count>0){
        for (int i = 0; i <[renArray count]; i++){
            renderItem = [renArray objectAtIndex:i];
            NSLog(@"Valuse %d",renderItem.taskId);
            NSString *renderTaskName = [NSString stringWithFormat:@"%@",renderItem.taskName];
            NSLog(@" new Value: %@",renderTaskName);
            [nameArray addObject:renderTaskName];
            NSString *renderTaskId = [NSString stringWithFormat:@"Progress: %d",renderItem.taskProgress];
            NSLog(@" new Value: %@",renderTaskId);
            [frameArray addObject:renderTaskId];
            [progressArray addObject:[NSNumber numberWithInt:renderItem.taskProgress]];
            NSLog(@" Namearray Value: %@",nameArray[i]);
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

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

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
        
        int progressValue = [[progressArray objectAtIndex:indexPath.row] intValue];
        [cell.renProgress setProgress:progressValue];
        
        if(progressValue < 100)
            [cell.downloadBtn setHidden:YES];
        else
            [cell.downloadBtn setHidden:NO];
        
        RenderItem* r = [renArray objectAtIndex:indexPath.row];
        
        [cell.downloadBtn setTag:r.taskId];
        [cell.downloadBtn addTarget:self action:@selector(downloadOutputVideo:) forControlEvents:UIControlEventTouchUpInside];
        
        
    }
    
    return cell;

}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath

{
    
    
    UIAlertView *selectedAlert = [[UIAlertView alloc]
                                  
                                  initWithTitle:[NSString stringWithFormat:@"%@ Selected", [nameArray objectAtIndex:indexPath.row]] message:[NSString stringWithFormat:@"Frames value %@",[frameArray objectAtIndex:indexPath.row]] delegate:nil cancelButtonTitle:@"Got It" otherButtonTitles:nil];
    [selectedAlert show];
    
}

- (void) downloadOutputVideo:(id)sender
{
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [docPaths objectAtIndex:0];

    int taskId = (int)((UIButton*)sender).tag;
    NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/renderFiles/%d/%d.mp4",taskId, taskId];
    NSString* outputFilePath = [NSString stringWithFormat:@"%@/%d.mp4", documentsDirectory, taskId];
    
    DownloadTask* task = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(donwloadCompleted:) returnObject:[NSNumber numberWithInt:taskId] outputFilePath:outputFilePath andURL:url];
    task.taskType = DOWNLOAD_AND_WRITE;
    task.queuePriority = NSOperationQueuePriorityHigh;
    [downloadQueue addOperation:task];

}

- (void) downloadImage:(int)taskId
{
    NSArray* docPaths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [docPaths objectAtIndex:0];
    
    NSString* url = [NSString stringWithFormat:@"https://iyan3dapp.com/appapi/renderFiles/%d/%d.png",taskId, taskId];
    NSString* outputFilePath = [NSString stringWithFormat:@"%@/%d.png", documentsDirectory, taskId];
    
    DownloadTask* task = [[DownloadTask alloc] initWithDelegateObject:self selectorMethod:@selector(donwloadCompleted:) returnObject:[NSNumber numberWithInt:taskId] outputFilePath:outputFilePath andURL:url];
    task.taskType = DOWNLOAD_AND_WRITE;
    task.queuePriority = NSOperationQueuePriorityHigh;
    [downloadQueue addOperation:task];
    
}

- (void) donwloadCompleted:(id)object
{
    DownloadTask* t = (DownloadTask*)object;
    NSString* extension = [t.outputPath pathExtension];
    int taskId = [t.returnObj intValue];
    if ([extension isEqualToString:@"png"]) {
        if([[NSFileManager defaultManager] fileExistsAtPath:t.outputPath]) {
            UIImage *img = [UIImage imageWithContentsOfFile:t.outputPath];
            UIImageWriteToSavedPhotosAlbum(img, self, nil,nil);
        }
    } else if ([extension isEqualToString:@"mp4"]) {
        if(![[NSFileManager defaultManager] fileExistsAtPath:t.outputPath])
            [self downloadImage:taskId];
        UISaveVideoAtPathToSavedPhotosAlbum(t.outputPath, self,  nil, nil);
    }
}

-(void) dealloc
{
    nameArray = nil;
    frameArray = nil;
    progressArray = nil;
    
}

@end
