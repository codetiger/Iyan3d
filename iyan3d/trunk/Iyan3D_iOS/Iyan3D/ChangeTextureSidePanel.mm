//
//  ChangeTextureSidePanel.m
//  Iyan3D
//
//  Created by Sankar on 22/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "ChangeTextureSidePanel.h"
#import "ObjCellView.h"
#import <AssetsLibrary/AssetsLibrary.h>
@interface ChangeTextureSidePanel ()

@end

@implementation ChangeTextureSidePanel{
    NSArray *filesList;
    NSString *imagefileName;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.texturefilesList registerNib:[UINib nibWithNibName:@"ObjCellView" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    NSArray *extensions = [NSArray arrayWithObjects:@"png", @"jpeg", @"jpg", @"PNG", @"JPEG", @"JPG",nil];
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSArray *dirFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirPath error:nil];
    filesList = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
    // Do any additional setup after loading the view from its nib.
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (IBAction)addBtnAction:(id)sender
{
    [self.textureDelegate changeTexture:imagefileName VertexColor:1];
    [self.textureDelegate showOrHideLeftView:NO withView:nil];
}

- (IBAction)importBtnAction:(id)sender
{
    self.ipc= [[UIImagePickerController alloc] init];
    self.ipc.delegate = self;
    self.ipc.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
    if(UI_USER_INTERFACE_IDIOM()==UIUserInterfaceIdiomPhone)
        [self presentViewController:_ipc animated:YES completion:nil];
    else
    {
        popover=[[UIPopoverController alloc]initWithContentViewController:_ipc];
        [popover presentPopoverFromRect:self.importBtn.frame inView:self.view permittedArrowDirections:UIPopoverArrowDirectionUp animated:YES];
    }
    
    
}
- (IBAction)cancelBtnAction:(id)sender
{
    [self.textureDelegate showOrHideLeftView:NO withView:nil];
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
     return [filesList count];
}
- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    ObjCellView *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
    cell.layer.borderColor = [UIColor grayColor].CGColor;
    cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* srcFilePath = [NSString stringWithFormat:@"%@/%@",docDirPath,filesList[indexPath.row]];
    cell.assetNameLabel.text = filesList[indexPath.row];
    cell.assetImageView.image=[UIImage imageWithContentsOfFile:srcFilePath];
    return cell;


}
- (void) collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath
{
    NSArray* indexPathArr = [collectionView indexPathsForVisibleItems];
    for (int i = 0; i < [indexPathArr count]; i++)
    {
        NSIndexPath* indexPath = [indexPathArr objectAtIndex:i];
        UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
        cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
    }
    UICollectionViewCell* cell = [collectionView cellForItemAtIndexPath:indexPath];
    cell.layer.backgroundColor = [UIColor colorWithRed:71.0/255.0 green:71.0/255.0 blue:71.0/255.0 alpha:1.0].CGColor;
    
    imagefileName = [filesList objectAtIndex:indexPath.row];
}

-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info{
    
    if(UI_USER_INTERFACE_IDIOM()==UIUserInterfaceIdiomPhone) {
        [picker dismissViewControllerAnimated:YES completion:nil];
    } else {
        [popover dismissPopoverAnimated:YES];
    }
//    NSURL *imaginfo =[info objectForKey:UIImagePickerControllerReferenceURL];
//    NSString *url = [imaginfo absoluteString];
//    NSArray *parts = [url componentsSeparatedByString:@"/"];
//    NSString *filename = [parts lastObject];
//    NSLog(@"Imagepath : %@",filename );
    NSDate *now = [NSDate date];
    
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    dateFormatter.dateFormat = @"hh:mm:ss";
    [dateFormatter setTimeZone:[NSTimeZone systemTimeZone]];
    NSLog(@"The Current Time is %@",[dateFormatter stringFromDate:now]);
    NSString *date = [@"Texture_"stringByAppendingString:[dateFormatter stringFromDate:now]];
    NSString *fileName = [date stringByAppendingString:@".png"];
    NSLog(@"Imagepath : %@",fileName );
    UIImage* image=(UIImage*)[info objectForKey:@"UIImagePickerControllerOriginalImage"];
    NSData *pngData = UIImagePNGRepresentation(image);
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsPath = [paths objectAtIndex:0]; //Get the docs directory
    NSString *filePath = [documentsPath stringByAppendingPathComponent:fileName]; //Add the file name
    [pngData writeToFile:filePath atomically:YES]; //Write the file
    filesList=nil;
    NSArray *extensions = [NSArray arrayWithObjects:@"png", @"jpeg", @"jpg", @"PNG", @"JPEG", @"JPG",nil];
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSArray *dirFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirPath error:nil];
    filesList = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
    [self.texturefilesList reloadData];


    
    
}
-(void)reloadData{
    [self.texturefilesList reloadData];
}
@end
