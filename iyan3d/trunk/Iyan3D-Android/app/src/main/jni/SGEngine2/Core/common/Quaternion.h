

#ifdef OPTIMSGM

#ifndef SGEngine2_Quaternion_h
#define SGEngine2_Quaternion_h

#include "common.h"

const int ROUNDING_ERROR_S32 = 0;
const unsigned int ROUNDING_ERROR_S64 = 0;
const float ROUNDING_ERROR_float = 0.000001f;
const double ROUNDING_ERROR_double = 0.00000001;
const float RECIPROCAL_PI = 1.0f / 3.14159265359f;
const float HALF_PI = 3.14159265359f / 2.0f;
const double PI64 = 3.1415926535897932384626433832795028841971693993751;
const double RECIPROCAL_PI64 = 1.0 / PI64;
const double DEGTORAD64 = 3.1415926535897932384626433832795028841971693993751 / 180.0;
class Quaternion {
public:
    Quaternion()
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
        , w(1.0f)
    {
    }
    Quaternion(float x, float y, float z, float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }
    Quaternion(float x, float y, float z);
    Quaternion(const Vector3& vec);
    Quaternion(const Mat4& mat);
    Quaternion& operator=(const Quaternion& other);
    Quaternion& operator=(const Mat4& other);
    Quaternion operator+(const Quaternion& other) const;
    Quaternion operator*(const Quaternion& other) const;
    Quaternion operator*(float s) const;
    Quaternion& operator*=(float s);
    Vector3 operator*(const Vector3& v) const;
    Quaternion& operator*=(const Quaternion& other);
    float dotProduct(const Quaternion& other) const;
    Quaternion& set(float x, float y, float z, float w);
    Quaternion& set(float x, float y, float z);
    Quaternion& set(const Vector3& vec);
    Quaternion& set(const Quaternion& quat);
    bool equals1(float a, float b, float tolerance = ROUNDING_ERROR_float);

    bool equals(const Quaternion& other,
        const float tolerance = ROUNDING_ERROR_float) const;
    Quaternion& normalize();
    Mat4 getMatrix() const;
    void getMatrix(Mat4& dest, const Vector3& translation = Vector3()) const;
    void getMatrixCenter(Mat4& dest, const Vector3& center, const Vector3& translation) const;
    void getMatrix_transposed(Mat4& dest) const;
    Quaternion& makeInverse();
    Quaternion& fromAngleAxis(float angle, const Vector3& axis);
    void toAngleAxis(float& angle, Vector3& axis) const;
    void toEuler(Vector3& euler) const;
    Quaternion& makeIdentity();
    Quaternion& rotationFromTo(const Vector3& from, const Vector3& to);
    float x; // vectorial (imaginary) part
    float y;
    float z;
    float w; // real part
};


#endif

#endif