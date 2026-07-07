//
// Created by bvasseur on 9/12/25.
//

#ifndef SCOP_UTILS_HPP
# define SCOP_UTILS_HPP

# include <iostream>
# include <cmath>
# include <string>
# include <imgui.h>
# include <backends/imgui_impl_glfw.h>
# include <backends/imgui_impl_opengl3.h>
# include <misc/cpp/imgui_stdlib.h>


# define MENU			0b1
# define F3				0b10
# define USE_TEX		0b100
# define USE_COLORS		0b1000
# define LOCK_MOUSE		0b10000

float		roundFloat(const float value, const int precision);
std::string	roundStringFloat(const std::string& str, const int precision);
float		lerp(float a, float b, float f);

#endif //SCOP_UTILS_HPP