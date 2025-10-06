//
// Created by bvasseur on 9/16/25.
//

#include "Characters.hpp"

#include "glm/gtc/type_ptr.hpp"


/* ==================== CONSTRUCTORS ==================== */


Characters::Characters() {
}

Characters::Characters(const Characters &other) {
	*this = other;
}

Characters & Characters::operator=(const Characters &other) {
	if (this != &other) {
		_characters = other._characters;
		_ft = other._ft;
		_face = other._face;
		_vao = other._vao;
		_vbo = other._vbo;
		_shader = other._shader;
		_projection = other._projection;
	}
	return *this;
}

Characters::~Characters() {
}


/* ==================== METHODS ==================== */


void Characters::loadASCII() {
	if (FT_Init_FreeType(&_ft))
		throw std::runtime_error("Could not init FreeType Library");
	if (FT_New_Face(_ft, "resources/fonts/pixel_code/fonts/PixelCode.otf", 0, &_face))
		throw std::runtime_error("Could not load font");
	FT_Set_Pixel_Sizes(_face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

	for (unsigned char c = 0; c < 128; ++c) {
		if (FT_Load_Char(_face, c, FT_LOAD_RENDER)) {
			std::cerr << "Could not load Glyph: \'" << c << "\'" << std::endl;
			continue ;
		}
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			_face->glyph->bitmap.width,
			_face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			_face->glyph->bitmap.buffer
		);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		character character = {
			texture,
			glm::ivec2(_face->glyph->bitmap.width, _face->glyph->bitmap.rows),
			glm::ivec2(_face->glyph->bitmap_left, _face->glyph->bitmap_top),
			static_cast<unsigned int>(_face->glyph->advance.x)
		};
		_characters[c] = character;
	}
}

void Characters::initializeGL(const float width, const float height) {
	_shader = Shader("./src/shaders/text.vert", "./src/shaders/text.frag");
	_projection = glm::ortho(0.0f, width, 0.0f, height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	_vao = VAO(true);
	_vbo.createEmptyVBO();
	_vao.bind();
	_vbo.bind();
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	_vao.linkAttrib(_vbo, 0, 4, GL_FLOAT, 4 * sizeof(float), (void*)0);
	_vbo.unbind();
	_vao.unbind();
	_shader.activate();
	glUniformMatrix4fv(glGetUniformLocation(_shader.getId(), "projection"), 1, GL_FALSE, glm::value_ptr(_projection));
}

void Characters::deleteCharacters() {
	_vao.deleteVAO();
	_vbo.deleteVBO();
	FT_Done_Face(_face);
	FT_Done_FreeType(_ft);
}

void Characters::render(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
	// activate corresponding render state
	_shader.activate();
	glUniform3f(glGetUniformLocation(_shader.getId(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	_vao.bind();

	// iterate through all characters
	for (const auto c : text) {
		if (_characters.find(c) == _characters.end())
			continue ;
		character ch = _characters[c];

		const float xPos = x + ch.bearing.x * scale;
		const float yPos = y - static_cast<float>(ch.size.y - ch.bearing.y) * scale;

		const float w = ch.size.x * scale;
		const float h = ch.size.y * scale;
		// update VBO for each character
		const float vertices[6][4] = {
			{ xPos,     yPos + h,   0.0f, 0.0f },
			{ xPos,     yPos,       0.0f, 1.0f },
			{ xPos + w, yPos,       1.0f, 1.0f },
			{ xPos,     yPos + h,   0.0f, 0.0f },
			{ xPos + w, yPos,       1.0f, 1.0f },
			{ xPos + w, yPos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.id);
		// update content of VBO memory
		_vbo.bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		_vbo.unbind();
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += static_cast<float>(ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	_vao.unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
}
