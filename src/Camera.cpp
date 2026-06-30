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

Camera::Camera(const int width, const int height, const Vec3& position) :
	_pos(position),
	_orientation(Vec3(0.0f, 0.0f, -1.0f)),
	_up(Vec3(0.0f, 1.0f, 0.0f)),
	_cameraMatrix(Mat4(1.0f)),
	_width(width), _height(height),
	_speed(4.0f),
	_speedModifier(0.0f),
	_multiplier(4.0f),
	_sensitivity(30.0f),
	_keysPressed(0)
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
		_speedModifier -= 1.0f * ((_keysPressed & SHIFT) ? _multiplier : 1.0f) * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		_speedModifier += 1.0f * ((_keysPressed & SHIFT) ? _multiplier : 1.0f) * deltaTime;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * _orientation * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * -_orientation * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * -normalize(cross(_orientation, _up)) * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * normalize(cross(_orientation, _up)) * (float)deltaTime;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * -_up * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * _up * (float)deltaTime;

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	float rotY = _sensitivity * (float)(mouseY - _height / 2) / _height;
	float rotX = _sensitivity * (float)(mouseX - _width / 2) / _width;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rotX -= _speed * (float)deltaTime * 20.0f;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rotX += _speed * (float)deltaTime * 20.0f;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		rotY -= _speed * (float)deltaTime * 20.0f;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		rotY += _speed * (float)deltaTime * 20.0f;
	// std::cout << "orientation : [" << _orientation.x << ", " << _orientation.y << ", " << _orientation.z << "]" << std::endl;



	Vec3 newOrientation = rotate(_orientation, radians(-rotY), normalize(cross(_orientation, _up)));
	if (!(angle(newOrientation, _up) <= radians(5.0f) || angle(newOrientation, -_up) <= radians(5.0f)))
		_orientation = newOrientation;

	_orientation = rotate(_orientation, radians(-rotX), _up);
	glfwSetCursorPos(window, (double)_width / 2, (double)_height / 2);
}

void Camera::inputHooks(GLFWwindow *window, int key, int scancode, int action, int mods) {
	(void)window, (void)key, (void)scancode, (void)action, (void)mods;
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
		if (_keysPressed & SHIFT) {
			_speed /= _multiplier;
			_speedModifier /= _multiplier;
		}
		else {
			_speed *= _multiplier;
			_speedModifier *= _multiplier;
		}
		_keysPressed ^= SHIFT;
	}
}
