//
//  SceneSelectionControllerNew.m
//  Iyan3D
//
//  Created by harishankarn on 16/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "SceneSelectionControllerNew.h"
#import "SceneSelectionEnteredView.h"
#import "AppDelegate.h"
#import "Utility.h"

@implementation SceneSelectionControllerNew

- (id) initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil {
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        cache = [CacheSystem cacheSystem];
        scenesArray = [cache GetSceneList];
        dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateFormat:@"yyyy/MM/dd HH:mm:ss"];
        currentSelectedScene = -1;
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.scenesCollectionView registerNib:[UINib nibWithNibName:@"SceneSelectionFrameCell" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    [self.sceneView setHidden:YES];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
}

- (IBAction)addSceneButtonAction:(id)sender {
    [self.view endEditing:YES];
    [self addNewScene];
}

- (IBAction)scenePreviewClosebtn:(id)sender {
    [self popZoomOut];
}

- (IBAction)cloneSceneButtonAction:(id)sender {
    if([scenesArray count] <= 0)
        return;
    NSString* sceneCount = [NSString stringWithFormat:@"%i", [scenesArray count]];
    SceneItem* scene = [[SceneItem alloc] init];
    scene.name = [NSString stringWithFormat:@"%s%@", "MyScene", sceneCount];
    scene.createdDate = [dateFormatter stringFromDate:[NSDate date]];
    
    if(![cache AddScene:scene]) {
        [self.view endEditing:YES];
        UIAlertView *errorAlert = [[UIAlertView alloc]initWithTitle:@"Duplicate Scene Name" message:@"Another Scene with the same name already exists. Please provide a different name." delegate:self cancelButtonTitle:@"Ok" otherButtonTitles:nil];
        [errorAlert show];
    } else {
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* documentsDirectory = [paths objectAtIndex:0];
        NSString *originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, scene.sceneFile];
        NSString *newFilePath = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, scene.sceneFile];
        [[NSFileManager defaultManager] copyItemAtPath:originalFilePath toPath:newFilePath error:nil];
        originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scene.sceneFile];
        newFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scene.sceneFile];
        [[NSFileManager defaultManager] copyItemAtPath:originalFilePath toPath:newFilePath error:nil];
        
        scenesArray = [cache GetSceneList];
        NSLog(@"Count%i" ,[scenesArray count]);
        [self.scenesCollectionView reloadData];
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:[scenesArray count]-1 inSection:0];
        [self.scenesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredVertically animated:YES];
        [self.scenesCollectionView reloadData];
        
    }

}

- (IBAction)deleteSceneButtonAction:(id)sender {
 
        SceneItem* scene = scenesArray[currentSelectedScene];
        
        [[AppHelper getAppHelper] removeFromUserDefaultsWithKey:scene.name];
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* documentsDirectory = [paths objectAtIndex:0];
        NSString *filePath = [NSString stringWithFormat:@"%@/Projects/%@.sgb", documentsDirectory, scene.sceneFile];
        self.fileBeginsWith=scene.sceneFile;
        NSArray* cachepath = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
        NSString* cacheDirectory = [cachepath objectAtIndex:0];
        NSArray *dirFiles = [[NSFileManager defaultManager]     contentsOfDirectoryAtPath:cacheDirectory error:nil];
        NSArray *jpgFiles = [dirFiles filteredArrayUsingPredicate:
                             [NSPredicate predicateWithFormat:@"self BEGINSWITH[cd] %@",self.fileBeginsWith]];
        for (int i=0; i<[jpgFiles count]; i++) {
            NSString *filePath1 = [NSString stringWithFormat:@"%@/%@-%d.png", cacheDirectory,self.fileBeginsWith,i];
            [[NSFileManager defaultManager] removeItemAtPath:filePath1 error:nil];
        }
        [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
        filePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scene.sceneFile];
        [[NSFileManager defaultManager] removeItemAtPath:filePath error:nil];
        
        [cache DeleteScene:scene];
        [scenesArray removeObjectAtIndex:currentSelectedScene];
        currentSelectedScene = -1;
        [self popZoomOut];
    
    [self.scenesCollectionView reloadData];

}

