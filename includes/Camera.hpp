//
// Created by bvasseur on 9/3/25.
//

#ifndef SCOP_CAMERA_HPP
# define SCOP_CAMERA_HPP

# include <glad/glad.h>
# include <GLFW/glfw3.h>

# include "Colors.hpp"
# include "Shader.hpp"
# include "math/Mat4.hpp"

# define SHIFT 0b1

class Camera {
private:
	Vec3	_pos;
	Vec3	_orientation;
	Vec3	_up;
	Mat4	_cameraMatrix;

	int				_width, _height;
	float			_speed;
	float			_speedModifier;
	float			_multiplier;
	float			_sensitivity;
	unsigned int	_keysPressed;

public:
	Camera();
	Camera(const int width, const int height, const Vec3& position);
	Camera(const Camera& other);
	Camera& operator=(const Camera& other);
	~Camera();


	Vec3		getPos() const;
	Vec3		getOrientation() const;
	Vec3		getUp() const;
	float		getSpeed() const;
	float		getTotalSpeed() const;

	void		updateMatrix(const float FOVDeg, const float nearPlane, const float farPlane);
	void		matrix(const Shader& shader, const char* uniform);
	void		inputs(GLFWwindow* window, const double deltaTime);
	void		inputHooks(GLFWwindow* window, int key, int scancode, int action, int mods);
};


#endif //SCOP_CAMERA_HPP