//
// Created by bvasseur on 7/15/25.
//

#include "VBO.hpp"

/* ==================== CONSTRUCTORS ==================== */

VBO::VBO() : _id(0) {
}

VBO::VBO(const std::vector<Vertex>& vertices) : _id(0) {
	glGenBuffers(1, &_id);
	glBindBuffer(GL_ARRAY_BUFFER, _id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

VBO::VBO(const VBO& other) {
	*this = other;
}

VBO::~VBO() {
}

VBO& VBO::operator=(const VBO& other) {
	if (this != &other) {
		_id = other._id;
	}
	return *this;
}

GLuint & VBO::operator()() {
	return _id;
}

void VBO::createEmptyVBO() {
	glGenBuffers(1, &_id);
}

/* ==================== METHODS ==================== */

void VBO::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VBO::unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::deleteVBO() {
	glDeleteBuffers(1, &_id);
	_id = 0;
}
