#ifndef __iyan3dengine__ScaleKey__
#define __iyan3dengine__ScaleKey__

#include <iostream>

class SGScaleKey
{
public:
    
    int id;
    Vector3 scale;
    
    static bool Compare (const SGScaleKey& scaleKey1, const SGScaleKey& scaleKey2){
        return scaleKey1.id < scaleKey2.id;
    }
    Vector3 getValue(){
        return scale;
    }
};



#endif