//
//  ChangeTextureSidePanel.m
//  Iyan3D
//
//  Created by Sankar on 22/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "ChangeTextureSidePanel.h"
#import "ObjCellView.h"
@interface ChangeTextureSidePanel ()

@end

@implementation ChangeTextureSidePanel{
    NSArray *filesList;
    NSString *imagefileName;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.texturefilesList registerNib:[UINib nibWithNibName:@"ObjCellView" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    NSArray *extensions = [NSArray arrayWithObjects:@"png", @"jpeg", @"jpg", @"PNG", @"JPEG", nil];
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
    NSLog(@"Array Count %@",imagefileName);
}

@end
