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


class Camera {
private:
	glm::vec3	_pos;
	glm::vec3	_orientation;
	glm::vec3	_up;
	glm::mat4	_cameraMatrix;

	int			_width, _height;
	float		_slowSpeed;
	float		_fastSpeed;
	float		_speed;
	float		_sensitivity;

public:
	Camera();
	Camera(const int width, const int height, const glm::vec3& position);
	Camera(const Camera& other);
	Camera& operator=(const Camera& other);
	~Camera();


	glm::vec3	getPos() const;

	void	updateMatrix(const float FOVDeg, const float nearPlane, const float farPlane);
	void	matrix(const Shader& shader, const char* uniform);
	void	inputs(GLFWwindow* window, const double deltaTime);
};


#endif //SCOP_CAMERA_HPP