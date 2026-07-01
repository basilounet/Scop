//
// Created by bvasseur on 6/19/25.
//

#ifndef SCOP_H
# define SCOP_H

# include <iostream>
# include <cmath>
# include <vector>
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include "stb_image.h"

# include "Camera.hpp"
# include "Mesh.hpp"
# include "Colors.hpp"
# include "Characters.hpp"
# include "Skybox.hpp"
# include "utils.hpp"
# include "math/Mat4.hpp"
# include "math/Vec4.hpp"
# include "math/Vec3.hpp"
# include "math/Vec2.hpp"

class Scop {
private:
	int							_width, _height;
	double						_lastTime;
	double						_deltaTime;
	GLFWwindow*					_window;
	Shader						_shaderProgram;
	Camera						_camera;
	std::vector<Object>			_objects;
	std::vector<Mesh>			_mesh;
	Skybox						_skybox;
	GLint						_modelUni;
	float						_rotation;
	GLint						_usePercentageUni;
	float						_usePercentage;
	GLint						_useColorPercentageUni;
	float						_useColorPercentage;
	GLint						_modelOffsetUni;
	Characters					_characters;
	unsigned int				_flags;
	unsigned int				_keysPressed;
	std::vector<unsigned short> _averageFPS;
	unsigned short				frameCount;

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
};



#endif //SCOP_H
