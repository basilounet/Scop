//
// Created by bvasseur on 7/15/25.
//

#include "VBO.hpp"

/* ==================== CONSTRUCTORS ==================== */

VBO::VBO() : _id(0) {
}

VBO::VBO(GLfloat* vertices, GLsizeiptr size) : _id(0) {
	glGenBuffers(1, &_id);
	glBindBuffer(GL_ARRAY_BUFFER, _id);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

VBO::VBO(const VBO& other) {
	*this = other;
}

VBO::~VBO() {
	// glDeleteBuffers(1, &_id);
}

VBO& VBO::operator=(const VBO& other) {
	if (this != &other) {
		// Copy the necessary data from 'other' if needed
		_id = other._id;
	}
	return *this;
}

/* ==================== METHODS ==================== */

void VBO::bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, _id);
}

void VBO::unbind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::deleteVBO() const {
	glDeleteBuffers(1, &_id);
}
