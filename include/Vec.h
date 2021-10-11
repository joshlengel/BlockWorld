#pragma once

#include<cstdint>

template <typename T>
class Vec2
{
public:
    T x, y;

    Vec2();
    Vec2(T x, T y);

    Vec2 operator+(const Vec2 &v) const;
    Vec2 operator-(const Vec2 &v) const;
    Vec2 operator*(T f) const;
    Vec2 operator/(T f) const;
    Vec2 operator-() const;

    Vec2 &operator+=(const Vec2 &v);
    Vec2 &operator-=(const Vec2 &v);
    Vec2 &operator*=(T f);
    Vec2 &operator/=(T f);

    static T Dot(const Vec2 &v1, const Vec2 &v2);
};

template <typename T>
Vec2<T> operator*(T f, const Vec2<T> &v);

template <typename T>
class Vec3
{
public:
    T x, y, z;

    Vec3();
    Vec3(T x, T y, T z);

    Vec3 operator+(const Vec3 &v) const;
    Vec3 operator-(const Vec3 &v) const;
    Vec3 operator*(T f) const;
    Vec3 operator/(T f) const;
    Vec3 operator-() const;

    Vec3 &operator+=(const Vec3 &v);
    Vec3 &operator-=(const Vec3 &v);
    Vec3 &operator*=(T f);
    Vec3 &operator/=(T f);

    static T Dot(const Vec3 &v1, const Vec3 &v2);
    static Vec3 Cross(const Vec3 &v1, const Vec3 &v2);
};

template <typename T>
Vec3<T> operator*(T f, const Vec3<T> &v);

template <typename T>
class Vec4
{
public:
    T x, y, z, w;

    Vec4();
    Vec4(T x, T y, T z, T w);

    Vec4 operator+(const Vec4 &v) const;
    Vec4 operator-(const Vec4 &v) const;
    Vec4 operator*(T f) const;
    Vec4 operator/(T f) const;
    Vec4 operator-() const;

    Vec4 &operator+=(const Vec4 &v);
    Vec4 &operator-=(const Vec4 &v);
    Vec4 &operator*=(T f);
    Vec4 &operator/=(T f);

    static T Dot(const Vec4 &v1, const Vec4 &v2);
};

template <typename T>
Vec4<T> operator*(T f, const Vec4<T> &v);

typedef Vec2<float> Vec2f;
typedef Vec3<float> Vec3f;
typedef Vec4<float> Vec4f;

typedef Vec2<uint16_t> Vec2ui16;
typedef Vec3<uint16_t> Vec3ui16;
typedef Vec4<uint16_t> Vec4ui16;