//
//  QuaternionGLK.cpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifdef OPTIMGLKM

#include "QuaternionGLK.h"

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

Quaternion::Quaternion(GLKQuaternion quat)
{
    x = quat.x;
    y = quat.y;
    z = quat.z;
    w = quat.w;
}

void Quaternion::setValues(GLKQuaternion quat)
{
    x = quat.x;
    y = quat.y;
    z = quat.z;
    w = quat.w;
}

GLKQuaternion Quaternion::glkQuaternion() const
{
    return GLKQuaternionMake(x, y, z, w);
}

Quaternion& Quaternion::operator=(const Mat4& m)
{
    const float diag = m[0] + m[5] + m[10] + 1;
    
    if (diag > 0.0f) {
        const float scale = sqrtf(diag) * 2.0f; // get scale from diagonal
        
        // TODO: speed this up
        x = (m[6] - m[9]) / scale;
        y = (m[8] - m[2]) / scale;
        z = (m[1] - m[4]) / scale;
        w = 0.25f * scale;
    }
    else {
        if (m[0] > m[5] && m[0] > m[10]) {
            // 1st element of diag is greatest value
            // find scale according to 1st element, and double it
            const float scale = sqrtf(1.0f + m[0] - m[5] - m[10]) * 2.0f;
            
            // TODO: speed this up
            x = 0.25f * scale;
            y = (m[4] + m[1]) / scale;
            z = (m[2] + m[8]) / scale;
            w = (m[6] - m[9]) / scale;
        }
        else if (m[5] > m[10]) {
            // 2nd element of diag is greatest value
            // find scale according to 2nd element, and double it
            const float scale = sqrtf(1.0f + m[5] - m[0] - m[10]) * 2.0f;
            
            // TODO: speed this up
            x = (m[4] + m[1]) / scale;
            y = 0.25f * scale;
            z = (m[9] + m[6]) / scale;
            w = (m[8] - m[2]) / scale;
        }
        else {
            // 3rd element of diag is greatest value
            // find scale according to 3rd element, and double it
            const float scale = sqrtf(1.0f + m[10] - m[0] - m[5]) * 2.0f;
            
            // TODO: speed this up
            x = (m[8] + m[2]) / scale;
            y = (m[9] + m[6]) / scale;
            z = 0.25f * scale;
            w = (m[1] - m[4]) / scale;
        }
    }
    
    return normalize();
}

Quaternion Quaternion::operator*(const Quaternion& other) const
{    
    Quaternion tmp;
    GLKQuaternion quat = GLKQuaternionMultiply(other.glkQuaternion(), glkQuaternion());
    tmp.setValues(quat);
    return tmp;
}

Quaternion Quaternion::operator*(float s) const
{
    return Quaternion(s * x, s * y, s * z, s * w);
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
    GLKQuaternion quat = GLKQuaternionAdd(glkQuaternion(), b.glkQuaternion());
    return Quaternion(quat);
}

Mat4 Quaternion::getMatrix() const
{
    Mat4 m;
    m.matrix = GLKMatrix4MakeWithQuaternion(glkQuaternion());
    return m;
}

Quaternion& Quaternion::makeInverse()
{
    GLKQuaternion quat = GLKQuaternionInvert(glkQuaternion());
    setValues(quat);
//    x = -x;
//    y = -y;
//    z = -z;
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
    GLKQuaternion quat = GLKQuaternionNormalize(glkQuaternion());
    setValues(quat);
    return *this;
}

float Quaternion::dotProduct(const Quaternion& q2) const
{
    return (x * q2.x) + (y * q2.y) + (z * q2.z) + (w * q2.w);
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

//    GLKQuaternion quat = GLKQuaternionMakeWithAngleAndAxis(angle, axis.x, axis.y, axis.z);
//    setValues(quat);
//    return *this;
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
    const double sqw = w * w;
    const double sqx = x * x;
    const double sqy = y * y;
    const double sqz = z * z;
    const double test = 2.0 * (y * w - x * z);
    double tolerance = 0.000001;
    if (((test + tolerance >= 1.0) && (test - tolerance <= 1.0))) {
        // heading = rotation about z-axis
        euler.z = (float)(-2.0 * atan2(x, w));
        // bank = rotation about x-axis
        euler.x = 0;
        // attitude = rotation about y-axis
        euler.y = (float)(PI64 / 2.0);
    }
    else if (((test + tolerance >= -1.0) && (test - tolerance <= -1.0))) {
        // heading = rotation about z-axis
        euler.z = (float)(2.0 * atan2(x, w));
        // bank = rotation about x-axis
        euler.x = 0;
        // attitude = rotation about y-axis
        euler.y = (float)(PI64 / -2.0);
    }
    else {
        // heading = rotation about z-axis
        euler.z = (float)atan2(2.0 * (x * y + z * w), (sqx - sqy - sqz + sqw));
        // bank = rotation about x-axis
        euler.x = (float)atan2(2.0 * (y * z + x * w), (-sqx - sqy + sqz + sqw));
        // attitude = rotation about y-axis
        euler.y = (float)asin(fmin(fmax(test, -1.0), 1.0));
    }
}
Vector3 Quaternion::operator*(const Vector3& v) const
{
    // nVidia SDK implementation
    Vector3 uv, uuv;
    Vector3 qvec(x, y, z);
    uv = qvec.crossProduct(v);
    uuv = qvec.crossProduct(uv);
    uv *= (2.0f * w);
    uuv *= 2.0f;
    
    return v + uv + uuv;
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
