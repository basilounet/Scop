//
// Created by bvasseur on 10/6/25.
//

#ifndef SCOP_VEC2_HPP
# define SCOP_VEC2_HPP

# include <cmath>


class Vec2 {
public:
	float x, y;

	Vec2();
	Vec2(float xy);
	Vec2(float x, float y);
	Vec2(const Vec2& other);
	Vec2& operator=(const Vec2& other);
	~Vec2();

	Vec2	operator+(const Vec2& other)	const ;
	Vec2	operator-(const Vec2& other)	const ;
	Vec2	operator*(float scalar)			const ;
	Vec2	operator/(float scalar)			const ;

	float	dot(const Vec2& other)			const ;
	float	length()						const ;
	float	cross(const Vec2& other)		const ;
	Vec2	normalize()						const ;

};

#endif //SCOP_VEC2_HPP