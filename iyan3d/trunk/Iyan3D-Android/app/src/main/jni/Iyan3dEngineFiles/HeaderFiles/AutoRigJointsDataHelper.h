#ifndef Iyan3D_AutoRigJointsDataHelper_h
#define Iyan3D_AutoRigJointsDataHelper_h

#include "json_json.h"
#include <vector>
#include <cstring>
#include "../../SGEngine2/Utilities/Logger.h"
#include <map>
#include "MathHelper.h"
#include "FileHelper.h"
#include "Constants.h"

using namespace std;

struct TPoseJoint{
    int id;
    int parentId;
    Vector3 position, rotation;
    float sphereRadius,envelopeRadius;
};

class AutoRigJointsDataHelper{
    
public:
    static void getTPoseJointsData(vector<TPoseJoint> & tPoseJoints);
    static void getTPoseJointsDataFromMesh(vector<TPoseJoint> & tPoseJoints, SkinMesh *sMesh);
    static void getTPoseJointsDataForOwnRig(vector<TPoseJoint> & tPoseJoints,int boneCount);
    static void addNewTPoseJointData(vector<TPoseJoint> & tPoseJoints,int fromJointId);
    
};
#endif