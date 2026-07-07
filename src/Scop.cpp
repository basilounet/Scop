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

	for (size_t i = 0; i < _objects.size(); ++i) {
		_mesh.push_back(Mesh(_objects[i]));
		_mesh[i].setPosOffset(Vec3(i * 5, i * 5, i * 5));
	}
	// _mesh = Mesh(_objects[0]);
	_skybox.createSkybox();
	_rotation = 0.0f;
	_averageFPS.resize(50, 60);
	_useColorPercentage = 1.0f;
	_usePercentage = 1.0f;
	_flags = F3 | USE_COLORS | USE_TEX | LOCK_MOUSE;

	_camera = Camera(_width, _height, Vec3(0.0f, 0.5f, 2.0f), &_flags);
	glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + ((_flags & LOCK_MOUSE) == 0));
	glfwSetCursorPos(_window, (double)_width / 2, (double)_height / 2);
	glfwSetFramebufferSizeCallback(_window, framebufferResize);

	glfwSetWindowUserPointer(_window, this);
	glfwSetKeyCallback(_window, keyCallback);

	glfwSwapInterval(0); // 1 : Enable vsync

	/* ========== ImGui ========== */
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(_window, true);
	ImGui_ImplOpenGL3_Init();
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
		_skybox = other._skybox;
		_modelUni = other._modelUni;
		_rotation = other._rotation;
		_usePercentageUni = other._usePercentageUni;
		_usePercentage = other._usePercentage;
		_useColorPercentageUni = other._useColorPercentageUni;
		_useColorPercentage = other._useColorPercentage;
		_modelOffsetUni = other._modelOffsetUni;
		_characters = other._characters;
		_flags = other._flags;
		_keysPressed = other._keysPressed;
		_averageFPS = other._averageFPS;
		_currentEditMeshID = other._currentEditMeshID;
	}
	return *this;
}

Scop::~Scop() {
	_characters.deleteCharacters();
	std::for_each(_mesh.begin(), _mesh.end(), [](Mesh& mesh) { mesh.destroy(); });
	_skybox.destroy();
	_shaderProgram.deleteShader();
	glfwDestroyWindow(_window);
	glfwTerminate();
}

/* ==================== PUBLIC METHODS ==================== */

void Scop::parse(int ac, char **av) {
	if (ac < 2)
		throw std::runtime_error("Usage: ./scop <path_to_obj_file>");
	for (int i = 1; i < ac; ++i) {
		_objects.emplace_back(av[i]);
	}
}

void Scop::gameLoop() {
	// const GLuint tex1IdUni = glGetUniformLocation(_shaderProgram.getId(), "texture1");
	_modelUni = glGetUniformLocation(_shaderProgram.getId(), "model");
	_usePercentageUni = glGetUniformLocation(_shaderProgram.getId(), "useTexturePercentage");
	_useColorPercentageUni = glGetUniformLocation(_shaderProgram.getId(), "useColorPercentage");
	_modelOffsetUni = glGetUniformLocation(_shaderProgram.getId(), "modelOffset");

	_lastTime = glfwGetTime() - 1.0f / 60.0f;
	_shaderProgram.activate();
	// glUniform1i(tex1IdUni, 0);
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(_window)) {
		draw();
		imGuiDisplay();
		// f3Display();

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
	scop->_camera.setWindowSize(w, h);
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

	_usePercentage = std::clamp(_usePercentage + (_flags & USE_TEX ? 1.0f : -1.0f) * (float)_deltaTime * 0.5f, 0.0f, 1.0f);
	_useColorPercentage = std::clamp(_useColorPercentage + (_flags & USE_COLORS ? 1.0f : -1.0f) * (float)_deltaTime * 0.5f, 0.0f, 1.0f);

	glUniform1f(_usePercentageUni, _usePercentage);
	glUniform1f(_useColorPercentageUni, _useColorPercentage);
	glUniform3f(glGetUniformLocation(_shaderProgram.getId(), "camPos"), _camera.getPos().x, _camera.getPos().y, _camera.getPos().z);

	static float oscil = 0.0f;
	oscil += _deltaTime * 3.f;
	_rotation += _deltaTime * 10.0f;

	for (size_t i = 0; i < _objects.size(); ++i) {
		Mat4 model = Mat4(1.0f);
		// model = rotate(model, radians(_rotation), Vec3(cos(oscil), sin(oscil), cos(oscil)));
		model = rotate(model, radians(_rotation), Vec3(0.f, 1.f, 0.f));
		model = translate(model, -_mesh[i].getCenterPoint());

		// _mesh[i].addPosOffset(cos(oscil));
		glUniformMatrix4fv(_modelUni, 1, GL_FALSE, model.m);
		glUniform3f(_modelOffsetUni, _mesh[i].getposOffset().x, _mesh[i].getposOffset().y, _mesh[i].getposOffset().z);

		_mesh[i].draw(_shaderProgram, _camera);
		// _mesh[i].addPosOffset(-cos(oscil));
	}
	_skybox.drawSkybox(_camera);
}

