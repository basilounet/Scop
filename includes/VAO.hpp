//
// Created by bvasseur on 7/15/25.
//

#ifndef VAO_HPP
#define VAO_HPP

# include "glad/glad.h"
# include "VBO.hpp"

class VAO {
private:
	GLuint _id;

public:
	VAO();
	VAO(const VAO &other);
	~VAO();

	VAO &operator=(const VAO &other);

	void linkVBO(VBO VBO, GLuint layout);
	void bind() const;
	void unbind() const;
	void deleteVAO() const;
};



#endif //VAO_HPP
