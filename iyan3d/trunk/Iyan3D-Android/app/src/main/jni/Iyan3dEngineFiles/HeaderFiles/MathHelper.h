#ifndef __iyan3dengine__MathHelper__
#define __iyan3dengine__MathHelper__

#include "SGEngineCommon.h"

using namespace std;

class MathHelper
{
    
public:
    
    static Quaternion RotateNodeInWorld(Quaternion rot, Quaternion q1);
    static Quaternion rotationBetweenVectors(Vector3 targetDirection,Vector3 initialDirection);
    static Quaternion getGlobalQuaternion(shared_ptr<Node> bone);
    static Quaternion irrGetLocalQuaternion(shared_ptr<Node> bone, Quaternion q);

    static Vector3 getRelativePosition(Mat4 &originTransformation,Vector3 targetGlobalPosition);
    static Vector3 getRelativeScale(Mat4 &originTransformation,Vector3 targetGlobalPosition);
    static Quaternion getRelativeParentRotation(shared_ptr<JointNode> moveNode, Vector3 targetGlobalPosition);
    
    static void copyVector3ToPointer(float * pointer , Vector3 value);
    
    static Vector3 packInterger(int toPack);
    static int unpackInterger(Vector3 toUnpack);

    template <typename T>
    static T getLinearInterpolation(float x1, T y1, float x2, T y2, float x)
    {
        return y1+(y2+(y1*-1.0))*(float)((x-x1)/(x2-x1));
    }
    
};

#endif