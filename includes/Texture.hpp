//
// Created by bvasseur on 9/3/25.
//

#ifndef SCOP_TEXTURE_HPP
# define SCOP_TEXTURE_HPP

# include <iostream>
# include <string>
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include "stb_image.h"

# include <Shader.hpp>
# include <Colors.hpp>

class Texture {
public:
	Texture();
	Texture(const std::string &path, const std::string& texType, const GLuint slot, const GLenum pixelType);
	Texture(const Texture& other);
	~Texture();

	Texture& operator=(const Texture& other);

	std::string	getType();
	GLuint		getID();
	int			getWidth();
	int			getHeight();

	void bind() const;
	void unbind() const;
	void deleteTexture() const;
	void texUnit(const Shader& shader, const std::string& uniform, const GLuint unit);

private:
	int				_imgWidth;
	int				_imgHeight;
	int				_numColCh;
	unsigned char*	_imgData;
	GLuint			_textureID;
	std::string		_type;
	GLuint			_unit;
};


#endif //SCOP_TEXTURE_HPP