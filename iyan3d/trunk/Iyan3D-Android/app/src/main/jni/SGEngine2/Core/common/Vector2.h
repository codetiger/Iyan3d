//
//  Vector2GLK.hpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifndef Vector2GLK_hpp
#define Vector2GLK_hpp

#include "GLKVector2.h"


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
    
    Vector2 normalize();
    Vector2 crossProduct(Vector2 p);
    float dotProduct(Vector2 other);
    float getLength();
    float getDistanceFrom(Vector2 other);
};

#endif
