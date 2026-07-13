//
// Created by bvasseur on 9/4/25.
//

#ifndef SCOP_MESH_HPP
# define SCOP_MESH_HPP

# include <vector>
# include <functional>

# include "math/Spline.hpp"
# include "Camera.hpp"
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

	friend Spline;

	const std::string&			getName()				const	{ return _name; }
	const Vec3&					getCenterPoint()		const	{ return _centerPoint; }
	const std::vector<Vertex>&	getVertices()			const	{ return _rawVertices; }
	const faceGroupMap&			getIndicesGroup()		const	{ return _indicesGroups; }
	Vec3&						getPos()						{ return _pos; }
	const size_t&				getTotalIndicesCount()	const	{ return _totalIndicesCount; }
	float&						getOutlineSize()				{ return _outlineSize; }

	void	setVertices(const std::vector<Vertex>& v)	{ _rawVertices = v; }
	void	setIndices(const faceGroupMap& indices);
	void	setPos(const Vec3& pos)						{ _pos = pos; }
	void	addPos(const Vec3& pos)						{ _pos += pos; }
	void	switchFlags(const int flag)					{ _flags ^= flag; }
	void	addFlags(const int flag)					{ _flags |= flag; }

	void	createMesh();

	void	updateStates(const double& deltaTime);
	void	draw(const Shader &shader, const Camera &camera, const Mat4& model, const std::string& type = "mesh", bool forceOutline = false);
	bool	splinePreview(const Camera& camera, float deltaTime);

	void	createFinalVertices();
	void	calculateCenter();
	void	assignTexCoords();
	void	calculateNormals();

	void	imGuiMeshInfos();
	void	imGuiSpline(const Camera& camera);

	void	destroy();


private:
	std::vector<Vertex>		_rawVertices;
	std::vector<Vertex>		_finalVertices;
	faceGroupMap			_indicesGroups;
	Vec3					_pos;
	Vec3					_centerPoint;

	int						_flags;
	float					_useTexPercentage;
	float					_useColorPercentage;
	float					_outlinePercentage;
	float					_useStaticTex;
	float					_outlineSize;

	std::string				_name;
	std::any				_spline; // always a Spline, any for the drag3 func
	float					_u; // spline progress
	float					_splineSpeed;
	Spline::SplineType		_splineType;
	size_t					_totalIndicesCount;

	VAO						_vaoRaw;
	VBO						_vboRaw;
	EBO						_eboRaw;

	VAO						_vaoFinal;
	VBO						_vboFinal;
};


#endif //SCOP_MESH_HPP