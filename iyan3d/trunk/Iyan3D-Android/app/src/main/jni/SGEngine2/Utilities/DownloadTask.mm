//
//  DownloadTask.m
//  Iyan3D
//
//  Created by karthik on 24/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#import "DownloadTask.h"

@implementation DownloadTask

- (id)initWithDelegateObject:(id)delegateObj selectorMethod:(SEL)method returnObject:(id)retObj outputFilePath:(NSString*)filePath andURL:(NSString*)url
{
    if (![super init])
        return nil;

    _taskType = DOWNLOAD_AND_WRITE;
    _delegate = delegateObj;
    selectorMethod = method;
    _outputPath = filePath;
    _inputURL = url;
    _returnObj = retObj;
    return self;
}

- (void)main
{
    switch (_taskType) {
    case DOWNLOAD_AND_WRITE: {
        [self downloadFile:_inputURL FileName:_outputPath];
        
        if (_delegate && selectorMethod != nil && ![self isCancelled])
            [_delegate performSelectorOnMainThread:selectorMethod withObject:self waitUntilDone:NO];
        break;
    }
    case DOWNLOAD_AND_READ: {
        NSURL* urlForJson = [NSURL URLWithString:_inputURL];
        NSURLRequest* request = [NSURLRequest requestWithURL:urlForJson];
        if ([NSURLConnection connectionWithRequest:request delegate:self]) {
            NSData* rawData = [NSData dataWithContentsOfURL:urlForJson];
            if (_delegate && selectorMethod != nil && ![self isCancelled])
                [_delegate performSelectorOnMainThread:selectorMethod withObject:rawData waitUntilDone:NO];
        }
        break;
    }
    case DOWNLOAD_AND_WRITE_IMAGE: {
        NSData* imgData = [NSData dataWithContentsOfURL:[NSURL URLWithString:_inputURL]];
        if (imgData) {
            UIImage* imageforAssetView = [UIImage imageWithData:imgData];
            [UIImagePNGRepresentation(imageforAssetView) writeToFile:_outputPath options:NSAtomicWrite error:nil];
            if (_delegate && selectorMethod != nil && ![self isCancelled])
                [_delegate performSelectorOnMainThread:selectorMethod withObject:_returnObj waitUntilDone:NO];
        }
        break;
    }
    case DOWNLOAD_AND_RETURN_OBJ: {
            [self downloadFile:_inputURL FileName:_outputPath];            
            if (_delegate && selectorMethod != nil && ![self isCancelled])
                [_delegate performSelectorOnMainThread:selectorMethod withObject:_returnObj waitUntilDone:NO];
            break;
        }
    default:
        break;
    }
}

- (void)downloadFile:(NSString*)url FileName:(NSString*)fileName
{
    if ([[NSFileManager defaultManager] fileExistsAtPath:fileName])
        return;
    NSLog(@"File NAme %@",fileName);
    NSData* data = [NSData dataWithContentsOfURL:[NSURL URLWithString:url]];
    [data writeToFile:fileName atomically:YES];
}

@end
