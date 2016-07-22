//
//  ImportImageNew.m
//  Iyan3D
//
//  Created by Sankar on 20/12/15.
//  Copyright © 2015 Smackall Games. All rights reserved.
//

#import "ImportImageNew.h"

#define PICK_IMAGE 1
#define PICK_VIDEO 2

@interface ImportImageNew ()

@end

@implementation ImportImageNew

- (id)initWithNibName:(NSString*)nibNameOrNil bundle:(NSBundle*)nibBundleOrNil Type:(int)type
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if(self){
        viewType = type;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    self.screenName = @"ImportImage iOS";
    
    [self.addBtn setEnabled:NO];
    self.cancelBtn.layer.cornerRadius = 8.0f;
    self.addBtn.layer.cornerRadius = 8.0f;
    
    if(viewType == PICK_VIDEO) {
        [_addBtn setHidden:YES];
    } else {
        [_addBtn setHidden:NO];
        [_cancelBtn setHidden:NO];
    }
   
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
   
}

#pragma mark Button Actions

- (IBAction)addBtnAction:(id)sender
{
    [self.delegate pickedImageWithInfo:imageInfo type:NO];
    [self.delegate showOrHideLeftView:NO withView:nil];
    [self.view removeFromSuperview];
}

- (IBAction)cancelBtnAction:(id)sender
{
    [self.delegate showOrHideLeftView:NO withView:nil];
    [self.delegate removeTempNodeFromScene];
    [self.view removeFromSuperview];
}

#pragma mark ImportImage imagepicker Delegate

- (void) imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info
{
    if(viewType == PICK_IMAGE) {
        imageInfo = nil;
        imageInfo = [NSDictionary dictionaryWithDictionary:info];
        
        if(imageInfo) {
            [self.delegate pickedImageWithInfo:imageInfo type:YES];
            [self.addBtn setEnabled:YES];
        } else
            [self.addBtn setEnabled:NO];
        
    } else if(viewType == PICK_VIDEO) {
        NSString *mediaType = [info objectForKey: UIImagePickerControllerMediaType];
        
        if (CFStringCompare ((__bridge CFStringRef) mediaType, kUTTypeMovie, 0) == kCFCompareEqualTo) {
            NSURL *videoUrl=(NSURL*)[info objectForKey:UIImagePickerControllerMediaURL];
            NSString *moviePath = [videoUrl path];
            if (UIVideoAtPathIsCompatibleWithSavedPhotosAlbum (moviePath)) {
                UISaveVideoAtPathToSavedPhotosAlbum (moviePath, nil, nil, nil);
            }
            [self moveTempToDoc:moviePath];
        }
    }
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker
{
    [self.delegate showOrHideLeftView:NO withView:nil];
    [self.delegate removeTempNodeFromScene];
    [self.view removeFromSuperview];
}

- (void)moveTempToDoc:(NSString*)path
{
    NSString* theFileName = [[path lastPathComponent] stringByDeletingPathExtension];
    NSArray* srcDirPath = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString* docDirPath = [srcDirPath objectAtIndex:0];
    NSString* videoFrom = [NSString stringWithFormat:@"%@",path];
    NSString* videoTo = [NSString stringWithFormat:@"%@/Resources/Videos/%@.MOV",docDirPath,theFileName];
    NSFileManager* fm = [[NSFileManager alloc]init];
    [fm moveItemAtPath:videoFrom toPath:videoTo error:nil];
    [self.delegate pickedVideoWithInfo:videoTo FileName:theFileName IsTemp:NO];
    [self.delegate showOrHideLeftView:NO withView:nil];
    [self.view removeFromSuperview];
}

- (void)dealloc{
    
}
@end
