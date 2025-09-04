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


class Scop {
private:
	int							_width, _height;
	double						_lastTime;
	double						_deltaTime;
	GLFWwindow*					_window;
	Shader						_shaderProgram;
	Camera						_camera;
	Mesh						_object;

public:
	Scop();
	Scop(const Scop& other);
	~Scop();
	Scop& operator=(const Scop& other);


	void gameLoop();
};



#endif //SCOP_H
