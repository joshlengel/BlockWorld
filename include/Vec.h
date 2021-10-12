#pragma once

#include"Core.h"

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
    static float Length(const Vec2 &v);
    static Vec2 Normalized(const Vec2 &v);
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
    static float Length(const Vec3 &v);
    static Vec3 Normalized(const Vec3 &v);
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
    static float Length(const Vec4 &v);
    static Vec4 Normalized(const Vec4 &v);
};

template <typename T>
Vec4<T> operator*(T f, const Vec4<T> &v);

typedef Vec2<float> Vec2f;
typedef Vec3<float> Vec3f;
typedef Vec4<float> Vec4f;

typedef Vec2<i32> Vec2i;
typedef Vec3<i32> Vec3i;
typedef Vec4<i32> Vec4i;

typedef Vec2<ui32> Vec2ui;
typedef Vec3<ui32> Vec3ui;
typedef Vec4<ui32> Vec4ui;