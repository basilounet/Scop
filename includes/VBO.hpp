//
// Created by bvasseur on 7/15/25.
//

#ifndef VBO_HPP
#define VBO_HPP

# include "glad/glad.h"
# include <glm/glm.hpp>
# include <vector>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texCoord;
};

class VBO {
private:
	GLuint	_id;

public:
	VBO();
	VBO(const std::vector<Vertex>& vertices);
	VBO(const VBO& other);
	~VBO();

	VBO& operator=(const VBO& other);

	void bind() const;
	void unbind() const;
	void deleteVBO() const;
};

#endif //VBO_HPP
