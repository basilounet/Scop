//
// Created by bvasseur on 6/19/25.
//

#include <Scop.hpp>
#include <unistd.h>


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
	// glfwMaximizeWindow(_window);
	stbi_set_flip_vertically_on_load(true);
	// Set the viewport to the size of the window
	glViewport(0, 0 ,_width, _height);

	_shaderProgram = Shader("./src/shaders/default.vert", "./src/shaders/default.frag");
	_characters.loadASCII();
	_characters.initializeGL(_width, _height);

	// _objects[0].setTextures(textures);
	Object::createTexture();
	// std::cout << "Random place: " << _objects[0].getIndicesGroup()_materials["Material"]._mapKdTexture.getType() << std::endl;
	_mesh = Mesh(_objects[0]);
	// _mesh = Mesh(vertices, indices, textures);
	_camera = Camera(_width, _height, Vec3(0.0f, 0.5f, 2.0f));
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(_window, (double)_width / 2, (double)_height / 2);
	glfwSetFramebufferSizeCallback(_window, framebufferResize);

	glfwSetWindowUserPointer(_window, this);
	glfwSetKeyCallback(_window, keyCallback);

	glfwSwapInterval(0); // Enable vsync

	_rotation = 0.0f;
	_averageFPS.resize(32, 60);
	_useColorPercentage = 1.0f;
	_flags = F3 | USE_COLORS;
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
		_usePercentageUni = other._usePercentageUni;
		_usePercentage = other._usePercentage;
		_useColorPercentageUni = other._useColorPercentageUni;
		_useColorPercentage = other._useColorPercentage;
		_characters = other._characters;
		_flags = other._flags;
		_keysPressed = other._keysPressed;
		_averageFPS = other._averageFPS;
		_fpsFrameCount = other._fpsFrameCount;
	}
	return *this;
}

Scop::~Scop() {
	_characters.deleteCharacters();
	_mesh.destroy();
	_shaderProgram.deleteShader();
	glfwDestroyWindow(_window);
	glfwTerminate();
}

/* ==================== PUBLIC METHODS ==================== */

void Scop::parse(int ac, char **av) {
	if (ac != 2)
		throw std::runtime_error("Usage: ./scop <path_to_obj_file>");
	for (int i = 1; i < ac; ++i) {
		_objects.emplace_back(av[i]);
		// _objects.emplace_back(av[i]);
	}
}

void Scop::gameLoop() {
	// const GLuint tex1IdUni = glGetUniformLocation(_shaderProgram.getId(), "texture1");
	_modelUni = glGetUniformLocation(_shaderProgram.getId(), "model");
	_usePercentageUni = glGetUniformLocation(_shaderProgram.getId(), "useTexturePercentage");
	_useColorPercentageUni = glGetUniformLocation(_shaderProgram.getId(), "useColorPercentage");

	_lastTime = glfwGetTime() - 1.0f / 60.0f;
	_shaderProgram.activate();
	// glUniform1i(tex1IdUni, 0);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(_window)) {
		draw();
		f3Display();

		glfwSwapBuffers(_window);
		glfwPollEvents();
	}
}

void Scop::framebufferResize(GLFWwindow *window, int w, int h) {
	Scop* scop = static_cast<Scop*>(glfwGetWindowUserPointer(window));

	glViewport(0, 0, w, h);
	if (!scop)
		return ;
	scop->_width = w;
	scop->_height = h;
}

void Scop::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	Scop* scop = static_cast<Scop*>(glfwGetWindowUserPointer(window));

	scop->_camera.inputHooks(window, key, scancode, action, mods);
	if (key == GLFW_KEY_R && mods & GLFW_MOD_CONTROL && action == GLFW_PRESS) {
		scop->_shaderProgram.deleteShader();
		scop->_shaderProgram = Shader("./src/shaders/default.vert", "./src/shaders/default.frag");
		std::cout << "Shaders reloaded" << std::endl;
	}
	if (key == GLFW_KEY_F3 && action == GLFW_PRESS)
		scop->_flags ^= F3;
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		scop->_flags ^= USE_TEX;
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		scop->_flags ^= USE_COLORS;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


/* ==================== PRIVATE METHODS ==================== */


void Scop::draw() {
	_deltaTime = glfwGetTime() - _lastTime;
	_lastTime = glfwGetTime();
	glClearColor(0.4f, 0.2f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_shaderProgram.activate();

	inputs();
	_camera.inputs(_window, _deltaTime);
	_camera.updateMatrix(45.0f, 0.1f, 1000.0f);
	_camera.matrix(_shaderProgram, "camMatrix");

	Mat4 model = Mat4(1.0f);
	_rotation += _deltaTime * 5.0f;
	model = rotate(model, radians(_rotation), Vec3(0.0f, 1.0f, 0.0f));
	model = translate(model, -_objects[0].getCenterPoint());

	_usePercentage = std::clamp(_usePercentage + (_flags & USE_TEX ? 1.0f : -1.0f) * (float)_deltaTime * 0.5f, 0.0f, 1.0f);
	_useColorPercentage = std::clamp(_useColorPercentage + (_flags & USE_COLORS ? 1.0f : -1.0f) * (float)_deltaTime * 0.5f, 0.0f, 1.0f);

	glUniformMatrix4fv(_modelUni, 1, GL_FALSE, model.m);
	glUniform1f(_usePercentageUni, _usePercentage);
	glUniform1f(_useColorPercentageUni, _useColorPercentage);

	_mesh.draw(_shaderProgram, _camera);
}

void Scop::inputs() {
}

void Scop::f3Display() {
	if ((_flags & F3) == 0)
		return;
	// usleep(35000);
	_averageFPS[_fpsFrameCount] = (unsigned short)(1.0f / _deltaTime);
	_fpsFrameCount = ++_fpsFrameCount % _averageFPS.size();
	unsigned long sum = 0;
	for (const short fps : _averageFPS) {
		// std::cout << "Fps : "  << fps << std::endl;
		sum += fps;
	}
	_characters.render("Speed : " + roundStringFloat(std::to_string(_camera.getTotalSpeed()), 2),
			0.0f, _height - 20, .35f, Vec3(1, 1, 1));
	_characters.render("FPS : " + std::to_string(sum / _averageFPS.size()), 0, _height - 40, .35f, Vec3(1, 1, 1));
	_characters.render("Frame Count : " + std::to_string(_fpsFrameCount), 200, _height - 40, .35f, Vec3(1, 1, 1));
	_characters.render("Pos : " +
		roundStringFloat(std::to_string(_camera.getPos().x), 2) + "/" +
		roundStringFloat(std::to_string(_camera.getPos().y), 2) + "/" +
		roundStringFloat(std::to_string(_camera.getPos().z), 2),
		0, _height - 60, .35f, Vec3(1, 1, 1));
	_characters.render("Use percentage : " + roundStringFloat(std::to_string(_usePercentage), 2),
		0, _height - 80, .35f, Vec3(1, 1, 1));
	_characters.render("Use color percentage : " + roundStringFloat(std::to_string(_useColorPercentage), 2),
		0, _height - 100, .35f, Vec3(1, 1, 1));
	// _characters.render("Triangles : " + std::to_string(
		// [](const faceGroupMap &group) {
			// int sum = 0;
			// for (const auto& it : group) {
				// sum += it.second._indices.size();
			// }
			// return sum;
		// }(_objects[0].getIndicesGroup()) / 3),
		// 0, _height - 120, .35f, Vec3(1, 1, 1));
}
