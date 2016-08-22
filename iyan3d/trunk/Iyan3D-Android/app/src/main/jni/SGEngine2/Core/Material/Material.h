//
//  Material.h
//  SGEngine2
//
//  Created by Vivek on 02/01/15.
//  Copyright (c) 2015 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__Material__
#define __SGEngine2__Material__

#include <iostream>
#include "../common/common.h"

class Material {
    
public:
    string name;
    bool isTransparent;

    Material();
    virtual ~Material();
    
    virtual short setPropertyValue(string name, float *values, DATA_TYPE type, u16 count, u16 paramIndex = 0, int nodeIndex = NOT_EXISTS) = 0;
    virtual short setPropertyValue(string name, int *values, DATA_TYPE type, u16 count, u16 paramIndex = 0, int nodeIndex = NOT_EXISTS) = 0;
};

#endif /* defined(__SGEngine2__Material__) */
