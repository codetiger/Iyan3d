//
//  Mat4.cpp
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "Mat4.h"

Mat4::Mat4()
{
    static const float IDENTITY_MATRIX[] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    memcpy(c, IDENTITY_MATRIX, sizeof(float) * 16);
}
Mat4::Mat4(float* pMat)
{
    memcpy(c, pMat, sizeof(float) * 16);
}
Mat4::Mat4(const Vector3& axis_x, const Vector3& axis_y, const Vector3& axis_z, const Vector3& trans)
{
    c[0] = axis_x.x;
    c[1] = axis_x.y;
    c[2] = axis_x.z;
    c[3] = 0.0f;
    c[4] = axis_y.x;
    c[5] = axis_y.y;
    c[6] = axis_y.z;
    c[7] = 0.0f;
    c[8] = axis_z.x;
    c[9] = axis_z.y;
    c[10] = axis_z.z;
    c[11] = 0.0f;
    c[12] = trans.x;
    c[13] = trans.y;
    c[14] = trans.z;
    c[15] = 1.0f;
}
Mat4::~Mat4()
{
}
Mat4& Mat4::operator=(const Mat4& m)
{
    if (this != &m) {
        memcpy(c, m.c, sizeof(float) * 16);
    }
    return *this;
}
bool Mat4::operator==(const Mat4& m) const
{
    return memcmp(c, m.c, sizeof(float) * 16) == 0;
}
Mat4& Mat4::operator*=(const Mat4& m)
{
    Mat4 m3;
    Mat4 m2 = m;
    Mat4 m1 = (*this);
    (*this)[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
    (*this)[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
    (*this)[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
    (*this)[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

    (*this)[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
    (*this)[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
    (*this)[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
    (*this)[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

    (*this)[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
    (*this)[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
    (*this)[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
    (*this)[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

    (*this)[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
    (*this)[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
    (*this)[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
    (*this)[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

    return *this;
}
Mat4 Mat4::operator*(const Mat4& m) const
{
    Mat4 ret = *this;
    ret *= m;
    return ret;
}

Vector4 Mat4::operator*(const Vector4& v) const
{
    Vector4 result;
    Mat4 ret = *this;

    result.x = ret[0] * v.x + ret[4] * v.y + ret[8] * v.z + ret[12] * v.w;
    result.y = ret[1] * v.x + ret[5] * v.y + ret[9] * v.z + ret[13] * v.w;
    result.z = ret[2] * v.x + ret[6] * v.y + ret[10] * v.z + ret[14] * v.w;
    result.w = ret[3] * v.x + ret[7] * v.y + ret[11] * v.z + ret[15] * v.w;

    //    result.x = ret[0] * v.x + ret[1] * v.y + ret[2] * v.z + ret[3] * v.w;
    //    result.y = ret[4] * v.x + ret[5] * v.y + ret[6] * v.z + ret[7] * v.w;
    //    result.z = ret[8] * v.x + ret[9] * v.y + ret[10] * v.z + ret[11] * v.w;
    //    result.w = ret[12] * v.x + ret[13] * v.y + ret[14] * v.z + ret[15] * v.w;

    return result;
}

float& Mat4::operator[](unsigned i)
{
    return c[i];
}
float Mat4::operator[](unsigned i) const
{
    return c[i];
}
void Mat4::perspective(float fov, float aspect, float nearz, float farz)
{
    memset(c, 0, sizeof(float) * 16);

    float range = tan(fov * 0.5) * nearz;
    c[0] = (2 * nearz) / ((range * aspect) - (-range * aspect));
    c[5] = (2 * nearz) / (2 * range);
    c[10] = -(farz + nearz) / (farz - nearz);
    c[11] = -1;
    c[14] = -(2 * farz * nearz) / (farz - nearz);
}
void Mat4::translate(float x, float y, float z)
{
    (*this)[12] = x;
    (*this)[13] = y;
    (*this)[14] = z;
}
void Mat4::translate(const Vector3& v)
{
    translate(v.x, v.y, v.z);
}
void Mat4::rotate(float angle, int axis)
{
    const int cos1[3] = { 5, 0, 0 };
    const int cos2[3] = { 10, 10, 5 };
    const int sin1[3] = { 9, 2, 4 };
    const int sin2[3] = { 6, 8, 1 };
    Mat4 newMatrix;

    newMatrix.c[cos1[axis]] = cos(angle);
    newMatrix.c[sin1[axis]] = -sin(angle);
    newMatrix.c[sin2[axis]] = -newMatrix.c[sin1[axis]];
    newMatrix.c[cos2[axis]] = newMatrix.c[cos1[axis]];

    *this *= newMatrix;
}
void Mat4::setRotationRadians(Vector3 rotation)
{
    const float cr = cos(rotation.x);
    const float sr = sin(rotation.x);
    const float cp = cos(rotation.y);
    const float sp = sin(rotation.y);
    const float cy = cos(rotation.z);
    const float sy = sin(rotation.z);

    (*this)[0] = (cp * cy);
    (*this)[1] = (cp * sy);
    (*this)[2] = (-sp);

    const float srsp = sr * sp;
    const float crsp = cr * sp;

    (*this)[4] = (srsp * cy - cr * sy);
    (*this)[5] = (srsp * sy + cr * cy);
    (*this)[6] = (sr * cp);

    (*this)[8] = (crsp * cy + sr * sy);
    (*this)[9] = (crsp * sy - sr * cy);
    (*this)[10] = (cr * cp);
}
void Mat4::scale(float x, float y, float z)
{
    Mat4 newMatrix;

    newMatrix.c[0] = x;
    newMatrix.c[5] = y;
    newMatrix.c[10] = z;

    *this *= newMatrix;
}
void Mat4::scale(const Vector3& v)
{
    scale(v.x, v.y, v.z);
}
void Mat4::scale(float s)
{
    scale(s, s, s);
}
void Mat4::copyMatTo(float *pointer)
{
    for (int i = 0; i < 16; ++i)
        *pointer++ = this->c[i];
}
void Mat4::bias()
{
    static const float BIAS_MATRIX[] = {
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    };
    memcpy(c, BIAS_MATRIX, sizeof(float) * 16);
}
void Mat4::ortho(float left, float right, float top, float bottom, float nearz, float farz)
{
    memset(c, 0, sizeof(float) * 16);
    c[0] = 2.0f / (right - left);
    c[5] = 2.0f / (bottom - top);
    c[10] = -1.0f / (farz - nearz);
    c[11] = -nearz / (farz - nearz);
    c[15] = 1.0f;
}
bool Mat4::invert()
{
    float inv[16], det;
    int i;

    inv[0] = c[5] * c[10] * c[15] - c[5] * c[11] * c[14] - c[9] * c[6] * c[15] + c[9] * c[7] * c[14] + c[13] * c[6] * c[11] - c[13] * c[7] * c[10];
    inv[4] = -c[4] * c[10] * c[15] + c[4] * c[11] * c[14] + c[8] * c[6] * c[15] - c[8] * c[7] * c[14] - c[12] * c[6] * c[11] + c[12] * c[7] * c[10];
    inv[8] = c[4] * c[9] * c[15] - c[4] * c[11] * c[13] - c[8] * c[5] * c[15] + c[8] * c[7] * c[13] + c[12] * c[5] * c[11] - c[12] * c[7] * c[9];
    inv[12] = -c[4] * c[9] * c[14] + c[4] * c[10] * c[13] + c[8] * c[5] * c[14] - c[8] * c[6] * c[13] - c[12] * c[5] * c[10] + c[12] * c[6] * c[9];
    inv[1] = -c[1] * c[10] * c[15] + c[1] * c[11] * c[14] + c[9] * c[2] * c[15] - c[9] * c[3] * c[14] - c[13] * c[2] * c[11] + c[13] * c[3] * c[10];
    inv[5] = c[0] * c[10] * c[15] - c[0] * c[11] * c[14] - c[8] * c[2] * c[15] + c[8] * c[3] * c[14] + c[12] * c[2] * c[11] - c[12] * c[3] * c[10];
    inv[9] = -c[0] * c[9] * c[15] + c[0] * c[11] * c[13] + c[8] * c[1] * c[15] - c[8] * c[3] * c[13] - c[12] * c[1] * c[11] + c[12] * c[3] * c[9];
    inv[13] = c[0] * c[9] * c[14] - c[0] * c[10] * c[13] - c[8] * c[1] * c[14] + c[8] * c[2] * c[13] + c[12] * c[1] * c[10] - c[12] * c[2] * c[9];
    inv[2] = c[1] * c[6] * c[15] - c[1] * c[7] * c[14] - c[5] * c[2] * c[15] + c[5] * c[3] * c[14] + c[13] * c[2] * c[7] - c[13] * c[3] * c[6];
    inv[6] = -c[0] * c[6] * c[15] + c[0] * c[7] * c[14] + c[4] * c[2] * c[15] - c[4] * c[3] * c[14] - c[12] * c[2] * c[7] + c[12] * c[3] * c[6];
    inv[10] = c[0] * c[5] * c[15] - c[0] * c[7] * c[13] - c[4] * c[1] * c[15] + c[4] * c[3] * c[13] + c[12] * c[1] * c[7] - c[12] * c[3] * c[5];
    inv[14] = -c[0] * c[5] * c[14] + c[0] * c[6] * c[13] + c[4] * c[1] * c[14] - c[4] * c[2] * c[13] - c[12] * c[1] * c[6] + c[12] * c[2] * c[5];
    inv[3] = -c[1] * c[6] * c[11] + c[1] * c[7] * c[10] + c[5] * c[2] * c[11] - c[5] * c[3] * c[10] - c[9] * c[2] * c[7] + c[9] * c[3] * c[6];
    inv[7] = c[0] * c[6] * c[11] - c[0] * c[7] * c[10] - c[4] * c[2] * c[11] + c[4] * c[3] * c[10] + c[8] * c[2] * c[7] - c[8] * c[3] * c[6];
    inv[11] = -c[0] * c[5] * c[11] + c[0] * c[7] * c[9] + c[4] * c[1] * c[11] - c[4] * c[3] * c[9] - c[8] * c[1] * c[7] + c[8] * c[3] * c[5];
    inv[15] = c[0] * c[5] * c[10] - c[0] * c[6] * c[9] - c[4] * c[1] * c[10] + c[4] * c[2] * c[9] + c[8] * c[1] * c[6] - c[8] * c[2] * c[5];

    det = c[0] * inv[0] + c[1] * inv[4] + c[2] * inv[8] + c[3] * inv[12];

    if (det == 0) {
        return false;
    }

    det = 1.0 / det;

    for (i = 0; i < 16; i++) {
        c[i] = inv[i] * det;
    }

    return true;
}
void Mat4::transpose()
{
    float trans[16];
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            trans[x + y * 4] = c[y + x * 4];
        }
    }

    memcpy(c, trans, sizeof(float) * 16);
}
float* Mat4::pointer()
{
    return c;
}
void Mat4::buildCameraLookAtMatrixLH(Vector3 position, Vector3 target, Vector3 upVector)
{
    Vector3 zaxis = target - position;
    zaxis.normalize();

    Vector3 xaxis = upVector.crossProduct(zaxis);
    xaxis.normalize();

    Vector3 yaxis = zaxis.crossProduct(xaxis);
    yaxis.normalize();

    (*this)[0] = xaxis.x;
    (*this)[1] = yaxis.x;
    (*this)[2] = zaxis.x;
    (*this)[3] = 0;

    (*this)[4] = xaxis.y;
    (*this)[5] = yaxis.y;
    (*this)[6] = zaxis.y;
    (*this)[7] = 0;

    (*this)[8] = xaxis.z;
    (*this)[9] = yaxis.z;
    (*this)[10] = zaxis.z;
    (*this)[11] = 0;

    (*this)[12] = -xaxis.dotProduct(position);
    (*this)[13] = -yaxis.dotProduct(position);
    (*this)[14] = -zaxis.dotProduct(position);
    (*this)[15] = 1;
}
void Mat4::buildProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
    const float h = 1.0 / (tan(fieldOfViewRadians * 0.5));
    const float w = (h / aspectRatio);

    (*this)[0] = w;
    (*this)[1] = 0;
    (*this)[2] = 0;
    (*this)[3] = 0;

    (*this)[4] = 0;
    (*this)[5] = h;
    (*this)[6] = 0;
    (*this)[7] = 0;

    (*this)[8] = 0;
    (*this)[9] = 0;
    (*this)[10] = (zFar / (zFar - zNear));
    (*this)[11] = 1;

    (*this)[12] = 0;
    (*this)[13] = 0;
    (*this)[14] = (-zNear * zFar / (zFar - zNear));
    (*this)[15] = 0;
}
Vector3 Mat4::getTranslation()
{
    return Vector3((*this)[12], (*this)[13], (*this)[14]);
}
Vector3 Mat4::getRotation()
{
    return Vector3((*this)[12], (*this)[13], (*this)[14]);
}
Vector3 Mat4::getScale()
{
    const Mat4& M = *this;
    if (Maths::iszero(M[1]) && Maths::iszero(M[2]) && Maths::iszero(M[4]) && Maths::iszero(M[6]) && Maths::iszero(M[8]) && Maths::iszero(M[9]))
        return Vector3(M[0], M[5], M[10]);

    // We have to do the full calculation.
    return Vector3(sqrtf(M[0] * M[0] + M[1] * M[1] + M[2] * M[2]),
        sqrtf(M[4] * M[4] + M[5] * M[5] + M[6] * M[6]),
        sqrtf(M[8] * M[8] + M[9] * M[9] + M[10] * M[10]));
}
Vector3 Mat4::getRotationInDegree()
{
    const Mat4& mat = *this;
    Vector3 scale = getScale();
    // we need to check for negative scale on to axes, which would bring up wrong results
    if (scale.y < 0 && scale.z < 0) {
        scale.y = -scale.y;
        scale.z = -scale.z;
    }
    else if (scale.x < 0 && scale.z < 0) {
        scale.x = -scale.x;
        scale.z = -scale.z;
    }
    else if (scale.x < 0 && scale.y < 0) {
        scale.x = -scale.x;
        scale.y = -scale.y;
    }
    Vector3 invScale(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);

    double Y = -asin(Maths::clamp(mat[2] * invScale.x, -1.0, 1.0));
    const double C = cos(Y);
    Y *= (180.0 / 3.1415926535897932384626433832795028841971693993751);

    double rotx, roty, X, Z;

    if (!Maths::iszero(C)) {
        const double invC = (double)1.0f / C;
        rotx = mat[10] * invC * invScale.z;
        roty = mat[6] * invC * invScale.y;
        X = atan2(roty, rotx) * (180.0 / 3.1415926535897932384626433832795028841971693993751);
        rotx = mat[0] * invC * invScale.x;
        roty = mat[1] * invC * invScale.x;
        Z = atan2(roty, rotx) * (180.0 / 3.1415926535897932384626433832795028841971693993751);
    }
    else {
        X = 0.0;
        rotx = mat[5] * invScale.y;
        roty = -mat[4] * invScale.y;
        Z = atan2(roty, rotx) * (180.0 / 3.1415926535897932384626433832795028841971693993751);
    }

    // fix values that get below zero
    if (X < 0.0)
        X += 360.0;
    if (Y < 0.0)
        Y += 360.0;
    if (Z < 0.0)
        Z += 360.0;
    return Vector3(X, Y, Z);
}
Mat4 Mat4::setbyproduct(Mat4& other_a, Mat4& other_b)
{
    float* m1 = other_a.pointer();
    float* m2 = other_b.pointer();

    (*this)[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
    (*this)[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
    (*this)[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
    (*this)[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

    (*this)[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
    (*this)[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
    (*this)[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
    (*this)[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

    (*this)[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
    (*this)[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
    (*this)[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
    (*this)[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

    (*this)[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
    (*this)[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
    (*this)[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
    (*this)[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
    return *this;
}

void Mat4::rotateVect(Vector3& vect) const
{
    Vector3 tmp = vect;
    vect.x = tmp.x * (*this)[0] + tmp.y * (*this)[4] + tmp.z * (*this)[8];
    vect.y = tmp.x * (*this)[1] + tmp.y * (*this)[5] + tmp.z * (*this)[9];
    vect.z = tmp.x * (*this)[2] + tmp.y * (*this)[6] + tmp.z * (*this)[10];
}
