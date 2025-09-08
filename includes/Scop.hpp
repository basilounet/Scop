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
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>

# include "Camera.hpp"
# include "Mesh.hpp"
# include "Colors.hpp"

class Scop {
private:
	int							_width, _height;
	double						_lastTime;
	double						_deltaTime;
	GLFWwindow*					_window;
	Shader						_shaderProgram;
	Camera						_camera;
	std::vector<Object>			_objects;
	Mesh						_mesh;

public:
	Scop(int ac, char **av);
	Scop(const Scop& other);
	~Scop();
	Scop& operator=(const Scop& other);


	void	parse(int ac, char **av);
	void	gameLoop();
};



#endif //SCOP_H
