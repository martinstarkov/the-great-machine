#pragma once

#include <type_traits>

#include "Utility.h"

template <typename T, std::enable_if_t<std::is_floating_point<T>::value || std::is_integral<T>::value, int> = 0>
struct Vector2 {
	T x = 0, y = 0;
	Vector2() = default;
	template <typename S, std::enable_if_t<std::is_convertible<S, T>::value, int> = 0>
	Vector2(S x, S y) : x(static_cast<T>(x)), y(static_cast<T>(y)) {}
	Vector2(const Vector2&) = default;
	Vector2& operator=(const Vector2&) = default;
	Vector2(Vector2&&) = default;
	Vector2& operator=(Vector2&&) = default;
	friend std::ostream& operator<<(std::ostream& os, const Vector2& obj) {
		os << "(" << obj.x << "," << obj.y << ")";
		return os;
	}
	static Vector2 Random(T min_x, T max_x, T min_y, T max_y) {
		return { internal::GetRandomValue<T>(min_x, max_x), internal::GetRandomValue<T>(min_y, max_y) };
	}
	inline bool HasZero() const {
		return x == 0 || y == 0;
	}
	inline bool IsZero() const {
		return x == 0 && y == 0;
	}
	inline Vector2& operator+=(const Vector2& other) {
		x += other.x;
		y += other.y;
		return *this;
	}
};

template <typename T>
inline Vector2<T> operator+(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return Vector2<T>{ lhs.x + rhs.x, lhs.y + rhs.y };
}
template <typename T, typename S, std::enable_if_t<std::is_convertible<S, T>::value, int> = 0>
inline Vector2<T> operator+(const Vector2<T>& lhs, S rhs) {
	return Vector2<T>{ lhs.x + rhs, lhs.y + rhs };
}
template <typename T, typename S, std::enable_if_t<std::is_convertible<S, T>::value, int> = 0>
inline Vector2<T> operator+(S lhs, const Vector2<T>& rhs) {
	return Vector2<T>{ lhs + rhs.x, lhs + rhs.y };
}
template <typename T>
inline Vector2<T> operator*(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return Vector2<T>{ lhs.x * rhs.x, lhs.y * rhs.y };
}
template <typename T, typename S, std::enable_if_t<std::is_convertible<S, T>::value, int> = 0>
inline Vector2<T> operator*(const Vector2<T>& lhs, S rhs) {
	return Vector2<T>{ lhs.x * rhs, lhs.y * rhs };
}
template <typename T, typename S, std::enable_if_t<std::is_convertible<S, T>::value, int> = 0>
inline Vector2<T> operator*(S lhs, const Vector2<T>& rhs) {
	return Vector2<T>{ lhs * rhs.x, lhs * rhs.y };
}
template <typename T>
inline Vector2<T> operator/(const Vector2<T>& lhs, const Vector2<T>& rhs) {
	return Vector2<T>{ lhs.x / rhs.x, lhs.y / rhs.y };
}
template <typename T, typename S, std::enable_if_t<std::is_convertible<S, T>::value, int> = 0>
inline Vector2<T> operator/(const Vector2<T>& lhs, S rhs) {
	return Vector2<T>{ lhs.x / rhs, lhs.y / rhs };
}
template <typename T, typename S, std::enable_if_t<std::is_convertible<S, T>::value, int> = 0>
inline Vector2<T> operator/(S lhs, const Vector2<T>& rhs) {
	return Vector2<T>{ lhs / rhs.x, lhs / rhs.y };
}

using V2_int = Vector2<int>;
using V2_float = Vector2<float>;
using V2_double = Vector2<double>;