//
//  Mat4.cpp
//  SGEngine2
//
//  Created by Harishankar on 13/11/14.
//  Copyright (c) 2014 Smackall Games Pvt Ltd. All rights reserved.
//

class  Mat4
{
public:
    float c[16];

    Mat4()
    {
        static const float IDENTITY_MATRIX[] = {
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0
        };
        memcpy(c, IDENTITY_MATRIX, sizeof(float) * 16);
    }
    Mat4(float* pMat)
    {
        memcpy(c, pMat, sizeof(float) * 16);
    }
    Mat4(const Vec3fa& axis_x, const Vec3fa& axis_y, const Vec3fa& axis_z, const Vec3fa& trans)
    {
        c[0] = axis_x.x;
        c[1] = axis_x.y;
        c[2] = axis_x.z;
        c[3] = 0.0f;
        c[4] = axis_y.x;
        c[5] = axis_y.y;
        c[6] = axis_y.z;
        c[7] = 0.0f;
        c[8] = axis_z.x;
        c[9] = axis_z.y;
        c[10] = axis_z.z;
        c[11] = 0.0f;
        c[12] = trans.x;
        c[13] = trans.y;
        c[14] = trans.z;
        c[15] = 1.0f;
    }
    ~Mat4()
    {
    }
    Mat4& operator=(const Mat4& m)
    {
        if (this != &m) {
            memcpy(c, m.c, sizeof(float) * 16);
        }
        return *this;
    }
    bool operator==(const Mat4& m) const
    {
        return memcmp(c, m.c, sizeof(float) * 16) == 0;
    }
    Mat4& operator*=(const Mat4& m)
    {
        Mat4 m3;
        Mat4 m2 = m;
        Mat4 m1 = (*this);
        (*this)[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
        (*this)[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
        (*this)[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
        (*this)[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

        (*this)[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
        (*this)[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
        (*this)[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
        (*this)[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

        (*this)[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
        (*this)[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
        (*this)[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
        (*this)[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

        (*this)[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
        (*this)[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
        (*this)[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
        (*this)[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];

        return *this;
    }

    Vec3fa operator*(const Vec3fa& v) const
    {
        Vec3fa result;
        Mat4 ret = *this;

        result.x = ret[0] * v.x + ret[4] * v.y + ret[8] * v.z;// + ret[12];
        result.y = ret[1] * v.x + ret[5] * v.y + ret[9] * v.z;// + ret[13];
        result.z = ret[2] * v.x + ret[6] * v.y + ret[10] * v.z;// + ret[14];

        return result;
    }

    Mat4 operator*(const Mat4& m) const
    {
        Mat4 ret = *this;
        ret *= m;
        return ret;
    }

    float& operator[](unsigned i)
    {
        return c[i];
    }
    float operator[](unsigned i) const
    {
        return c[i];
    }
    void perspective(float fov, float aspect, float nearz, float farz)
    {
        memset(c, 0, sizeof(float) * 16);

        float range = tan(fov * 0.5) * nearz;
        c[0] = (2 * nearz) / ((range * aspect) - (-range * aspect));
        c[5] = (2 * nearz) / (2 * range);
        c[10] = -(farz + nearz) / (farz - nearz);
        c[11] = -1;
        c[14] = -(2 * farz * nearz) / (farz - nearz);
    }
    void translate(float x, float y, float z)
    {
        (*this)[12] = x;
        (*this)[13] = y;
        (*this)[14] = z;
    }
    void translate(const Vec3fa& v)
    {
        translate(v.x, v.y, v.z);
    }
    void rotate(float angle, int axis)
    {
        const int cos1[3] = { 5, 0, 0 };
        const int cos2[3] = { 10, 10, 5 };
        const int sin1[3] = { 9, 2, 4 };
        const int sin2[3] = { 6, 8, 1 };
        Mat4 newMatrix;

        newMatrix.c[cos1[axis]] = cos(angle);
        newMatrix.c[sin1[axis]] = -sin(angle);
        newMatrix.c[sin2[axis]] = -newMatrix.c[sin1[axis]];
        newMatrix.c[cos2[axis]] = newMatrix.c[cos1[axis]];

        *this *= newMatrix;
    }
    void setRotationRadians(Vec3fa rotation)
    {
        const float cr = cos(rotation.x);
        const float sr = sin(rotation.x);
        const float cp = cos(rotation.y);
        const float sp = sin(rotation.y);
        const float cy = cos(rotation.z);
        const float sy = sin(rotation.z);

        (*this)[0] = (cp * cy);
        (*this)[1] = (cp * sy);
        (*this)[2] = (-sp);

        const float srsp = sr * sp;
        const float crsp = cr * sp;

        (*this)[4] = (srsp * cy - cr * sy);
        (*this)[5] = (srsp * sy + cr * cy);
        (*this)[6] = (sr * cp);

        (*this)[8] = (crsp * cy + sr * sy);
        (*this)[9] = (crsp * sy - sr * cy);
        (*this)[10] = (cr * cp);
    }
    void scale(float x, float y, float z)
    {
        Mat4 newMatrix;

        newMatrix.c[0] = x;
        newMatrix.c[5] = y;
        newMatrix.c[10] = z;

        *this *= newMatrix;
    }
    void scale(const Vec3fa& v)
    {
        scale(v.x, v.y, v.z);
    }
    void scale(float s)
    {
        scale(s, s, s);
    }
    void copyMatTo(float *pointer)
    {
        for (int i = 0; i < 16; ++i)
            *pointer++ = this->c[i];
    }
    void bias()
    {
        static const float BIAS_MATRIX[] = {
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0
        };
        memcpy(c, BIAS_MATRIX, sizeof(float) * 16);
    }
    void ortho(float left, float right, float top, float bottom, float nearz, float farz)
    {
        memset(c, 0, sizeof(float) * 16);
        c[0] = 2.0f / (right - left);
        c[5] = 2.0f / (bottom - top);
        c[10] = -1.0f / (farz - nearz);
        c[11] = -nearz / (farz - nearz);
        c[15] = 1.0f;
    }
    bool invert()
    {
        float inv[16], det;
        int i;

        inv[0] = c[5] * c[10] * c[15] - c[5] * c[11] * c[14] - c[9] * c[6] * c[15] + c[9] * c[7] * c[14] + c[13] * c[6] * c[11] - c[13] * c[7] * c[10];
        inv[4] = -c[4] * c[10] * c[15] + c[4] * c[11] * c[14] + c[8] * c[6] * c[15] - c[8] * c[7] * c[14] - c[12] * c[6] * c[11] + c[12] * c[7] * c[10];
        inv[8] = c[4] * c[9] * c[15] - c[4] * c[11] * c[13] - c[8] * c[5] * c[15] + c[8] * c[7] * c[13] + c[12] * c[5] * c[11] - c[12] * c[7] * c[9];
        inv[12] = -c[4] * c[9] * c[14] + c[4] * c[10] * c[13] + c[8] * c[5] * c[14] - c[8] * c[6] * c[13] - c[12] * c[5] * c[10] + c[12] * c[6] * c[9];
        inv[1] = -c[1] * c[10] * c[15] + c[1] * c[11] * c[14] + c[9] * c[2] * c[15] - c[9] * c[3] * c[14] - c[13] * c[2] * c[11] + c[13] * c[3] * c[10];
        inv[5] = c[0] * c[10] * c[15] - c[0] * c[11] * c[14] - c[8] * c[2] * c[15] + c[8] * c[3] * c[14] + c[12] * c[2] * c[11] - c[12] * c[3] * c[10];
        inv[9] = -c[0] * c[9] * c[15] + c[0] * c[11] * c[13] + c[8] * c[1] * c[15] - c[8] * c[3] * c[13] - c[12] * c[1] * c[11] + c[12] * c[3] * c[9];
        inv[13] = c[0] * c[9] * c[14] - c[0] * c[10] * c[13] - c[8] * c[1] * c[14] + c[8] * c[2] * c[13] + c[12] * c[1] * c[10] - c[12] * c[2] * c[9];
        inv[2] = c[1] * c[6] * c[15] - c[1] * c[7] * c[14] - c[5] * c[2] * c[15] + c[5] * c[3] * c[14] + c[13] * c[2] * c[7] - c[13] * c[3] * c[6];
        inv[6] = -c[0] * c[6] * c[15] + c[0] * c[7] * c[14] + c[4] * c[2] * c[15] - c[4] * c[3] * c[14] - c[12] * c[2] * c[7] + c[12] * c[3] * c[6];
        inv[10] = c[0] * c[5] * c[15] - c[0] * c[7] * c[13] - c[4] * c[1] * c[15] + c[4] * c[3] * c[13] + c[12] * c[1] * c[7] - c[12] * c[3] * c[5];
        inv[14] = -c[0] * c[5] * c[14] + c[0] * c[6] * c[13] + c[4] * c[1] * c[14] - c[4] * c[2] * c[13] - c[12] * c[1] * c[6] + c[12] * c[2] * c[5];
        inv[3] = -c[1] * c[6] * c[11] + c[1] * c[7] * c[10] + c[5] * c[2] * c[11] - c[5] * c[3] * c[10] - c[9] * c[2] * c[7] + c[9] * c[3] * c[6];
        inv[7] = c[0] * c[6] * c[11] - c[0] * c[7] * c[10] - c[4] * c[2] * c[11] + c[4] * c[3] * c[10] + c[8] * c[2] * c[7] - c[8] * c[3] * c[6];
        inv[11] = -c[0] * c[5] * c[11] + c[0] * c[7] * c[9] + c[4] * c[1] * c[11] - c[4] * c[3] * c[9] - c[8] * c[1] * c[7] + c[8] * c[3] * c[5];
        inv[15] = c[0] * c[5] * c[10] - c[0] * c[6] * c[9] - c[4] * c[1] * c[10] + c[4] * c[2] * c[9] + c[8] * c[1] * c[6] - c[8] * c[2] * c[5];

        det = c[0] * inv[0] + c[1] * inv[4] + c[2] * inv[8] + c[3] * inv[12];

        if (det == 0) {
            return false;
        }

        det = 1.0 / det;

        for (i = 0; i < 16; i++) {
            c[i] = inv[i] * det;
        }

        return true;
    }
    void transpose()
    {
        float trans[16];
        for (int x = 0; x < 4; x++) {
            for (int y = 0; y < 4; y++) {
                trans[x + y * 4] = c[y + x * 4];
            }
        }

        memcpy(c, trans, sizeof(float) * 16);
    }
    float* pointer()
    {
        return c;
    }
    void buildCameraLookAtMatrixLH(Vec3fa position, Vec3fa target, Vec3fa upVector)
    {
        Vec3fa zaxis = target - position;
        zaxis.normalize();

        Vec3fa xaxis = upVector.cross(zaxis);
        xaxis.normalize();

        Vec3fa yaxis = zaxis.cross(xaxis);
        yaxis.normalize();

        (*this)[0] = xaxis.x;
        (*this)[1] = yaxis.x;
        (*this)[2] = zaxis.x;
        (*this)[3] = 0;

        (*this)[4] = xaxis.y;
        (*this)[5] = yaxis.y;
        (*this)[6] = zaxis.y;
        (*this)[7] = 0;

        (*this)[8] = xaxis.z;
        (*this)[9] = yaxis.z;
        (*this)[10] = zaxis.z;
        (*this)[11] = 0;

        (*this)[12] = -xaxis.dot(position);
        (*this)[13] = -yaxis.dot(position);
        (*this)[14] = -zaxis.dot(position);
        (*this)[15] = 1;
    }
    void buildProjectionMatrixPerspectiveFovLH(float fieldOfViewRadians, float aspectRatio, float zNear, float zFar)
    {
        const float h = 1.0 / (tan(fieldOfViewRadians * 0.5));
        const float w = (h / aspectRatio);

        (*this)[0] = w;
        (*this)[1] = 0;
        (*this)[2] = 0;
        (*this)[3] = 0;

        (*this)[4] = 0;
        (*this)[5] = h;
        (*this)[6] = 0;
        (*this)[7] = 0;

        (*this)[8] = 0;
        (*this)[9] = 0;
        (*this)[10] = (zFar / (zFar - zNear));
        (*this)[11] = 1;

        (*this)[12] = 0;
        (*this)[13] = 0;
        (*this)[14] = (-zNear * zFar / (zFar - zNear));
        (*this)[15] = 0;
    }
    Vec3fa getTranslation()
    {
        return Vec3fa((*this)[12], (*this)[13], (*this)[14]);
    }
    Vec3fa getRotation()
    {
        return Vec3fa((*this)[12], (*this)[13], (*this)[14]);
    }

      Mat4 setbyproduct(Mat4& other_a, Mat4& other_b)
    {
        float* m1 = other_a.pointer();
        float* m2 = other_b.pointer();

        (*this)[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
        (*this)[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
        (*this)[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
        (*this)[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

        (*this)[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
        (*this)[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
        (*this)[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
        (*this)[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

        (*this)[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
        (*this)[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
        (*this)[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
        (*this)[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

        (*this)[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
        (*this)[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
        (*this)[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
        (*this)[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
        return *this;
    }

    void rotateVect(Vec3fa& vect) const
    {
        Vec3fa tmp = vect;
        vect.x = tmp.x * (*this)[0] + tmp.y * (*this)[4] + tmp.z * (*this)[8];
        vect.y = tmp.x * (*this)[1] + tmp.y * (*this)[5] + tmp.z * (*this)[9];
        vect.z = tmp.x * (*this)[2] + tmp.y * (*this)[6] + tmp.z * (*this)[10];
    }

};