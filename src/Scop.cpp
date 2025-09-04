//
// Created by bvasseur on 6/19/25.
//

#include <Scop.hpp>

/* ==================== CONSTRUCTORS ==================== */

Scop::Scop() : _width(1400), _height(800), _lastTime(0), _deltaTime(0.0f) {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	_window = glfwCreateWindow(_width, _height, "Scop", nullptr, nullptr);
	if (!_window)
		throw std::runtime_error("Failed to create GLFW window");
	glfwMakeContextCurrent(_window);
	gladLoadGL();
	// Set the viewport to the size of the window
	glViewport(0, 0 ,_width, _height);
	_vertices.insert(_vertices.end(), {
		//     COORDINATES     /        COLORS      /   TexCoord  //
		-0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
		 0.5f, 0.0f, -0.5f,     0.83f, 0.70f, 0.44f,	0.0f, 0.0f,
		 0.5f, 0.0f,  0.5f,     0.83f, 0.70f, 0.44f,	5.0f, 0.0f,
		 0.0f, 0.8f,  0.0f,     0.92f, 0.86f, 0.76f,	2.5f, 5.0f
	});
	_indices.insert(_indices.end(), {
		0, 1, 2,
		0, 2, 3,
		0, 1, 4,
		1, 2, 4,
		2, 3, 4,
		3, 0, 4
	});
	_shaderProgram = Shader("./src/shaders/default.vert", "./src/shaders/default.frag");
	_vao = VAO();
	_vao.bind();

	_vbo = VBO(_vertices.data(), _vertices.size() * sizeof(GLfloat));
	_ebo = EBO(_indices.data(), _indices.size() * sizeof(GLuint));

	_vao.linkAttrib(_vbo, 0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)0); // Position
	_vao.linkAttrib(_vbo, 1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // Color
	_vao.linkAttrib(_vbo, 2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))); // TexCoord
	_vao.unbind();
	_vbo.unbind();
	_ebo.unbind();

	_camera = Camera(_width, _height, glm::vec3(0.0f, 0.0f, 2.0f));
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(_window, (double)_width / 2, (double)_height / 2);

	stbi_set_flip_vertically_on_load(true);
	_texture = Texture("./resources/textures/image.png");
}

Scop::Scop(const Scop& other) {
	*this = other;
}

Scop& Scop::operator=(const Scop& other) {
	if (this != &other) {
		_width = other._width;
		_height = other._height;
		_lastTime = other._lastTime;
		_deltaTime = other._deltaTime;
		_window = other._window;
		_vertices = other._vertices;
		_indices = other._indices;
		_shaderProgram = other._shaderProgram;
		_vao = other._vao;
		_vbo = other._vbo;
		_ebo = other._ebo;
		_camera = other._camera;
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
	const GLuint tex1IdUni = glGetUniformLocation(_shaderProgram.getId(), "texture1");
	const GLuint modelUni = glGetUniformLocation(_shaderProgram.getId(), "model");
	double rotation = 0.0f;

	_lastTime = glfwGetTime();
	_deltaTime = 0.0f;
	_shaderProgram.activate();
	glUniform1i(tex1IdUni, 0);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(_window)) {
		_deltaTime = glfwGetTime() - _lastTime;
		_lastTime = glfwGetTime();
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		_shaderProgram.activate();

		_camera.inputs(_window, _deltaTime);
		_camera.matrix(45.0f, 0.1f, 100.0f, _shaderProgram, "camMatrix");

		glm::mat4 model = glm::mat4(1.0f);
		// rotation += _deltaTime * 5.0f;
		model = glm::rotate(model, glm::radians((float)rotation), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniformMatrix4fv(modelUni, 1, GL_FALSE, glm::value_ptr(model));

		_texture.bind();
		_vao.bind();
		glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(_window);

		glfwPollEvents();
	}
}
