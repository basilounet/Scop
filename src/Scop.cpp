//
// Created by bvasseur on 6/19/25.
//

#include <Scop.hpp>
#include <unistd.h>


/* ==================== CONSTRUCTORS ==================== */

Scop::Scop(int ac, char **av) : _width(1400), _height(800), _lastTime(0), _deltaTime(0.0f), _currentEditMeshID(0) {
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
	glViewport(0, 0 ,_width, _height);

	_shaderProgram = Shader("./src/shaders/default.vert", "./src/shaders/default.frag");
	_outlineShader = Shader("./src/shaders/outline.vert", "./src/shaders/outline.frag");

	Object::loadTextures();

	for (auto& obj: _objects)
		_meshes.emplace_back(obj.second);
	// for (size_t i = 0; i < _objects.size(); ++i) {
		// _mesh.push_back(Mesh(_objects[i].second));
		// _mesh[i].setPosOffset(Vec3(i * 5, i * 5, i * 5));
	// }
	_skybox.createSkybox();
	_rotation = 0.0f;
	_averageFPS.resize(50, 60);
	_flags = LOCK_MOUSE;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

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
		_outlineShader = other._outlineShader;
		_camera = other._camera;
		_objects = other._objects;
		_meshes = other._meshes;
		_skybox = other._skybox;
		_rotation = other._rotation;
		_flags = other._flags;
		_keysPressed = other._keysPressed;
		_averageFPS = other._averageFPS;
		_currentEditMeshID = other._currentEditMeshID;
	}
	return *this;
}

Scop::~Scop() {
	Object::deleteTextures();
	std::for_each(_meshes.begin(), _meshes.end(), [](Mesh& mesh) { mesh.destroy(); });
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
		if (_objects.find(av[i]) == _objects.end())
			_objects[std::string(av[i]).substr(std::string(av[i]).find_last_of("/\\") + 1)] = Object(av[i]);
		// _objects.emplace_back(av[i]);
	}
}

void Scop::gameLoop() {
	_lastTime = glfwGetTime() - 1.0f / 60.0f;
	// glUniform1i(tex1IdUni, 0);

	while (!glfwWindowShouldClose(_window)) {
		draw();
		imGuiDisplay();

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
		scop->_outlineShader = Shader("./src/shaders/outline.vert", "./src/shaders/outline.frag");
		std::cout << "Shaders reloaded" << std::endl;
	}
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		scop->_meshes[scop->_currentEditMeshID].switchFlags(USE_TEX);
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		scop->_meshes[scop->_currentEditMeshID].switchFlags(USE_COLORS);
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		scop->_meshes[scop->_currentEditMeshID].switchFlags(USE_OUTLINE_PER);
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
		scop->_meshes[scop->_currentEditMeshID].switchFlags(HIDE_MESH);
	if (key == GLFW_KEY_H && action == GLFW_PRESS)
		scop->_meshes[scop->_currentEditMeshID].switchFlags(HIDE_OUTLINE);
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		scop->_currentEditMeshID = ++scop->_currentEditMeshID % scop->_meshes.size();
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


/* ==================== PRIVATE METHODS ==================== */


void Scop::draw() {
	_deltaTime = glfwGetTime() - _lastTime;
	_lastTime = glfwGetTime();
	glClearColor(0.4f, 0.2f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	inputs();
	_shaderProgram.activate();
	_camera.inputs(_window, _deltaTime);
	_camera.updateMatrix(45.0f, 0.1f, 1000.0f);
	_camera.sendUniforms(_shaderProgram);

	static float oscil = 0.0f; // TODO : remove
	oscil += _deltaTime * 3.f;
	_rotation += _deltaTime * 10.0f;
	// double xPos, yPos;
	// glfwGetCursorPos(_window, &xPos, &yPos);
	// Vec2 mousePos = Vec2((float)xPos, (float)yPos);

	for (size_t i = 0; i < _meshes.size(); ++i) {
		Mat4 model = Mat4(1.0f);
		model = rotate(model, radians(_rotation), Vec3(0.f, 1.f, 0.f));
		// model = rotate(model, radians(_rotation), Vec3(cos(oscil + .5f), sin(oscil), cos(oscil)));
		model = translate(model, -_meshes[i].getCenterPoint());
		// _meshes[i].addPos(cos(oscil));

		_meshes[i].updateStates(_deltaTime);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		_meshes[i].draw(_shaderProgram, _camera, model, "mesh");

		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);

		_outlineShader.activate();
		_meshes[i].draw(_outlineShader, _camera, model, "outline");

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);


		// _meshes[i].addPos(-cos(oscil));
	}
	_skybox.drawSkybox(_camera);
}

