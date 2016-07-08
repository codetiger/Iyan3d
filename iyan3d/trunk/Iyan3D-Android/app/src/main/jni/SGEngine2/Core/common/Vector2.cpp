//
//  Vector2.cpp
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

#ifdef OPTIMSGM

#include "Vector2.h"

Vector2::Vector2()
{
    x = 0.0;
    y = 0.0;
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
    return Vector2(x + b.x, y + b.y);
}

Vector2 Vector2::operator-(const Vector2& b) const
{
    return Vector2(x - b.x, y - b.y);
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
    return Vector2(x * v, y * v);
}

Vector2 Vector2::operator/(const float v) const
{
    float inv_v = 1.0f / v;
    return Vector2(x * inv_v, y * inv_v);
}

Vector2& Vector2::operator+=(const Vector2& b)
{
    x += b.x;
    y += b.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& b)
{
    x -= b.x;
    y -= b.y;
    return *this;
}

Vector2& Vector2::operator*=(const float v)
{
    x *= v;
    y *= v;
    return *this;
}

Vector2& Vector2::operator/=(const float v)
{
    float inv_v = 1.0f / v;
    x *= inv_v;
    y *= inv_v;
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
    float length = x * x + y * y;
    length = 1.0 / sqrt(length);

    x = (x * length);
    y = (y * length);
    return Vector2(x, y);
}
Vector2 Vector2::crossProduct(Vector2 p)
{
    return Vector2(y * p.x - x * p.y, x * p.y - y * p.x);
}

float Vector2::dotProduct(Vector2 other)
{
    return x * other.x + y * other.y;
}

float Vector2::getLength()
{
    return sqrtf(x * x + y * y);
}
float Vector2::getDistanceFrom(Vector2 other)
{
    return Vector2(x - other.x, y - other.y).getLength();
}

#endif