#include"Mat.h"

Mat3::Mat3(): Mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f) {}
Mat3::Mat3(
    float m00, float m10, float m20,
    float m01, float m11, float m21,
    float m02, float m12, float m22):
    m00(m00), m10(m10), m20(m20),
    m01(m01), m11(m11), m21(m21),
    m02(m02), m12(m12), m22(m22)
{}

Vec3 Mat3::operator*(const Vec3 &v) const
{
    return
    {
        v.x * m00 + v.y * m10 + v.z * m20,
        v.x * m01 + v.y * m11 + v.z * m21,
        v.x * m02 + v.y * m12 + v.z * m22
    };
}

Mat3 Mat3::operator*(const Mat3 &m) const
{
    return
    {
        m.m00 * m00 + m.m01 * m10 + m.m02 * m20, m.m10 * m00 + m.m11 * m10 + m.m12 * m20, m.m20 * m00 + m.m21 * m10 + m.m22 * m20,
        m.m00 * m01 + m.m01 * m11 + m.m02 * m21, m.m10 * m01 + m.m11 * m11 + m.m12 * m21, m.m20 * m01 + m.m21 * m11 + m.m22 * m21,
        m.m00 * m02 + m.m01 * m12 + m.m02 * m22, m.m10 * m02 + m.m11 * m12 + m.m12 * m22, m.m20 * m02 + m.m21 * m12 + m.m22 * m22
    };
}

Mat3 &Mat3::operator*=(const Mat3 &m) { return *this = *this * m; }

Vec3 operator*(const Vec3 &v, const Mat3 &m) { return m * v; }

Mat4::Mat4(): Mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f) {}
Mat4::Mat4(
    float m00, float m10, float m20, float m30,
    float m01, float m11, float m21, float m31,
    float m02, float m12, float m22, float m32,
    float m03, float m13, float m23, float m33):
    m00(m00), m10(m10), m20(m20), m30(m30),
    m01(m01), m11(m11), m21(m21), m31(m31),
    m02(m02), m12(m12), m22(m22), m32(m32),
    m03(m03), m13(m13), m23(m23), m33(m33)
{}

Vec4 Mat4::operator*(const Vec4 &v) const
{
    return
    {
        v.x * m00 + v.y * m10 + v.z * m20 + v.w * m30,
        v.x * m01 + v.y * m11 + v.z * m21 + v.w * m31,
        v.x * m02 + v.y * m12 + v.z * m22 + v.w * m32,
        v.x * m03 + v.y * m13 + v.z * m23 + v.w * m33
    };
}

Mat4 Mat4::operator*(const Mat4 &m) const
{
    return
    {
        m.m00 * m00 + m.m01 * m10 + m.m02 * m20 + m.m03 * m30, m.m10 * m00 + m.m11 * m10 + m.m12 * m20 + m.m13 * m30, m.m20 * m00 + m.m21 * m10 + m.m22 * m20 + m.m23 * m30, m.m30 * m00 + m.m31 * m10 + m.m32 * m20 + m.m33 * m30,
        m.m00 * m01 + m.m01 * m11 + m.m02 * m21 + m.m03 * m31, m.m10 * m01 + m.m11 * m11 + m.m12 * m21 + m.m13 * m31, m.m20 * m01 + m.m21 * m11 + m.m22 * m21 + m.m23 * m31, m.m30 * m01 + m.m31 * m11 + m.m32 * m21 + m.m33 * m31,
        m.m00 * m02 + m.m01 * m12 + m.m02 * m22 + m.m03 * m32, m.m10 * m02 + m.m11 * m12 + m.m12 * m22 + m.m13 * m32, m.m20 * m02 + m.m21 * m12 + m.m22 * m22 + m.m23 * m32, m.m30 * m02 + m.m31 * m12 + m.m32 * m22 + m.m33 * m32,
        m.m00 * m03 + m.m01 * m13 + m.m02 * m23 + m.m03 * m33, m.m10 * m03 + m.m11 * m13 + m.m12 * m23 + m.m13 * m33, m.m20 * m03 + m.m21 * m13 + m.m22 * m23 + m.m23 * m33, m.m30 * m03 + m.m31 * m13 + m.m32 * m23 + m.m33 * m33
    };
}

Mat4 &Mat4::operator*=(const Mat4 &m) { return *this = *this * m; }

Vec4 operator*(const Vec4 &v, const Mat4 &m) { return m * v; }