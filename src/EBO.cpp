//
// Created by bvasseur on 7/15/25.
//

#include "EBO.hpp"

/* ==================== CONSTRUCTORS ==================== */

EBO::EBO() : _id(0) {
}

EBO::EBO(GLuint *indices, GLsizeiptr size) : _id(0) {
	glGenBuffers(1, &_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

EBO::EBO(const EBO &other) {
	*this = other;
}

EBO::~EBO() {
	// glDeleteBuffers(1, &_id);
}

EBO &EBO::operator=(const EBO &other) {
	if (this != &other) {
		_id = other._id;
	}
	return *this;
}

/* ==================== METHODS ==================== */

void EBO::bind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _id);
}

void EBO::unbind() const {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::deleteEBO() const {
	glDeleteBuffers(1, &_id);
}
