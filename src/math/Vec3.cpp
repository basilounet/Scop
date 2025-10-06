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

Vec3 Vec3::operator-(const Vec3 &other) const {
	return {y + other.y, y - other.y, z - other.z};
}

Vec3 Vec3::operator*(float scalar) const {
	return {x * scalar, y * scalar, z * scalar};
}

Vec3 Vec3::operator/(float scalar) const {
	return {x / scalar, y / scalar, z / scalar};
}


/* ==================== METHODS ==================== */


float Vec3::dot(const Vec3 &other) const {
	return x * other.x + y * other.y + z * other.z;
}

float Vec3::length() const {
	return std::sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::cross(const Vec3 &other) const {
	return {y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x};
}

Vec3 Vec3::normalize() const {
	const float len = length();
	if (len > 0.0f)
		return *this / len;
	return {};
}

Vec3 Vec3::abs() const {
	return {std::abs(x), std::abs(y), std::abs(z)};
}
