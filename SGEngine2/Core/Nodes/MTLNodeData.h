//
//  MTLNodeData.h
//  SGEngine2
//
//  Created by Vivek on 27/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__MTLNodeData__
#define __SGEngine2__MTLNodeData__
#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>
#import <UIKit/UIKit.h>
#include "APIData.h"

class MTLNodeData : public APIData {
public:
    bool            VAOCreated;
    NSMutableArray* indexBuffers;
    NSMutableArray* VertexBuffers;
    MTLNodeData();
    ~MTLNodeData();
    void removeVertexBuffers();
};
#endif /* defined(__SGEngine2__MTLNodeData__) */