- (IBAction)feedBackButtonAction:(id)sender {
    
    
}

- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section{
    return [scenesArray count]+1;
}

- (SceneSelectionFrameCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath{
    SceneSelectionFrameCell *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
    if(indexPath.row < [scenesArray count]){
        SceneItem* scenes = scenesArray[indexPath.row];
        cell.name.text = [NSString stringWithFormat:@"%@",scenes.name];
        NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* documentsDirectory = [paths objectAtIndex:0];
        NSString* originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scenes.sceneFile];
        BOOL fileExit = [[NSFileManager defaultManager] fileExistsAtPath:originalFilePath];
    if(fileExit)
        cell.image.image = [UIImage imageWithContentsOfFile:originalFilePath];
    else
        cell.image.image= [UIImage imageNamed:@"bgImageforall.png"];
    }
    else{
        cell.name.text = @"";
        cell.image.image = [UIImage imageNamed:@"New-scene.png"];
    }
    
    return cell;
}

- (void)collectionView:(UICollectionView *)collectionView didSelectItemAtIndexPath:(NSIndexPath *)indexPath{
    SceneSelectionFrameCell *cell = (SceneSelectionFrameCell*)[collectionView cellForItemAtIndexPath:indexPath];
    cell_center = [self.scenesCollectionView convertPoint:cell.center fromView:cell];
    cell_center = [self.view convertPoint:cell_center fromView:self.scenesCollectionView];
    if(indexPath.row == [scenesArray count])
        [self addNewScene];
    else
        [self showSceneEnteredView:indexPath];
}

- (void) addNewScene
{
    NSString* sceneCount = [NSString stringWithFormat:@"%i", [scenesArray count]];
    SceneItem* scene = [[SceneItem alloc] init];
    if([scenesArray count] != 0)
        scene = scenesArray[[scenesArray count]-1];
    scene.name = [NSString stringWithFormat:@"MyScene%d", scene.sceneId];
    scene.createdDate = [dateFormatter stringFromDate:[NSDate date]];
    
    
    if([cache AddScene:scene]) {
        scenesArray = [cache GetSceneList];
        NSLog(@"Count%i" ,[scenesArray count]);
        [self.scenesCollectionView reloadData];
        NSIndexPath* toPath = [NSIndexPath indexPathForItem:[scenesArray count]-1 inSection:0];
        [self.scenesCollectionView scrollToItemAtIndexPath:toPath atScrollPosition:UICollectionViewScrollPositionCenteredVertically animated:YES];
        [self.scenesCollectionView reloadData];
    }
}

- (void) showSceneEnteredView:(NSIndexPath*)sceneIndex{
    
    [self.sceneView setHidden:NO];
    [self popUpZoomIn];
    currentSelectedScene = sceneIndex.row;
    SceneItem* scenes = scenesArray[sceneIndex.row];
    
    _sceneTitle.text = scenes.name;
    _sceneDate.text = scenes.createdDate;
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* documentsDirectory = [paths objectAtIndex:0];
    NSString* originalFilePath = [NSString stringWithFormat:@"%@/Projects/%@.png", documentsDirectory, scenes.sceneFile];
    BOOL fileExit = [[NSFileManager defaultManager] fileExistsAtPath:originalFilePath];
    if(fileExit)
        _scenePreview.image = [UIImage imageWithContentsOfFile:originalFilePath];
    else
        _scenePreview.image = [UIImage imageNamed:@"bgImageforall.png"];
}

- (void)popUpZoomIn{
    [_sceneView setHidden:NO];
    _sceneView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.001, 0.001);
    _sceneView.center = cell_center;
    [UIView animateWithDuration:0.5
                     animations:^{
                         _sceneView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 1.0, 1.0);
                         _sceneView.center = self.view.center;
                     } completion:^(BOOL finished) {
                         
                     }];
}

- (void)popZoomOut{
    [UIView animateWithDuration:0.5
                     animations:^{
                         _sceneView.transform = CGAffineTransformScale(CGAffineTransformIdentity, 0.001, 0.001);
                     } completion:^(BOOL finished) {
                         _sceneView.hidden = TRUE;
                     }];
}

- (void) loadScene{
    [self removeFromParentViewController];
}

@end

