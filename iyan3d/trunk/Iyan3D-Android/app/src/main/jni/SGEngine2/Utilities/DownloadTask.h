//
//  DownloadTask.h
//  Iyan3D
//
//  Created by karthik on 24/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#ifndef DownloadTask_h
#define DownloadTask_h


#import <Foundation/Foundation.h>
#import "Constants.h"

@interface DownloadTask : NSOperation {
    SEL selectorMethod;
}

@property(nonatomic , strong) id delegate;
@property(nonatomic , strong) NSString* inputURL;
@property(nonatomic , strong) NSString* outputPath;
@property(nonatomic , strong) id returnObj;
@property(nonatomic , assign) OPERATION_TYPE taskType;


- (id)initWithDelegateObject:(id) delegateObj selectorMethod:(SEL)method returnObject:(id) retObj outputFilePath:(NSString*) filePath andURL:(NSString *)url;

@end

#endif
