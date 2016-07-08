
#ifdef OPTIMSGM

#include "Quaternion.h"

Quaternion::Quaternion(float x, float y, float z)
{
    set(x, y, z);
}
Quaternion::Quaternion(const Vector3& vec)
{
    set(vec.x, vec.y, vec.z);
}
Quaternion::Quaternion(const Mat4& mat)
{
    (*this) = mat;
}
Quaternion& Quaternion::operator=(const Quaternion& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
    return *this;
}
Quaternion& Quaternion::operator=(const Mat4& m)
{
    const float diag = m[0] + m[5] + m[10] + 1;
    
    if (diag > 0.0f) {
        const float scale = sqrtf(diag) * 2.0f; // get scale from diagonal
        
        // TODO: speed this up
        x = (m[6] - m[9]) / scale;
        y = (m[8] - m[2]) / scale;
        z = (m[1] - m[4]) / scale;
        w = 0.25f * scale;
    }
    else {
        if (m[0] > m[5] && m[0] > m[10]) {
            // 1st element of diag is greatest value
            // find scale according to 1st element, and double it
            const float scale = sqrtf(1.0f + m[0] - m[5] - m[10]) * 2.0f;
            
            // TODO: speed this up
            x = 0.25f * scale;
            y = (m[4] + m[1]) / scale;
            z = (m[2] + m[8]) / scale;
            w = (m[6] - m[9]) / scale;
        }
        else if (m[5] > m[10]) {
            // 2nd element of diag is greatest value
            // find scale according to 2nd element, and double it
            const float scale = sqrtf(1.0f + m[5] - m[0] - m[10]) * 2.0f;
            
            // TODO: speed this up
            x = (m[4] + m[1]) / scale;
            y = 0.25f * scale;
            z = (m[9] + m[6]) / scale;
            w = (m[8] - m[2]) / scale;
        }
        else {
            // 3rd element of diag is greatest value
            // find scale according to 3rd element, and double it
            const float scale = sqrtf(1.0f + m[10] - m[0] - m[5]) * 2.0f;
            
            // TODO: speed this up
            x = (m[8] + m[2]) / scale;
            y = (m[9] + m[6]) / scale;
            z = 0.25f * scale;
            w = (m[1] - m[4]) / scale;
        }
    }
    
    return normalize();
}
Quaternion Quaternion::operator*(const Quaternion& other) const
{
    Quaternion tmp;
    
    tmp.w = (other.w * w) - (other.x * x) - (other.y * y) - (other.z * z);
    tmp.x = (other.w * x) + (other.x * w) + (other.y * z) - (other.z * y);
    tmp.y = (other.w * y) + (other.y * w) + (other.z * x) - (other.x * z);
    tmp.z = (other.w * z) + (other.z * w) + (other.x * y) - (other.y * x);
    
    return tmp;
}
Quaternion Quaternion::operator*(float s) const
{
    return Quaternion(s * x, s * y, s * z, s * w);
}
Quaternion& Quaternion::operator*=(float s)
{
    x *= s;
    y *= s;
    z *= s;
    w *= s;
    return *this;
}
Quaternion& Quaternion::operator*=(const Quaternion& other)
{
    return (*this = other * (*this));
}
Quaternion Quaternion::operator+(const Quaternion& b) const
{
    return Quaternion(x + b.x, y + b.y, z + b.z, w + b.w);
}
Mat4 Quaternion::getMatrix() const
{
    Mat4 m;
    getMatrix(m);
    return m;
}
void Quaternion::getMatrix(Mat4& dest,
                                  const Vector3& center) const
{
    dest[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
    dest[1] = 2.0f * x * y + 2.0f * z * w;
    dest[2] = 2.0f * x * z - 2.0f * y * w;
    dest[3] = 0.0f;
    
    dest[4] = 2.0f * x * y - 2.0f * z * w;
    dest[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
    dest[6] = 2.0f * z * y + 2.0f * x * w;
    dest[7] = 0.0f;
    
    dest[8] = 2.0f * x * z + 2.0f * y * w;
    dest[9] = 2.0f * z * y - 2.0f * x * w;
    dest[10] = 1.0f - 2.0f * x * x - 2.0f * y * y;
    dest[11] = 0.0f;
    
    dest[12] = center.x;
    dest[13] = center.y;
    dest[14] = center.z;
    dest[15] = 1.f;
}
void Quaternion::getMatrixCenter(Mat4& dest,
                                        const Vector3& center,
                                        const Vector3& translation) const
{
}
void Quaternion::getMatrix_transposed(Mat4& dest) const
{
    dest[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
    dest[4] = 2.0f * x * y + 2.0f * z * w;
    dest[8] = 2.0f * x * z - 2.0f * y * w;
    dest[12] = 0.0f;
    
    dest[1] = 2.0f * x * y - 2.0f * z * w;
    dest[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
    dest[9] = 2.0f * z * y + 2.0f * x * w;
    dest[13] = 0.0f;
    
    dest[2] = 2.0f * x * z + 2.0f * y * w;
    dest[6] = 2.0f * z * y - 2.0f * x * w;
    dest[10] = 1.0f - 2.0f * x * x - 2.0f * y * y;
    dest[14] = 0.0f;
    
    dest[3] = 0.f;
    dest[7] = 0.f;
    dest[11] = 0.f;
    dest[15] = 1.f;
}
Quaternion& Quaternion::makeInverse()
{
    x = -x;
    y = -y;
    z = -z;
    return *this;
}
Quaternion& Quaternion::set(float x, float y, float z, float w)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->w = w;
    return *this;
}
Quaternion& Quaternion::set(float X, float Y, float Z)
{
    double angle;
    
    angle = X * 0.5;
    const double sr = sin(angle);
    const double cr = cos(angle);
    
    angle = Y * 0.5;
    const double sp = sin(angle);
    const double cp = cos(angle);
    
    angle = Z * 0.5;
    const double sy = sin(angle);
    const double cy = cos(angle);
    
    const double cpcy = cp * cy;
    const double spcy = sp * cy;
    const double cpsy = cp * sy;
    const double spsy = sp * sy;
    
    x = (float)(sr * cpcy - cr * spsy);
    y = (float)(cr * spcy + sr * cpsy);
    z = (float)(cr * cpsy - sr * spcy);
    w = (float)(cr * cpcy + sr * spsy);
    
    return normalize();
}
Quaternion& Quaternion::set(const Vector3& vec)
{
    return set(vec.x, vec.y, vec.z);
}
Quaternion& Quaternion::set(const Quaternion& quat)
{
    return (*this = quat);
}
bool Quaternion::equals1(float a, float b, float tolerance)
{
    return (a + tolerance >= b) && (a - tolerance <= b);
}
bool Quaternion::equals(const Quaternion& other, const float tolerance) const
{
    return ((x + tolerance >= other.x) && (x - tolerance <= other.x)) && ((y + tolerance >= other.y) && (y - tolerance <= other.y)) && ((z + tolerance >= other.z) && (z - tolerance <= other.z)) && ((w + tolerance >= other.w) && (w - tolerance <= other.w));
}
Quaternion& Quaternion::normalize()
{
    const float n = x * x + y * y + z * z + w * w;
    
    if (n == 1)
        return *this;
    
    //n = 1.0f / sqrtf(n);
    return (*this *= (1.0 / sqrtf(n)));
}
float Quaternion::dotProduct(const Quaternion& q2) const
{
    return (x * q2.x) + (y * q2.y) + (z * q2.z) + (w * q2.w);
}
Quaternion& Quaternion::fromAngleAxis(float angle, const Vector3& axis)
{
    
    const float fHalfAngle = 0.5f * angle;
    const float fSin = sinf(fHalfAngle);
    w = cosf(fHalfAngle);
    x = fSin * axis.x;
    y = fSin * axis.y;
    z = fSin * axis.z;
    return *this;
}
void Quaternion::toAngleAxis(float& angle, Vector3& axis) const
{
    const float scale = sqrtf(x * x + y * y + z * z);
    
    if (scale == 0.0 || w > 1.0f || w < -1.0f) {
        angle = 0.0f;
        axis.x = 0.0f;
        axis.y = 1.0f;
        axis.z = 0.0f;
    }
    else {
        const float invscale = 1.0 / (scale);
        angle = 2.0f * acosf(w);
        axis.x = x * invscale;
        axis.y = y * invscale;
        axis.z = z * invscale;
    }
}
void Quaternion::toEuler(Vector3& euler) const
{
    const double sqw = w * w;
    const double sqx = x * x;
    const double sqy = y * y;
    const double sqz = z * z;
    const double test = 2.0 * (y * w - x * z);
    double tolerance = 0.000001;
    if (((test + tolerance >= 1.0) && (test - tolerance <= 1.0))) {
        // heading = rotation about z-axis
        euler.z = (float)(-2.0 * atan2(x, w));
        // bank = rotation about x-axis
        euler.x = 0;
        // attitude = rotation about y-axis
        euler.y = (float)(PI64 / 2.0);
    }
    else if (((test + tolerance >= -1.0) && (test - tolerance <= -1.0))) {
        // heading = rotation about z-axis
        euler.z = (float)(2.0 * atan2(x, w));
        // bank = rotation about x-axis
        euler.x = 0;
        // attitude = rotation about y-axis
        euler.y = (float)(PI64 / -2.0);
    }
    else {
        // heading = rotation about z-axis
        euler.z = (float)atan2(2.0 * (x * y + z * w), (sqx - sqy - sqz + sqw));
        // bank = rotation about x-axis
        euler.x = (float)atan2(2.0 * (y * z + x * w), (-sqx - sqy + sqz + sqw));
        // attitude = rotation about y-axis
        euler.y = (float)asin(fmin(fmax(test, -1.0), 1.0));
    }
}
Vector3 Quaternion::operator*(const Vector3& v) const
{
    // nVidia SDK implementation
    
    Vector3 uv, uuv;
    Vector3 qvec(x, y, z);
    uv = qvec.crossProduct(v);
    uuv = qvec.crossProduct(uv);
    uv *= (2.0f * w);
    uuv *= 2.0f;
    
    return v + uv + uuv;
}
Quaternion& Quaternion::makeIdentity()
{
    w = 1.f;
    x = 0.f;
    y = 0.f;
    z = 0.f;
    return *this;
}

#endif
