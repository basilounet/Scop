//
// Created by bvasseur on 9/3/25.
//

#include "Camera.hpp"

/* ==================== CONSTRUCTORS ==================== */

Camera::Camera() :
	_pos(Vec3(0.0f, 0.0f, -2.0f)),
	_orientation(Vec3(0.0f, 0.0f, -1.0f)),
	_up(Vec3(0.0f, 1.0f, 0.0f)),
	_cameraMatrix(Mat4(1.0f)),
	_width(1400), _height(800),
	_speed(4.0f),
	_speedModifier(0.0f),
	_multiplier(4.0f),
	_sensitivity(30.0f),
	_keysPressed(0)
{
}

Camera::Camera(const int width, const int height, const Vec3& position, unsigned int* flags) :
	_pos(position),
	_orientation(Vec3(0.0f, 0.0f, -1.0f)),
	_up(Vec3(0.0f, 1.0f, 0.0f)),
	_cameraMatrix(Mat4(1.0f)),
	_width(width), _height(height),
	_speed(4.0f),
	_speedModifier(0.0f),
	_multiplier(4.0f),
	_sensitivity(30.0f),
	_keysPressed(0),
	_flags(flags)
{
}

Camera::Camera(const Camera &other) {
	*this = other;
}

Camera & Camera::operator=(const Camera &other) {
	if (this != &other) {
		_pos = other._pos;
		_orientation = other._orientation;
		_up = other._up;
		_cameraMatrix = other._cameraMatrix;
		_width = other._width;
		_height = other._height;
		_speed = other._speed;
		_speedModifier = other._speedModifier;
		_multiplier = other._multiplier;
		_sensitivity = other._sensitivity;
		_keysPressed = other._keysPressed;
		_flags = other._flags;
	}
	return *this;
}

Camera::~Camera() {
}

/* ==================== GETTERS / SETTERS ==================== */

Vec3 Camera::getPos() const {
	return _pos;
}

Vec3 Camera::getOrientation() const {
	return _orientation;
}

Vec3 Camera::getUp() const {
	return _up;
}

float Camera::getSpeed() const {
	return _speed;
}

float Camera::getTotalSpeed() const {
	return _speed + _speedModifier;
}

void Camera::setWindowSize(const int width, const int height) {
	_width = width;
	_height = height;
}

/* ==================== METHODS ==================== */

void Camera::updateMatrix(const float FOVDeg, const float nearPlane, const float farPlane) {
	Mat4 viewM = Mat4(1.0f);
	Mat4 projM = Mat4(1.0f);
	viewM = lookAt(_pos, _pos + _orientation, _up);
	projM = perspective(FOVDeg, (float)_width / (float)_height, nearPlane, farPlane);
	_cameraMatrix = projM * viewM;
}

void Camera::matrix(const Shader &shader, const char *uniform) {
	glUniformMatrix4fv(glGetUniformLocation(shader.getId(), uniform), 1, GL_FALSE, _cameraMatrix.m);
}

void Camera::inputs(GLFWwindow *window, const double deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && _speedModifier > 0.1f)
		_speedModifier -= 1.0f * ((_keysPressed & SHIFT) ? _multiplier : 1.0f) * deltaTime * 2.f;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		_speedModifier += 1.0f * ((_keysPressed & SHIFT) ? _multiplier : 1.0f) * deltaTime * 2.f;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && _speedModifier > 0.1f)
		_multiplier -= 1.0f * ((_keysPressed & SHIFT) ? _multiplier : 1.0f) * deltaTime * 2.f;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		_multiplier += 1.0f * ((_keysPressed & SHIFT) ? _multiplier : 1.0f) * deltaTime * 2.f;


	float speed = (_speed + _speedModifier) * ((_keysPressed & SHIFT) ? _multiplier : 1.0f);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		_pos +=  speed * _orientation * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		_pos += speed * -_orientation * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		_pos += speed * -normalize(cross(_orientation, _up)) * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		_pos += speed * normalize(cross(_orientation, _up)) * (float)deltaTime;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		_pos += speed * normalize(cross(_orientation, normalize(cross(_orientation, _up)))) * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		_pos += speed * -normalize(cross(_orientation, normalize(cross(_orientation, _up)))) * (float)deltaTime;

	double mouseX, mouseY, rotX = 0, rotY = 0;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rotX -= _sensitivity * .15f * (float)deltaTime * 20.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rotX += _sensitivity * .15f * (float)deltaTime * 20.0f;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		rotY -= _sensitivity * .15f * (float)deltaTime * 20.0f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		rotY += _sensitivity * .15f * (float)deltaTime * 20.0f;

	if (!(*_flags & LOCK_MOUSE)) {
		glfwGetCursorPos(window, &mouseX, &mouseY);
		rotY += _sensitivity * (float)(mouseY - _height / 2) / _height;
		rotX += _sensitivity * (float)(mouseX - _width / 2) / _width;
		glfwSetCursorPos(window, (double)_width / 2, (double)_height / 2);
	}

	Vec3 newOrientation = rotate(_orientation, radians(-rotY), normalize(cross(_orientation, _up)));
	if (!(angle(newOrientation, _up) <= radians(5.0f) || angle(newOrientation, -_up) <= radians(5.0f)))
		_orientation = newOrientation;

	_orientation = rotate(_orientation, radians(-rotX), _up);
}

