//
// Created by bvasseur on 9/3/25.
//

#include "Texture.hpp"

/* ==================== CONSTRUCTORS ==================== */

Texture::Texture() : _imgWidth(0), _imgHeight(0), _numColCh(0), _imgData(nullptr), _textureID(0) {

}

Texture::Texture(const std::string &path) {
	_imgData = stbi_load(path.c_str(), &_imgWidth, &_imgHeight, &_numColCh, 0);
	if (!_imgData)
		throw std::runtime_error("Failed to load image");

	glGenTextures(1, &_textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _imgWidth, _imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, _imgData);
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
	}
	return *this;
}

Texture::~Texture() {
}

void Texture::bind() const {
	glBindTexture(GL_TEXTURE_2D, _textureID);
}

void Texture::unbind() const {
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::deleteTexture() const {
	glDeleteTextures(1, &_textureID);
}


/* ==================== METHODS ==================== */
