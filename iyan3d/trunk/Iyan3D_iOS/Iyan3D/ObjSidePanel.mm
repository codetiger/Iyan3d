//
//  ObjSidePanel.m
//  Iyan3D
//
//  Created by Sankar on 08/01/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#import "ObjSidePanel.h"
#import "ObjCellView.h"
#import <AssetsLibrary/AssetsLibrary.h>
#import <Utility.h>


#define OBJ 0
#define Texture 1
#define IMPORT_OBJFILE 5
#define CHANGE_TEXTURE 7

@interface ObjSidePanel ()

@end

@implementation ObjSidePanel{
    NSArray *filesList;
}

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil Type:(int)type
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        haveTexture = NO;
        color = Vector3(1.0,1.0,1.0);
        basicShapes = [NSArray arrayWithObjects:@"Cone",@"Cube",@"Cylinder",@"Plane",@"Sphere",@"Torus",nil];
        indexPathOfOBJ = -1;
        viewType = type;
        textureFileName = @"-1";
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
     [self.importFilesCollectionView registerNib:[UINib nibWithNibName:@"ObjCellView" bundle:nil] forCellWithReuseIdentifier:@"CELL"];
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSArray *dirFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirPath error:nil];
    filesList = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"self ENDSWITH '.obj'"]];
    self.importBtn.layer.cornerRadius=8.0;
    self.addBtn.layer.cornerRadius=8.0;
    self.cancelBtn.layer.cornerRadius=8.0;
    [_colorWheelBtn setHidden:YES];
    indexPathOfOBJ =  (viewType == IMPORT_OBJFILE) ? -1 : 0;
    if(viewType != IMPORT_OBJFILE){
        [_importBtn setHidden:NO];
        [self addBtnAction:nil];
    }
    else
        [_importBtn setHidden:YES];
    _addBtn.tag = (viewType == IMPORT_OBJFILE) ? OBJ : Texture;
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];

}


- (NSInteger)collectionView:(UICollectionView *)collectionView numberOfItemsInSection:(NSInteger)section
{
    if(_addBtn.tag == OBJ)
        return [filesList count]+6;
    else
        return [filesList count]+1;
}
- (UICollectionViewCell *)collectionView:(UICollectionView *)collectionView cellForItemAtIndexPath:(NSIndexPath *)indexPath
{
    
    if(_addBtn.tag == OBJ)
    {
        if(indexPath.row > basicShapes.count-1){
            NSString *extension = [[filesList objectAtIndex:indexPath.row-[basicShapes count]]pathExtension];
            if([extension isEqualToString:@"obj"]){
                ObjCellView *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
                cell.assetNameLabel.text = filesList[indexPath.row-[basicShapes count]];
                cell.layer.borderColor = [UIColor grayColor].CGColor;
                cell.assetImageView.image =[UIImage imageNamed:@"objfile.png"];
                cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
                return cell;
            }
        }
        else{
            ObjCellView *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
            cell.assetNameLabel.text = [basicShapes objectAtIndex:indexPath.row];
            cell.layer.borderColor = [UIColor grayColor].CGColor;
            NSString* imageName = [NSString stringWithFormat:@"%@%s",[basicShapes objectAtIndex:indexPath.row],".png"];
            cell.assetImageView.image =[UIImage imageNamed:imageName];
            NSLog(@"\nImage Name : %@",imageName);
            cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
            return cell;
        }
    }
    else
    {
        ObjCellView *cell = [collectionView dequeueReusableCellWithReuseIdentifier:@"CELL" forIndexPath:indexPath];
        cell.layer.borderColor = [UIColor grayColor].CGColor;
        cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
        
        if(indexPath.row == 0){
            cell.assetNameLabel.text = @"Pick Color";
            cell.assetImageView.backgroundColor = [UIColor colorWithRed:color.x green:color.y blue:color.z alpha:1.0];
            cell.assetImageView.image = NULL;
        }
        else{
            NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            NSString* docDirPath = [srcDirPath objectAtIndex:0];
            NSString* srcFilePath = [NSString stringWithFormat:@"%@/%@",docDirPath,filesList[indexPath.row-1]];
            cell.assetNameLabel.text = filesList[indexPath.row-1];
            cell.layer.backgroundColor = [UIColor colorWithRed:15/255.0 green:15/255.0 blue:15/255.0 alpha:1].CGColor;
            cell.assetImageView.image=[UIImage imageWithContentsOfFile:srcFilePath];
        }
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

    if(_addBtn.tag == OBJ)
        indexPathOfOBJ = (int)indexPath.row;
    else
    {
        if(indexPath.row == 0){
            haveTexture = NO;
            _vertexColorProp = [[TextColorPicker alloc] initWithNibName:@"TextColorPicker" bundle:nil TextColor:nil];
            _vertexColorProp.delegate = self;
            self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_vertexColorProp];
            self.popoverController.popoverContentSize = CGSizeMake(200, 200);
            self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
            self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
            [_popUpVc.view setClipsToBounds:YES];
            CGRect rect = _colorWheelBtn.frame;
            rect = [self.view convertRect:rect fromView:_colorWheelBtn.superview];
            [self.popoverController presentPopoverFromRect:rect
                                                    inView:self.view
                                  permittedArrowDirections:UIPopoverArrowDirectionUp
                                                  animated:NO];
            textureFileName = @"-1";
        }
        else{
            haveTexture = YES;
            textureFileName = [[filesList objectAtIndex:indexPath.row-1]stringByDeletingPathExtension];
        }
    }
    
    if(viewType == IMPORT_OBJFILE)
        [_objSlideDelegate importObjAndTexture:indexPathOfOBJ TextureName:textureFileName VertexColor:color haveTexture:haveTexture IsTempNode:YES];
    else
        [_objSlideDelegate changeTexture:textureFileName VertexColor:color IsTemp:YES];
}

