//
//  Math.h
//  SGEngine2
//
//  Created by Vivek shivam on 11/10/1936 SAKA.
//  Copyright (c) 1936 SAKA Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__Maths__
#define __SGEngine2__Maths__

#include <iostream>
#include <math.h>
#include <stdlib.h>

static const double ROUNDING_ERROR_f64 = 0.00000001;

class Maths{
public:
    static  bool iszero( const double value, const double tolerance = ROUNDING_ERROR_f64){
        return fabs(value) <= tolerance;
    }
    static double clamp(double value, double low,double high)
    {
        return fmin(fmax(value,low), high);
    }
    static int getRandInRange(int min,int max){
        return min + ((float)rand()/(float)RAND_MAX) * max;
    }
};

#endif /* defined(__SGEngine2__Maths__) */
