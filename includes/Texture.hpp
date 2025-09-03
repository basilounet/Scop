//
// Created by bvasseur on 9/3/25.
//

#ifndef SCOP_TEXTURE_HPP
# define SCOP_TEXTURE_HPP
# include <string>
# include <glad/glad.h>
# include <GLFW/glfw3.h>
# include "stb_image.h"
# include <iostream>


class Texture {
private:
	int				_imgWidth;
	int				_imgHeight;
	int				_numColCh;
	unsigned char*	_imgData;
	GLuint			_textureID;

public:
	Texture();
	Texture(const std::string& path);
	Texture(const Texture& other);
	~Texture();

	Texture& operator=(const Texture& other);

	void bind() const;
	void unbind() const;
	void deleteTexture() const;

};


#endif //SCOP_TEXTURE_HPP