//
//  BoundingBox.h
//  FatMan
//
//  Created by Vivek on 27/01/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//

#ifndef __FatMan__BoundingBox__
#define __FatMan__BoundingBox__

#include <iostream>
#include "common.h"

class BoundingBox {

private:
    Vector3 MaxEdge;
    Vector3 MinEdge;
    Vector3 edges[8];

public:
    BoundingBox();
    ~BoundingBox();
    void addPointsToCalculateBoundingBox(Vector3 pointPosition);
    Vector3 getMaxEdge();
    Vector3 getMinEdge();
    Vector3 getCenter();
    Vector3 getEdgeByIndex(unsigned short index);
    BoundingBox transformBoundingBox(Mat4 absTransformation);
    void calculateEdges();
    bool isValid();
    float getZExtend();
    float getXExtend();
    float getYExtend();
};

#endif /* defined(__FatMan__BoundingBox__) */
