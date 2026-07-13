//
// Created by bvasseur on 7/15/25.
//

#ifndef VBO_HPP
#define VBO_HPP

# include "glad/glad.h"
# include <vector>

# include "math/Vec2.hpp"
# include "math/Vec3.hpp"

struct Vertex {
	Vec3 pos;
	Vec3 normal;
	Vec3 color;
	Vec2 texCoord;
};

class VBO {
private:
	GLuint	_id;

public:
	VBO();
	VBO(const std::vector<Vertex>& vertices);
	VBO(const VBO& other);
	~VBO();

	VBO&	operator=(const VBO& other);
	GLuint& operator()();

	void createEmptyVBO();

	void bind() const;
	void unbind() const;
	void deleteVBO();
};

#endif //VBO_HPP
