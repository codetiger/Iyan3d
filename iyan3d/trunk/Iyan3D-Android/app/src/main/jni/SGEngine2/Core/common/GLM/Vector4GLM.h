//
//  Vector4GLM.hpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifdef OPTIMGLM

#ifndef Vector4GLM_hpp
#define Vector4GLM_hpp

#include <glm/glm.hpp>
#include "Vector3GLM.h"

class Vector4 {
public:
    float x, y, z, w;
    
    Vector4();
    Vector4(float value);
    Vector4(Vector3 a, float W);
    Vector4(float X, float Y, float Z, float W);
    Vector4(glm::vec4 vect);
    ~Vector4();
    void setValues(glm::vec4 vect);
    
    glm::vec4 glmVector() const;
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

#endif /* Vector4GLM_hpp */

#endif