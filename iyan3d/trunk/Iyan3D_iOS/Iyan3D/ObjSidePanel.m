//
//  ObjSidePanel.m
//  Iyan3D
//
//  Created by Sankar on 08/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "ObjSidePanel.h"
#import "ObjCellView.h"
@interface ObjSidePanel ()

@end

@implementation ObjSidePanel{
    NSArray *filesList;
    NSArray *tempFiles;
}

- (void)viewDidLoad {
    [super viewDidLoad];
     [self.importFilesCollectionView registerNib:[UINib nibWithNibName:@"ObjCellView" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSArray *dirFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirPath error:nil];
    filesList = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.obj'"]];
    
    NSLog(@"Obj path: %@",docDirPath);
    
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];

}


- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    return [filesList count];
}
- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    NSString *extension = [[filesList objectAtIndex:indexPath.row]pathExtension];
    NSLog(@"path extension %@",extension);
    if([extension isEqualToString:@"obj"]){
        ObjCellView *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
        cell.assetNameLabel.text = filesList[indexPath.row];
        cell.layer.borderColor = [UIColor grayColor].CGColor;
        cell.assetImageView.image =[UIImage imageNamed:@"objfile.png"];
        cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
        return cell;
   
    }
    else{
        NSArray* cachepaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* cacheDirectory = [cachepaths objectAtIndex:0];
        NSString* srcFilePath = [NSString stringWithFormat:@"%@/texureFile/%@",cacheDirectory,tempFiles[indexPath.row]];
        ObjCellView *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
        cell.assetNameLabel.text = tempFiles[indexPath.row];
        cell.layer.borderColor = [UIColor grayColor].CGColor;
        cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
        cell.assetImageView.image=[UIImage imageWithContentsOfFile:srcFilePath];
        return cell;

    }
    
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
    NSString *extension = [[filesList objectAtIndex:indexPath.row]pathExtension];
    if([extension isEqualToString:@"obj"])
    {
        NSLog(@"Obj File Selected %@",[filesList objectAtIndex:indexPath.row]);
    }
    else
    {
        NSLog(@"Texture File Selected %@",[filesList objectAtIndex:indexPath.row]);
    }
}

- (IBAction)addBtnAction:(id)sender {
    NSLog(@"Sender %@",self.addBtn.titleLabel.text);
    if([self.addBtn.titleLabel.text isEqualToString:@"Next"]){
        filesList=nil;
        NSArray *extensions = [NSArray arrayWithObjects:@"png", @"jpeg", @"jpg", @"PNG", @"JPEG", nil];
        NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* docDirPath = [srcDirPath objectAtIndex:0];
        NSArray *dirFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirPath error:nil];
        filesList = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
        NSArray* cachepaths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* cacheDirectory = [cachepaths objectAtIndex:0];
        NSString *dataPath = [cacheDirectory stringByAppendingPathComponent:@"/texureFile"];
         NSArray *dirFiles1 = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:dataPath error:nil];
        tempFiles = [dirFiles1 filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
        [self.importFilesCollectionView reloadData];
        [self.addBtn setTitle:@"ADD TO SCENE" forState:UIControlStateNormal];
        [self.viewTitle setText:@"Import Texture"];
    }
    if([self.addBtn.titleLabel.text isEqualToString:@"ADD TO SCENE"]){
        [self.delegate showOrHideLeftView:NO withView:nil];
    }
}

- (IBAction)cancelBtnAction:(id)sender {
    [self.delegate showOrHideLeftView:NO withView:nil];
}
@end
