//
//  Vector4.h
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifndef __SGEngine2__Vector4__
#define __SGEngine2__Vector4__

#include "Vector3.h"

class Vector4 {
public:
    float x, y, z, w;

    Vector4();
    Vector4(float value);
    Vector4(Vector3 a, float W);
    Vector4(float X, float Y, float Z, float W);
    ~Vector4();

    Vector4& operator=(const Vector4& b);
    Vector4 operator+(const Vector4& b) const;
    Vector4 operator-(const Vector4& b) const;
    Vector4 operator+() const;
    Vector4 operator-() const;
    Vector4 operator*(const float v) const;
    Vector4 operator/(const float v) const;

    Vector4& operator+=(const Vector4& b);
    Vector4& operator-=(const Vector4& b);
    Vector4& operator*=(const float v);
    Vector4& operator/=(const float v);

    Vector4 normalize();

    bool operator==(const Vector4& b) const;
    bool operator!=(const Vector4& b) const;

    float& operator[](unsigned i);
    float operator[](unsigned i) const;
};

#endif /* defined(__SGEngine2__Vector4__) */
