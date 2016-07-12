//
//  Mat4GLK.cpp
//  Iyan3D
//
//  Created by Karthik on 25/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifdef OPTIMGLKM

#include "Mat4GLK.h"
#include "QuaternionGLK.h"

Mat4::Mat4()
{
    static const float IDENTITY_MATRIX[] = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    float c[16];
    memcpy(c, IDENTITY_MATRIX, sizeof(float) * 16);
    matrix = GLKMatrix4MakeWithArray(c);
}

Mat4::Mat4(float* pMat)
{
    float c[16];
    memcpy(c, pMat, sizeof(float) * 16);
    matrix = GLKMatrix4MakeWithArray(c);
}

Mat4::Mat4(const Vector3& axis_x, const Vector3& axis_y, const Vector3& axis_z, const Vector3& trans)
{
    float c[16];
    
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
    
    matrix = GLKMatrix4MakeWithArray(c);
}

Mat4::~Mat4()
{
}

Mat4& Mat4::operator=(const Mat4& m)
{
    if (this != &m) {
        matrix = m.matrix;
    }
    return *this;
}

bool Mat4::operator==(const Mat4& m) const
{
    return memcmp(matrix.m, m.matrix.m, sizeof(float) * 16) == 0;
}

Mat4& Mat4::operator*=(const Mat4& m)
{
    matrix = GLKMatrix4Multiply(matrix, m.matrix);
    return *this;
}

Mat4 Mat4::operator*(const Mat4& m) const
{
    Mat4 ret;
    ret.matrix = GLKMatrix4Multiply(matrix, m.matrix);
    return ret;
}

Vector4 Mat4::operator*(const Vector4& v) const
{
    GLKVector4 vect = GLKMatrix4MultiplyVector4(matrix, v.glkVector());
    return Vector4(vect);
}

void Mat4::setElement(unsigned int index, float value) {
    matrix.m[index] = value;
}

float& Mat4::operator[](unsigned i)
{
    return matrix.m[i];
}

float Mat4::operator[](unsigned i) const
{
    return matrix.m[i];
}

void Mat4::perspective(float fov, float aspect, float nearz, float farz)
{
    memset(matrix.m, 0, sizeof(float) * 16);
    
    float range = tan(fov * 0.5) * nearz;
    matrix.m[0] = (2 * nearz) / ((range * aspect) - (-range * aspect));
    matrix.m[5] = (2 * nearz) / (2 * range);
    matrix.m[10] = -(farz + nearz) / (farz - nearz);
    matrix.m[11] = -1;
    matrix.m[14] = -(2 * farz * nearz) / (farz - nearz);
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
    matrix = GLKMatrix4Scale(matrix, x, y, z);
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
    //memcpy(pointer, matrix.m, sizeof(float) * 16);
    for (int i = 0; i < 16; ++i)
        *pointer++ = this->matrix.m[i];
}

