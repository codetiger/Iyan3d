#ifndef __iyan3dengine__MathHelper__
#define __iyan3dengine__MathHelper__

#include "SGEngineCommon.h"

using namespace std;

class MathHelper
{
    
public:
    
    static Quaternion toQuaternion(Vector3 eulerRotation) ;
    static Vector3 toEuler(Quaternion quaternionRotation);
    static Quaternion RotateNodeInWorld(Vector3 rot, Quaternion q1);
    static Quaternion rotationBetweenVectors(Vector3 targetDirection,Vector3 initialDirection);
    static Quaternion getGlobalQuaternion(shared_ptr<Node> bone);
    static Quaternion irrGetLocalQuaternion(shared_ptr<Node> bone, Quaternion q);
    static Vector3 getEulerRotation(Quaternion rotation);
    static Vector3 getRelativePosition(Mat4 &originTransformation,Vector3 targetGlobalPosition);
    static Vector3 getRelativeParentRotation(shared_ptr<JointNode> moveNode, Vector3 targetGlobalPosition);
    static void copyVector4ToPointer(float * pointer , Vector4 value);
    static void copyVector3ToPointer(float * pointer , Vector3 value);
    static float GetScreenScaleFactor(const Vector3& worldpnt,  shared_ptr<CameraNode> camera);
    template <typename T>
    static T getLinearInterpolation(float x1, T y1, float x2, T y2, float x)
    {
        return y1+(y2+(y1*-1.0))*(float)((x-x1)/(x2-x1));
        //Source: https://en.wikipedia.org/wiki/Linear_interpolation
    }
    
};

#endif