#include"Vec.h"

template <typename T> Vec2<T>::Vec2(): Vec2(0.0f, 0.0f) {}
template <typename T> Vec2<T>::Vec2(T x, T y): x(x), y(y) {}

template <typename T> Vec2<T> Vec2<T>::operator+(const Vec2 &v) const { return { static_cast<T>(x + v.x), static_cast<T>(y + v.y) }; }
template <typename T> Vec2<T> Vec2<T>::operator-(const Vec2 &v) const { return { static_cast<T>(x - v.x), static_cast<T>(y - v.y) }; }
template <typename T> Vec2<T> Vec2<T>::operator*(T f) const { return Vec2{ static_cast<T>(x * f), static_cast<T>(y * f) }; }
template <typename T> Vec2<T> Vec2<T>::operator/(T f) const { return Vec2{ static_cast<T>(x / f), static_cast<T>(y / f) }; }
template <typename T> Vec2<T> Vec2<T>::operator-() const { return Vec2{ static_cast<T>(-x), static_cast<T>(-y) }; }

template <typename T> Vec2<T> &Vec2<T>::operator+=(const Vec2 &v) { return *this = *this + v; }
template <typename T> Vec2<T> &Vec2<T>::operator-=(const Vec2 &v) { return *this = *this - v; }
template <typename T> Vec2<T> &Vec2<T>::operator*=(T f) { return *this = *this * f; }
template <typename T> Vec2<T> &Vec2<T>::operator/=(T f) { return *this = *this / f; }

template <typename T> Vec2<T> operator*(T f, const Vec2<T> &v) { return v * f; }

template <typename T> Vec3<T>::Vec3(): Vec3(0.0f, 0.0f, 0.0f) {}
template <typename T> Vec3<T>::Vec3(T x, T y, T z): x(x), y(y), z(z) {}

template <typename T> Vec3<T> Vec3<T>::operator+(const Vec3 &v) const { return { static_cast<T>(x + v.x), static_cast<T>(y + v.y), static_cast<T>(z + v.z) }; }
template <typename T> Vec3<T> Vec3<T>::operator-(const Vec3 &v) const { return { static_cast<T>(x - v.x), static_cast<T>(y - v.y), static_cast<T>(z - v.z) }; }
template <typename T> Vec3<T> Vec3<T>::operator*(T f) const { return { static_cast<T>(x * f), static_cast<T>(y * f), static_cast<T>(z * f) }; }
template <typename T> Vec3<T> Vec3<T>::operator/(T f) const { return { static_cast<T>(x / f), static_cast<T>(y / f), static_cast<T>(z / f) }; }
template <typename T> Vec3<T> Vec3<T>::operator-() const { return { static_cast<T>(-x), static_cast<T>(-y), static_cast<T>(-z) }; }

template <typename T> Vec3<T> &Vec3<T>::operator+=(const Vec3 &v) { return *this = *this + v; }
template <typename T> Vec3<T> &Vec3<T>::operator-=(const Vec3 &v) { return *this = *this - v; }
template <typename T> Vec3<T> &Vec3<T>::operator*=(T f) { return *this = *this * f; }
template <typename T> Vec3<T> &Vec3<T>::operator/=(T f) { return *this = *this / f; }

template <typename T> Vec3<T> operator*(T f, const Vec3<T> &v) { return v * f; }

template <typename T> Vec4<T>::Vec4(): Vec4(0.0f, 0.0f, 0.0f, 0.0f) {}
template <typename T> Vec4<T>::Vec4(T x, T y, T z, T w): x(x), y(y), z(z), w(w) {}

template <typename T> Vec4<T> Vec4<T>::operator+(const Vec4 &v) const { return { static_cast<T>(x + v.x), static_cast<T>(y + v.y), static_cast<T>(z + v.z), static_cast<T>(w + v.w) }; }
template <typename T> Vec4<T> Vec4<T>::operator-(const Vec4 &v) const { return { static_cast<T>(x - v.x), static_cast<T>(y - v.y), static_cast<T>(z - v.z), static_cast<T>(w - v.w) }; }
template <typename T> Vec4<T> Vec4<T>::operator*(T f) const { return { static_cast<T>(x * f), static_cast<T>(y * f), static_cast<T>(z * f), static_cast<T>(w * f) }; }
template <typename T> Vec4<T> Vec4<T>::operator/(T f) const { return { static_cast<T>(x / f), static_cast<T>(y / f), static_cast<T>(z / f), static_cast<T>(w / f) }; }
template <typename T> Vec4<T> Vec4<T>::operator-() const { return { static_cast<T>(-x), static_cast<T>(-y), static_cast<T>(-z), static_cast<T>(-w) }; }

template <typename T> Vec4<T> &Vec4<T>::operator+=(const Vec4 &v) { return *this = *this + v; }
template <typename T> Vec4<T> &Vec4<T>::operator-=(const Vec4 &v) { return *this = *this - v; }
template <typename T> Vec4<T> &Vec4<T>::operator*=(T f) { return *this = *this * f; }
template <typename T> Vec4<T> &Vec4<T>::operator/=(T f) { return *this = *this / f; }

template <typename T> Vec4<T> operator*(T f, const Vec4<T> &v) { return v * f; }

template <typename T> T Vec2<T>::Dot(const Vec2 &v1, const Vec2 &v2) { return static_cast<T>(v1.x * v2.x + v1.y * v2.y); }
template <typename T> T Vec3<T>::Dot(const Vec3 &v1, const Vec3 &v2) { return static_cast<T>(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }
template <typename T> T Vec4<T>::Dot(const Vec4 &v1, const Vec4 &v2) { return static_cast<T>(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w); }

template <typename T> Vec3<T> Vec3<T>::Cross(const Vec3 &v1, const Vec3 &v2) { return { static_cast<T>(v1.y * v2.z - v1.z * v2.y), static_cast<T>(v1.z * v2.x - v1.x * v2.z), static_cast<T>(v1.x * v2.y - v1.y * v2.x) }; }

template class Vec2<float>;
template class Vec3<float>;
template class Vec4<float>;

template class Vec2<uint16_t>;
template class Vec3<uint16_t>;
template class Vec4<uint16_t>;

template Vec2f operator*(float f, const Vec2f &v);
template Vec3f operator*(float f, const Vec3f &v);
template Vec4f operator*(float f, const Vec4f &v);

template Vec2ui16 operator*(uint16_t f, const Vec2ui16 &v);
template Vec3ui16 operator*(uint16_t f, const Vec3ui16 &v);
template Vec4ui16 operator*(uint16_t f, const Vec4ui16 &v);