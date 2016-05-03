//
//  Vector2GLK.cpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#ifdef OPTIMGLKM

#include "math.h"
#include "Vector2GLK.h"


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
    GLKVector2 vect = GLKVector2Add(GLKVector2Make(x, y), GLKVector2Make(b.x, b.y));
    return Vector2(vect.x, vect.y);
}

Vector2 Vector2::operator-(const Vector2& b) const
{
    GLKVector2 vect = GLKVector2Subtract(GLKVector2Make(x, y), GLKVector2Make(b.x, b.y));
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
    GLKVector2 v2 = GLKVector2MultiplyScalar(GLKVector2Make(x, y), v);
    return Vector2(v2.x, v2.y);
}

Vector2 Vector2::operator/(const float v) const
{
    GLKVector2 v2 = GLKVector2DivideScalar(GLKVector2Make(x, y), v);
    return Vector2(v2.x, v2.y);
}

Vector2& Vector2::operator+=(const Vector2& b)
{
    GLKVector2 vect = GLKVector2Add(GLKVector2Make(x, y), GLKVector2Make(b.x, b.y));
    x = vect.x;
    y = vect.y;
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& b)
{
    GLKVector2 vect = GLKVector2Subtract(GLKVector2Make(x, y), GLKVector2Make(b.x, b.y));
    x = vect.x;
    y = vect.y;
    return *this;
}

Vector2& Vector2::operator*=(const float v)
{
    GLKVector2 vect = GLKVector2MultiplyScalar(GLKVector2Make(x, y), v);
    x = vect.x;
    y = vect.y;
    return *this;
}

Vector2& Vector2::operator/=(const float v)
{
    GLKVector2 vect = GLKVector2DivideScalar(GLKVector2Make(x, y), v);
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
    GLKVector2 vect = GLKVector2Normalize(GLKVector2Make(x, y));
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
    return GLKVector2DotProduct(GLKVector2Make(x, y), GLKVector2Make(other.x, other.y));
}

float Vector2::getLength()
{
    return GLKVector2Length(GLKVector2Make(x, y));
}
float Vector2::getDistanceFrom(Vector2 other)
{
    return ((*this) - other).getLength();
}

#endif
