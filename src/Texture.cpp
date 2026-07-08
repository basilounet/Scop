//
// Created by bvasseur on 9/3/25.
//

#include "Texture.hpp"

/* ==================== CONSTRUCTORS ==================== */

Texture::Texture() : _imgWidth(0), _imgHeight(0), _numColCh(0), _imgData(nullptr), _textureID(0), _type("none"), _unit(0) {
// std::cout << "Texture::Texture()" << std::endl;
}

Texture::Texture(const std::string &path, const std::string& texType, const GLuint slot, const GLenum pixelType) {
	_type = texType;
	_imgData = stbi_load(path.c_str(), &_imgWidth, &_imgHeight, &_numColCh, 0);
	if (!_imgData)
		throw std::runtime_error(RED "Failed to load texture: " PRP + path + RESET);

	glGenTextures(1, &_textureID);
	glActiveTexture(GL_TEXTURE0 + slot);
	_unit = slot;
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _imgWidth, _imgHeight, 0,
		path.substr(path.size() - 4) == ".png" ? GL_RGBA : GL_RGB, pixelType, _imgData);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(_imgData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const Texture &other) {
	*this = other;
}

Texture & Texture::operator=(const Texture &other) {
	if (this != &other) {
		_imgWidth = other._imgWidth;
		_imgHeight = other._imgHeight;
		_numColCh = other._numColCh;
		_imgData = other._imgData;
		_textureID = other._textureID;
		_type = other._type;
		_unit = other._unit;
	}
	return *this;
}

Texture::~Texture() {
}

/* ==================== GETTERS / SETTERS ==================== */

std::string Texture::getType() {
	return _type;
}

GLuint Texture::getID() {
	return _textureID;
}

int Texture::getWidth() {
	return _imgWidth;
}

int Texture::getHeight() {
	return _imgHeight;
}

/* ==================== METHODS ==================== */

void Texture::bind() const {
	glActiveTexture(GL_TEXTURE0 +_unit);
	glBindTexture(GL_TEXTURE_2D, _textureID);
}

void Texture::unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::deleteTexture() const {
	glDeleteTextures(1, &_textureID);
}

void Texture::texUnit(const Shader& shader, const std::string& uniform, const GLuint unit) {
	// Gets the location of the uniform
	// std::cout << "Setting texture unit " << unit << " to " << uniform << std::endl;
	// std::cout << "texType: " << _type << "$" << std::endl;
	GLuint texUni = glGetUniformLocation(shader.getId(), uniform.c_str());
	// Shader needs to be activated before changing the value of a uniform
	shader.activate();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}
