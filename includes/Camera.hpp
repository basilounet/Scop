//
// Created by bvasseur on 9/3/25.
//

#ifndef SCOP_CAMERA_HPP
# define SCOP_CAMERA_HPP

# include "utils.hpp"
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include "imgui/imgui.h"

# include "Colors.hpp"
# include "Shader.hpp"
# include "math/Mat4.hpp"

# define SHIFT 0b1

class Camera {
public:
	Camera();
	Camera(int width, int height, const Vec3& position, unsigned int* flags);
	Camera(const Camera& other);
	Camera& operator=(const Camera& other);
	~Camera();


	Vec3		getPos() const;
	Vec3		getOrientation() const;
	Vec3		getUp() const;
	float		getSpeed() const;
	float		getTotalSpeed() const;

	void		setWindowSize(int width, int height);

	void		updateMatrix(float FOVDeg, float nearPlane, float farPlane);
	void		matrix(const Shader& shader, const char* uniform);
	void		inputs(GLFWwindow* window, double deltaTime);
	void		inputHooks(GLFWwindow* window, int key, int scancode, int action, int mods);
	void		imGuiDisplay();

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
	unsigned int*	_flags; // ptr to the flags variable in Scop class
};


#endif //SCOP_CAMERA_HPP