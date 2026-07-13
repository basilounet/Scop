//
// Created by bvasseur on 4/27/26.
//

#include "math/Spline.hpp"


/* ==================== CONSTRUCTORS ==================== */

// Shader Spline::_shader = Shader("./src/shaders/splinePreview.vert", "./src/shaders/splinePreview.frag");
Shader Spline::_shader = Shader();

std::unordered_map<Spline::SplineType, Mat4> Spline::_allCharacteristics = {
	{
		LINEAR,
		Mat4(0)
	},
	{
		BEZIER,
		Mat4(
	   -1,  3, -3, 1,
		3, -6,  3, 0,
	   -3,  3,  0, 0,
		1,  0,  0, 0)
	},
	{
		HERMITE,
		Mat4(
			 2,  1, -2,  1,
			-3, -2,  3, -1,
			 0,  1,  0,  0,
			 1,  0,  0,  0
		)
	},
	// {
	// 	CARDINAL,
	// 	float s = 0.5f;
	// 	Mat4(
	// 		 -s,  2-s, s-2,  s,
	// 		2*s, s-3,  3-2*s, -s,
	// 		 -s,  0,  s,  0,
	// 		 0,  1,  0,  0
	// 	)
	// },
	{
		CATMULL,
		Mat4(
			-1, 3, -3, 1,
			2, -5, 4, -1,
			-1, 0, 1, 0,
			0, 2, 0, 0
		) / 2.f
	},
	{
		BSPLINE,
		Mat4(
			-1, 3, -3, 1,
			3, -6, 3, 0,
			-3, 0, 3, 0,
			1, 4, 1, 0
		) / 6.f
	}
};

Spline::Spline() :
	_type(LINEAR),
	_characteristic(_allCharacteristics[_type]),
	_origin({0, 0, 0}),
	_compiled(false),
	_showPreview(true) {
}

Spline::Spline(const SplineType type, const std::vector<Vec3>& vertices) :
		_type(type),
		_vertices(vertices),
		_origin({0, 0, 0}),
		_compiled(false),
		_showPreview(true) {
	_characteristic = _allCharacteristics[type];
	_nbCurves = 1;
	_min = Vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	_max = Vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	if (_vertices.empty())
		return ;
	for (const auto& vertex : vertices)
		_updateMinMaxPos(vertex);
	compile();
	computePreview();
}

Spline::Spline(const Spline &other) {
	*this = other;
}

Spline & Spline::operator==(const Spline &other) {
	if (this != &other) {
		_type = other._type;
		_characteristic = other._characteristic;
		_vertices = other._vertices;
		_catmullTangents = other._catmullTangents;
		_nbCurves = other._nbCurves;
		_origin = other._origin;
		_min = other._min;
		_max = other._max;
		_compiled = other._compiled;
		_pointsPerCurve = other._pointsPerCurve;
		_offset = other._offset;
		_showPreview = other._showPreview;
	}
	return *this;
}

Spline::~Spline() {
}


/* ==================== METHODS ==================== */


void Spline::compile() {
	PRINT BLU "Compiling spline" CENDL;
	_compiled = false;
	if (_vertices.empty())
		throw std::runtime_error("Spline: Cannot compile a spline with no vertices.");
	switch (_type) {
		case LINEAR:
			_pointsPerCurve = 2;
			_nbCurves = _vertices.empty() ? 0 : (_vertices.size() - 1);
			_offset = 1;
			if (overflowAmount() != 0)
				throw std::runtime_error(/*RED*/ "Spline: LINEAR Number of control points must be at least 2 for n curves." /*/RESET*/);
			break;
		case BEZIER:
			_pointsPerCurve = 4;
			_nbCurves = _vertices.empty() ? 0 : (_vertices.size() - 1) / 3;
			_offset = 3;
			if (overflowAmount() != 0)
				throw std::runtime_error(/*RED*/ "Spline: BEZIER Number of control points must be 3n + 1 for n curves." /*RESET*/);
			break;
		case HERMITE:
			_pointsPerCurve = 4;
			_nbCurves = _vertices.size() < 2 ? 0 : (_vertices.size() - 2) / 2;
			_offset = 2;
			if (overflowAmount() != 0)
				throw std::runtime_error(/*RED*/ "Spline: HERMITE Number of control points must be at least 4 and even for n curves." /*RESET*/);
			break;
		case CATMULL:
			_pointsPerCurve = 2;
			_nbCurves = _vertices.empty() ? 0 : (_vertices.size() - 1);
			_offset = 1;
			if (overflowAmount() != 0)
				throw std::runtime_error(/*RED*/ "Spline: CATMULL Number of control points must be at least 2 for n curves." /*RESET*/);
			_catmullTangents[0] = _vertices[0] - (_vertices[1] - _vertices[0]);
			_catmullTangents[1] = _vertices[_vertices.size() - 1] - (_vertices[_vertices.size() - 2] - _vertices[_vertices.size() - 1]);
			break;
		case BSPLINE:
			_pointsPerCurve = 2;
			_nbCurves = _vertices.size() < 3 ? 0 : (_vertices.size() - 3);
			_offset = 1;
			if (overflowAmount() != 0)
				throw std::runtime_error(/*RED*/ "Spline: BSPLINE Number of control points must be at least 2 for n curves." /*RESET*/);
			break;
	}
	_compiled = true;
	computePreview();
}

