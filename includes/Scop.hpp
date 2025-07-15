//
// Created by bvasseur on 6/19/25.
//

#ifndef SCOP_H
#define SCOP_H

# include <iostream>
# include <cmath>
# include <vector>
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include "Shader.hpp"
# include "VAO.hpp"
# include "VBO.hpp"
# include "EBO.hpp"


class Scop {
private:
	GLFWwindow*					_window;
	std::vector<GLfloat>		_vertices;
	std::vector<GLuint>			_indices;
	Shader						_shaderProgram;
	VAO							_vao;
	VBO							_vbo;
	EBO							_ebo;

public:
	Scop();
	Scop(const Scop& other);
	~Scop();
	Scop& operator=(const Scop& other);


	void gameLoop();
};



#endif //SCOP_H
