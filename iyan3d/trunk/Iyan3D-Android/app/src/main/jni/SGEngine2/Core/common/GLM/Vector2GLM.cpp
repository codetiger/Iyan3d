//
//  Vector2GLM.cpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifdef OPTIMGLM

#include "math.h"
#include "Vector2GLM.h"


Vector2::Vector2()
{
    x = y = 0.0;
}

Vector2::Vector2(float X, float Y)
{
    x = X;
    y = Y;
}

Vector2::~Vector2()
{
}

Vector2& Vector2::operator=(const Vector2& b)
{
    x = b.x;
    y = b.y;
    return *this;
}



Vector2 Vector2::operator+(const Vector2& b) const
{
    glm::vec2 vect = glm::vec2(x, y) + glm::vec2(b.x, b.y);
    return Vector2(vect.x, vect.y);
}

Vector2 Vector2::operator-(const Vector2& b) const
{
    glm::vec2 vect = glm::vec2(x, y) - glm::vec2(b.x, b.y);
    return Vector2(vect.x, vect.y);
}

Vector2 Vector2::operator+() const
{
    return *this;
}

Vector2 Vector2::operator-() const
{
    return Vector2(-x, -y);
}

Vector2 Vector2::operator*(const float v) const
{
    glm::vec2 v2 = glm::vec2(x, y) * v;
    return Vector2(v2.x, v2.y);
}

Vector2 Vector2::operator/(const float v) const
{
    glm::vec2 v2 = glm::vec2(x, y) / v;
    return Vector2(v2.x, v2.y);
}

Vector2& Vector2::operator+=(const Vector2& b)
{
    glm::vec2 vect = glm::vec2(x, y) + glm::vec2(b.x, b.y);
    x = vect.x;
    y = vect.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& b)
{
    glm::vec2 vect = glm::vec2(x, y) - glm::vec2(b.x, b.y);
    x = vect.x;
    y = vect.y;
    return *this;
}

Vector2& Vector2::operator*=(const float v)
{
    glm::vec2 vect = glm::vec2(x, y) * v;
    x = vect.x;
    y = vect.y;
    return *this;
}

Vector2& Vector2::operator/=(const float v)
{
    glm::vec2 vect = glm::vec2(x, y) / v;
    x = vect.x;
    y = vect.y;
    return *this;
}

bool Vector2::operator==(const Vector2& b) const
{
    return x == b.x && y == b.y;
}

bool Vector2::operator!=(const Vector2& b) const
{
    return x != b.x || y != b.y;
}

float& Vector2::operator[](unsigned i)
{
    switch (i) {
        case 0:
            return x;
        case 1:
        default:
            return y;
    }
}

float Vector2::operator[](unsigned i) const
{
    switch (i) {
        case 0:
            return x;
        case 1:
        default:
            return y;
    }
}

Vector2 Vector2::normalize()
{
    glm::vec2 vect = glm::normalize(glm::vec2(x, y));
    x = vect.x;
    y = vect.y;
    return *this;
}

Vector2 Vector2::crossProduct(Vector2 p)
{
    return Vector2(y * p.x - x * p.y, x * p.y - y * p.x);
}

float Vector2::dotProduct(Vector2 other)
{
    return glm::dot(glm::vec2(x, y), glm::vec2(other.x, other.y));
}

float Vector2::getLength()
{
    return glm::length(glm::vec2(x, y));
}
float Vector2::getDistanceFrom(Vector2 other)
{
    return ((*this) - other).getLength();
}

#endif
