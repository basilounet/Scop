//
// Created by bvasseur on 9/4/25.
//

#include "Mesh.hpp"
/* ==================== CONSTRUCTORS ==================== */

Mesh::Mesh() {
}

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<GLuint> &indices,
	const std::vector<Texture> &textures) :
		_vertices(vertices),
		_indices(indices),
		_textures(textures) {
	_vao = VAO();
	_vao.bind();

	_vbo = VBO(_vertices);
	_ebo = EBO(_indices);

	_vao.linkAttrib(_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Position
	_vao.linkAttrib(_vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(GLfloat))); // normal
	_vao.linkAttrib(_vbo, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(GLfloat))); // Color
	_vao.linkAttrib(_vbo, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(GLfloat))); // TexCoord
	_vao.unbind();
	_vbo.unbind();
	_ebo.unbind();

}

Mesh::Mesh(Object &object) {
	*this = Mesh(object.getVertices(), object.getIndicesGroup(), object.getTextures());
}

Mesh::Mesh(const Object &object) {
	*this = Mesh(
	((Object)object).getVertices(),
	((Object)object).getIndicesGroup(),
	((Object)object).getTextures());
}

Mesh::Mesh(const Mesh &other) {
	*this = other;
}

Mesh & Mesh::operator=(const Mesh &other) {
	if (this != &other) {
		_vertices = other._vertices;
		_indices = other._indices;
		_textures = other._textures;
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

	for (unsigned long i = 0; i < _textures.size(); ++i) {
		_textures[i].texUnit(shader, _textures[i].getType() + std::to_string(i), i);
		_textures[i].bind();
	}
	glUniform3f(glGetUniformLocation(shader.getId(), "camPos"), camera.getPos().x, camera.getPos().y, camera.getPos().z);
	glDrawElements(GL_TRIANGLES, _indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::setVertices(const std::vector<Vertex> &vertices) {
	_vertices = vertices;
}

void Mesh::setIndices(const std::vector<GLuint> &indices) {
	_indices = indices;
}

void Mesh::setTextures(const std::vector<Texture>& textures) {
	_textures = textures;
}

void Mesh::destroy() {
	_vao.deleteVAO();
	_vbo.deleteVBO();
	_ebo.deleteEBO();
	for (const Texture& texture : _textures)
		texture.deleteTexture();
}

