//
//  QuaternionGLM.cpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifdef OPTIMGLM

#include "QuaternionGLM.h"

Quaternion::Quaternion()
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;
}

Quaternion::Quaternion(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Quaternion::Quaternion(float x, float y, float z)
{
    set(x, y, z);
}
Quaternion::Quaternion(const Vector3& vec)
{
    set(vec.x, vec.y, vec.z);
}
Quaternion::Quaternion(const Mat4& mat)
{
    (*this) = mat;
}
Quaternion& Quaternion::operator=(const Quaternion& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    return *this;
}

Quaternion::Quaternion(glm::quat quat)
{
    x = quat.x;
    y = quat.y;
    z = quat.z;
    w = quat.w;
}

void Quaternion::setValues(glm::quat quat)
{
    x = quat.x;
    y = quat.y;
    z = quat.z;
    w = quat.w;
}

glm::quat Quaternion::glkQuaternion() const
{
    return glm::quat(w, x, y, z);
}

Quaternion& Quaternion::operator=(const Mat4& m)
{
    glm::quat quat = glm::quat(m.matrix);
    setValues(quat);
    return *this;
}
Quaternion Quaternion::operator*(const Quaternion& other) const
{
    glm::quat q = glkQuaternion() * other.glkQuaternion();
    return Quaternion(q);
}

Quaternion Quaternion::operator*(float s) const
{
    glm::quat q = glkQuaternion() * s;
    return Quaternion(q);
}

Quaternion& Quaternion::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& other)
{
    return (*this = other * (*this));
}

Quaternion Quaternion::operator+(const Quaternion& b) const
{
    glm::quat quat = glkQuaternion() + b.glkQuaternion();
    return Quaternion(quat);
}
Mat4 Quaternion::getMatrix() const
{
    Mat4 m;
    getMatrix(m);
    return m;
}

void Quaternion::getMatrix(Mat4& dest, const Vector3& center) const
{
    dest.matrix = glm::mat4_cast(glkQuaternion());
}

Quaternion& Quaternion::makeInverse()
{
    x = -x;
    y = -y;
    z = -z;
    return *this;
}

Quaternion& Quaternion::set(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    return *this;
}

Quaternion& Quaternion::set(float X, float Y, float Z)
{
    double angle;
    
    angle = X * 0.5;
    const double sr = sin(angle);
    const double cr = cos(angle);
    
    angle = Y * 0.5;
    const double sp = sin(angle);
    const double cp = cos(angle);
    
    angle = Z * 0.5;
    const double sy = sin(angle);
    const double cy = cos(angle);
    
    const double cpcy = cp * cy;
    const double spcy = sp * cy;
    const double cpsy = cp * sy;
    const double spsy = sp * sy;
    
    x = (float)(sr * cpcy - cr * spsy);
    y = (float)(cr * spcy + sr * cpsy);
    z = (float)(cr * cpsy - sr * spcy);
    w = (float)(cr * cpcy + sr * spsy);
    
    return normalize();
}

Quaternion& Quaternion::set(const Vector3& vec)
{
    return set(vec.x, vec.y, vec.z);
}

Quaternion& Quaternion::set(const Quaternion& quat)
{
    return (*this = quat);
}

Quaternion& Quaternion::normalize()
{
    glm::quat quat = glm::normalize(glkQuaternion());
    setValues(quat);
    return *this;
}

float Quaternion::dotProduct(const Quaternion& q2) const
{
    return glm::dot(glkQuaternion(), q2.glkQuaternion());
}

Quaternion& Quaternion::fromAngleAxis(float angle, const Vector3& axis)
{
    const float fHalfAngle = 0.5f * angle;
    const float fSin = sinf(fHalfAngle);
    w = cosf(fHalfAngle);
    x = fSin * axis.x;
    y = fSin * axis.y;
    z = fSin * axis.z;
    return *this;
}

void Quaternion::toAngleAxis(float& angle, Vector3& axis) const
{
    //TODO use for setrotation radians
    const float scale = sqrtf(x * x + y * y + z * z);
    
    if (scale == 0.0 || w > 1.0f || w < -1.0f) {
        angle = 0.0f;
        axis.x = 0.0f;
        axis.y = 1.0f;
        axis.z = 0.0f;
    }
    else {
        const float invscale = 1.0 / (scale);
        angle = 2.0f * acosf(w);
        axis.x = x * invscale;
        axis.y = y * invscale;
        axis.z = z * invscale;
    }
}

void Quaternion::toEuler(Vector3& euler) const
{
    glm::vec3 v = glm::eulerAngles(glkQuaternion());
    euler.setValues(v);
}

Vector3 Quaternion::operator*(const Vector3& v) const
{
    glm::vec3 nv = glkQuaternion() * v.glmVector();
    return Vector3(nv);
}

Quaternion& Quaternion::makeIdentity()
{
    w = 1.f;
    x = 0.f;
    y = 0.f;
    z = 0.f;
    return *this;
}

#endif
