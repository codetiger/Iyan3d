//
//  InstanceNode.cpp
//  SGEngine2
//
//  Created by Vivek on 21/01/15.
//  Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
//

#include "EmptyNode.h"

EmptyNode::EmptyNode() {
    type = NODE_TYPE_EMPTY;
}

EmptyNode::~EmptyNode() {
}

void EmptyNode::update() {
}

void EmptyNode::updateBoundingBox() {
    if (Children->size()) {
        for (unsigned short i = 0; i < Children->size(); i++) {
            if ((*Children)[i])
                (*Children)[i]->updateBoundingBox();
        }
    }

    BoundingBox bb;

    if (Children->size()) {
        for (unsigned short i = 0; i < Children->size(); i++) {
            if ((*Children)[i]) {
                for (int j = 0; j < 8; j++) {
                    Vector3 edge = (*Children)[i]->getBoundingBox().getEdgeByIndex(j);
                    bb.addPointsToCalculateBoundingBox(edge);
                }
            }
        }
    }
    bb.calculateEdges();
    bBox = bb;
}
