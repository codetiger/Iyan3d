#ifndef Iyan3D_ActionKey_h
#define Iyan3D_ActionKey_h

class ActionKey
{
public:
    
    bool isPositionKey;
    bool isRotationKey;
    bool isScaleKey;
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;
    
    ActionKey()
    {
        isPositionKey = isRotationKey = isScaleKey = false;
    }
};

#endif
