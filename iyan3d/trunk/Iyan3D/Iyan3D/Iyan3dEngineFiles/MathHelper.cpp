#include "HeaderFiles/MathHelper.h"

Quaternion MathHelper::toQuaternion(Vector3 eulerRotation)
{
    return Quaternion(eulerRotation);
}
Vector3 MathHelper::toEuler(Quaternion QuaternionRotation)
{
    // Have to be verified
    Vector3 eulerRotation;
    QuaternionRotation.toEuler(eulerRotation);
    return eulerRotation;
}
Quaternion MathHelper::RotateNodeInWorld(Vector3 rot, Quaternion q1)
{
    // Have to be verified
    
    Mat4 m1 = q1.getMatrix();
    Quaternion q2 = Quaternion(rot * DEGTORAD);
    Mat4 m2 = q2.getMatrix();
    Mat4 m = m1*m2;
    return Quaternion(m);
}
Quaternion MathHelper::rotationBetweenVectors(Vector3 targetDirection,Vector3 initialDirection)
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
    Quaternion junk;
    if(!bone)    return junk;
    
    shared_ptr<Node> parent = (bone->Parent);
    if(parent)
        return Quaternion(bone->getRotationInDegrees() * DEGTORAD) * getGlobalQuaternion(parent);
    else
        return Quaternion(bone->getRotationInDegrees() * DEGTORAD);
}
Quaternion MathHelper::irrGetLocalQuaternion(shared_ptr<Node> node, Quaternion q)
{
    Quaternion junk;
    if(!node)
        return junk;
    shared_ptr<Node> parent = node->Parent;
    return q * getGlobalQuaternion((parent)).makeInverse();
}
Vector3 MathHelper::getEulerRotation(Quaternion rotation)
{
    Vector3 eulerRotation;
    rotation.toEuler(eulerRotation);
    eulerRotation *= RADTODEG;
    eulerRotation.x = eulerRotation.x < 0 ? eulerRotation.x += 360.0:eulerRotation.x;
    eulerRotation.y = eulerRotation.y < 0 ? eulerRotation.y += 360.0:eulerRotation.y;
    eulerRotation.z = eulerRotation.z < 0 ? eulerRotation.z += 360.0:eulerRotation.z;
    return eulerRotation;
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
Vector3 MathHelper::getRelativeParentRotation(shared_ptr<JointNode> moveNode, Vector3 targetGlobalPosition)
{
    shared_ptr<JointNode> parent = dynamic_pointer_cast<JointNode>(moveNode->Parent);
    moveNode->updateAbsoluteTransformation();
    parent->updateAbsoluteTransformation();
    Vector3 currentDir = (moveNode->getAbsolutePosition() - parent->getAbsolutePosition()).normalize();
    Vector3 targetDir = (targetGlobalPosition - parent->getAbsolutePosition()).normalize();
    Quaternion delta = MathHelper::rotationBetweenVectors(targetDir,currentDir);
    Quaternion global = MathHelper::getGlobalQuaternion(parent) * delta;
    Quaternion local = global * MathHelper::getGlobalQuaternion(dynamic_pointer_cast<JointNode>(parent->Parent)).makeInverse();
    //parent.reset();
    //moveNode.reset();
    return MathHelper::toEuler(local)*RADTODEG;
}

void MathHelper::copyVector4ToPointer(float * pointer , Vector4 value)
{
    *pointer++ = value.x;
    *pointer++ = value.y;
    *pointer++ = value.z;
    *pointer++ = value.w;
}

void MathHelper::copyVector3ToPointer(float * pointer , Vector3 value)
{
    *pointer++ = value.x;
    *pointer++ = value.y;
    *pointer++ = value.z;
}

float MathHelper::GetScreenScaleFactor(const Vector3& worldpnt, shared_ptr<CameraNode> camera)
{
    //    if(camera->isOrthogonal())
    //        return 2.0f/camera->getProjectionMatrix()[0];
    //
    //    Vector3 viewpnt = worldpnt;
    //    camera->getViewMatrix().transformVect(viewpnt);
    //    return  2.0f * viewpnt.Z / camera->getProjectionMatrix()[0];
    
    /*
    Plane3D        near_fplane = camera->getViewFrustum()->planes[SViewFrustum::VF_NEAR_PLANE];
    Vector3       cam_z = near_fplane.Normal;
    Vector3       width_vtr = camera->getUpVector().crossProduct(cam_z);
    plane3df left_fplane = camera->getViewFrustum()->planes[SViewFrustum::VF_LEFT_PLANE];
    plane3df right_fplane = camera->getViewFrustum()->planes[SViewFrustum::VF_RIGHT_PLANE];
    
    Vector3 p1,p2;
    bool i1 = left_fplane.getIntersectionWithLine(worldpnt,-width_vtr,p1);
    bool i2 = right_fplane.getIntersectionWithLine(worldpnt,width_vtr,p2);
    
    return p1.getDistanceFrom(p2);
    */
    //camera.reset();
    return 0;
}


