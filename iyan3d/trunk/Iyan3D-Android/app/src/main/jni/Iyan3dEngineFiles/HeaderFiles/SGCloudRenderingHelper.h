//
//  SGCloudRenderingHelper.h
//  Iyan3D
//
//  Created by karthik on 18/08/15.
//  Copyright (c) 2015 Smackall Games. All rights reserved.
//

#ifndef Iyan3D_SGCloudRenderingHelper_h
#define Iyan3D_SGCloudRenderingHelper_h

#include <iostream>
//#include <Constants.h>
//#include <SGNode.h>
//#include "SGJoint.h"
//#include "KeyHelper.h"
//#include "BoneLimitsHelper.h"
//#include "ShaderManager.h"
//#include "SGRotationKey.h"
#include "SGAnimationScene.h"
#include "../../SGEngine2/Utilities/Logger.h"

struct TriangleData
{
    Vector3 Pos1;
    Vector3 Pos2;
    Vector3 Pos3;
    Vector3 Normal1;
    Vector3 Normal2;
    Vector3 Normal3;
    Vector2 UV1;
    Vector2 UV2;
    Vector2 UV3;
};


using namespace std;

class SGCloudRenderingHelper
{
public:
    SGCloudRenderingHelper();
    static bool writeFrameData(SGAnimationScene *scene , int frameId, string fileName, vector<string> &textureFileNames);
    vector<TriangleData> calculateTriangleDataForNode(SGNode * sgNode);
    vertexData calculateFinalVertexData(shared_ptr<Node> node , void * vertex);
    void calculateJointTransforms(vertexDataHeavy *vertex , vector<Mat4> jointTransforms , Vector3 &position , Vector3 &normal);
    void copyMat(float* pointer,Mat4& mat);
};

#endif