void Scop::inputs() {
	if (glfwGetKey(_window, GLFW_KEY_KP_7) == GLFW_PRESS)
		_meshes[_currentEditMeshID].addPos({0.f, -(float)_deltaTime, 0.f});
	if (glfwGetKey(_window, GLFW_KEY_KP_9) == GLFW_PRESS)
		_meshes[_currentEditMeshID].addPos({0.f,  (float)_deltaTime, 0.f});
	if (glfwGetKey(_window, GLFW_KEY_KP_6) == GLFW_PRESS)
		_meshes[_currentEditMeshID].addPos({ (float)_deltaTime, 0.f, 0.f});
	if (glfwGetKey(_window, GLFW_KEY_KP_4) == GLFW_PRESS)
		_meshes[_currentEditMeshID].addPos({-(float)_deltaTime, 0.f, 0.f});
	if (glfwGetKey(_window, GLFW_KEY_KP_8) == GLFW_PRESS)
		_meshes[_currentEditMeshID].addPos({0.f, 0.f, -(float)_deltaTime});
	if (glfwGetKey(_window, GLFW_KEY_KP_5) == GLFW_PRESS)
		_meshes[_currentEditMeshID].addPos({0.f, 0.f,  (float)_deltaTime});
}

void Scop::imGuiDisplay() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	debugDisplay();
	matDisplay();
	objectDisplay();
	meshesDisplay();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Scop::debugDisplay() {
	if (ImGui::Begin("Debug", (bool *)__null)) {
		static double sum = 50 * 60;
		sum -= _averageFPS[0];
		for (int i = 0; i < 49; ++i)
			_averageFPS[i] = _averageFPS[i + 1];
		_averageFPS[49] = 1.0f / (float)_deltaTime;
		sum += _averageFPS[49];
		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.9f);
		ImGui::PlotLines("Fps", _averageFPS.data(), _averageFPS.size(), 0, std::to_string(sum / _averageFPS.size()).c_str(), 0.f);

		_camera.imGuiDisplay();
		static unsigned int	nbMeshes = 0;
		static unsigned int totalTriangleCount = 0;
		static unsigned int totalVerticesCount = 0;
		static unsigned int totalTexturesCount = 0;
		if (nbMeshes != _meshes.size()) {
			totalTriangleCount = 0;
			totalVerticesCount = 0;
			totalTexturesCount = Object::getTotalTextureCount();
			for (const auto& mesh : _meshes) {
				totalVerticesCount += mesh.getVertices().size();
				for (const auto& it : mesh.getIndicesGroup())
					totalTriangleCount += it.second._indices.size() / 3;
			}
			nbMeshes = _meshes.size();
		}


		static ImGuiTableFlags table_flags = ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerH |
			ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders |
			ImGuiTableFlags_HighlightHoveredColumn | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoHostExtendX;
		if (ImGui::BeginTable("Meshes infos", 4, table_flags)) {
			ImGui::TableSetupColumn("Total Meshes", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Total Vertices", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Total Triangle", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Total Textures", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();
			ImGui::TableNextRow();
			for (int column = 0;  column < 4; ++column) {
				ImGui::TableSetColumnIndex(column);
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::Text(std::to_string([&]()->size_t{switch (column) {
						case 0: return _meshes.size();
						case 1: return totalVerticesCount;
						case 2: return totalTriangleCount;
						case 3: return totalTexturesCount;
						default: return 0;
					}}()).c_str(),
					0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void Scop::matDisplay() {
	if (ImGui::Begin("Materials && Textures", (bool *)__null)) {
		const std::function<std::vector<std::string>()> loadResources = []() {
			std::vector<std::string> items;
			std::string path = "./resources/textures/";
			for (const auto & entry : std::filesystem::directory_iterator(path))
				if (entry.path().extension() != ".png" || entry.path().extension() != ".jpg")
					items.push_back(entry.path().filename());
			return items;
		};
		static std::vector<std::string> items = loadResources();
		const std::function<void(MaterialData&, const bool)> resourcesCombo = [] (MaterialData& mat, const bool reloadTextures){
			if (ImGui::BeginCombo("mapKd", mat._mapKd.c_str(), 0)) {
				for (int n = 0; n < (int)items.size(); ++n) {
					const bool is_selected = (mat._mapKd == items[n]);
					if (ImGui::Selectable(items[n].c_str(), is_selected)) {
						mat._mapKd = items[n];
						if (reloadTextures)
							Object::loadTextures();
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		};
		const std::function<void(Texture&)> imagePopup = [](Texture& tex) {
			static ExampleImageViewerData image_viewer;
			if (ImGui::Button("Show image")) {
				image_viewer.ViewOffset = ImVec2(tex.getWidth() / 2.f, tex.getHeight() / 2.f);
				ImGui::OpenPopup("my_select_popup");
			}

			if (ImGui::BeginPopup("my_select_popup")) {
				if (ImGui::Button("Close"))
					ImGui::CloseCurrentPopup();
				ImGui::SameLine();
				ImGui::Text("%ix%i", tex.getWidth(), tex.getHeight());
				ImGui::SameLine();
				if (ImGui::Button("Recenter"))
					image_viewer.ViewOffset = ImVec2(tex.getWidth() / 2.f, tex.getHeight() / 2.f);
				ImGui::SameLine();
				ImVec2 canvas_size(tex.getWidth(), tex.getHeight());
				ExampleImageViewer_DrawOptions(&image_viewer);
				ExampleImageViewer_DrawCanvas(&image_viewer, canvas_size, tex.getID(), tex.getWidth(), tex.getHeight());
				ImGui::EndPopup();
			}
		};

		if (ImGui::Button("Create a new Material"))
			ImGui::OpenPopup("Create Material");
		ImGui::SameLine();
		if (ImGui::Button("Refresh resources"))
			items = loadResources();
		ImGui::SameLine();
		if (ImGui::Button("Reload Textures"))
			Object::loadTextures();

		static MaterialData mat = {};
		if (ImGui::BeginPopupModal("Create Material", nullptr, ImGuiWindowFlags_MenuBar)) {
			ImGui::InputText("name", &mat._name);
			ImGui::DragFloat("ns", &mat._ns, 0.01f, -FLT_MAX, FLT_MAX);
			drag3(mat._ka, "ka", 0.01f, -FLT_MAX, FLT_MAX, ImGui::GetContentRegionAvail().x * .6f);
			drag3(mat._kd, "kd", 0.01f, -FLT_MAX, FLT_MAX, ImGui::GetContentRegionAvail().x * .6f);
			drag3(mat._ks, "ks", 0.01f, -FLT_MAX, FLT_MAX, ImGui::GetContentRegionAvail().x * .6f);
			drag3(mat._ke, "ke", 0.01f, -FLT_MAX, FLT_MAX, ImGui::GetContentRegionAvail().x * .6f);
			ImGui::DragFloat("ni", &mat._ni, 0.01f, -FLT_MAX, FLT_MAX);
			ImGui::DragFloat("d", &mat._d, 0.01f, -FLT_MAX, FLT_MAX);

			resourcesCombo(mat, false);
			// imagePopup(mat._mapKdTexture);
			if (ImGui::Button("Add")) {
				Object::addMaterial(std::move(mat));
				Object::loadTextures();
				mat = {};
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
				mat = {};
			ImGui::SameLine();
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

		static ImGuiTableFlags table_flags = ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerH |
			ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders |
			ImGuiTableFlags_HighlightHoveredColumn | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoHostExtendX;

		matMap& allMats = Object::getMaterials();

		if (ImGui::BeginTable("Textures", 10, table_flags)) {
			ImGui::TableSetupColumn("name", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("ns", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("ka", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("kd", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("ks", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("ke", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("ni", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("d", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("mapKd", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("mapKdTexture", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();
			for (int row = 0;  row < (int)allMats.size(); ++row) {
				MaterialData* mat = &allMats.at(std::next(allMats.begin(), row)->first);
				Texture* tex = &mat->_mapKdTexture;

				ImGui::PushID(row);
				ImGui::TableNextRow();
				for (int column = 0;  column < 10; ++column) {
					ImGui::TableSetColumnIndex(column);
					ImGui::PushItemWidth(-FLT_MIN);
					switch (column) {
						case 0:
							ImGui::Text("%s", mat->_name.c_str());
							break;
						case 1:
							ImGui::DragFloat("##ns", &mat->_ns, 0.01f, -FLT_MAX, FLT_MAX);
							break;
						case 2:
							drag3(mat->_ka, "ka", 0.01f, -FLT_MAX, FLT_MAX, ImGui::GetContentRegionAvail().x * .9f, false);
							break;
						case 3:
							drag3(mat->_kd, "kd", 0.01f, -FLT_MAX, FLT_MAX, ImGui::GetContentRegionAvail().x * .9f, false);
							break;
						case 4:
							drag3(mat->_ks, "ks", 0.01f, -FLT_MAX, FLT_MAX, ImGui::GetContentRegionAvail().x * .9f, false);
							break;
						case 5:
							drag3(mat->_ke, "ke", 0.01f, -FLT_MAX, FLT_MAX, ImGui::GetContentRegionAvail().x * .9f, false);
							break;
						case 6:
							ImGui::DragFloat("##ni", &mat->_ni, 0.01f, -FLT_MAX, FLT_MAX);
							break;
						case 7:
							ImGui::DragFloat("##d", &mat->_d, 0.01f, -FLT_MAX, FLT_MAX);
							break;
						case 8:
							resourcesCombo(*mat, true);
							break;
						case 9:
							imagePopup(*tex);
							break ;
						default: ;
					}
				}
				ImGui::PopID();
			}
			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void Scop::objectDisplay() {
	const std::function<std::vector<std::string>()> loadObjectsNames = [&]() {
		std::vector<std::string> items;
		std::string path = "./resources/objects/";
		for (const auto & entry : std::filesystem::directory_iterator(path))
			if (entry.path().extension() == ".obj" && _objects.find(entry.path().filename()) == _objects.end())
				items.push_back(entry.path().filename());
		return items;
	};
	static std::vector<std::string> items = loadObjectsNames();

	static int selectedObj = 0;
	if (ImGui::Begin("Objects", (bool *)__null)) {
		if (!items.empty() && ImGui::BeginCombo("Unloaded Objects", items[selectedObj].c_str(), 0)) {
			for (int n = 0; n < (int)items.size(); ++n) {
				const bool is_selected = (selectedObj == n);
				if (ImGui::Selectable(items[n].c_str(), is_selected))
					selectedObj = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		if (!items.empty() && ImGui::Button("Add Object")) {
			_objects[items[selectedObj]] = Object("./resources/objects/" + items[selectedObj]);
			_meshes.emplace_back(_objects[items[selectedObj]]);
			Object::loadTextures();
			items = loadObjectsNames();
			selectedObj = 0;
		}
		const char* column_names[] = { "Name", "Vertices", "Triangles", "Textures", "Add" };
		const int columns_count = IM_COUNTOF(column_names);

		static ImGuiTableFlags table_flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersOuter |
			ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_Hideable | ImGuiTableFlags_Resizable |
			ImGuiTableFlags_Borders | ImGuiTableFlags_HighlightHoveredColumn | ImGuiTableFlags_RowBg |
			ImGuiTableFlags_NoHostExtendX;
		static ImGuiTableColumnFlags column_flags = ImGuiTableColumnFlags_AngledHeader |
			ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultSort;

		if (ImGui::BeginTable("table_angled_headers", columns_count, table_flags | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY, ImVec2(0.0f, 200))) {
			ImGui::TableSetupColumn(column_names[0], ImGuiTableColumnFlags_NoHide, 0.f, 0);
			for (int n = 1; n < columns_count; n++)
				ImGui::TableSetupColumn(column_names[n], column_flags, 0.f, n);

			ImGui::TableAngledHeadersRow();
			ImGui::TableHeadersRow();
			for (int row = 0; row < (int)_objects.size(); ++row) {
				Object obj = _objects.at(std::next(_objects.begin(), row)->first);
				ImGui::PushID(row);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("%s", obj.getName().c_str());

				for (int column = 1; column < columns_count; ++column)
					if (ImGui::TableSetColumnIndex(column)) {
						switch (column) {
							case 1:
								ImGui::Text("%zu", obj.getVertices().size());
								break;
							case 2:
								ImGui::Text("%zu", obj.getTotalIndicesCount() / 3);
								break;
							case 3:
								ImGui::Text("%zu", obj.getIndicesGroup().size());
								break;
							case 4:
								if (ImGui::Button("Add Mesh")) {
									_meshes.emplace_back(obj);
									_meshes.back().setPos(Vec3(1));
								}
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

void Scop::meshesDisplay() {
	if (ImGui::Begin("Meshes", (bool *)__null)) {
		ImGui::DragInt("current Edit Mesh ID", &_currentEditMeshID, 1, 0, _meshes.size() - 1);
		_meshes[_currentEditMeshID].imGuiMeshInfos();
	}
	ImGui::End();
}


