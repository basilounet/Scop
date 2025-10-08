//
// Created by bvasseur on 10/6/25.
//

#include "math/Vec3.hpp"


/* ==================== CONSTRUCTORS ==================== */


Vec3::Vec3() : x(0.0f), y(0.0f), z(0.0f) {
}

Vec3::Vec3(float xyz) : x(xyz), y(xyz), z(xyz) {
}

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {
}

Vec3::Vec3(const Vec3 &other) {
	*this = other;
}

Vec3 & Vec3::operator=(const Vec3 &other) {
	if (this != &other) {
		x = other.x;
		y = other.y;
		z = other.z;
	}
	return *this;
}

Vec3::~Vec3() {
}


/* ==================== OPERATORS ==================== */


Vec3 Vec3::operator+(const Vec3 &other) const {
	return {x + other.x, y + other.y, z + other.z};
}

Vec3 Vec3::operator+=(const Vec3 &other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return *this;
}

Vec3 Vec3::operator-(const Vec3 &other) const {
	return {x - other.x, y - other.y, z - other.z};
}

Vec3 Vec3::operator-() const {
	return {-x, -y, -z};
}

Vec3 Vec3::operator*(float scalar) const {
	return {x * scalar, y * scalar, z * scalar};
}

Vec3 Vec3::operator/(const Vec3& op) const {
	return {x / op.x, y / op.y, z / op.z};
}

float & Vec3::operator[](int place) {
	place %= 3;
	place = place < 0 ? place + 3 : place;
	switch (place) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		default:
			return x;
	}
}


/* ==================== METHODS ==================== */


float Vec3::dot(const Vec3 &other) const {
	return x * other.x + y * other.y + z * other.z;
}

float Vec3::length() const {
	return std::sqrt(x * x + y * y + z * z);
}

/*Vec3 Vec3::cross(const Vec3 &other) const {
	return {y * other.z - other.y * z,
				z * other.x - other.z * x,
				x * other.y - other.x * y};
}

Vec3 Vec3::normalize() const {
	const float len = length();
	if (len > 0.0f)
		return *this / (1 / len);
	return {0};
}*/

Vec3 Vec3::abs() const {
	return {std::abs(x), std::abs(y), std::abs(z)};
}


/* ==================== NON-MEMBER OPERATORS ==================== */


Vec3 operator*(float scalar, const Vec3 &vec) {
	return {vec.x * scalar, vec.y * scalar, vec.z * scalar};
}

float	angle(const Vec3 &a, const Vec3 &b) {
	return std::acos(a.dot(b) / (a.length() * b.length()));
}

float dot(const Vec3 &a, const Vec3 &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 rotate(const Vec3& v, float angle, const Vec3 &b) {
	return (v * std::cos(angle)) +
		   (cross(b, v) * std::sin(angle)) +
		   (b * (b.dot(v)) * (1 - std::cos(angle)));
}

Vec3 cross(const Vec3 &a, const Vec3 &b) {
	return {
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	};
}

Vec3 normalize(const Vec3 &v) {
	const float len = v.length();
	if (len > 0.0f)
		return v / len;
	return {0};
}