void Scop::inputs() {
	if (glfwGetKey(_window, GLFW_KEY_KP_7) == GLFW_PRESS)
		_mesh[_currentEditMeshID].addPosOffset({0.f, -(float)_deltaTime, 0.f});
	if (glfwGetKey(_window, GLFW_KEY_KP_9) == GLFW_PRESS)
		_mesh[_currentEditMeshID].addPosOffset({0.f,  (float)_deltaTime, 0.f});
	if (glfwGetKey(_window, GLFW_KEY_KP_6) == GLFW_PRESS)
		_mesh[0].addPosOffset({ (float)_deltaTime, 0.f, 0.f});
	if (glfwGetKey(_window, GLFW_KEY_KP_4) == GLFW_PRESS)
		_mesh[0].addPosOffset({-(float)_deltaTime, 0.f, 0.f});
	if (glfwGetKey(_window, GLFW_KEY_KP_8) == GLFW_PRESS)
		_mesh[0].addPosOffset({0.f, 0.f, -(float)_deltaTime});
	if (glfwGetKey(_window, GLFW_KEY_KP_5) == GLFW_PRESS)
		_mesh[0].addPosOffset({0.f, 0.f,  (float)_deltaTime});
}

void Scop::f3Display() {
	// if ((_flags & F3) == 0)
	// 	return;
	// unsigned long sum = 0;
	// for (const float fps : _averageFPS)
	// 	sum += fps;
	// _characters.render("Speed : " + roundStringFloat(std::to_string(_camera.getTotalSpeed()), 2),
	// 		0.0f, _height - 20, .35f, Vec3(1, 1, 1));
	// _characters.render("FPS : " + std::to_string(sum / _averageFPS.size()), 0, _height - 40, .35f, Vec3(1, 1, 1));
	// _characters.render("Pos : " +
	// 	roundStringFloat(std::to_string(_camera.getPos().x), 2) + "/" +
	// 	roundStringFloat(std::to_string(_camera.getPos().y), 2) + "/" +
	// 	roundStringFloat(std::to_string(_camera.getPos().z), 2),
	// 	0, _height - 60, .35f, Vec3(1, 1, 1));
	// _characters.render("Use percentage : " + roundStringFloat(std::to_string(_usePercentage), 2),
	// 	0, _height - 80, .35f, Vec3(1, 1, 1));
	// _characters.render("Use color percentage : " + roundStringFloat(std::to_string(_useColorPercentage), 2),
	// 	0, _height - 100, .35f, Vec3(1, 1, 1));
	// if (nbObjects != _objects.size() || triangleCount == 0) {
	// 	triangleCount = 0;
	// 	for (const auto& object : _objects)
	// 		for (const auto& it : object.getIndicesGroup())
	// 			triangleCount += it.second._indices.size() / 3;
	// 	nbObjects = _objects.size();
	// }
	// _characters.render("Triangles : " + std::to_string(triangleCount),
	// 	0, _height - 120, .35f, Vec3(1, 1, 1));
}

