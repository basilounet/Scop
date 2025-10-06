//
// Created by bvasseur on 10/6/25.
//

#ifndef SCOP_VEC3_HPP
# define SCOP_VEC3_HPP

# include <cmath>


class Vec3 {
public:
	float x, y, z;

	Vec3();
	Vec3(float xyz);
	Vec3(float x, float y, float z);
	Vec3(const Vec3& other);
	Vec3& operator=(const Vec3& other);
	~Vec3();

	Vec3	operator+(const Vec3& other)	const ;
	Vec3	operator-(const Vec3& other)	const ;
	Vec3	operator*(float scalar)			const ;
	Vec3	operator/(float scalar)			const ;

	float	dot(const Vec3& other)			const ;
	float	length()						const ;
	Vec3	cross(const Vec3& other)		const ;
	Vec3	normalize()						const ;
	Vec3	abs()							const ;
};


#endif //SCOP_VEC3_HPP