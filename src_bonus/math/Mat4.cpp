//
// Created by bvasseur on 10/6/25.
//

# include "math/Mat4.hpp"
# include "Colors.hpp"


/* ==================== CONSTRUCTORS ==================== */


Mat4::Mat4() {
	std::fill(m, m + 16, 0.0f);
}

Mat4::Mat4(float value) {
	std::fill(m, m + 16, 0.0f);
	this->m[0] = this->m[5] = this->m[10] = this->m[15] = value;
}

Mat4::Mat4(const Mat4 &other) {
	*this = other;
}

Mat4 & Mat4::operator=(const Mat4 &other) {
	if (this != &other) {
		std::copy(other.m, other.m + 16, m);
	}
	return *this;
}

Mat4::~Mat4() {
}


/* ==================== OPERATORS ==================== */


const float& Mat4::operator()(int col, int row) const {
	return m[std::clamp(col * 4 + row, 0, 15)];
}

float& Mat4::operator()(int col, int row) {
	return m[std::clamp(col * 4 + row, 0, 15)];
}


/* ==================== METHODS ==================== */


Vec4 Mat4::getRow(int row) const {
	row = std::clamp(row, 0, 3);
	return {m[row], m[4 + row], m[8 + row], m[12 + row]};
}

Vec4 Mat4::getCol(int col) const {
	col = std::clamp(col, 0, 3);
	return {m[4 * col], m[4 * col + 1], m[4 * col + 2], m[4 * col + 3]};
}

Vec4 Mat4::setRow(int row, const Vec4 &v) {
	row = std::clamp(row, 0, 3);
	m[row] = v.x;
	m[row + 4] = v.y;
	m[row + 8] = v.z;
	m[row + 12] = v.w;
	return v;
}

Vec4 Mat4::setCol(int col, const Vec4 &v) {
	col = std::clamp(col, 0, 3);
	m[4 * col] = v.x;
	m[4 * col + 1] = v.y;
	m[4 * col + 2] = v.z;
	m[4 * col + 3] = v.w;
	return v;
}

Mat4 Mat4::identity() {
	Mat4 result(0.0f);
	result.m[0] = result.m[5] = result.m[10] = result.m[15] = 1.0f;
	return result;
}


/* ==================== NON-MEMBER FUNCTIONS ==================== */


std::ostream & operator<<(std::ostream &os, const Mat4 &mat) {
	std::cout << "[" << mat.m[0] << ", " << mat.m[4] << ", " << mat.m[8] << ", " << mat.m[12] << "]" << std::endl;
	std::cout << "[" << mat.m[1] << ", " << mat.m[5] << ", " << mat.m[9] << ", " << mat.m[13] << "]" << std::endl;
	std::cout << "[" << mat.m[2] << ", " << mat.m[6] << ", " << mat.m[10] << ", " << mat.m[14] << "]" << std::endl;
	std::cout << "[" << mat.m[3] << ", " << mat.m[7] << ", " << mat.m[11] << ", " << mat.m[15] << "]";
	return os;
}

Mat4 operator*(const Mat4 &a, const Mat4 &b) {
	Mat4 result;
	for (int col = 0; col < 4; ++col)
	{
		for (int row = 0; row < 4; ++row)
		{
			result(col, row) = 0.0f;
			for (int k = 0; k < 4; ++k)
				result(col, row) += a(k, row) * b(col, k);
		}
	}
	return (result);
}

Mat4 perspective(float fovDeg, float aspect, float near, float far) {
	float fovRad = fovDeg * M_PI / 180.0f;
	float f = 1.0f / std::tan(fovRad / 2.0f);

	Mat4 mat;
	mat.m[0] = f / aspect;
	mat.m[5] = f;
	mat.m[10] = (far + near) / (near - far);
	mat.m[11] = -1.0f;
	mat.m[14] = (2.0f * far * near) / (near - far);
	return mat;
}

Mat4 lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up) {
	// std::cout << "eye: " << eye.x << ", " << eye.y << ", " << eye.z << std::endl;
	// std::cout << "center: " << center.x << ", " << center.y << ", " << center.z << std::endl;

	const Vec3 f = normalize(center - eye);
	const Vec3 s = normalize(cross(f, up));
	const Vec3 u = cross(s, f);
	// std::cout << RED"my f: " << f.x << ", " << f.y << ", " << f.z << RESET << std::endl;
	// std::cout << GREEN"my s: " << s.x << ", " << s.y << ", " << s.z << RESET << std::endl;
	// std::cout << MAGENTA"my u: " << u.x << ", " << u.y << ", " << u.z << RESET << std::endl;

	Mat4 mat = Mat4::identity();

	mat(0, 0) = s.x; mat(1, 0) = s.y; mat(2, 0) = s.z;
	mat(0, 1) = u.x;  mat(1, 1) = u.y;  mat(2, 1) = u.z;
	mat(0, 2) = -f.x; mat(1, 2) = -f.y; mat(2, 2) = -f.z;
	mat(3, 0) = -dot(s, eye);
	mat(3, 1) = -dot(u, eye);
	mat(3, 2) = dot(f, eye);
	return (mat);
}

Mat4 ortho(float left, float right, float bottom, float top) {
	const float near = -1.0f;
	const float far = 1.0f;

	Mat4 mat = Mat4::identity();

	mat(0, 0) = 2.0f / (right - left);
	mat(1, 1) = 2.0f / (top - bottom);
	mat(2, 2) = -2.0f / (far - near);

	mat(3, 0) = -(right + left) / (right - left);
	mat(3, 1) = -(top + bottom) / (top - bottom);
	mat(3, 2) = -(far + near) / (far - near);

	return mat;
}

Mat4 rotate(Mat4 &mat, float angleRad, const Vec3 &axis) {
	return (mat * rotate(angleRad, axis));
}

Mat4 rotate(float angleRad, const Vec3 &axis) {
	Vec3 a = normalize(axis);
	float c = std::cos(angleRad);
	float s = std::sin(angleRad);
	float oneMinusC = 1.0f - c;

	Mat4 mat = Mat4::identity();
	mat(0, 0) = c + a.x * a.x * oneMinusC;
	mat(0, 1) = a.x * a.y * oneMinusC + a.z * s;
	mat(0, 2) = a.x * a.z * oneMinusC - a.y * s;

	mat(1, 0) = a.y * a.x * oneMinusC - a.z * s;
	mat(1, 1) = c + a.y * a.y * oneMinusC;
	mat(1, 2) = a.y * a.z * oneMinusC + a.x * s;

	mat(2, 0) = a.z * a.x * oneMinusC + a.y * s;
	mat(2, 1) = a.z * a.y * oneMinusC - a.x * s;
	mat(2, 2) = c + a.z * a.z * oneMinusC;

	return (mat);
}


Mat4 translate(const Vec3 &t) {
	Mat4 mat = Mat4::identity();
	// Result[3] = m[0] * v[0] + m[1] * v[1] + m[2] * v[2] + m[3];
	mat.setCol(3, mat.getCol	(0) * t.x + mat.getCol(1) * t.y + mat.getCol(2) * t.z + mat.getCol(3));
	return (mat);
}

Mat4 translate(const Mat4 &mat, const Vec3 &v) {
	Mat4 result = mat;

	Vec4 translation = mat.getCol(0) * v.x + mat.getCol(1) * v.y + mat.getCol(2) * v.z + mat.getCol(3);

	result.setCol(3, translation);

	return (result);
}

float radians(float degrees) {
	return degrees * (M_PI / 180);
}
