//
// Created by bvasseur on 10/6/25.
//

#ifndef SCOP_VEC4_HPP
# define SCOP_VEC4_HPP

# include <cmath>

# include "Vec3.hpp"

class Vec4 {
public:
	float x, y, z, w;

	Vec4();
	Vec4(float xyzw);
	Vec4(const Vec3& xyz, float w);
	Vec4(float x, float y, float z, float w);
	Vec4(const Vec4& other);
	Vec4& operator=(const Vec4& other);
	~Vec4();

	Vec4	operator+(const Vec4& other)	const ;
	Vec4	operator-(const Vec4& other)	const ;
	Vec4	operator*(float scalar)			const ;
	Vec4	operator/(float scalar)			const ;
	float&	operator[](int place);

	float	dot(const Vec4& other)			const ;
	float	length()						const ;
	Vec4	normalize()						const ;

};


#endif //SCOP_VEC4_HPP