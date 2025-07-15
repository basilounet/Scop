//
// Created by bvasseur on 6/19/25.
//

#include <Scop.hpp>

/* ==================== CONSTRUCTORS ==================== */

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
	// Set the viewport to the size of the window
	glViewport(0, 0 ,600, 600);
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
	_shaderProgram = Shader("./src/shaders/default.vert", "./src/shaders/default.frag");
	_vao = VAO();
	_vao.bind();

	VBO vbo(_vertices.data(), _vertices.size() * sizeof(GLfloat));
	EBO ebo(_indices.data(), _indices.size() * sizeof(GLuint));

	_vao.linkVBO(vbo, 0);
	_vao.unbind();
	vbo.unbind();
	ebo.unbind();
}

Scop::Scop(const Scop& other) {
	*this = other;
}

Scop::~Scop() {
	glfwDestroyWindow(_window);
	_shaderProgram.deleteShader();
	_vao.deleteVAO();
	_vbo.deleteVBO();
	_ebo.deleteEBO();
	glfwTerminate();
}

Scop& Scop::operator=(const Scop& other) {
	if (this != &other) {
		_window = other._window;
		_vertices = other._vertices;
		_indices = other._indices;
		_shaderProgram = other._shaderProgram;
		_vao = other._vao;
		_vbo = other._vbo;
		_ebo = other._ebo;
	}
	return *this;
}

/* ==================== METHODS ==================== */

void Scop::gameLoop() {
	while (!glfwWindowShouldClose(_window)) {
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		_shaderProgram.activate();
		_vao.bind();
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(_window);

		glfwPollEvents();
	}
}
