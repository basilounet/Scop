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

# include "Shader.hpp"
# include "VAO.hpp"
# include "VBO.hpp"
# include "EBO.hpp"
# include "Texture.hpp"
# include "Camera.hpp"


class Scop {
private:
	int							_width, _height;
	double						_lastTime;
	double						_deltaTime;
	GLFWwindow*					_window;
	std::vector<GLfloat>		_vertices;
	std::vector<GLuint>			_indices;
	Shader						_shaderProgram;
	VAO							_vao;
	VBO							_vbo;
	EBO							_ebo;
	Camera						_camera;
	Texture						_texture;

public:
	Scop();
	Scop(const Scop& other);
	~Scop();
	Scop& operator=(const Scop& other);


	void gameLoop();
};



#endif //SCOP_H
