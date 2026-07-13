//
// Created by bvasseur on 7/15/25.
//

#ifndef EBO_H
#define EBO_H

# include "glad/glad.h"
# include <vector>
# include <Object.hpp>

class EBO {
private:
	GLuint	_id;

public:
	EBO();
	EBO(const faceGroupMap& indices);
	EBO(const EBO& other);
	~EBO();

	EBO& operator=(const EBO& other);

	void bind() const;
	void unbind() const;
	void deleteEBO();
};



#endif //EBO_H
