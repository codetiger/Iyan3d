//
//  Vector4.cpp
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef OPTIMGLKM

#include "Vector4.h"

Vector4::Vector4()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
    w = 0.0;
}

Vector4::Vector4(float value)
{
    x = y = z = w = value;
}

Vector4::Vector4(Vector3 a, float W)
{
    x = a.x;
    y = a.y;
    z = a.z;
    w = W;
}

Vector4::Vector4(float X, float Y, float Z, float W)
{
    x = X;
    y = Y;
    z = Z;
    w = W;
}

Vector4::~Vector4()
{
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
    return Vector4(x + b.x, y + b.y, z + b.z, w + b.w);
}

Vector4 Vector4::operator-(const Vector4& b) const
{
    return Vector4(x - b.x, y - b.y, z - b.z, w - b.w);
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
    return Vector4(x * v, y * v, z * v, w * v);
}

Vector4 Vector4::operator/(const float v) const
{
    float inv_v = 1.0f / v;
    return Vector4(x * inv_v, y * inv_v, z * inv_v, w * inv_v);
}

Vector4& Vector4::operator+=(const Vector4& b)
{
    x += b.x;
    y += b.y;
    z += b.z;
    w += b.w;
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& b)
{
    x -= b.x;
    y -= b.y;
    z -= b.z;
    w -= b.w;
    return *this;
}

Vector4& Vector4::operator*=(const float v)
{
    x *= v;
    y *= v;
    z *= v;
    w *= v;
    return *this;
}

Vector4& Vector4::operator/=(const float v)
{
    float inv_v = 1.0f / v;
    x *= inv_v;
    y *= inv_v;
    z *= inv_v;
    w *= inv_v;
    return *this;
}

Vector4 Vector4::normalize()
{
    float length = x * x + y * y + z * z + w * w;
    if (length == 0)
        return *this;
    length = 1.0 / sqrt(length);

    x = (x * length);
    y = (y * length);
    z = (z * length);
    w = (w * length);
    return Vector4(x, y, z, w);
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
    default:
    case 3:
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
    default:
    case 3:
        return w;
    }
}

#endif
