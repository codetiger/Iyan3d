//
//  Joint.cpp
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#include "Joint.h"
Joint::Joint(){
    PaintedVertices = make_shared< vector< shared_ptr<PaintedVertex> > >();
    childJoints = new vector<Joint*>();
    envelopeRadius = 0.5;
    sphereRadius = 1.0;
    Index = 0;
}
Joint::~Joint(){
    for(int i = 0; i < PaintedVertices->size();i++){
        if((*PaintedVertices)[i])
            (*PaintedVertices)[i].reset();
        PaintedVertices->erase(PaintedVertices->begin() + i);
    }
    for(int i = 0; i < childJoints->size();i++){
        if((*childJoints)[i])
            (*childJoints)[i] = NULL;
        childJoints->erase(childJoints->begin() + i);
    }
    PaintedVertices->clear();
    childJoints->clear();
    
    if(PaintedVertices)
        PaintedVertices.reset();
    if(childJoints)
        delete childJoints;
}
