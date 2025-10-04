//
// Created by bvasseur on 9/12/25.
//

#ifndef SCOP_UTILS_HPP
# define SCOP_UTILS_HPP

# include <iostream>
# include <cmath>
# include <string>

# define MENU			0b1
# define F3				0b10
# define USE_TEX		0b100
# define USE_COLORS		0b1000

float		roundFloat(const float value, const int precision);
std::string	roundStringFloat(const std::string& str, const int precision);
float		lerp(float a, float b, float f);

#endif //SCOP_UTILS_HPP