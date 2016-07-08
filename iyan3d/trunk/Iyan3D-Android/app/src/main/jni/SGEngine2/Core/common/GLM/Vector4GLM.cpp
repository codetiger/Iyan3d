//
//  Vector4GLK.cpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifdef OPTIMGLM

#include "Vector4GLM.h"

Vector4::Vector4()
{
    x = y = z = w = 0.0;
}

Vector4::Vector4(float value)
{
    x = y = z = w = value;
}

Vector4::Vector4(Vector3 a, float W)
{
    
    glm::vec4 vect = glm::vec4(a.glmVector(), W);
    setValues(vect);
}

Vector4::Vector4(float X, float Y, float Z, float W)
{
    x = X;
    y = Y;
    z = Z;
    w = W;
}

Vector4::Vector4(glm::vec4 vect)
{
    x = vect.x;
    y = vect.y;
    z = vect.z;
    w = vect.w;
}

glm::vec4 Vector4::glmVector() const
{
    return glm::vec4(x, y, z, w);
}


Vector4::~Vector4()
{
}

void Vector4::setValues(glm::vec4 vect)
{
    x = vect.x;
    y = vect.y;
    z = vect.z;
    w = vect.w;
}

Vector4& Vector4::operator=(const Vector4& b)
{
    x = b.x;
    y = b.y;
    z = b.z;
    w = b.w;
    return *this;
}

Vector4 Vector4::operator+(const Vector4& b) const
{
    glm::vec4 vect = glmVector() + b.glmVector();
    return Vector4(vect);
}

Vector4 Vector4::operator-(const Vector4& b) const
{
    glm::vec4 vect = glmVector() - b.glmVector();
    return Vector4(vect);
}

Vector4 Vector4::operator+() const
{
    return *this;
}

Vector4 Vector4::operator-() const
{
    return Vector4(-x, -y, -z, -w);
}

Vector4 Vector4::operator*(const float v) const
{
    glm::vec4 vect = glmVector() * v;
    return Vector4(vect);
}

Vector4 Vector4::operator/(const float v) const
{
    glm::vec4 vect = glmVector() / v;
    return Vector4(vect);
}

Vector4& Vector4::operator+=(const Vector4& b)
{
    glm::vec4 vect = glmVector() + b.glmVector();
    setValues(vect);
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& b)
{
    glm::vec4 vect = glmVector() - b.glmVector();
    setValues(vect);
    return *this;
}

Vector4& Vector4::operator*=(const float v)
{
    glm::vec4 vect = glmVector() * v;
    setValues(vect);
    return *this;
}

Vector4& Vector4::operator/=(const float v)
{
    glm::vec4 vect = glmVector() / v;
    setValues(vect);
    return *this;
}

bool Vector4::operator==(const Vector4& b) const
{
    return x == b.x && y == b.y && z == b.z && w == b.w;
}

bool Vector4::operator!=(const Vector4& b) const
{
    return x != b.x || y != b.y || z != b.z || w != b.w;
}

float& Vector4::operator[](unsigned i)
{
    switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
        default:
            return w;
    }
}

float Vector4::operator[](unsigned i) const
{
    switch (i) {
        case 0:
            return x;
        case 1:
            return y;
        case 2:
            return z;
        case 3:
        default:
            return w;
    }
}

Vector4 Vector4::normalize()
{
    glm::vec4 vect = glm::normalize(glmVector());
    setValues(vect);
    return *this;
}

#endif