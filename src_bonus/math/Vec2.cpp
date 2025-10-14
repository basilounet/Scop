//
// Created by bvasseur on 10/6/25.
//

#include "math/Vec2.hpp"



/* ==================== CONSTRUCTORS ==================== */


Vec2::Vec2() : x(0.0f), y(0.0f) {
}

Vec2::Vec2(float xy) : x(xy), y(xy) {
}

Vec2::Vec2(float x, float y) : x(x), y(y) {
}

Vec2::Vec2(const Vec2 &other) {
	*this = other;
}

Vec2 & Vec2::operator=(const Vec2 &other) {
	if (this != &other) {
		x = other.x;
		y = other.y;
	}
	return *this;
}

Vec2::~Vec2() {
}


/* ==================== OPERATORS ==================== */


Vec2 Vec2::operator+(const Vec2 &other) const {
	return {x + other.x, y + other.y};
}

Vec2 Vec2::operator-(const Vec2 &other) const {
	return {x - other.x, y - other.y};
}

Vec2 Vec2::operator*(float scalar) const {
	return {x * scalar, y * scalar};
}

Vec2 Vec2::operator/(float scalar) const {
	return {x / scalar, y / scalar};
}


/* ==================== METHODS ==================== */


float Vec2::dot(const Vec2 &other) const {
	return x * other.x + y * other.y;
}

float Vec2::length() const {
	return std::sqrt(x * x + y * y);
}

float Vec2::cross(const Vec2 &other) const {
	return x * other.y - y * other.x;
}

Vec2 Vec2::normalize() const {
	float len = length();
	if (len > 0.0f)
		return *this / len;
	return {};
}
