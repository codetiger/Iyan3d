//
//  Joint.h
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__Joint__
#define __SGEngine2__Joint__

#include <iostream>
#include "../common/common.h"
#include <math.h>
#include <string>
#include <vector>
#include  <memory>

using std::vector;

struct PaintedVertex {
    int vertexId;
    float weight;
};

class Joint {
    
public:
    Joint();
    ~Joint();
    
    double envelopeRadius;
    double sphereRadius;
    unsigned short Index;
    Mat4 LocalAnimatedMatrix; //  filed from jointnode local transformation and ,esh file loader
    Mat4 GlobalAnimatedMatrix; // calculated from the local matrix
    Mat4 GlobalInversedMatrix; // from GlobalAnimatedMatrix
    Joint *Parent;
    shared_ptr< vector< shared_ptr<PaintedVertex> > > PaintedVertices;
    vector<Joint*> *childJoints;
};

#endif /* defined(__SGEngine2__Joint__) */
