//
//  Vector2GLM.hpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifdef OPTIMGLM

#ifndef Vector2GLM_hpp
#define Vector2GLM_hpp

#include <glm/glm.hpp>

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
#endif /* Vector2GLM_hpp */
