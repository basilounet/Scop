//
// Created by bvasseur on 7/15/25.
//

#include "VAO.hpp"

#include <iostream>
#include <bits/ostream.tcc>

/* ==================== CONSTRUCTORS ==================== */

VAO::VAO() : _id(0) {
	if (glGenVertexArrays)
		glGenVertexArrays(1, &_id);
}

VAO::VAO(const VAO &other) {
	*this = other;
}

VAO::~VAO() {
	// glDeleteVertexArrays(1, &_id);
}

VAO &VAO::operator=(const VAO &other) {
	if (this != &other) {
		_id = other._id;
	}
	return *this;
}

/* ==================== METHODS ==================== */

void VAO::linkAttrib(VBO &VBO, GLuint layout, GLuint nbComponents, GLenum type, GLsizeiptr stride, void *offset) {
	VBO.bind();
	glVertexAttribPointer(layout, nbComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.unbind();
}

void VAO::bind() const {
	glBindVertexArray(_id);
}

void VAO::unbind() const {
	glBindVertexArray(0);
}

void VAO::deleteVAO() const {
	glDeleteVertexArrays(1, &_id);
}
