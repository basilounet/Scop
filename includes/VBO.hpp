//
// Created by bvasseur on 7/15/25.
//

#ifndef VBO_HPP
#define VBO_HPP

# include "glad/glad.h"

class VBO {
private:
	GLuint	_id;

public:
	VBO();
	VBO(GLfloat* vertices, GLsizeiptr size);
	VBO(const VBO& other);
	~VBO();

	VBO& operator=(const VBO& other);

	void bind() const;
	void unbind() const;
	void deleteVBO() const;
};

#endif //VBO_HPP
