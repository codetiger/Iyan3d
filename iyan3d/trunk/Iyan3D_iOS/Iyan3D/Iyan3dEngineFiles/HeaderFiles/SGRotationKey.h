#ifndef __iyan3dengine__RotationKey__
#define __iyan3dengine__RotationKey__

#include <iostream>

class SGRotationKey
{
public:
    
    int id;
    Quaternion rotation;
    
    static bool Compare (const SGRotationKey& rotationKey1, const SGRotationKey& rotationKey2){
        return rotationKey1.id < rotationKey2.id;
    }
    Quaternion getValue(){
        return rotation;
    }
    
};

#endif