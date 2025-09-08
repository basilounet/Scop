//
// Created by bvasseur on 9/4/25.
//

#ifndef SCOP_MESH_HPP
# define SCOP_MESH_HPP

#include <vector>

#include "Camera.hpp"
# include "VAO.hpp"
# include "VBO.hpp"
# include "EBO.hpp"
# include "Shader.hpp"
# include "Texture.hpp"
# include "Object.hpp"


class Mesh {
private:
	std::vector<Vertex>		_vertices;
	std::vector<GLuint>		_indices;
	std::vector<Texture>	_textures;

	VAO						_vao;
	VBO						_vbo;
	EBO						_ebo;


public:
	Mesh();
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures);
	explicit Mesh(Object& object);
	explicit Mesh(const Object & object);
	Mesh(const Mesh& other);

	Mesh& operator=(const Mesh& other);
	~Mesh();

	void draw(const Shader& shader, const Camera& camera);

	void destroy();
};


#endif //SCOP_MESH_HPP