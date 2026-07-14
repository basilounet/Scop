//
// Created by bvasseur on 9/12/25.
//

#ifndef SCOP_UTILS_HPP
# define SCOP_UTILS_HPP

# include <iostream>
# include <cmath>
# include <string>
# include <functional>
# include <any>
# include <imgui.h>
# include <backends/imgui_impl_glfw.h>
# include <backends/imgui_impl_opengl3.h>
# include <misc/cpp/imgui_stdlib.h>

# include "math/Vec2.hpp"
# include "math/Vec3.hpp"

# define USE_TEX			0b1
# define USE_COLORS			0b10
# define USE_TRI_PER		0b100
# define OUTLINE_PER		0b1000
# define HIDE_MESH			0b10000
# define HIDE_OUTLINE		0b100000
# define USE_STATIC_TEX		0b1000000
# define USE_PITCH_YAW		0b10000000
# define RUN_SPLINE			0b100000000
# define HIDE_SPLINE		0b1000000000
# define LOOP_SPLINE		0b10000000000
# define LOCK_MOUSE			0b100000000000

inline auto emptyAny = std::any();

float		roundFloat(const float value, const int precision);
std::string	roundStringFloat(const std::string& str, const int precision);
float		lerp(float a, float b, float f);

struct ExampleImageViewerData {
	ImU32   ImageBgColor = IM_COL32(100, 100, 100, 255);
	ImU32   GridColor = IM_COL32(255, 255, 255, 100);
	bool    GridEnabled = true;
	bool    ViewReset = true;
	ImVec2  ViewOffset; // in image space
	float   Zoom = 1.0f;
	float   ZoomMin = .1f;
	float   ZoomMax = 100.0f;
};

void	drag3(Vec3 &vec, const std::string& label, float speed, float min, float max, float xSize, bool printLabel = true,
	const std::function<void(std::any&)> &callback = [](std::any&){}, std::any& data = emptyAny);
void	drag2(Vec2 &vec, const std::string& label, float speed, float min, float max, float xSize, bool printLabel = true,
	const std::function<void(std::any&)> &callback = [](std::any&){}, std::any& data = emptyAny);
void	ExampleImageViewer_DrawOptions(ExampleImageViewerData* data);
void	ExampleImageViewer_DrawCanvas(ExampleImageViewerData* data, ImVec2 canvas_size, ImTextureRef image_tex_ref,
	int image_w, int image_h);

#endif //SCOP_UTILS_HPP