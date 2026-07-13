//
// Created by bvasseur on 7/15/25.
//

#include "VAO.hpp"

#include <iostream>
#include <bits/ostream.tcc>

/* ==================== CONSTRUCTORS ==================== */

VAO::VAO() : _id(0) {

}

VAO::VAO(bool b) {
	(void)b;
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

GLuint & VAO::operator()() {
	return _id;
}

/* ==================== METHODS ==================== */

void VAO::linkAttrib(const VBO &VBO,
					const GLuint layout,
					const GLuint nbComponents,
					const GLenum type,
					const GLsizeiptr stride,
					const void *offset) {
	VBO.bind();
	glEnableVertexAttribArray(layout);
	glVertexAttribPointer(layout, nbComponents, type, GL_FALSE, stride, offset);
	VBO.unbind();
}

void VAO::bind() const {
	glBindVertexArray(_id);
}

void VAO::unbind() {
	glBindVertexArray(0);
}

void VAO::deleteVAO() {
	glDeleteVertexArrays(1, &_id);
	_id = 0;
}
