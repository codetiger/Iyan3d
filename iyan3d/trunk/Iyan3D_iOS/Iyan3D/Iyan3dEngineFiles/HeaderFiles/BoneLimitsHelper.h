#ifndef __Iyan3D__BoneLimitsHelper__
#define __Iyan3D__BoneLimitsHelper__

#include "json_json.h"
#include "Constants.h"
#include "../../SGEngine2/Utilities/Logger.h"
#include <fstream>
#include <string>
#include "KeyHelper.h"
#include "BoneLimits.h"



class BoneLimitsHelper{


public:
    static void init();
    static void printBoneLimits();
    static BoneLimits getBoneLimits(int boneId);
    static BoneLimits getBoneLimits(std::string name);
    static Vector3 getTPoseRotationDeg(int boneId);
    static Vector3 limitRotation(shared_ptr<Node> node, Vector3 expectedRotation);
    static int getMirrorJointId(int jointId);
};

#endif