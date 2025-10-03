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

Mesh::Mesh(Object &object) {
	*this = Mesh(object.getVertices(), object.getIndicesGroup()); // TODO : put references?
}

Mesh::Mesh(const Object &object) {
	*this = Mesh(
	((Object)object).getVertices(),
	((Object)object).getIndicesGroup());
}

Mesh::Mesh(const Mesh &other) {
	*this = other;
}

Mesh & Mesh::operator=(const Mesh &other) {
	if (this != &other) {
		_vertices = other._vertices;
		_indicesGroup = other._indicesGroup;
		_vao = other._vao;
		_vbo = other._vbo;
		_ebo = other._ebo;
	}
	return *this;
}

Mesh::~Mesh() {
}

/* ==================== METHODS ==================== */

void Mesh::draw(const Shader &shader, const Camera &camera) {
	shader.activate();
	_vao.bind();

	size_t offset = 0;
	Texture *texture = nullptr;
	size_t i = 0;
	glUniform3f(glGetUniformLocation(shader.getId(), "camPos"), camera.getPos().x, camera.getPos().y, camera.getPos().z);
	for (auto & it : _indicesGroup) {

		texture = &it.second._material->_mapKdTexture;
		texture->texUnit(shader, texture->getType() + std::to_string(i), i);
		texture->bind();
		glDrawElements(GL_TRIANGLES, it.second._indices.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
		// glDrawElements(GL_LINE, it.second._indices.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
		offset += it.second._indices.size();
		++i;
		texture->unbind();
	}
	_vao.unbind();
}

void Mesh::setVertices(const std::vector<Vertex> &vertices) {
	_vertices = vertices;
}

void Mesh::setIndices(const faceGroupMap &indices) {
	_indicesGroup = indices;
}

void Mesh::destroy() {
	_vao.deleteVAO();
	_vbo.deleteVBO();
	_ebo.deleteEBO();

	for (auto& it : _indicesGroup)
		it.second._material->_mapKdTexture.deleteTexture();
}