void Scop::imGuiDisplay() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	debugDisplay();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Scop::debugDisplay() {
	if (ImGui::Begin("Debug", (bool *)__null)) {
		static double sum = 50 * 60;
		sum -= _averageFPS[0];
		for (int i = 0; i < 49; ++i) {
			_averageFPS[i] = _averageFPS[i + 1];
		}
		_averageFPS[49] = 1.0f / (float)_deltaTime;
		sum += _averageFPS[49];
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
		ImGui::PlotLines("Fps", _averageFPS.data(), _averageFPS.size(), 0, std::to_string(sum / _averageFPS.size()).c_str(), 0.f);

		_camera.imGuiDisplay();
		static unsigned int	nbObjects = 0;
		static unsigned int totalTriangleCount = 0;
		static unsigned int totalVerticesCount = 0;
		static unsigned int totalTexturesCount = 0;
		if (nbObjects != _objects.size()) {
			totalTriangleCount = 0;
			totalVerticesCount = 0;
			totalTexturesCount = Object::getTotalTextureCount();
			for (const auto& object : _objects) {
				totalVerticesCount += object.getVertices().size();
				for (const auto& it : object.getIndicesGroup())
					totalTriangleCount += it.second._indices.size() / 3;
			}
			nbObjects = _objects.size();
		}


		static ImGuiTableFlags table_flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollX |
			ImGuiTableFlags_ScrollY | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerH |
			ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
			ImGuiTableFlags_HighlightHoveredColumn | ImGuiTableFlags_RowBg;
		static ImGuiTableColumnFlags column_flags = ImGuiTableColumnFlags_AngledHeader |
			ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort;
		static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
			ImGuiTableFlags_Borders | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
		if (ImGui::BeginTable("Meshes infos", 4, flags)) {
			ImGui::TableSetupColumn("Total Meshes", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Total Vertices", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Total Triangle", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Total Textures", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			for (int column = 0;  column < 4; column++) {
				ImGui::TableSetColumnIndex(column);
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::Text(std::to_string([&]()->size_t{switch (column) {
						case 0: return _objects.size();
						case 1: return totalVerticesCount;
						case 2: return totalTriangleCount;
						case 3: return totalTexturesCount;
						default: return 0;
					}}()).c_str(),
					0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
			}
			ImGui::EndTable();
		}

		const char* column_names[] = { "ID", "Vertices", "Triangles", "Textures" };
		const int columns_count = IM_COUNTOF(column_names);

		if (ImGui::BeginTable("table_angled_headers", columns_count, table_flags, ImVec2(0.0f, 200))) {
			ImGui::TableSetupColumn(column_names[0], ImGuiTableColumnFlags_NoHide, 0.f, 0);
			for (int n = 1; n < columns_count; n++)
				ImGui::TableSetupColumn(column_names[n], column_flags, 0.f, n);

			ImGui::TableSetupScrollFreeze(1, 1);
			ImGui::TableAngledHeadersRow(); // Draw angled headers for all columns with the ImGuiTableColumnFlags_AngledHeader flag.
			ImGui::TableHeadersRow();       // Draw remaining headers and allow access to context-menu and other functions.
			for (int row = 0; row < (int)_objects.size(); row++) {
				ImGui::PushID(row);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("%d", row);
				for (int column = 1; column < columns_count; column++)
					if (ImGui::TableSetColumnIndex(column)) {
						switch (column) {
							case 1:
								ImGui::Text("%zu", _objects[row].getVertices().size());
								break;
							case 2:
								ImGui::Text("%zu", _objects[row].getTotalIndicesCount() / 3);
								break;
							case 3:
								ImGui::Text("%zu", _objects[row].getIndicesGroup().size());
								break;
							default:
								break;
						}
					}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}
