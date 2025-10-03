//
// Created by bvasseur on 9/3/25.
//

#include "Camera.hpp"

/* ==================== CONSTRUCTORS ==================== */

Camera::Camera() :
	_pos(glm::vec3(0.0f, 0.0f, -2.0f)),
	_orientation(glm::vec3(0.0f, 0.0f, -1.0f)),
	_up(glm::vec3(0.0f, 1.0f, 0.0f)),
	_cameraMatrix(glm::mat4(1.0f)),
	_width(1400), _height(800),
	_speed(4.0f),
	_speedModifier(0.0f),
	_multiplier(4.0f),
	_sensitivity(30.0f),
	_shifted(false)
{
}

Camera::Camera(const int width, const int height, const glm::vec3& position) :
	_pos(position),
	_orientation(glm::vec3(0.0f, 0.0f, -1.0f)),
	_up(glm::vec3(0.0f, 1.0f, 0.0f)),
	_cameraMatrix(glm::mat4(1.0f)),
	_width(width), _height(height),
	_speed(4.0f),
	_speedModifier(0.0f),
	_multiplier(4.0f),
	_sensitivity(30.0f),
	_shifted(false)
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
		_shifted = other._shifted;
	}
	return *this;
}

Camera::~Camera() {
}

/* ==================== GETTERS / SETTERS ==================== */

glm::vec3 Camera::getPos() const {
	return _pos;
}

float Camera::getSpeed() const {
	return _speed;
}

float Camera::getTotalSpeed() const {
	return _speed + _speedModifier;
}

/* ==================== METHODS ==================== */

void Camera::updateMatrix(const float FOVDeg, const float nearPlane, const float farPlane) {
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	view = glm::lookAt(_pos, _pos + _orientation, _up);
	proj = glm::perspective(glm::radians(FOVDeg), (float)_width / (float)_height, nearPlane, farPlane);
	_cameraMatrix = proj * view;
}

void Camera::matrix(const Shader &shader, const char *uniform) {
	glUniformMatrix4fv(glGetUniformLocation(shader.getId(), uniform), 1, GL_FALSE, glm::value_ptr(_cameraMatrix));
}

void Camera::inputs(GLFWwindow *window, const double deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && _speedModifier > 0.1f)
		_speedModifier -= 0.1f * (_shifted ? _multiplier : 1.0f);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		_speedModifier += 0.1f * (_shifted ? _multiplier : 1.0f);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * _orientation * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * -_orientation * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * -glm::normalize(glm::cross(_orientation, _up)) * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * glm::normalize(glm::cross(_orientation, _up)) * (float)deltaTime;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * -_up * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		_pos += (_speed + _speedModifier) * _up * (float)deltaTime;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && !_shifted) {
		_shifted = true;
		_speed *= _multiplier;
		_speedModifier *= _multiplier;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE && _shifted) {
		_shifted = false;
		_speed /= _multiplier;
		_speedModifier /= _multiplier;
	}

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



	glm::vec3 newOrientation = glm::rotate(_orientation, glm::radians(-rotY), glm::normalize(glm::cross(_orientation, _up)));
	if (!(glm::angle(newOrientation, _up) <= glm::radians(5.0f) || glm::angle(newOrientation, -_up) <= glm::radians(5.0f)))
		_orientation = newOrientation;

	_orientation = glm::rotate(_orientation, glm::radians(-rotX), _up);
	glfwSetCursorPos(window, (double)_width / 2, (double)_height / 2);
}
