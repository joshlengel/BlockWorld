#pragma once

class Vec2
{
public:
    float x, y;

    Vec2();
    Vec2(float x, float y);

    Vec2 operator+(const Vec2 &v) const;
    Vec2 operator-(const Vec2 &v) const;
    Vec2 operator*(float f) const;
    Vec2 operator/(float f) const;
    Vec2 operator-() const;

    Vec2 &operator+=(const Vec2 &v);
    Vec2 &operator-=(const Vec2 &v);
    Vec2 &operator*=(float f);
    Vec2 &operator/=(float f);

    static float Dot(const Vec2 &v1, const Vec2 &v2);
};

Vec2 operator*(float f, const Vec2 &v);

class Vec3
{
public:
    float x, y, z;

    Vec3();
    Vec3(float x, float y, float z);

    Vec3 operator+(const Vec3 &v) const;
    Vec3 operator-(const Vec3 &v) const;
    Vec3 operator*(float f) const;
    Vec3 operator/(float f) const;
    Vec3 operator-() const;

    Vec3 &operator+=(const Vec3 &v);
    Vec3 &operator-=(const Vec3 &v);
    Vec3 &operator*=(float f);
    Vec3 &operator/=(float f);

    static float Dot(const Vec3 &v1, const Vec3 &v2);
    static Vec3 Cross(const Vec3 &v1, const Vec3 &v2);
};

Vec3 operator*(float f, const Vec3 &v);

class Vec4
{
public:
    float x, y, z, w;

    Vec4();
    Vec4(float x, float y, float z, float w);

    Vec4 operator+(const Vec4 &v) const;
    Vec4 operator-(const Vec4 &v) const;
    Vec4 operator*(float f) const;
    Vec4 operator/(float f) const;
    Vec4 operator-() const;

    Vec4 &operator+=(const Vec4 &v);
    Vec4 &operator-=(const Vec4 &v);
    Vec4 &operator*=(float f);
    Vec4 &operator/=(float f);

    static float Dot(const Vec4 &v1, const Vec4 &v2);
};

Vec4 operator*(float f, const Vec4 &v);