short Spline::overflowAmount() const {
	switch (_type) {
		case LINEAR:
			if (_vertices.size() < 2)
				return _vertices.size();
			break;
		case BEZIER:
			if (_vertices.size() < 4)
				return _vertices.size();
			if ((_vertices.size() - 1) % 3 != 0)
				return (_vertices.size() - 1) % 3;
			break;
		case HERMITE:
			if (_vertices.size() < 4)
				return _vertices.size();
			if (_vertices.size() % 2 != 0)
				return _vertices.size() % 2;
			break;
		case CATMULL:
			if (_vertices.size() < 2)
				return _vertices.size();
			break;
		case BSPLINE:
			if (_vertices.size() < 2) // TODO : 2 or 4?
				return _vertices.size();
			break;
	}
	return 0;
}

SplineOutput Spline::getPoint(float u) {
	if (!_compiled)
		throw std::runtime_error(/*RED*/ "Spline: Spline must be compiled before it is used." /*RESET*/);

	SplineOutput out{};
	if (u == std::numeric_limits<float>::max())
		u = _nbCurves;
	out.u = u;
	out.intPart = static_cast<int>(std::floor(u));
	out.t = u - static_cast<float>(out.intPart);
	if (out.intPart < 0) {
		out.intPart = 0;
		out.t = 0.f;
	}
	if (out.intPart >= _nbCurves) {
		out.intPart = _nbCurves - 1;
		out.t = 1.f;
	}
	out.intPart *= _offset;
	std::function<Vec3(const Vec4&)> convertToVec3 = [&](const Vec4& v) {
		return Vec3(_vertices[0 + out.intPart]) * v.x +
		   Vec3(_vertices[1 + out.intPart]) * v.y +
		   Vec3(_vertices[2 + out.intPart]) * v.z +
		   Vec3(_vertices[3 + out.intPart]) * v.w;
	};

	if (_type == LINEAR) {
		out.pos = lerp(_vertices[0 + out.intPart], _vertices[1 + out.intPart], out.t) + _origin;
		// derivative of lerp(P0, P1, t) w.r.t t is (P1 - P0)
		out.velocity = _vertices[1 + out.intPart] - _vertices[0 + out.intPart];
		out.dir = normalize(out.velocity);
		out.normal = normalize(cross(out.dir, Vec3(0, 0, 1)));
		out.acceleration = Vec3(0.0f);
		out.jerk = Vec3(0.0f);
		return out;
	}
	if (_type == CATMULL) {
		_vertices.insert(_vertices.begin(), _catmullTangents[0]);
		_vertices.push_back(_catmullTangents[1]);
	}

	const Vec4 T1 = Vec4(std::pow(out.t, 3), std::pow(out.t, 2), out.t, 1);
	const Vec4 Td1 = Vec4(3 * std::pow(out.t, 2), 2 * out.t, 1, 0);
	const Vec4 Td2 = Vec4(6 * out.t, 2, 0, 0);
	const Vec4 Td3 = Vec4(6 , 0, 0, 0);

	out.pos = convertToVec3(_characteristic * T1) + _origin;
	out.velocity = convertToVec3(_characteristic * Td1);
	out.dir = normalize(out.velocity);
	out.normal = normalize(cross(out.dir, Vec3(0, 0, 1)));
	out.acceleration = convertToVec3(_characteristic * Td2);
	out.jerk = convertToVec3(_characteristic * Td3);

	if (_type == CATMULL) {
		_vertices.erase(_vertices.begin());
		_vertices.pop_back();
	}
	return out;
}

