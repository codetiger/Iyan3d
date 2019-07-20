//
//  QuaternionGLK.cpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "Quaternion.h"
#include "GLKMatrix4.h"

Quaternion::Quaternion() {
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    w = 1.0f;
}

Quaternion::Quaternion(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
}

Quaternion::Quaternion(float x, float y, float z) {
    set(x, y, z);
}

Quaternion::Quaternion(const Vector3& vec) {
    set(vec.x, vec.y, vec.z);
}

Quaternion::Quaternion(const Mat4& mat) {
    (*this) = mat;
}

Quaternion& Quaternion::operator=(const Quaternion& other) {
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    return *this;
}

Quaternion::Quaternion(GLKQuaternion quat) {
    x = quat.x;
    y = quat.y;
    z = quat.z;
    w = quat.w;
}

void Quaternion::setValues(GLKQuaternion quat) {
    x = quat.x;
    y = quat.y;
    z = quat.z;
    w = quat.w;
}

GLKQuaternion Quaternion::glkQuaternion() const {
    return GLKQuaternionMake(x, y, z, w);
}

Quaternion& Quaternion::operator=(const Mat4& mm) {
    // Most of the Transformation Matrix decomposition code were taken from http://opensource.apple.com//source/WebCore/WebCore-514/platform/graphics/transforms/TransformationMatrix.cpp

    Mat4 m = mm;

    for (int i = 0; i < 16; i++)
        m[i] /= m[15];

    m[3] = m[7] = m[11] = 0.0;
    m[15]               = 1.0;

    //clearing out translation
    m[12] = m[13] = m[14] = 0.0;

    Vector3 row[3], pdum3;

    for (int i = 0; i < 3; i++) {
        row[i].x = m[i * 4 + 0];
        row[i].y = m[i * 4 + 1];
        row[i].z = m[i * 4 + 2];
    }

    row[0].normalize();

    float skewXY = row[0].dotProduct(row[1]);
    row[1]       = row[1] + (row[0] * -skewXY);
    row[1].normalize();

    float skewXZ = row[0].dotProduct(row[2]);
    row[2]       = row[2] + (row[0] * -skewXZ);
    float skewYZ = row[1].dotProduct(row[2]);
    row[2]       = row[2] + (row[1] * -skewYZ);
    row[2].normalize();

    pdum3 = row[1].crossProduct(row[2]);
    if (row[0].dotProduct(pdum3) < 0) {
        for (int i = 0; i < 3; i++) {
            row[i].x *= -1;
            row[i].y *= -1;
            row[i].z *= -1;
        }
    }

    float t = row[0].x + row[1].y + row[2].z + 1.0;

    if (t > 1e-4) {
        float s = 0.5 / sqrt(t);
        w       = 0.25 / s;
        x       = (row[2].y - row[1].z) * s;
        y       = (row[0].z - row[2].x) * s;
        z       = (row[1].x - row[0].y) * s;
    } else if (row[0].x > row[1].y && row[0].x > row[2].z) {
        float s = sqrt(1.0 + row[0].x - row[1].y - row[2].z) * 2.0; // S=4*qx
        x       = 0.25 * s;
        y       = (row[0].y + row[1].x) / s;
        z       = (row[0].z + row[2].x) / s;
        w       = (row[2].y - row[1].z) / s;
    } else if (row[1].y > row[2].z) {
        float s = sqrt(1.0 + row[1].y - row[0].x - row[2].z) * 2.0; // S=4*qy
        x       = (row[0].y + row[1].x) / s;
        y       = 0.25 * s;
        z       = (row[1].z + row[2].y) / s;
        w       = (row[0].z - row[2].x) / s;
    } else {
        float s = sqrt(1.0 + row[2].z - row[0].x - row[1].y) * 2.0; // S=4*qz
        x       = (row[0].z + row[2].x) / s;
        y       = (row[1].z + row[2].y) / s;
        z       = 0.25 * s;
        w       = (row[1].x - row[0].y) / s;
    }

    x = -x;
    y = -y;
    z = -z;
    return *this;
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
    Quaternion    tmp;
    GLKQuaternion quat = GLKQuaternionMultiply(other.glkQuaternion(), glkQuaternion());
    tmp.setValues(quat);
    return tmp;
}

Quaternion Quaternion::operator*(float s) const {
    return Quaternion(s * x, s * y, s * z, s * w);
}

Quaternion& Quaternion::operator*=(float s) {
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& other) {
    return (*this = other * (*this));
}

