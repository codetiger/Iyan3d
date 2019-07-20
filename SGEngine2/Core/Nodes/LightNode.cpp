//
//  LightNode.cpp
//  SGEngine2
//
//  Created by Harishankar on 17/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "LightNode.h"

LightNode::LightNode() {
    decayStartDistance = decayEndDistance = 20.0;
    type                                  = LIGHT_TYPE_POINT;
    lightColor                            = Vector3(1.0, 1.0, 1.0);
}

LightNode::~LightNode() {
}

void LightNode::update() {
}