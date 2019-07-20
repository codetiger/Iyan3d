//
//  APIData.h
//  SGEngine2
//
//  Created by Vivek on 27/12/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__APIData__
#define __SGEngine2__APIData__

#include <stdio.h>
#include <iostream>
#include "../common/common.h"

class APIData {
public:
    APIData();
    virtual ~APIData();
    virtual void removeVertexBuffers() = 0;
};

#endif /* defined(__SGEngine2__APIData__) */