Quaternion Quaternion::operator+(const Quaternion& b) const {
    GLKQuaternion quat = GLKQuaternionAdd(glkQuaternion(), b.glkQuaternion());
    return Quaternion(quat);
}

bool Quaternion::operator==(const Quaternion& b) const {
    return x == b.x && y == b.y && z == b.z && w == b.w;
}

bool Quaternion::operator!=(const Quaternion& b) const {
    return x != b.x || y != b.y || z != b.z || w != b.w;
}

Mat4 Quaternion::getMatrix() const {
    Mat4 m;
    m.matrix = GLKMatrix4MakeWithQuaternion(glkQuaternion());
    return m;
}

Quaternion& Quaternion::makeInverse() {
    GLKQuaternion quat = GLKQuaternionInvert(glkQuaternion());
    setValues(quat);
    //    x = -x;
    //    y = -y;
    //    z = -z;
    return *this;
}

Quaternion& Quaternion::set(float x, float y, float z, float w) {
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    return *this;
}

Quaternion& Quaternion::set(float X, float Y, float Z) {
    double angle;

    angle           = X * 0.5;
    const double sr = sin(angle);
    const double cr = cos(angle);

    angle           = Y * 0.5;
    const double sp = sin(angle);
    const double cp = cos(angle);

    angle           = Z * 0.5;
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

Quaternion& Quaternion::set(const Vector3& vec) {
    return set(vec.x, vec.y, vec.z);
}

Quaternion& Quaternion::set(const Quaternion& quat) {
    return (*this = quat);
}

Quaternion& Quaternion::normalize() {
    GLKQuaternion quat = GLKQuaternionNormalize(glkQuaternion());
    setValues(quat);
    return *this;
}

float Quaternion::dotProduct(const Quaternion& q2) const {
    return (x * q2.x) + (y * q2.y) + (z * q2.z) + (w * q2.w);
}

Quaternion& Quaternion::fromAngleAxis(float angle, const Vector3& axis) {
    const float fHalfAngle = 0.5f * angle;
    const float fSin       = sinf(fHalfAngle);
    w                      = cosf(fHalfAngle);
    x                      = fSin * axis.x;
    y                      = fSin * axis.y;
    z                      = fSin * axis.z;
    return *this;

    //    GLKQuaternion quat = GLKQuaternionMakeWithAngleAndAxis(angle, axis.x, axis.y, axis.z);
    //    setValues(quat);
    //    return *this;
}

void Quaternion::toAngleAxis(float& angle, Vector3& axis) const {
    //TODO use for setrotation radians
    const float scale = sqrtf(x * x + y * y + z * z);

    if (scale == 0.0 || w > 1.0f || w < -1.0f) {
        angle  = 0.0f;
        axis.x = 0.0f;
        axis.y = 1.0f;
        axis.z = 0.0f;
    } else {
        const float invscale = 1.0 / (scale);
        angle                = 2.0f * acosf(w);
        axis.x               = x * invscale;
        axis.y               = y * invscale;
        axis.z               = z * invscale;
    }
}

void Quaternion::toEuler(Vector3& euler) const {
    const double sqw       = w * w;
    const double sqx       = x * x;
    const double sqy       = y * y;
    const double sqz       = z * z;
    const double test      = 2.0 * (y * w - x * z);
    double       tolerance = 0.000001;

    if (((test + tolerance >= 1.0) && (test - tolerance <= 1.0))) {
        euler.z = (float)(-2.0 * atan2(x, w));
        euler.x = 0;
        euler.y = (float)(PI64 / 2.0);
    } else if (((test + tolerance >= -1.0) && (test - tolerance <= -1.0))) {
        euler.z = (float)(2.0 * atan2(x, w));
        euler.x = 0;
        euler.y = (float)(PI64 / -2.0);
    } else {
        euler.z = (float)atan2(2.0 * (x * y + z * w), (sqx - sqy - sqz + sqw));
        euler.x = (float)atan2(2.0 * (y * z + x * w), (-sqx - sqy + sqz + sqw));
        euler.y = (float)asin(fmin(fmax(test, -1.0), 1.0));
    }
}

Vector3 Quaternion::operator*(const Vector3& v) const {
    // nVidia SDK implementation
    Vector3 uv, uuv;
    Vector3 qvec(x, y, z);
    uv  = qvec.crossProduct(v);
    uuv = qvec.crossProduct(uv);
    uv *= (2.0f * w);
    uuv *= 2.0f;

    return v + uv + uuv;
}

Quaternion& Quaternion::makeIdentity() {
    w = 1.f;
    x = 0.f;
    y = 0.f;
    z = 0.f;
    return *this;
}
