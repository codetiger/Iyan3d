#ifndef __iyan3dengine__VisibilityKey__
#define __iyan3dengine__VisibilityKey__

#include <iostream>

class SGVisibilityKey
{
public:
    
    int id;
    bool visibility;
    
    static bool Compare (const SGVisibilityKey& visibilityKey1, const SGVisibilityKey& visibilityKey2){
        return visibilityKey1.id < visibilityKey2.id;
    }
    
};
#endif