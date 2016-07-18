//
//  Vector4GLK.hpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef Vector4GLK_hpp
#define Vector4GLK_hpp

#include <GLKit/GLKVector4.h>
#include "Vector3.h"

class Vector4 {
public:
    float x, y, z, w;
    
    Vector4();
    Vector4(float value);
    Vector4(Vector3 a, float W);
    Vector4(float X, float Y, float Z, float W);
    Vector4(GLKVector4 vect);
    ~Vector4();
    void setValues(GLKVector4 vect);
    
    GLKVector4 glkVector() const;
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

#endif