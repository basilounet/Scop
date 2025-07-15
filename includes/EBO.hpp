//
// Created by bvasseur on 7/15/25.
//

#ifndef EBO_H
#define EBO_H

# include "glad/glad.h"

class EBO {
private:
	GLuint	_id;

public:
	EBO();
	EBO(GLuint* indices, GLsizeiptr size);
	EBO(const EBO& other);
	~EBO();

	EBO& operator=(const EBO& other);

	void bind() const;
	void unbind() const;
	void deleteEBO() const;
};



#endif //EBO_H
