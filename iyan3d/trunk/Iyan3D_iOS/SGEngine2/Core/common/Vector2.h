//
//  Vector2.h
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__Vector2__
#define __SGEngine2__Vector2__

#include "common.h"

class Vector2 {
public:
    float x, y;

    Vector2();
    Vector2(float X, float Y);
    ~Vector2();

    Vector2& operator=(const Vector2& b);
    Vector2 operator+(const Vector2& b) const;
    Vector2 operator-(const Vector2& b) const;
    Vector2 operator+() const;
    Vector2 operator-() const;
    Vector2 operator*(const float v) const;
    Vector2 operator/(const float v) const;

    Vector2& operator+=(const Vector2& b);
    Vector2& operator-=(const Vector2& b);
    Vector2& operator*=(const float v);
    Vector2& operator/=(const float v);

    bool operator==(const Vector2& b) const;
    bool operator!=(const Vector2& b) const;

    float& operator[](unsigned i);
    float operator[](unsigned i) const;

    float sqrMagnitude() const;
    float magnitude() const;
    static float Cross(const Vector2& v1, const Vector2& v2);
    static float Dot(const Vector2& v1, const Vector2& v2);
    Vector2 normalize();
    Vector2 crossProduct(Vector2 p);
    float dotProduct(Vector2 other);
    float getLength();
    float getDistanceFrom(Vector2 other);
};

#endif /* defined(__SGEngine2__Vector2__) */
