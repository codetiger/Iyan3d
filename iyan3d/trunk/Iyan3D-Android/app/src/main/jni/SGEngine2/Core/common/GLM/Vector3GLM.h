//
//  Vector3GLM.hpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifdef OPTIMGLM

#ifndef Vector3GLM_hpp
#define Vector3GLM_hpp

#include <glm/glm.hpp>

#include <iostream>
#include <math.h>

class Vector3 {
public:
    float x, y, z;
        
    Vector3();
    Vector3(float value);
    Vector3(float X, float Y, float Z);
    Vector3(glm::vec3 vect);
    ~Vector3();
    
    void setValues(glm::vec3 vect);
    glm::vec3 glmVector() const;
    Vector3& operator=(const Vector3& b);
    Vector3 operator+(const Vector3& b) const;
    Vector3 operator-(const Vector3& b) const;
    Vector3 operator/(const Vector3& other) const;
    Vector3 operator*(const Vector3 v) const;
    Vector3& operator*=(const Vector3 v);
    Vector3& operator/=(const Vector3& other);
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
    
    Vector3 normalize();
    Vector3 crossProduct(Vector3 p);
    float dotProduct(Vector3 other);
    double getLength();
    double getDistanceFrom(Vector3 other);
    Vector3 rotationToDirection(const Vector3& forwards = Vector3(0, 0, 1));
};

#endif
#endif /* Vector3GLM_hpp */
