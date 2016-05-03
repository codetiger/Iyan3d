//
//  Vector4GLK.cpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifdef OPTIMGLKM

#include "Vector4GLK.h"

Vector4::Vector4()
{
    x = y = z = w = 0.0;
}

Vector4::Vector4(float value)
{
    x = y = z = w = value;
}

Vector4::Vector4(Vector3 a, float W)
{
    
    GLKVector4 vect = GLKVector4MakeWithVector3(a.glkVector(), W);
    setValues(vect);
}

Vector4::Vector4(float X, float Y, float Z, float W)
{
    x = X;
    y = Y;
    z = Z;
    w = W;
}

Vector4::Vector4(GLKVector4 vect)
{
    x = vect.x;
    y = vect.y;
    z = vect.z;
    w = vect.w;
}

GLKVector4 Vector4::glkVector() const
{
    return GLKVector4Make(x, y, z, w);
}


Vector4::~Vector4()
{
}

void Vector4::setValues(GLKVector4 vect)
{
    x = vect.x;
    y = vect.y;
    z = vect.z;
    w = vect.w;
}

Vector4& Vector4::operator=(const Vector4& b)
{
    x = b.x;
    y = b.y;
    z = b.z;
    w = b.w;
    return *this;
}

Vector4 Vector4::operator+(const Vector4& b) const
{
    GLKVector4 vect = GLKVector4Add(glkVector(), b.glkVector());
    return Vector4(vect);
}

Vector4 Vector4::operator-(const Vector4& b) const
{
    GLKVector4 vect = GLKVector4Subtract(glkVector(), b.glkVector());
    return Vector4(vect);
}

Vector4 Vector4::operator+() const
{
    return *this;
}

Vector4 Vector4::operator-() const
{
    return Vector4(-x, -y, -z, -w);
}

Vector4 Vector4::operator*(const float v) const
{
    GLKVector4 vect = GLKVector4MultiplyScalar(glkVector(), v);
    return Vector4(vect);
}

Vector4 Vector4::operator/(const float v) const
{
    GLKVector4 vect = GLKVector4DivideScalar(glkVector(), v);
    return Vector4(vect);
}

Vector4& Vector4::operator+=(const Vector4& b)
{
    GLKVector4 vect = GLKVector4Add(glkVector(), b.glkVector());
    setValues(vect);
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& b)
{
    GLKVector4 vect = GLKVector4Subtract(glkVector(), b.glkVector());
    setValues(vect);
    return *this;
}

Vector4& Vector4::operator*=(const float v)
{
    GLKVector4 vect = GLKVector4MultiplyScalar(glkVector(), v);
    setValues(vect);
    return *this;
}

Vector4& Vector4::operator/=(const float v)
{
    GLKVector4 vect = GLKVector4DivideScalar(glkVector(), v);
    setValues(vect);
    return *this;
}

bool Vector4::operator==(const Vector4& b) const
{
    return x == b.x && y == b.y && z == b.z && w == b.w;
}

bool Vector4::operator!=(const Vector4& b) const
{
    return x != b.x || y != b.y || z != b.z || w != b.w;
}

float& Vector4::operator[](unsigned i)
{
    switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
        default:
            return w;
    }
}

float Vector4::operator[](unsigned i) const
{
    switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
        default:
            return w;
    }
}

Vector4 Vector4::normalize()
{
    GLKVector4 vect = GLKVector4Normalize(glkVector());
    setValues(vect);
    return *this;
}

#endif