//
// Created by bvasseur on 9/3/25.
//

#ifndef SCOP_CAMERA_HPP
# define SCOP_CAMERA_HPP

# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include <glm/glm.hpp>
# include <glm/gtc/matrix_transform.hpp>
# include <glm/gtc/type_ptr.hpp>
# include <glm/gtx/rotate_vector.hpp>
# include <glm/gtx/vector_angle.hpp>

# include "Shader.hpp"

# define SHIFT 0b1

class Camera {
private:
	glm::vec3	_pos;
	glm::vec3	_orientation;
	glm::vec3	_up;
	glm::mat4	_cameraMatrix;

	int				_width, _height;
	float			_speed;
	float			_speedModifier;
	float			_multiplier;
	float			_sensitivity;
	unsigned int	_keysPressed;

public:
	Camera();
	Camera(const int width, const int height, const glm::vec3& position);
	Camera(const Camera& other);
	Camera& operator=(const Camera& other);
	~Camera();


	glm::vec3	getPos() const;
	float		getSpeed() const;
	float		getTotalSpeed() const;

	void		updateMatrix(const float FOVDeg, const float nearPlane, const float farPlane);
	void		matrix(const Shader& shader, const char* uniform);
	void		inputs(GLFWwindow* window, const double deltaTime);
	void		inputHooks(GLFWwindow* window, int key, int scancode, int action, int mods);
};


#endif //SCOP_CAMERA_HPP