void Spline::printTerm(const float u, const bool clear, const std::string& lineType) {
	auto spline = getPoint(u);
	Vec3 A, B, C, D, E;
	if (lineType.contains('I')) {
		A = lerp(_vertices[0 + spline.intPart], _vertices[1 + spline.intPart], spline.t);
		B = lerp(_vertices[1 + spline.intPart], _vertices[2 + spline.intPart], spline.t);
		C = lerp(_vertices[2 + spline.intPart], _vertices[3 + spline.intPart], spline.t);
		D = lerp(A, B, spline.t);
		E = lerp(B, C, spline.t);
	}

	const std::function<int(Vec3)> getColor = [&](const Vec3 pix)->int {
		if (lineType.contains("P") && std::ranges::find_if(_vertices, [&](const Vec3& p)->bool {
			if (_type == CATMULL && (&p == &_vertices[0] || &p == &_vertices[_vertices.size() - 1]))
				return false; // skip the extra control points added for Catmull-Rom
			return Vec3(p) == Vec3(pix.x, pix.y, 0);
		}) != _vertices.end())
			return 0xFFFFFF; // Purple for control points
		if (lineType.contains('N') && _distToLine(pix, spline.pos, (spline.pos + spline.normal * 10.f)) <= 0.5f)
			return 0x2123AD; // Dark blue for the normal vector
		auto endPosVelocity = spline.pos + spline.velocity / 4.5f;
		if (lineType.contains('V') && _distToLine(pix, spline.pos, endPosVelocity) <= 0.5f)
			return 0xE6A139; // Orange-Yellow for the velocity vector
		if (lineType.contains('A') && _distToLine(pix, endPosVelocity, endPosVelocity + spline.acceleration / 7.f) <= 0.5f)
			return 0xCE10E3; // Pink for the acceleration vector
		if (lineType.contains('J') && _distToLine(pix, endPosVelocity, endPosVelocity + spline.jerk / 75.f) <= 0.5f)
			return 0x679630; // Dark green for the jerk vector
		if (lineType.contains('U') &&  distance(pix, spline.pos) <= 2.5f)
			return 0xFF0000; // Red for the point on the curve
		if (lineType.contains('C')) {
			SplineOutput lastSpline = getPoint(0.f);
			for (unsigned int i = 0; i <= _nbCurves * 8; ++i) {
				float tIter = (i / (_nbCurves * 8.f)) * (float)(_nbCurves);
				if (tIter > u && !lineType.contains('F'))
					break;
				SplineOutput out = getPoint(tIter + 0.1f);
				if (_distToLine(pix, lastSpline.pos, out.pos) <= 0.75f) // the curve itself
					return 0xFF << (out.intPart / _offset % 3 * 8);
				lastSpline = out;
			}
		}
		if (lineType.contains('L') && (_type == BEZIER || _type == HERMITE) &&
			(_distToLine(pix, _vertices[0 + spline.intPart], _vertices[1 + spline.intPart]) <= 0.5f ||
			_distToLine(pix, _vertices[2 + spline.intPart], _vertices[3 + spline.intPart]) <= 0.5f))
			return 0xb9BBC7; // Blue for the lines between control points
		if (!lineType.contains('I') || _type != BEZIER)
			return 0x000000;
		if (distance2(pix, A) <= 2.25f || distance2(pix, B) <= 2.25f || distance2(pix, C) <= 2.25f)
			return 0xFF00FF; // Magenta for the first level of interpolation points
		if (distance2(pix, D) <= 2.25f || distance2(pix, E) <= 2.25f)
			return 0xFC7303; // Orange for the second level of interpolation points
		if (_distToLine(pix, A, B) <= 0.5f || _distToLine(pix, B, C) <= 0.5f)
			return 0x00FFFF; // Cyan for the lines between the first level of interpolation
		if (_distToLine(pix, D, E) <= 0.5f)
			return 0xFFFF00; // Yellow for the line between the second level of interpolation

		return 0x000000; // Black for empty space
	};
	std::string str;
	for (int y = _min.y; y <= _max.y; y += 2) {
		for (int x = _min.x; x <= _max.x; ++x) {
			str += RGB_BG + RGB_CODE_INT(getColor(Vec3(x, y, 0)));
			str += RGB_FG + RGB_CODE_INT(getColor(Vec3(x, y + 1, 0))) + DBLOCK;
		}
		str += RESET "\n";
	}
	if (clear)
		str = "\033[" + std::to_string(_max.y - _min.y) + "A\033[H" + str;
	std::cout << str << RESET;

}

void Spline::printVerticesPos() const {
	PRINT "Vertices positions:" CENDL;
	for (auto& vertice : _vertices)
		PRINT vertice.x << "," << vertice.y << "," << vertice.z << "|";
	PRINT "" CENDL;
}