bool Mat4::invert()
{
    float inv[16], det;
    int i;
    
    inv[0] = (*this)[5] * (*this)[10] * (*this)[15] - (*this)[5] * (*this)[11] * (*this)[14] - (*this)[9] * (*this)[6] * (*this)[15] + (*this)[9] * (*this)[7] * (*this)[14] + (*this)[13] * (*this)[6] * (*this)[11] - (*this)[13] * (*this)[7] * (*this)[10];
    inv[4] = -(*this)[4] * (*this)[10] * (*this)[15] + (*this)[4] * (*this)[11] * (*this)[14] + (*this)[8] * (*this)[6] * (*this)[15] - (*this)[8] * (*this)[7] * (*this)[14] - (*this)[12] * (*this)[6] * (*this)[11] + (*this)[12] * (*this)[7] * (*this)[10];
    inv[8] = (*this)[4] * (*this)[9] * (*this)[15] - (*this)[4] * (*this)[11] * (*this)[13] - (*this)[8] * (*this)[5] * (*this)[15] + (*this)[8] * (*this)[7] * (*this)[13] + (*this)[12] * (*this)[5] * (*this)[11] - (*this)[12] * (*this)[7] * (*this)[9];
    inv[12] = -(*this)[4] * (*this)[9] * (*this)[14] + (*this)[4] * (*this)[10] * (*this)[13] + (*this)[8] * (*this)[5] * (*this)[14] - (*this)[8] * (*this)[6] * (*this)[13] - (*this)[12] * (*this)[5] * (*this)[10] + (*this)[12] * (*this)[6] * (*this)[9];
    inv[1] = -(*this)[1] * (*this)[10] * (*this)[15] + (*this)[1] * (*this)[11] * (*this)[14] + (*this)[9] * (*this)[2] * (*this)[15] - (*this)[9] * (*this)[3] * (*this)[14] - (*this)[13] * (*this)[2] * (*this)[11] + (*this)[13] * (*this)[3] * (*this)[10];
    inv[5] = (*this)[0] * (*this)[10] * (*this)[15] - (*this)[0] * (*this)[11] * (*this)[14] - (*this)[8] * (*this)[2] * (*this)[15] + (*this)[8] * (*this)[3] * (*this)[14] + (*this)[12] * (*this)[2] * (*this)[11] - (*this)[12] * (*this)[3] * (*this)[10];
    inv[9] = -(*this)[0] * (*this)[9] * (*this)[15] + (*this)[0] * (*this)[11] * (*this)[13] + (*this)[8] * (*this)[1] * (*this)[15] - (*this)[8] * (*this)[3] * (*this)[13] - (*this)[12] * (*this)[1] * (*this)[11] + (*this)[12] * (*this)[3] * (*this)[9];
    inv[13] = (*this)[0] * (*this)[9] * (*this)[14] - (*this)[0] * (*this)[10] * (*this)[13] - (*this)[8] * (*this)[1] * (*this)[14] + (*this)[8] * (*this)[2] * (*this)[13] + (*this)[12] * (*this)[1] * (*this)[10] - (*this)[12] * (*this)[2] * (*this)[9];
    inv[2] = (*this)[1] * (*this)[6] * (*this)[15] - (*this)[1] * (*this)[7] * (*this)[14] - (*this)[5] * (*this)[2] * (*this)[15] + (*this)[5] * (*this)[3] * (*this)[14] + (*this)[13] * (*this)[2] * (*this)[7] - (*this)[13] * (*this)[3] * (*this)[6];
    inv[6] = -(*this)[0] * (*this)[6] * (*this)[15] + (*this)[0] * (*this)[7] * (*this)[14] + (*this)[4] * (*this)[2] * (*this)[15] - (*this)[4] * (*this)[3] * (*this)[14] - (*this)[12] * (*this)[2] * (*this)[7] + (*this)[12] * (*this)[3] * (*this)[6];
    inv[10] = (*this)[0] * (*this)[5] * (*this)[15] - (*this)[0] * (*this)[7] * (*this)[13] - (*this)[4] * (*this)[1] * (*this)[15] + (*this)[4] * (*this)[3] * (*this)[13] + (*this)[12] * (*this)[1] * (*this)[7] - (*this)[12] * (*this)[3] * (*this)[5];
    inv[14] = -(*this)[0] * (*this)[5] * (*this)[14] + (*this)[0] * (*this)[6] * (*this)[13] + (*this)[4] * (*this)[1] * (*this)[14] - (*this)[4] * (*this)[2] * (*this)[13] - (*this)[12] * (*this)[1] * (*this)[6] + (*this)[12] * (*this)[2] * (*this)[5];
    inv[3] = -(*this)[1] * (*this)[6] * (*this)[11] + (*this)[1] * (*this)[7] * (*this)[10] + (*this)[5] * (*this)[2] * (*this)[11] - (*this)[5] * (*this)[3] * (*this)[10] - (*this)[9] * (*this)[2] * (*this)[7] + (*this)[9] * (*this)[3] * (*this)[6];
    inv[7] = (*this)[0] * (*this)[6] * (*this)[11] - (*this)[0] * (*this)[7] * (*this)[10] - (*this)[4] * (*this)[2] * (*this)[11] + (*this)[4] * (*this)[3] * (*this)[10] + (*this)[8] * (*this)[2] * (*this)[7] - (*this)[8] * (*this)[3] * (*this)[6];
    inv[11] = -(*this)[0] * (*this)[5] * (*this)[11] + (*this)[0] * (*this)[7] * (*this)[9] + (*this)[4] * (*this)[1] * (*this)[11] - (*this)[4] * (*this)[3] * (*this)[9] - (*this)[8] * (*this)[1] * (*this)[7] + (*this)[8] * (*this)[3] * (*this)[5];
    inv[15] = (*this)[0] * (*this)[5] * (*this)[10] - (*this)[0] * (*this)[6] * (*this)[9] - (*this)[4] * (*this)[1] * (*this)[10] + (*this)[4] * (*this)[2] * (*this)[9] + (*this)[8] * (*this)[1] * (*this)[6] - (*this)[8] * (*this)[2] * (*this)[5];
    
    det = (*this)[0] * inv[0] + (*this)[1] * inv[4] + (*this)[2] * inv[8] + (*this)[3] * inv[12];
    
    if (det == 0)
        return false;
    
    det = 1.0 / det;
    
    for (i = 0; i < 16; i++) {
        inv[i] = inv[i] * det;
    }
    
    matrix = GLKMatrix4MakeWithArray(inv);

    return true;
}

void Mat4::transpose()
{
    matrix = GLKMatrix4Transpose(matrix);
}

float* Mat4::pointer()
{
    return matrix.m;
}

void Mat4::buildCameraLookAtMatrixLH(Vector3 position, Vector3 target, Vector3 upVector)
{
    matrix = GLKMatrix4MakeLookAt(position.x, position.y, position.z, target.x, target.y, target.z, upVector.x, upVector.y, upVector.z);
}

void Mat4::buildProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
{
    matrix = GLKMatrix4MakePerspective(fieldOfViewRadians, aspectRatio, zNear, zFar);
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

#endif
