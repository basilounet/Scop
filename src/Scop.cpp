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
		-0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower left corner
		 0.5f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f, // Lower right corner
		 0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,     1.0f, 0.6f,  0.32f, // Upper corner
		-0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner left
		 0.25f, 0.5f * float(sqrt(3)) * 1 / 6, 0.0f,     0.9f, 0.45f, 0.17f, // Inner right
		 0.0f, -0.5f * float(sqrt(3)) * 1 / 3, 0.0f,     0.8f, 0.3f,  0.02f  // Inner down
	});
	_indices.insert(_indices.end(), {
		0, 3, 5,
		3, 2, 4,
		5, 4, 1
	});
	_shaderProgram = Shader("./src/shaders/default.vert", "./src/shaders/default.frag");
	_vao = VAO();
	_vao.bind();

	_vbo = VBO(_vertices.data(), _vertices.size() * sizeof(GLfloat));
	_ebo = EBO(_indices.data(), _indices.size() * sizeof(GLuint));

	_vao.linkAttrib(_vbo, 0, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)0); // Position
	_vao.linkAttrib(_vbo, 1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // Color
	_vao.unbind();
	_vbo.unbind();
	_ebo.unbind();
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
	GLuint uniId = glGetUniformLocation(_shaderProgram.getId(), "scale");
	while (!glfwWindowShouldClose(_window)) {
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		_shaderProgram.activate();
		glUniform1f(uniId, 0.5f);
		_vao.bind();
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(_window);

		glfwPollEvents();
	}
}