void Spline::addVertex(const Vec3 &pos) {
	_vertices.push_back(pos - _origin);
	_updateMinMaxPos(_vertices.back());
	_compiled = false;
}

void Spline::addVertex(const std::vector<Vec3> &pos) {
	for (auto& vertice : pos)
		addVertex(vertice);
}

void Spline::addVertex(std::string &str) {
	std::stringstream ss(str);
	while (std::getline(ss, str, '|')) {
		std::stringstream ssVertice(str);
		std::string component;
		std::vector<float> components;
		while (std::getline(ssVertice, component, ',')) {
			try {
				components.push_back(std::stof(component));
			}
			catch (const std::exception& e) {
				// throw std::runtime_error("Spline: Invalid vertex format in string.");
				break ;
			}
		}
		if (components.size() != 3)
			continue ; // ! must have exactly xyz
			// throw std::runtime_error("Spline: Each vertex must have exactly 3 components.");
		addVertex(Vec3(components[0], components[1], components[2]));
	}
}

void Spline::randomiseVertices(std::default_random_engine& rng, const Vec2& range) {
	std::uniform_real_distribution<float> x_dist(0, range.x);
	std::uniform_real_distribution<float> y_dist(0, range.y);
	for (auto& vertice : _vertices)
		vertice = Vec3(x_dist(rng), y_dist(rng), 0.f);
}

void Spline::setPoint(const int index, const Vec3 &pos) {
	if (index < 0 || static_cast<size_t>(index) >= _vertices.size())
		throw std::out_of_range("Spline::setPoint: Index out of range." DSTR RESET
								PRP "\nIndex:" CYN " " + std::to_string(index) + RESET " | " YLW "0 - " + std::to_string(_vertices.size() - 1) + RESET);
	_vertices[index] = pos;
	_updateMinMaxPos(pos);
}

void Spline::setMinMax(const Vec3 &min, const Vec3 &max) {
	_min = min;
	_max = max;
}

void Spline::setType(const SplineType type) {
	_type = type;
	_characteristic = _allCharacteristics[type];
	compile();
}


/* ==================== PRIVATE METHODS ==================== */


void Spline::_updateMinMaxPos(const Vec3 &pos) {
	_min.x = std::min(_min.x, pos.x);
	_max.x = std::max(_max.x, pos.x);
	_min.y = std::min(_min.y, pos.y);
	_max.y = std::max(_max.y, pos.y);
	_min.z = std::min(_min.z, pos.z);
	_max.z = std::max(_max.z, pos.z);
}

float Spline::_distToLine(const Vec3 &pos, const Vec3 &start, const Vec3 &end) {
	const Vec3 ab = end - start;
	const Vec3 ac = pos - start;
	if (dot(ab, ac) <= 0.f)
		return length(ac);
	const Vec3 bv = pos - end;
	if (dot(ab, bv) >= 0.f)
		return length(bv);
	return length(cross(ab, ac)) / length(ab);
}



void Spline::computePreview() {
	_renderVertices.clear();
	_vao.deleteVAO();
	if (_VBO)
		glDeleteBuffers(1, &_VBO);
	_VBO = 0;

	if (_vertices.empty())
		return;
	for (unsigned int i = 0; i <= _nbCurves * 20; ++i) {
		float tIter = (i / (_nbCurves * 20.f)) * (float)(_nbCurves);
		_renderVertices.push_back(Vec4(getPoint(tIter).pos, i / (_nbCurves * 20.f)));
	}

	_vao = VAO(true);
	glGenBuffers(1, &_VBO);

	_vao.bind();
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(_renderVertices.size() * sizeof(Vec4)), _renderVertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	_vao.unbind();
}

void Spline::renderPreview(const Camera& camera) {
	if (_showPreview * _VBO * _vao() * _renderVertices.size() == 0)
		return ;

	_shader.activate();
	_vao.bind();
	camera.sendUniforms(_shader);

	Mat4	model(1.f);
	// model = translate(model, _origin);
	// glUniform3f(glGetUniformLocation(_shader.getID(), "translation"), _origin.x, _origin.y, _origin.z);
	glUniformMatrix4fv(glGetUniformLocation(_shader.getID(), "model"), 1, GL_FALSE, model.m);

	glLineWidth((GLfloat)10);

	glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)_renderVertices.size());

    glLineWidth((GLfloat)1.f);
	_vao.unbind();
}

void Spline::destroyPreview() {
	_vao.deleteVAO();
	if (_VBO)
		glDeleteBuffers(1, &_VBO);
	_VBO = 0;

}
