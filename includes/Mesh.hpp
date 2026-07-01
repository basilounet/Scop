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
	faceGroupMap			_indicesGroup;
	Vec3					_posOffset;
	Vec3					_centerPoint;

	VAO						_vao;
	VBO						_vbo;
	EBO						_ebo;


public:
	Mesh();
	Mesh(const std::vector<Vertex>& vertices, const faceGroupMap& indicesGroup);
	explicit Mesh(Object& object);
	Mesh(const Mesh& other);

	Mesh& operator=(const Mesh& other);
	~Mesh();

	void	createMesh();

	void	draw(const Shader& shader, const Camera& camera);

	void	calculateCenter();
	void	assignTexCoords();
	void	calculateNormals();

	const Vec3&	getCenterPoint()	const;
	const Vec3&	getposOffset()		const;

	void	setVertices(const std::vector<Vertex>& vertices);
	void	setIndices(const faceGroupMap& indices);
	void	setPosOffset(const Vec3& pos);
	void	addPosOffset(const Vec3& pos);

	void	destroy();
};


#endif //SCOP_MESH_HPP