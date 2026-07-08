//
// Created by bvasseur on 6/19/25.
//

#ifndef SCOP_H
# define SCOP_H

# include "utils.hpp"
# include <iostream>
# include <cmath>
# include <vector>
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include "stb_image.h"
# include <imgui.h>
# include <backends/imgui_impl_glfw.h>
# include <backends/imgui_impl_opengl3.h>
# include <misc/cpp/imgui_stdlib.h>
# include <filesystem>
# include <functional>

# include "Camera.hpp"
# include "Mesh.hpp"
# include "Colors.hpp"
# include "Skybox.hpp"
# include "math/Mat4.hpp"
# include "math/Vec4.hpp"
# include "math/Vec3.hpp"
# include "math/Vec2.hpp"

class Scop {
public:
	Scop(int ac, char **av);
	Scop(const Scop& other);
	~Scop();
	Scop& operator=(const Scop& other);


	void	parse(int ac, char **av);
	void	gameLoop();

private:
	static void framebufferResize(GLFWwindow *window, int w, int h);
	static void	keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void	draw();
	void	inputs();
	void	f3Display();
	void	imGuiDisplay();
	void	debugDisplay();
	void	matDisplay();
	void	objectDisplay();

private:
	int					_width, _height;
	double				_lastTime;
	double				_deltaTime;
	GLFWwindow*			_window;
	Shader				_shaderProgram;
	Camera				_camera;
	std::map<std::string, Object>	_objects;
	std::vector<Mesh>	_meshes;
	Skybox				_skybox;
	GLint				_modelUni;
	float				_rotation;
	GLint				_usePercentageUni;
	float				_usePercentage;
	GLint				_useColorPercentageUni;
	float				_useColorPercentage;
	GLint				_modelOffsetUni;
	unsigned int		_flags;
	unsigned int		_keysPressed;
	std::vector<float>	_averageFPS;
	unsigned int		_currentEditMeshID;

};



#endif //SCOP_H
