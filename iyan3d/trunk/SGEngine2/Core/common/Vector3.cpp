//
//  Vector3GLK.cpp
//  Iyan3D
//
//  Created by Karthik on 26/04/16.
//  Copyright © 2016 Smackall Games. All rights reserved.
//

#include "Vector3.h"

Vector3::Vector3()
{
    x = y = z = 0.0;
}

Vector3::Vector3(float value)
{
    x = y = z =  value;
}

Vector3::Vector3(float X, float Y, float Z)
{
    x = X;
    y = Y;
    z = Z;
}

Vector3::Vector3(GLKVector3 vect)
{
    x = vect.x;
    y = vect.y;
    z = vect.z;
}

Vector3::~Vector3()
{
}

void Vector3::setValues(GLKVector3 vect)
{
    x = vect.x;
    y = vect.y;
    z = vect.z;
}

GLKVector3 Vector3::glkVector() const
{
    return GLKVector3Make(x, y, z);
}

Vector3& Vector3::operator=(const Vector3& b)
{
    x = b.x;
    y = b.y;
    z = b.z;
    return *this;
}

Vector3 Vector3::operator+(const Vector3& b) const
{
    GLKVector3 vect = GLKVector3Add(glkVector(), b.glkVector());
    return Vector3(vect);
}

Vector3 Vector3::operator-(const Vector3& b) const
{
    GLKVector3 vect = GLKVector3Subtract(glkVector(), b.glkVector());
    return Vector3(vect);
}

Vector3 Vector3::operator/(const Vector3& other) const
{
    GLKVector3 vect = GLKVector3Divide(glkVector(), other.glkVector());
    return Vector3(vect);
}

Vector3 Vector3::operator*(const Vector3 v) const
{
    GLKVector3 vect = GLKVector3Multiply(glkVector(), v.glkVector());
    return Vector3(vect);
}


Vector3 Vector3::operator+() const
{
    return *this;
}

Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

Vector3 Vector3::operator*(const float v) const
{
    GLKVector3 vect = GLKVector3MultiplyScalar(glkVector(), v);
    return Vector3(vect);
}

Vector3 Vector3::operator/(const float v) const
{
    GLKVector3 vect = GLKVector3DivideScalar(glkVector(), v);
    return Vector3(vect);
}

Vector3& Vector3::operator+=(const Vector3& b)
{
    GLKVector3 vect = GLKVector3Add(glkVector(), b.glkVector());
    setValues(vect);
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& b)
{
    GLKVector3 vect = GLKVector3Subtract(glkVector(), b.glkVector());
    setValues(vect);
    return *this;
}

Vector3& Vector3::operator*=(const float v)
{
    GLKVector3 vect = GLKVector3MultiplyScalar(glkVector(), v);
    setValues(vect);
    return *this;
}

Vector3& Vector3::operator/=(const float v)
{
    GLKVector3 vect = GLKVector3DivideScalar(glkVector(), v);
    setValues(vect);
    return *this;
}

bool Vector3::operator==(const Vector3& b) const
{
    return x == b.x && y == b.y && z == b.z;
}

bool Vector3::operator!=(const Vector3& b) const
{
    return x != b.x || y != b.y || z != b.z;
}

Vector3 Vector3::normalize()
{
    float length = x * x + y * y + z * z;
    if (length == 0)
        return *this;
    length = 1.0 / sqrt(length);
    
    x = (x * length);
    y = (y * length);
    z = (z * length);
    return Vector3(x, y, z);
}

Vector3 Vector3::crossProduct(Vector3 p)
{
    return Vector3(y * p.z - z * p.y, z * p.x - x * p.z, x * p.y - y * p.x);
}

float Vector3::dotProduct(Vector3 other)
{
    return GLKVector3DotProduct(glkVector(), other.glkVector());
}

double Vector3::getLength()
{
    return sqrtf(x * x + y * y + z * z);
}
double Vector3::getDistanceFrom(Vector3 other)
{
    return ((*this) - other).getLength();
}

Vector3 Vector3::rotationToDirection(const Vector3& forwards)
{
    double DEGTORAD64 = M_PI / 180.0f;
    const double cr = cos(DEGTORAD64 * x);
    const double sr = sin(DEGTORAD64 * x);
    const double cp = cos(DEGTORAD64 * y);
    const double sp = sin(DEGTORAD64 * y);
    const double cy = cos(DEGTORAD64 * z);
    const double sy = sin(DEGTORAD64 * z);
    
    const double srsp = sr * sp;
    const double crsp = cr * sp;
    
    const double pseudoMatrix[] = {
        (cp * cy), (cp * sy), (-sp),
        (srsp * cy - cr * sy), (srsp * sy + cr * cy), (sr * cp),
        (crsp * cy + sr * sy), (crsp * sy - sr * cy), (cr * cp)
    };
    
    return Vector3((forwards.x * pseudoMatrix[0] + forwards.y * pseudoMatrix[3] + forwards.z * pseudoMatrix[6]),
                   (forwards.x * pseudoMatrix[1] + forwards.y * pseudoMatrix[4] + forwards.z * pseudoMatrix[7]),
                   (forwards.x * pseudoMatrix[2] + forwards.y * pseudoMatrix[5] + forwards.z * pseudoMatrix[8]));
}

