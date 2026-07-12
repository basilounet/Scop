//
// Created by bvasseur on 6/30/26.
//

#ifndef SCOP_SKYBOX_HPP
# define SCOP_SKYBOX_HPP

# include <array>

# include "Texture.hpp"
# include "Camera.hpp"
# include "VAO.hpp"


class Skybox {
public:
	Skybox(int width = 1400, int height = 800);
	Skybox(const Skybox& other);
	~Skybox();
	Skybox& operator=(const Skybox& other);

	static const std::vector<Vertex>&	getVertices();
	static const std::vector<GLuint>&	getIndices();

	void	createSkybox();
	void	drawSkybox(const Camera &camera);

	void	destroy();

private:
	int		_width, _height;
	GLuint	_textureID;
	VAO		_vao;
	VBO		_vbo;
	GLuint	_ebo;
	Shader	_shader;

	static std::vector<Vertex>	_vertices;
	static std::vector<GLuint>	_indices;
};


#endif //SCOP_SKYBOX_HPP