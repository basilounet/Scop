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
public:
	Mesh();
	Mesh(const std::string& name, const std::vector<Vertex>& vertices, const faceGroupMap& indicesGroup);
	explicit Mesh(const Object& object);
	Mesh(const Mesh& other);

	Mesh& operator=(const Mesh& other);
	~Mesh();

	const std::string&			getName()				const;
	const std::vector<Vertex>&	getVertices()			const;
	const faceGroupMap&			getIndicesGroup()		const;
	const size_t&				getTotalIndicesCount()	const;

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


private:
	std::vector<Vertex>		_vertices;
	faceGroupMap			_indicesGroups;
	Vec3					_posOffset;
	Vec3					_centerPoint;

	std::string				_name;
	size_t					_totalIndicesCount;

	VAO						_vao;
	VBO						_vbo;
	EBO						_ebo;
};


#endif //SCOP_MESH_HPP