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
			//     COORDINATES     /        COLORS      /   TexCoord  //
			-0.5f, -0.5f, 0.0f,     1.0f, 0.0f, 0.0f,	0.0f, 0.0f, // Lower left corner
			-0.5f,  0.5f, 0.0f,     0.0f, 1.0f, 0.0f,	0.0f, 1.0f, // Upper left corner
			 0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // Upper right corner
			 0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,	1.0f, 0.0f  // Lower right corner
		});
	_indices.insert(_indices.end(), {
		0, 2, 1, // Upper triangle
		0, 3, 2 // Lower triangle
	});
	_shaderProgram = Shader("./src/shaders/default.vert", "./src/shaders/default.frag");
	_vao = VAO();
	_vao.bind();

	_vbo = VBO(_vertices.data(), _vertices.size() * sizeof(GLfloat));
	_ebo = EBO(_indices.data(), _indices.size() * sizeof(GLuint));

	_vao.linkAttrib(_vbo, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)0); // Position
	_vao.linkAttrib(_vbo, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // Color
	_vao.linkAttrib(_vbo, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))); // Color
	_vao.unbind();
	_vbo.unbind();
	_ebo.unbind();

	stbi_set_flip_vertically_on_load(true);
	_texture = Texture("./resources/textures/image.png");
}

Scop::Scop(const Scop& other) {
	*this = other;
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
		_texture = other._texture;
	}
	return *this;
}

Scop::~Scop() {
	_shaderProgram.deleteShader();
	_vao.deleteVAO();
	_vbo.deleteVBO();
	_ebo.deleteEBO();
	_texture.deleteTexture();
	glfwDestroyWindow(_window);
	glfwTerminate();
}

/* ==================== METHODS ==================== */

void Scop::gameLoop() {
	const GLuint scaleUni = glGetUniformLocation(_shaderProgram.getId(), "scale");
	const GLuint tex1IdUni = glGetUniformLocation(_shaderProgram.getId(), "texture1");

	_shaderProgram.activate();
	glUniform1i(tex1IdUni, 0);

	while (!glfwWindowShouldClose(_window)) {
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		_shaderProgram.activate();
		glUniform1f(scaleUni, 0.5f);
		_texture.bind();
		_vao.bind();
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(_window);

		glfwPollEvents();
	}
}
