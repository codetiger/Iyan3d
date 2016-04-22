//
//  Vector3.cpp
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#include "Vector3.h"
const double DEGTORAD64 = 3.1415926535897932384626433832795028841971693993751 / 180.0;
Vector3::Vector3()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

Vector3::Vector3(float X, float Y, float Z)
{
    x = X;
    y = Y;
    z = Z;
}
Vector3::Vector3(float value)
{
    x = value;
    y = value;
    z = value;
}

Vector3::~Vector3()
{
}

Vector3& Vector3::operator=(const Vector3& b)
{
    x = b.x;
    y = b.y;
    z = b.z;
    return *this;
}

Vector3 Vector3::operator+(const Vector3& b) const
{
    return Vector3(x + b.x, y + b.y, z + b.z);
}

Vector3 Vector3::operator-(const Vector3& b) const
{
    return Vector3(x - b.x, y - b.y, z - b.z);
}

Vector3 Vector3::operator+() const
{
    return *this;
}

Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator*(const float v) const
{
    return Vector3(x * v, y * v, z * v);
}

Vector3 Vector3::operator/(const float v) const
{
    float inv_v = 1.0f / v;
    return Vector3(x * inv_v, y * inv_v, z * inv_v);
}

Vector3& Vector3::operator+=(const Vector3& b)
{
    x += b.x;
    y += b.y;
    z += b.z;
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& b)
{
    x -= b.x;
    y -= b.y;
    z -= b.z;
    return *this;
}

Vector3& Vector3::operator*=(const float v)
{
    x *= v;
    y *= v;
    z *= v;
    return *this;
}

Vector3& Vector3::operator/=(const float v)
{
    float inv_v = 1.0f / v;
    x *= inv_v;
    y *= inv_v;
    z *= inv_v;
    return *this;
}

bool Vector3::operator==(const Vector3& b) const
{
    return x == b.x && y == b.y && z == b.z;
}

bool Vector3::operator!=(const Vector3& b) const
{
    return x != b.x || y != b.y || z != b.z;
}

float& Vector3::operator[](unsigned i)
{
    switch (i) {
    case 0:
        return x;
    case 1:
        return y;
    default:
    case 2:
        return z;
    }
}

float Vector3::operator[](unsigned i) const
{
    switch (i) {
    case 0:
        return x;
    case 1:
        return y;
    case 2:
    default:
        return z;
    }
}

float Vector3::sqrMagnitude() const
{
    return x * x + y * y + z * z;
}

float Vector3::magnitude() const
{
    return sqrtf(x * x + y * y + z * z);
}

Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2)
{
    return Vector3(v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x);
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
Vector3 Vector3::normalize()
{
    float length = x * x + y * y + z * z;
    if (length == 0)
        return *this;
    length = 1.0 / sqrt(length);

    x = (x * length);
    y = (y * length);
    z = (z * length);
    return Vector3(x, y, z);
}
Vector3 Vector3::crossProduct(Vector3 p)
{
    return Vector3(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
}

float Vector3::dotProduct(Vector3 other)
{
    return x * other.x + y * other.y + z * other.z;
}

double Vector3::getLength()
{
    return sqrtf(x * x + y * y + z * z);
}
double Vector3::getDistanceFrom(Vector3 other)
{
    return Vector3(x - other.x, y - other.y, z - other.z).getLength();
}
Vector3 Vector3::rotationToDirection(const Vector3& forwards)
{
    const double cr = cos(DEGTORAD64 * x);
    const double sr = sin(DEGTORAD64 * x);
    const double cp = cos(DEGTORAD64 * y);
    const double sp = sin(DEGTORAD64 * y);
    const double cy = cos(DEGTORAD64 * z);
    const double sy = sin(DEGTORAD64 * z);

    const double srsp = sr * sp;
    const double crsp = cr * sp;

    const double pseudoMatrix[] = {
        (cp * cy), (cp * sy), (-sp),
        (srsp * cy - cr * sy), (srsp * sy + cr * cy), (sr * cp),
        (crsp * cy + sr * sy), (crsp * sy - sr * cy), (cr * cp)
    };

    return Vector3((forwards.x * pseudoMatrix[0] + forwards.y * pseudoMatrix[3] + forwards.z * pseudoMatrix[6]),
        (forwards.x * pseudoMatrix[1] + forwards.y * pseudoMatrix[4] + forwards.z * pseudoMatrix[7]),
        (forwards.x * pseudoMatrix[2] + forwards.y * pseudoMatrix[5] + forwards.z * pseudoMatrix[8]));
}