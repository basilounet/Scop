//
// Created by bvasseur on 10/6/25.
//

#include "math/Vec4.hpp"


/* ==================== CONSTRUCTORS ==================== */


Vec4::Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {
}

Vec4::Vec4(float xyzw) : x(xyzw), y(xyzw), z(xyzw), w(xyzw) {
}

Vec4::Vec4(const Vec3 &xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {
}

Vec4::Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {
}

Vec4::Vec4(const Vec4 &other) {
	*this = other;
}

Vec4 & Vec4::operator=(const Vec4 &other) {
	if (this != &other) {
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
	}
	return *this;
}

Vec4::~Vec4() {
}


/* ==================== OPERATORS ==================== */


Vec4 Vec4::operator+(const Vec4 &other) const {
	return {x + other.x, y + other.y, z + other.z, w + other.w};
}

Vec4 Vec4::operator-(const Vec4 &other) const {
	return {x - other.y, y - other.y, z - other.z, w - other.w};
}

Vec4 Vec4::operator*(float scalar) const {
	return {x * scalar, y * scalar, z * scalar, w * scalar};
}

Vec4 Vec4::operator/(float scalar) const {
	return {x / scalar, y / scalar, z / scalar, w / scalar};
}

float & Vec4::operator[](int place) {
	place %= 4;
	place = place < 0 ? place + 4 : place;
	switch (place) {
		case 0:
			return x;
		case 1:
			return y;
		case 2:
			return z;
		case 3:
			return w;
		default:
			return x;
	}
}


/* ==================== METHODS ==================== */


float Vec4::dot(const Vec4 &other) const {
	return x * other.x + y * other.y + z * other.z + w * other.w;
}

float Vec4::length() const {
	return std::sqrt(x * x + y * y + z * z + w * w);
}

Vec4 Vec4::normalize() const {
	float len = length();
	if (len > 0.0f)
		return *this / len;
	return {};
}
