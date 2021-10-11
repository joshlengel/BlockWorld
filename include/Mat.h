#pragma once

#include"Vec.h"

class Mat3
{
public:
    float m00, m10, m20;
    float m01, m11, m21;
    float m02, m12, m22;

    Mat3();
    Mat3(
        float m00, float m10, float m20,
        float m01, float m11, float m21,
        float m02, float m12, float m22);
    
    Vec3f operator*(const Vec3f &v) const;
    Mat3 operator*(const Mat3 &m) const;
    Mat3 &operator*=(const Mat3 &m);
};

Vec3f operator*(const Vec3f &v, const Mat3 &m);

class Mat4
{
public:
    float m00, m10, m20, m30;
    float m01, m11, m21, m31;
    float m02, m12, m22, m32;
    float m03, m13, m23, m33;

    Mat4();
    Mat4(
        float m00, float m10, float m20, float m30,
        float m01, float m11, float m21, float m31,
        float m02, float m12, float m22, float m32,
        float m03, float m13, float m23, float m33);
    
    Vec4f operator*(const Vec4f &v) const;
    Mat4 operator*(const Mat4 &m) const;
    Mat4 &operator*=(const Mat4 &m);
};

Vec4f operator*(const Vec4f &v, const Mat4 &m);