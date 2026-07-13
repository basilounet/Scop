//
// Created by bvasseur on 4/27/26.
//

#ifndef VOXEL_RPG_BEZIER_HPP
# define VOXEL_RPG_BEZIER_HPP

class Mesh;
class Spline;

# include <unordered_map>
# include <vector>
# include <sstream>
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include <random>
# include <functional>

# include "Colors.hpp"
# include "utils.hpp"
# include "Vec3.hpp"
# include "Vec2.hpp"
# include "Mat4.hpp"
# include "Shader.hpp"
# include "Camera.hpp"
# include "VAO.hpp"


struct SplineOutput {
	float			u;
	unsigned int	intPart;
	float			t;
	Vec3			pos;
	Vec3			velocity;
	Vec3			dir;
	Vec3			normal;
	Vec3			acceleration;
	Vec3			jerk;
};

class Spline {
public:
	static Shader		_shader;

	enum SplineType {
		LINEAR,
		BEZIER,
		HERMITE,
		// CARDINAL,
		CATMULL,
		BSPLINE
	};

	Spline();
	Spline(SplineType type, const std::vector<Vec3>& vertices);
	Spline(const Spline &other);
	Spline& operator==(const Spline &other);
	~Spline();

	friend Mesh;

	void	compile();
	short	overflowAmount() const; // tell if the number of vertices is invalid for the current type

	[[nodiscard]] SplineOutput	getPoint(float u);

/*
* lineType symbols can be :
* "U" for the point on the curve itself at u.
* "C" for the curve.
* "F" for the entire curve at all time, C required (not adding it gradually).
* "P" for control points.
* "L" for control points lines (BEZIER && HERMITE).
* "I" for the levels of interpolation lines of bezier (BEZIER).
* "V" for the velocity of the vector.
* "N" for the normal of the vector.
* "A" for the acceleration of the vector.
* "J" for the Jerk of the vector.
*/
	void	printTerm(float u, bool clear = true, const std::string& lineType = "UCFPL");
	void	printVerticesPos() const;

	void	addVertex(const Vec3 &pos);
	void	addVertex(const std::vector<Vec3> &pos);
	void	addVertex(std::string &str); // expects a string of the form "x1,y1,z1|x2,y2,z2|..." where | separates vertices and, separates components of a vertex
	void	randomiseVertices(std::default_random_engine& rng, const Vec2& range);

	void	setPoint(int index, const Vec3 &pos);
	void	setMinMax(const Vec3 &min, const Vec3 &max);
	void	setOrigin(const Vec3 &origin) { _origin = origin; }
	void	setType(const SplineType type);
	void	setShowPreview(const bool show) { _showPreview = show; }

	std::vector<Vec3>&	getVertices()			{ return _vertices; }
	unsigned int		getNbCurves() const		{ return _nbCurves; }
	bool				isCompiled() const		{ return _compiled; }

	void				computePreview();
	void				renderPreview(const Camera& camera);
	void				destroyPreview();

private:
	void			_updateMinMaxPos(const Vec3 &pos);

	static float	_distToLine(const Vec3 &pos, const Vec3 &start, const Vec3 &end);

	static std::unordered_map<SplineType, Mat4>	_allCharacteristics;

	SplineType			_type;
	Mat4				_characteristic;
	std::vector<Vec3>	_vertices;
	std::array<Vec3, 2>	_catmullTangents; // only used for catmull, stores the tangents at the start and end of the curve
	unsigned int		_nbCurves;
	Vec3				_origin;
	Vec3				_min;
	Vec3				_max;
	bool				_compiled;
	unsigned int		_pointsPerCurve;
	unsigned short		_offset; // offset for points beyond the first curve (e.g. for BEZIER, the first curve uses vertices 0-3, the second curve uses vertices 3-6, etc.)

	bool				_showPreview;
	VAO					_vao;
	GLuint				_VBO = 0;
	std::vector<Vec4>	_renderVertices;
};

#endif //VOXEL_RPG_BEZIER_HPP
