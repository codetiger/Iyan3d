//
//  Vector3GLK.hpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef Vector3GLK_hpp
#define Vector3GLK_hpp

#include "GLKVector3.h"
#include <iostream>
#include <math.h>

class Vector3 {
public:
    float x, y, z;

    Vector3();
    Vector3(float value);
    Vector3(float X, float Y, float Z);
    Vector3(GLKVector3 vect);
    ~Vector3();

    void       setValues(GLKVector3 vect);
    GLKVector3 glkVector() const;
    Vector3&   operator=(const Vector3& b);
    Vector3    operator+(const Vector3& b) const;
    Vector3    operator-(const Vector3& b) const;
    Vector3    operator/(const Vector3& other) const;
    Vector3    operator*(const Vector3 v) const;
    Vector3&   operator*=(const Vector3 v);
    Vector3&   operator/=(const Vector3& other);
    Vector3    operator+() const;
    Vector3    operator-() const;
    Vector3    operator*(const float v) const;
    Vector3    operator/(const float v) const;

    Vector3& operator+=(const Vector3& b);
    Vector3& operator-=(const Vector3& b);
    Vector3& operator*=(const float v);
    Vector3& operator/=(const float v);

    bool operator==(const Vector3& b) const;
    bool operator!=(const Vector3& b) const;

    Vector3 normalize();
    Vector3 crossProduct(Vector3 p);
    float   dotProduct(Vector3 other);
    double  getLength();
    double  getDistanceFrom(Vector3 other);
    Vector3 rotationToDirection(const Vector3& forwards = Vector3(0, 0, 1));
};

#endif
