//
//  MTLNodeData.h
//  SGEngine2
//
//  Created by Vivek on 27/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__MTLNodeData__
#define __SGEngine2__MTLNodeData__
#ifdef IOS
#if !(TARGET_IPHONE_SIMULATOR)
    #import <QuartzCore/CAMetalLayer.h>
    #import <Metal/Metal.h>
#endif
#import <UIKit/UIKit.h>
#endif
#include "APIData.h"


class MTLNodeData:public APIData{

public:
    NSMutableArray *indexBuffers;
    NSMutableArray *VertexBuffers;
    MTLNodeData();
    ~MTLNodeData();
    void removeVertexBuffers();
};
#endif /* defined(__SGEngine2__MTLNodeData__) */