- (IBAction)importBtnAction:(id)sender {

    self.ipc= [[UIImagePickerController alloc] init];
    self.ipc.delegate = self;
    self.ipc.sourceType = UIImagePickerControllerSourceTypeSavedPhotosAlbum;
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:self.ipc];
    if ([Utility IsPadDevice]) {
      self.popoverController.popoverContentSize = CGSizeMake(400, 500);
    }
    else{
        self.popoverController.popoverContentSize = CGSizeMake(250.0, 180.0);
    }
    
    CGRect rect = _importBtn.frame;
    rect = [self.view convertRect:rect fromView:_importBtn.superview];
    [self.popoverController presentPopoverFromRect:rect inView:self.view permittedArrowDirections:UIPopoverArrowDirectionDown animated:YES];
}


- (IBAction)addBtnAction:(id)sender {
    if(indexPathOfOBJ == -1)
        return;
    if(self.addBtn.tag == OBJ){
        filesList=nil;
        NSArray *extensions = [NSArray arrayWithObjects:@"png", @"jpeg", @"jpg", @"PNG", @"JPEG", nil];
        NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString* docDirPath = [srcDirPath objectAtIndex:0];
        NSArray *dirFiles = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docDirPath error:nil];
        filesList = [dirFiles filteredArrayUsingPredicate:[NSPredicate predicateWithFormat:@"pathExtension IN %@", extensions]];
        [self.addBtn setTitle:@"ADD TO SCENE" forState:UIControlStateNormal];
        [self.viewTitle setText:@"Import Texture"];
        self.addBtn.tag = Texture;
        [self.importFilesCollectionView reloadData];
        [_colorWheelBtn setHidden:NO];
    }
   else if(self.addBtn.tag == Texture){
       if(viewType == IMPORT_OBJFILE)
           [_objSlideDelegate importObjAndTexture:indexPathOfOBJ TextureName:textureFileName VertexColor:color haveTexture:haveTexture IsTempNode:NO];
       else
           [_objSlideDelegate changeTexture:textureFileName VertexColor:color IsTemp:NO];
        [self.objSlideDelegate showOrHideLeftView:NO withView:nil];
        [self removeFromParentViewController];
    }
}

- (IBAction)colorPickerAction:(id)sender {
    _vertexColorProp = [[TextColorPicker alloc] initWithNibName:@"TextColorPicker" bundle:nil TextColor:nil];
    _vertexColorProp.delegate = self;
    self.popoverController = [[WEPopoverController alloc] initWithContentViewController:_vertexColorProp];
    self.popoverController.popoverContentSize = CGSizeMake(200, 200);
    self.popoverController.popoverLayoutMargins= UIEdgeInsetsMake(0.0, 0.0, 0.0, 0.0);
    self.popoverController.animationType=WEPopoverAnimationTypeCrossFade;
    [_popUpVc.view setClipsToBounds:YES];
    CGRect rect = _colorWheelBtn.frame;
    rect = [self.view convertRect:rect fromView:_colorWheelBtn.superview];
    [self.popoverController presentPopoverFromRect:rect
                                            inView:self.view
                          permittedArrowDirections:UIPopoverArrowDirectionUp
                                          animated:NO];
}

- (void) changeVertexColor:(Vector3)vetexColor dragFinish:(BOOL)isDragFinish
{
    haveTexture = NO;
    color = vetexColor;
    [self.importFilesCollectionView reloadData];
    if(isDragFinish){
        if(viewType == IMPORT_OBJFILE)
        [_objSlideDelegate importObjAndTexture:indexPathOfOBJ TextureName:textureFileName VertexColor:color haveTexture:haveTexture IsTempNode:YES];
        else
            [_objSlideDelegate changeTexture:@"-1" VertexColor:color IsTemp:YES];

    }
}

-(void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info{
    [_popoverController dismissPopoverAnimated:YES];
    NSDate *now = [NSDate date];
    
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    dateFormatter.dateFormat = @"hh:mm:ss";
    [dateFormatter setTimeZone:[NSTimeZone systemTimeZone]];
    NSString *date = [@"Texture_"stringByAppendingString:[dateFormatter stringFromDate:now]];
    NSString *fileName = [date stringByAppendingString:@".png"];
    UIImage* image=(UIImage*)[info objectForKey:@"UIImagePickerControllerOriginalImage"];
    NSData *pngData = UIImagePNGRepresentation(image);
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsPath = [paths objectAtIndex:0]; //Get the docs directory
    NSString *filePath = [documentsPath stringByAppendingPathComponent:fileName]; //Add the file name
    [pngData writeToFile:filePath atomically:YES]; //Write the file
    _addBtn.tag = OBJ;
    [self addBtnAction:nil];
}



- (IBAction)cancelBtnAction:(id)sender {
    if(viewType == IMPORT_OBJFILE)
        [self.objSlideDelegate removeTempNodeFromScene];
    else
        [self.objSlideDelegate removeTempTextureAndVertex];
    [self.objSlideDelegate showOrHideLeftView:NO withView:nil];
}
@end
