//
// Created by bvasseur on 6/19/25.
//

#include <Scop.hpp>

/* ==================== CONSTRUCTORS ==================== */

Scop::Scop(int ac, char **av) : _width(1400), _height(800), _lastTime(0), _deltaTime(0.0f) {
	parse(ac, av);

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	_window = glfwCreateWindow(_width, _height, "Scop", nullptr, nullptr);
	if (!_window)
		throw std::runtime_error("Failed to create GLFW window");
	glfwMakeContextCurrent(_window);
	gladLoadGL();
	glfwMaximizeWindow(_window);
	stbi_set_flip_vertically_on_load(true);
	// Set the viewport to the size of the window
	glViewport(0, 0 ,_width, _height);

	_shaderProgram = Shader("./src/shaders/default.vert", "./src/shaders/default.frag");


	// _objects[0].setTextures(textures);
	Object::createTexture();
	// std::cout << "Random place: " << _objects[0].getIndicesGroup()_materials["Material"]._mapKdTexture.getType() << std::endl;
	_mesh = Mesh(_objects[0]);
	// _mesh = Mesh(vertices, indices, textures);

	_camera = Camera(_width, _height, glm::vec3(0.0f, 0.0f, 2.0f));
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(_window, (double)_width / 2, (double)_height / 2);
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
		_shaderProgram = other._shaderProgram;
		_camera = other._camera;
		_objects = other._objects;
		_mesh = other._mesh;
		_modelUni = other._modelUni;
		_rotation = other._rotation;
	}
	return *this;
}

Scop::~Scop() {
	_mesh.destroy();
	_shaderProgram.deleteShader();
	glfwDestroyWindow(_window);
	glfwTerminate();
}

/* ==================== METHODS ==================== */

void Scop::parse(int ac, char **av) {
	if (ac != 2)
		throw std::runtime_error("Usage: ./scop <path_to_obj_file>");
	for (int i = 1; i < ac; ++i) {
		_objects.emplace_back(av[i]);
		// _objects.emplace_back(av[i]);
	}
}

void framebufferResize(GLFWwindow *window, int w, int h) {
	(void)window;
	glViewport(0, 0, w, h);
}

void Scop::gameLoop() {
	// const GLuint tex1IdUni = glGetUniformLocation(_shaderProgram.getId(), "texture1");
	_modelUni = glGetUniformLocation(_shaderProgram.getId(), "model");

	_lastTime = glfwGetTime();
	_deltaTime = 0.0f;
	_shaderProgram.activate();
	// glUniform1i(tex1IdUni, 0);
	glEnable(GL_DEPTH_TEST);
	glfwSetFramebufferSizeCallback(_window, framebufferResize);

	while (!glfwWindowShouldClose(_window)) {
		draw();
	}
}

void Scop::draw() {
	_deltaTime = glfwGetTime() - _lastTime;
	_lastTime = glfwGetTime();
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_shaderProgram.activate();

	_camera.inputs(_window, _deltaTime);
	_camera.updateMatrix(45.0f, 0.1f, 100.0f);
	_camera.matrix(_shaderProgram, "camMatrix");

	glm::mat4 model = glm::mat4(1.0f);
	_rotation += _deltaTime * 15.0f;
	model = glm::rotate(model, glm::radians(_rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	_mesh.draw(_shaderProgram, _camera);

	glUniformMatrix4fv(_modelUni, 1, GL_FALSE, glm::value_ptr(model));

	glfwSwapBuffers(_window);

	glfwPollEvents();
}


