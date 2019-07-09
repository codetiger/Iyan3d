
#include "HeaderFiles/MathHelper.h"

Quaternion MathHelper::RotateNodeInWorld(Quaternion rot, Quaternion q1)
{
    Mat4 m1 = q1.getMatrix();
    Mat4 m2 = rot.getMatrix();
    Mat4 m = m1 * m2;
    return Quaternion(m);
}

Quaternion MathHelper::rotationBetweenVectors(Vector3 targetDirection, Vector3 initialDirection)
{
    //Gets the initialDirectionVector and targetDirectionVector and finds the rotation that is needed to rotatate initialDirection to targetDirection.
    //If initialDirection points the baseRotation(when the angle with all the axis is 0) then you can apply the return value directly.
    //else if the initialDirection points the currentRotation, you have to multiply the return value with the currentRotation. (Note that multiplication of two Quaternions mean that you are adding two angles.)
    
    float dot = initialDirection.dotProduct(targetDirection);
    Vector3 cross = initialDirection.crossProduct(targetDirection);
    cross = cross.normalize();
    dot = (dot < -1 || dot > 1) ? 0.0:dot;
    float turnAngle = acos(dot);
    Quaternion delta;
    delta.fromAngleAxis(turnAngle, cross);
    delta.normalize();
    return delta;
}

Quaternion MathHelper::getGlobalQuaternion(shared_ptr<Node> bone)
{
    Quaternion modelQuaernion = Quaternion();
    if(!bone)
        return modelQuaernion;
    
    shared_ptr<Node> parent = (bone->Parent);
    if(parent)
        return bone->getRotation() * getGlobalQuaternion(parent);
    else
        return bone->getRotation();
}

Quaternion MathHelper::irrGetLocalQuaternion(shared_ptr<Node> node, Quaternion q)
{
    Quaternion modelQuaernion = Quaternion();
    if(!node)
        return modelQuaernion;

    shared_ptr<Node> parent = node->Parent;
    return q * getGlobalQuaternion((parent)).makeInverse();
}

Vector3 MathHelper::getRelativePosition(Mat4 &originTransformation,Vector3 targetGlobalPosition)
{
    Mat4 targetGlobalTrans;
    Mat4 originInvTrans = originTransformation;
    targetGlobalTrans.translate(targetGlobalPosition);
    originInvTrans.invert();
    Mat4 targetLocalTrans = originInvTrans * targetGlobalTrans;
    return targetLocalTrans.getTranslation();
}

Vector3 MathHelper::getRelativeScale(Mat4 &originTransformation,Vector3 targetGlobalScale)
{
    Mat4 targetGlobalTrans;
    Mat4 originInvTrans = originTransformation;
    targetGlobalTrans.scale(targetGlobalScale);
    originInvTrans.invert();
    Mat4 targetLocalTrans = originInvTrans * targetGlobalTrans;
    return targetLocalTrans.getScale();
}

Quaternion MathHelper::getRelativeParentRotation(shared_ptr<JointNode> moveNode, Vector3 targetGlobalPosition)
{
    shared_ptr<JointNode> parent = dynamic_pointer_cast<JointNode>(moveNode->Parent);
    moveNode->updateAbsoluteTransformation();
    if(!parent)
        return Vector3(0.0);
    
    parent->updateAbsoluteTransformation();
    Vector3 currentDir = (moveNode->getAbsolutePosition() - parent->getAbsolutePosition()).normalize();
    Vector3 targetDir = (targetGlobalPosition - parent->getAbsolutePosition()).normalize();
    Quaternion delta = MathHelper::rotationBetweenVectors(targetDir, currentDir);
    Quaternion global = MathHelper::getGlobalQuaternion(parent) * delta;
    Quaternion local = global * MathHelper::getGlobalQuaternion(dynamic_pointer_cast<JointNode>(parent->Parent)).makeInverse();
    return local;
}

void MathHelper::copyVector3ToPointer(float * pointer , Vector3 value)
{
    *pointer++ = value.x;
    *pointer++ = value.y;
    *pointer++ = value.z;
}

Vector3 MathHelper::packInterger(int toPack)
{
    Vector3 res;
    res.x = (int)((toPack >> 8) & 0xFF);
    res.y = (int)(toPack  & 0xFF);
    res.z = 255.0;

    return res;
}

int MathHelper::unpackInterger(Vector3 toUnpack)
{
    int res = 0;
    
    res = res | ((int)toUnpack.x & 0xFF);
    res = res << 8;
    res = res | ((int)toUnpack.y & 0xFF);
    return res;
}



