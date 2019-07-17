//
//  LightNode.h
//  SGEngine2
//
//  Created by Harishankar on 17/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__LightNode__
#define __SGEngine2__LightNode__

#include "../common/common.h"
#include "../Nodes/MeshNode.h"

typedef enum {
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_DIRECTIONAL,
} light_type;

//TODO Fix inheritance issue
class LightNode : public MeshNode {
public:
    bool       isShadowCaster;
    light_type type;
    Vector3    lightColor;

    float decayStartDistance, decayEndDistance;

    LightNode();
    ~LightNode();
    virtual void update();
};

#endif /* defined(__SGEngine2__LightNode__) */
