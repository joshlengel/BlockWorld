#include"Vec.h"

Vec2::Vec2(): Vec2(0.0f, 0.0f) {}
Vec2::Vec2(float x, float y): x(x), y(y) {}

Vec2 Vec2::operator+(const Vec2 &v) const { return { x + v.x, y + v.y }; }
Vec2 Vec2::operator-(const Vec2 &v) const { return { x - v.x, y - v.y }; }
Vec2 Vec2::operator*(float f) const { return { x * f, y * f }; }
Vec2 Vec2::operator/(float f) const { return { x / f, y / f }; }
Vec2 Vec2::operator-() const { return { -x, -y }; }

Vec2 &Vec2::operator+=(const Vec2 &v) { return *this = *this + v; }
Vec2 &Vec2::operator-=(const Vec2 &v) { return *this = *this - v; }
Vec2 &Vec2::operator*=(float f) { return *this = *this * f; }
Vec2 &Vec2::operator/=(float f) { return *this = *this / f; }

Vec2 operator*(float f, const Vec2 &v) { return v * f; }

Vec3::Vec3(): Vec3(0.0f, 0.0f, 0.0f) {}
Vec3::Vec3(float x, float y, float z): x(x), y(y), z(z) {}

Vec3 Vec3::operator+(const Vec3 &v) const { return { x + v.x, y + v.y, z + v.z }; }
Vec3 Vec3::operator-(const Vec3 &v) const { return { x - v.x, y - v.y, z - v.z }; }
Vec3 Vec3::operator*(float f) const { return { x * f, y * f, z * f }; }
Vec3 Vec3::operator/(float f) const { return { x / f, y / f, z / f }; }
Vec3 Vec3::operator-() const { return { -x, -y, -z }; }

Vec3 &Vec3::operator+=(const Vec3 &v) { return *this = *this + v; }
Vec3 &Vec3::operator-=(const Vec3 &v) { return *this = *this - v; }
Vec3 &Vec3::operator*=(float f) { return *this = *this * f; }
Vec3 &Vec3::operator/=(float f) { return *this = *this / f; }

Vec3 operator*(float f, const Vec3 &v) { return v * f; }

Vec4::Vec4(): Vec4(0.0f, 0.0f, 0.0f, 0.0f) {}
Vec4::Vec4(float x, float y, float z, float w): x(x), y(y), z(z), w(w) {}

Vec4 Vec4::operator+(const Vec4 &v) const { return { x + v.x, y + v.y, z + v.z, w + v.w }; }
Vec4 Vec4::operator-(const Vec4 &v) const { return { x - v.x, y - v.y, z - v.z, w - v.w }; }
Vec4 Vec4::operator*(float f) const { return { x * f, y * f, z * f, w * f }; }
Vec4 Vec4::operator/(float f) const { return { x / f, y / f, z / f, w / f }; }
Vec4 Vec4::operator-() const { return { -x, -y, -z, -w }; }

Vec4 &Vec4::operator+=(const Vec4 &v) { return *this = *this + v; }
Vec4 &Vec4::operator-=(const Vec4 &v) { return *this = *this - v; }
Vec4 &Vec4::operator*=(float f) { return *this = *this * f; }
Vec4 &Vec4::operator/=(float f) { return *this = *this / f; }

Vec4 operator*(float f, const Vec4 &v) { return v * f; }

float Vec2::Dot(const Vec2 &v1, const Vec2 &v2) { return v1.x * v2.x + v1.y * v2.y; }
float Vec3::Dot(const Vec3 &v1, const Vec3 &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }
float Vec4::Dot(const Vec4 &v1, const Vec4 &v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w; }

Vec3 Vec3::Cross(const Vec3 &v1, const Vec3 &v2) { return { v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x }; }