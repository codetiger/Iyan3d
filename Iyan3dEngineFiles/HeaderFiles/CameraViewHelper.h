#ifndef __Iyan3D__cameraViewHelper__
#define __Iyan3D__cameraViewHelper__

#include "Constants.h"
#include "json_json.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include "../../SGEngine2/Utilities/Logger.h"

using namespace std;

class CameraViewHelper{
    
public:
    static void readData();
    static Vector3 getCameraPosition(CAMERA_VIEW_MODE mode);
    static Vector2 getCameraAngle(CAMERA_VIEW_MODE mode);
};

#endif