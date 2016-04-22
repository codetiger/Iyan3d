//
//  Vector3.h
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__Vector3__
#define __SGEngine2__Vector3__

#include <iostream>
#include <math.h>

class Vector3 {
public:
    float x, y, z;

    Vector3();
    Vector3(float value);
    Vector3(float X, float Y, float Z);
    ~Vector3();

    Vector3& operator=(const Vector3& b);
    Vector3 operator+(const Vector3& b) const;
    Vector3 operator-(const Vector3& b) const;
    Vector3 operator/(const Vector3& other) const { return Vector3(x / other.x, y / other.y, z / other.z); }
    Vector3 operator*(const Vector3 v) const { return Vector3(x * v.x, y * v.y, z * v.z); }
    Vector3& operator*=(const Vector3 v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    Vector3& operator/=(const Vector3& other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        return *this;
    }
    Vector3 operator+() const;
    Vector3 operator-() const;
    Vector3 operator*(const float v) const;
    Vector3 operator/(const float v) const;

    Vector3& operator+=(const Vector3& b);
    Vector3& operator-=(const Vector3& b);
    Vector3& operator*=(const float v);
    Vector3& operator/=(const float v);

    bool operator==(const Vector3& b) const;
    bool operator!=(const Vector3& b) const;

    float& operator[](unsigned i);
    float operator[](unsigned i) const;

    float sqrMagnitude() const;
    float magnitude() const;
    static Vector3 Cross(const Vector3& v1, const Vector3& v2);
    static float Dot(const Vector3& v1, const Vector3& v2);
    Vector3 normalize();
    Vector3 crossProduct(Vector3 p);
    float dotProduct(Vector3 other);
    double getLength();
    double getDistanceFrom(Vector3 other);
    Vector3 rotationToDirection(const Vector3& forwards = Vector3(0, 0, 1));
};

#endif /* defined(__SGEngine2__Vector3__) */
