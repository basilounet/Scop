//
// Created by bvasseur on 9/16/25.
//

#ifndef SCOP_CHARACTERS_HPP
# define SCOP_CHARACTERS_HPP

# include <map>
# include <string>
# include "glm/vec2.hpp"
# include "glm/ext/matrix_clip_space.hpp"
# include <stdexcept>
# include "glad/glad.h"
# include <iostream>
# include "ft2build.h"
# include FT_FREETYPE_H

# include "VAO.hpp"
# include "Shader.hpp"

struct character {
	unsigned int id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

class Characters {
private:
	std::map<unsigned char, character> _characters;

	FT_Library					_ft;
	FT_Face						_face;
	VAO							_vao;
	VBO							_vbo;
	Shader						_shader;
	glm::mat4					_projection;


public:
	Characters();
	Characters(const Characters &other);
	Characters &operator=(const Characters &other);
	~Characters();

	void loadASCII();
	void initializeGL(const float width, const float height);
	void deleteCharacters();

	void render(const std::string& text, float x, float y, float scale, const glm::vec3& color);
};


#endif //SCOP_CHARACTERS_HPP