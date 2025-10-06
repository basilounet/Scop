//
// Created by bvasseur on 10/6/25.
//

#ifndef SCOP_MAT4_HPP
# define SCOP_MAT4_HPP

# include <algorithm>

# include "Vec4.hpp"
# include "Vec3.hpp"


class Mat4 {
public:
	float m[16];

	Mat4();
	Mat4(float value);
	Mat4(const Mat4& other);
	Mat4& operator=(const Mat4& other);
	~Mat4();

	const float&	operator()(int col, int row)	const ;
	float&			operator()(int col, int row);
	Mat4			operator*(const Mat4 &b);

	Vec4			getRow(int row) const;
	Vec4			getCol(int col) const;

	Vec4			setRow(int row, const Vec4 &v);
	Vec4			setCol(int col, const Vec4 &v);

	static Mat4		identity();
	Mat4			translate(const Vec3 &v);
	Mat4			scale(const Vec3 &s);
	Mat4			rotate(float angleRad, const Vec3 &axis);
};

Mat4	perspective(float fovDeg, float aspect, float near, float far);
Mat4	lookAt(const Vec3 &eye, const Vec3 &center, const Vec3 &up);
Mat4	translate(const Vec3 &t);
Mat4	ortho(float left, float right, float bottom, float top);

#endif //SCOP_MAT4_HPP