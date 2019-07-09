//
//  BoundingBox.cpp
//  FatMan
//
//  Created by Vivek on 27/01/15.
//  Copyright (c) 2015 Vivek. All rights reserved.
//

#include "BoundingBox.h"
BoundingBox::BoundingBox()
{
    MaxEdge = Vector3(-999.0);
    MinEdge = Vector3(999.0);
}

BoundingBox::~BoundingBox()
{
}

void BoundingBox::clearPoints()
{
    MaxEdge = Vector3(-999.0);
    MinEdge = Vector3(999.0);
}

void BoundingBox::addPointsToCalculateBoundingBox(Vector3 point)
{
    if (point.x > MaxEdge.x)
        MaxEdge.x = point.x;
    if (point.y > MaxEdge.y)
        MaxEdge.y = point.y;
    if (point.z > MaxEdge.z)
        MaxEdge.z = point.z;

    if (point.x < MinEdge.x)
        MinEdge.x = point.x;
    if (point.y < MinEdge.y)
        MinEdge.y = point.y;
    if (point.z < MinEdge.z)
        MinEdge.z = point.z;
}

Vector3 BoundingBox::getMaxEdge()
{
    return MaxEdge;
}

Vector3 BoundingBox::getMinEdge()
{
    return MinEdge;
}

Vector3 BoundingBox::getCenter()
{
    return (MaxEdge + MinEdge) / 2.0;
}

Vector3 BoundingBox::getEdgeByIndex(u16 index)
{
    return edges[index];
}

BoundingBox BoundingBox::transformBoundingBox(Mat4 absTransformation)
{
    BoundingBox bb;
    bb = *this;
    for(int i = 0; i < 8; i++) {
        Vector4 newEdge = absTransformation * Vector4(bb.edges[i].x, bb.edges[i].y, bb.edges[i].z, 1.0);
        bb.edges[i]= Vector3(newEdge.x, newEdge.y, newEdge.z);
    }
    return bb;
}

float BoundingBox::getZExtend()
{
    return MaxEdge.z - MinEdge.z;
}

float BoundingBox::getYExtend()
{
    return MaxEdge.y - MinEdge.y;
}

float BoundingBox::getXExtend()
{
    return MaxEdge.x - MinEdge.x;
}

void BoundingBox::calculateEdges()
{
    //
    //      /5--------/6
    //     / |       / |
    //    /  |      /  |
    //    0---------1  |
    //    |  4- - - |- 7
    //    | /       | /
    //    |/        |/
    //    3---------2

    Vector3 center = getCenter();
    edges[0] = MaxEdge;
    edges[1] = Vector3(MaxEdge.x - getXExtend(), MaxEdge.y, MaxEdge.z);
    edges[2] = Vector3(MaxEdge.x - getXExtend(), MaxEdge.y - getYExtend(), MaxEdge.z);
    edges[3] = Vector3(MaxEdge.x, MaxEdge.y - getYExtend(), MaxEdge.z);
    edges[4] = Vector3(MinEdge.x + getXExtend(), MinEdge.y, MinEdge.z);
    edges[5] = Vector3(MinEdge.x + getXExtend(), MinEdge.y + getYExtend(), MinEdge.z);
    edges[6] = Vector3(MinEdge.x, MinEdge.y + getYExtend(), MinEdge.z);
    edges[7] = MinEdge;
}

bool BoundingBox::isValid()
{
    return (MaxEdge.x >= MinEdge.x && MaxEdge.y >= MinEdge.y && MaxEdge.z >= MinEdge.z);
}
