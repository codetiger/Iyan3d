#ifndef __iyan3dengine__PositionKey__
#define __iyan3dengine__PositionKey__

#include <iostream>

using namespace std;

class SGPositionKey
{
public:
    
    int id;
    Vector3 position;
    
    static bool Compare (const SGPositionKey& positionKey1, const SGPositionKey& positionKey2){
        return positionKey1.id < positionKey2.id;
    }
    Vector3 getValue(){
        return position;
    }
};



#endif