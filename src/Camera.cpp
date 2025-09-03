//
// Created by bvasseur on 9/3/25.
//

#include "Camera.hpp"

/* ==================== CONSTRUCTORS ==================== */

Camera::Camera() :
	_pos(glm::vec3(0.0f, 0.0f, -2.0f)),
	_orientation(glm::vec3(0.0f, 0.0f, -1.0f)),
	_up(glm::vec3(0.0f, 1.0f, 0.0f)),
	_width(1400), _height(800),
	_slowSpeed(4.0f),
	_fastSpeed(15.0f),
	_speed(_slowSpeed),
	_sensitivity(30.0f)
{
}

Camera::Camera(const int width, const int height, const glm::vec3& position) :
	_pos(position),
	_orientation(glm::vec3(0.0f, 0.0f, -1.0f)),
	_up(glm::vec3(0.0f, 1.0f, 0.0f)),
	_width(width), _height(height),
	_slowSpeed(4.0f),
	_fastSpeed(15.0f),
	_speed(_slowSpeed),
	_sensitivity(30.0f)
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
		_width = other._width;
		_height = other._height;
		_speed = other._speed;
		_fastSpeed = other._fastSpeed;
		_sensitivity = other._sensitivity;
	}
	return *this;
}

Camera::~Camera() {
}

/* ==================== METHODS ==================== */

void Camera::matrix(const float FOVDeg, const float nearPlane, const float farPlane, const Shader &shader,
	const char *uniform) {
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);
	view = glm::lookAt(_pos, _pos + _orientation, _up);
	proj = glm::perspective(glm::radians(FOVDeg), (float)_width / (float)_height, nearPlane, farPlane);
	glUniformMatrix4fv(glGetUniformLocation(shader.getId(), uniform), 1, GL_FALSE, glm::value_ptr(proj * view));
}

void Camera::inputs(GLFWwindow *window, const double deltaTime) {
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		_pos += _speed * _orientation * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		_pos += _speed * -_orientation * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		_pos += _speed * -glm::normalize(glm::cross(_orientation, _up)) * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		_pos += _speed * glm::normalize(glm::cross(_orientation, _up)) * (float)deltaTime;

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		_pos += _speed * -_up * (float)deltaTime;
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		_pos += _speed * _up * (float)deltaTime;

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		_speed = _fastSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
		_speed = _slowSpeed;

	// if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	// 	_orientation += (float)deltaTime * glm::rotate(_orientation, glm::radians(_speed * 50), _up);
	// 	// _orientation += (float)deltaTime * _speed* -glm::normalize(glm::cross(_up, _pos));
	// if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	// 	_orientation += (float)deltaTime * -glm::rotate(_orientation, glm::radians(_speed * 50), _up);
	// 	// _orientation += (float)deltaTime * _speed * glm::normalize(glm::cross(_up, _pos));
	// if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && _orientation.y < 0.99f)
	// 	_orientation +=  (float)deltaTime * _speed * _up;
	// if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && _orientation.y > -0.99f)
	// 	_orientation +=  (float)deltaTime * _speed * -_up;
	// _orientation = glm::normalize(_orientation);
	// std::cout << "orientation : [" << _orientation.x << ", " << _orientation.y << ", " << _orientation.z << "]" << std::endl;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	float rotX = _sensitivity * (float)(mouseY - _height / 2) / _height;
	float rotY = _sensitivity * (float)(mouseX - _width / 2) / _width;


	glm::vec3 newOrientation = glm::rotate(_orientation, glm::radians(-rotX), glm::normalize(glm::cross(_orientation, _up)));
	if (!(glm::angle(newOrientation, _up) <= glm::radians(5.0f) || glm::angle(newOrientation, -_up) <= glm::radians(5.0f)))
		_orientation = newOrientation;

	_orientation = glm::rotate(_orientation, glm::radians(-rotY), _up);
	glfwSetCursorPos(window, (double)_width / 2, (double)_height / 2);

}