void Camera::inputHooks(GLFWwindow *window, int key, int scancode, int action, int mods) {
	(void)window, (void)key, (void)scancode, (void)action, (void)mods;
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		_keysPressed ^= SHIFT;
	}
	if ((key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER || key == GLFW_MOUSE_BUTTON_MIDDLE)
		&& action == GLFW_PRESS) {
		*_flags ^= LOCK_MOUSE;
		// PRINT "Mouse lock : " << (*_flags & LOCK_MOUSE ? "ON" : "OFF") << " | " << ((*_flags & LOCK_MOUSE) == 0) CENDL;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL + ((*_flags & LOCK_MOUSE) == 0));
		if (!(*_flags & LOCK_MOUSE))
			glfwSetCursorPos(window, (double)_width / 2, (double)_height / 2);
	}
}

void Camera::imGuiDisplay() {
	static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_Hideable | ImGuiTableFlags_NoHostExtendX;
	if (ImGui::BeginTable("table1", 6, flags)) {
		ImGui::TableSetupColumn("Pos X", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Pos Y", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Pos Z", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Rot X", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Rot Y", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Rot Z", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();
		ImGui::TableNextRow();
		for (int column = 0;  column < 6; column++) {
			ImGui::TableSetColumnIndex(column);
			ImGui::PushItemWidth(-FLT_MIN);
			ImGui::DragFloat(("##" + std::to_string(column)).c_str(),
				[&]()->float*{switch (column) {
					case 0: return &_pos.x;
					case 1: return &_pos.y;
					case 2: return &_pos.z;
					case 3: return &_orientation.x;
					case 4: return &_orientation.y;
					case 5: return &_orientation.z;
					default: return nullptr;
				}}(),
				0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
		}
		ImGui::EndTable();
	}
	ImGui::Text("Speed = (base speed + modifier) * (shift ? mult : 1)");
	if (ImGui::BeginTable("table2", 4, flags)) {
		ImGui::TableSetupColumn("Speed (total)", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Modifier (1/2)", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Mult (3/4)", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Sensitivity", ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableHeadersRow();
		ImGui::TableNextRow();
		for (int column = 0;  column < 4; column++) {
			ImGui::TableSetColumnIndex(column);
			ImGui::PushItemWidth(-FLT_MIN);
			switch (column) {
				case 0:
					ImGui::Text("%.3f", (_speed + _speedModifier) * ((_keysPressed & SHIFT) ? _multiplier : 1.0f));
					break;
				case 1:
					ImGui::DragFloat("##modifier", &_speedModifier, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
					break;
				case 2:
					ImGui::DragFloat("##multiplier", &_multiplier, 0.005f, -FLT_MAX, +FLT_MAX, "%.3f");
					break;
				case 3:
					ImGui::DragFloat("##sensitivity", &_sensitivity, 0.015f, -FLT_MAX, +FLT_MAX, "%.3f");
					break;
				default:
					break;
			}
		}
		ImGui::EndTable();
	}
}
