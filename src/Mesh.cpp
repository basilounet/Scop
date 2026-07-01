//
// Created by bvasseur on 9/4/25.
//

#include "Mesh.hpp"


/* ==================== CONSTRUCTORS ==================== */

Mesh::Mesh() {
}

Mesh::Mesh(const std::vector<Vertex> &vertices, const faceGroupMap& indicesGroup) :
		_vertices(vertices),
		_indicesGroup(indicesGroup) {
	calculateNormals();
	assignTexCoords();
	calculateCenter();
	createMesh();
}

Mesh::Mesh(Object& object) {
	*this = Mesh(object.getVertices(), object.getIndicesGroup()); // TODO : put references?
}

Mesh::Mesh(const Mesh& other) {
	*this = other;
}

Mesh & Mesh::operator=(const Mesh &other) {
	if (this != &other) {
		_vertices = other._vertices;
		_indicesGroup = other._indicesGroup;
		_posOffset = other._posOffset;
		_centerPoint = other._centerPoint;
		_vao = other._vao;
		_vbo = other._vbo;
		_ebo = other._ebo;
	}
	return *this;
}

Mesh::~Mesh() {
}


/* ==================== METHODS ==================== */


void Mesh::createMesh() {
	_vao = VAO(true);
	_vao.bind();

	_vbo = VBO(_vertices);
	_ebo = EBO(_indicesGroup);

	_vao.linkAttrib(_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Position
	_vao.linkAttrib(_vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(GLfloat))); // normal
	_vao.linkAttrib(_vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(GLfloat))); // Color
	_vao.linkAttrib(_vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(GLfloat))); // TexCoord
	_vao.unbind();
	_vbo.unbind();
	_ebo.unbind();
}

void Mesh::draw(const Shader &shader, const Camera &camera) {
	(void)camera;
	shader.activate();

	_vao.bind();

	size_t offset = 0;
	Texture *texture = nullptr;
	size_t i = 0;
	for (auto&[fst, snd] : _indicesGroup) {
		texture = &snd._material->_mapKdTexture;
		texture->texUnit(shader, texture->getType() + std::to_string(i), i);
		texture->bind();
		glDrawElements(GL_TRIANGLES, snd._indices.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
		// glDrawElements(GL_LINE, snd._indices.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
		offset += snd._indices.size();
		++i;
		texture->unbind();
	}
	_vao.unbind();
}

void Mesh::calculateCenter() {
	Vec3 max = Vec3(
		std::max_element(_vertices.begin(), _vertices.end(),
			[](const Vertex &a, const Vertex &b) {
			return a.position.x < b.position.x;})->position.x,
		std::max_element(_vertices.begin(), _vertices.end(),
			[](const Vertex &a, const Vertex &b) {
			return a.position.y < b.position.y; })->position.y,
		std::max_element(_vertices.begin(), _vertices.end(),
			[](const Vertex &a, const Vertex &b) {
			return a.position.z < b.position.z;	})->position.z);
	Vec3 min = Vec3(
	std::min_element(_vertices.begin(), _vertices.end(),
		[](const Vertex &a, const Vertex &b) {
		return a.position.x < b.position.x;})->position.x,
	std::min_element(_vertices.begin(), _vertices.end(),
		[](const Vertex &a, const Vertex &b) {
		return a.position.y < b.position.y; })->position.y,
	std::min_element(_vertices.begin(), _vertices.end(),
		[](const Vertex &a, const Vertex &b) {
		return a.position.z < b.position.z;	})->position.z);
	_centerPoint = (min + max) / 2.0f;
}

void Mesh::assignTexCoords() {
	for (Vertex & v : _vertices) {
		v.texCoord = Vec2(v.position.z, v.position.y);
	}
}

void Mesh::calculateNormals() {
	std::vector<GLuint> indicesBuffer;

	for (auto &group : _indicesGroup) {
		indicesBuffer.insert(indicesBuffer.end(), group.second._indices.begin(), group.second._indices.end());
	}
	// for (auto & vertice : _vertices) {
	// vertice.normal = Vec3(0, 0, 0);
	// }
	for (size_t i = 0; i < indicesBuffer.size() - 2; i += 3) {
		Vec3 p = cross(
			_vertices[indicesBuffer[i + 1]].position - _vertices[indicesBuffer[i]].position,
			_vertices[indicesBuffer[i + 2]].position - _vertices[indicesBuffer[i]].position);
		// std::cout << "Face" YLW" [" <<indicesBuffer[i]<<", "<<indicesBuffer[i + 1]<<", "<<indicesBuffer[i + 2]<<
		// "]" RESET " product: " << p.x << ", " << p.y << ", " << p.z << std::endl;
		_vertices[indicesBuffer[i]].normal += p;
		_vertices[indicesBuffer[i + 1]].normal += p;
		_vertices[indicesBuffer[i + 2]].normal += p;

	}
	for (size_t i = 0; i < _vertices.size(); ++i) {
		// if (_vertices[i].normal.x == 0 && _vertices[i].normal.y == 0 && _vertices[i].normal.z == 0)
		// _vertices[i].normal = Vec3(0.0f, 0.0f, 1.0f);
		_vertices[i].normal = normalize(_vertices[i].normal).abs();
		_vertices[i].color = _vertices[i].normal;
		// std::cout << "Vertex " YLW"["<<i<<"]" RESET" normal: "
		// << _vertices[i].normal.x << ", " << _vertices[i].normal.y << ", " << _vertices[i].normal.z << std::endl;
	}
}

const Vec3 & Mesh::getCenterPoint() const {
	return _centerPoint;
}

const Vec3 & Mesh::getposOffset() const {
	return _posOffset;
}

void Mesh::setVertices(const std::vector<Vertex> &vertices) {
	_vertices = vertices;
}

void Mesh::setIndices(const faceGroupMap &indices) {
	_indicesGroup = indices;
}

void Mesh::setPosOffset(const Vec3 &pos) {
	// _vao.deleteVAO();
	// _vbo.deleteVBO();
	// _ebo.deleteEBO();
	// std::for_each(_vertices.begin(), _vertices.end(), [&](Vertex &v) {v.position += -_posOffset + pos; });
	_posOffset = pos;
	// calculateNormals();
	// assignTexCoords();
	// calculateCenter();
	// createMesh();
}

void Mesh::addPosOffset(const Vec3 &pos) {
	_posOffset += pos;
}

void Mesh::destroy() {
	_vao.deleteVAO();
	_vbo.deleteVBO();
	_ebo.deleteEBO();

	for (auto&[fst, snd] : _indicesGroup)
		snd._material->_mapKdTexture.deleteTexture();
}

