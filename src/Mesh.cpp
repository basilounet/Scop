//
// Created by bvasseur on 9/4/25.
//

#include "Mesh.hpp"


/* ==================== CONSTRUCTORS ==================== */

Mesh::Mesh() {
}

Mesh::Mesh(const std::string& name, const std::vector<Vertex> &vertices, const faceGroupMap& indicesGroup) :
		_vertices(vertices),
		_indicesGroups(indicesGroup),
		_name(name),
		_totalIndicesCount(0) {
	calculateNormals();
	assignTexCoords();
	calculateCenter();
	createMesh();
	for (auto& it: _indicesGroups)
		_totalIndicesCount += it.second._indices.size();
}

Mesh::Mesh(const Object& object) {
	*this = Mesh(object.getName(), object.getVertices(), object.getIndicesGroup());
}

Mesh::Mesh(const Mesh& other) {
	*this = other;
}

Mesh & Mesh::operator=(const Mesh &other) {
	if (this != &other) {
		_vertices = other._vertices;
		_indicesGroups = other._indicesGroups;
		_pos = other._pos;
		_centerPoint = other._centerPoint;
		_totalIndicesCount = other._totalIndicesCount;
		_name = other._name;
		_vao = other._vao;
		_vbo = other._vbo;
		_ebo = other._ebo;
	}
	return *this;
}

Mesh::~Mesh() {
}

const std::string & Mesh::getName() const {
	return _name;
}

const std::vector<Vertex> & Mesh::getVertices() const {
	return _vertices;
}

const faceGroupMap & Mesh::getIndicesGroup() const {
	return _indicesGroups;
}

Vec3 & Mesh::getPos() {
	return _pos;
}

const size_t & Mesh::getTotalIndicesCount() const {
	return _totalIndicesCount;
}


/* ==================== METHODS ==================== */


void Mesh::createMesh() {
	_vao = VAO(true);
	_vao.bind();

	_vbo = VBO(_vertices);
	_ebo = EBO(_indicesGroups);

	_vao.linkAttrib(_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Position
	_vao.linkAttrib(_vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(GLfloat))); // normal
	_vao.linkAttrib(_vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(GLfloat))); // Color
	_vao.linkAttrib(_vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(GLfloat))); // TexCoord
	_vao.unbind();
	_vbo.unbind();
	_ebo.unbind();
}

void Mesh::draw(const Shader &shader, const Camera &camera, const Mat4& model) {
	(void)camera;
	shader.activate();

	_vao.bind();

	size_t offset = 0;
	Texture *texture = nullptr;
	size_t i = 0;

	camera.sendUniforms(shader);
	glUniform3f(glGetUniformLocation(shader.getID(), "translation"), _pos.x, _pos.y, _pos.z);
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "model"), 1, GL_FALSE, model.m);
	glUniform1f(glGetUniformLocation(shader.getID(), "iTime"), glfwGetTime());
	glUniform3f(glGetUniformLocation(shader.getID(), "iResolution"), (float)camera.getWidth(), (float)camera.getHeight(), 0.f);

	for (auto&[fst, snd] : _indicesGroups) {
		texture = &snd._material->_mapKdTexture;
		texture->texUnit(shader, texture->getType() + std::to_string(i), i);
		texture->bind();
		glDrawElements(GL_TRIANGLES, snd._indices.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
		// glDrawElements(GL_LINE, snd._indices.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
		offset += snd._indices.size();
		// PRINT "loop: " << i CENDL;
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

	for (auto &group : _indicesGroups) {
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
		_vertices[i].normal = normalize(_vertices[i].normal);
		// _vertices[i].normal = normalize(_vertices[i].normal).abs();
		_vertices[i].color = _vertices[i].normal;
		// std::cout << "Vertex " YLW"["<<i<<"]" RESET" normal: "
		// << _vertices[i].normal.x << ", " << _vertices[i].normal.y << ", " << _vertices[i].normal.z << std::endl;
	}
}

const Vec3 & Mesh::getCenterPoint() const {
	return _centerPoint;
}

void Mesh::setVertices(const std::vector<Vertex> &vertices) {
	_vertices = vertices;
}

void Mesh::setIndices(const faceGroupMap &indices) {
	_indicesGroups = indices;
	for (auto& it: _indicesGroups)
		_totalIndicesCount += it.second._indices.size();
}

void Mesh::setPos(const Vec3 &pos) {
	// _vao.deleteVAO();
	// _vbo.deleteVBO();
	// _ebo.deleteEBO();
	// std::for_each(_vertices.begin(), _vertices.end(), [&](Vertex &v) {v.position += -_posOffset + pos; });
	_pos = pos;
	// calculateNormals();
	// assignTexCoords();
	// calculateCenter();
	// createMesh();
}

void Mesh::addPos(const Vec3 &pos) {
	_pos += pos;
}

void Mesh::destroy() {
	_vao.deleteVAO();
	_vbo.deleteVBO();
	_ebo.deleteEBO();

	// for (auto&[fst, snd] : _indicesGroup)
		// snd._material->_mapKdTexture.deleteTexture();
}

