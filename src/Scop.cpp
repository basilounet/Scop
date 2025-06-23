//
// Created by bvasseur on 6/19/25.
//

#include <Scop.hpp>

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
//Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";


Scop::Scop() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	_window = glfwCreateWindow(600, 600, "Scop", nullptr, nullptr);
	if (!_window)
		throw std::runtime_error("Failed to create GLFW window");
	glfwMakeContextCurrent(_window);
	gladLoadGL();
	_vertices.insert(_vertices.end(), {
		-0.5f, -0.5f * static_cast<float>(sqrt(3)) / 3, 0.0f, // Lower left corner
		0.5f, -0.5f * static_cast<float>(sqrt(3)) / 3, 0.0f, // Lower right corner
		0.0f, 0.5f * static_cast<float>(sqrt(3)) * 2 / 3, 0.0f, // Upper corner
		-0.5f / 2, 0.5f * static_cast<float>(sqrt(3)) / 6, 0.0f, // Inner left
		0.5f / 2, 0.5f * static_cast<float>(sqrt(3)) / 6, 0.0f, // Inner right
		0.0f, -0.5f * static_cast<float>(sqrt(3)) / 3, 0.0f // Inner down
	});
	_indices.insert(_indices.end(), {
		0, 3, 5,
		3, 2, 4,
		5, 4, 1
	});

	std::cout << "Here" << std::endl;
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::cout << "Here" << std::endl;
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, vertexShader);
	glAttachShader(_shaderProgram, fragmentShader);
	glLinkProgram(_shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);

	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(GLfloat), _vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Scop::Scop(const Scop& other) {
	*this = other;
}

Scop::~Scop() {
	glfwDestroyWindow(_window);
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
	glDeleteBuffers(1, &_ebo);
	glDeleteProgram(_shaderProgram);
	glfwTerminate();
}

Scop& Scop::operator=(const Scop& other) {
	if (this != &other) {
		_window = other._window;
		_vertices = other._vertices;
		_indices = other._indices;
		_vbo = other._vbo;
		_vao = other._vao;
		_ebo = other._ebo;
	}
	return *this;
}



void Scop::gameLoop() {
	gladLoadGL();
	// Set the viewport to the size of the window
	glViewport(0, 0 ,600, 600);

	glfwSwapBuffers(_window);

	while (!glfwWindowShouldClose(_window)) {
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(_shaderProgram);
		glBindVertexArray(_vao);
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(_window);

		glfwPollEvents();
	}
}
