#ifndef Iyan3D_BoneLimits_h
#define Iyan3D_BoneLimits_h

#include "SGEngineCommon.h"
class BoneLimits{
public:
    int id;
    std::string name;
    Vector3 maxRotation;
    Vector3 minRotation;
    
    BoneLimits(){
        id = -1;
        name = "";
    }
};

#endif