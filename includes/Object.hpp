//
// Created by bvasseur on 9/5/25.
//

#ifndef SCOP_OBJECT_HPP
# define SCOP_OBJECT_HPP

#include <map>
# include <string>
# include <vector>
# include "glad/glad.h"

# include "Texture.hpp"
# include "VAO.hpp"
# include "Colors.hpp"

# define INT 0
# define POSITIVE_INT 1
# define FLOAT 2

class Object;

typedef std::map<std::string, void (Object::*)(const std::vector<std::string>& tokens, size_t line)> mapFunc;

class Object {
private:
	std::string							_rawData;
	std::vector<Vertex>					_vertices;
	std::vector<GLuint>					_indices;
	std::vector<Texture>				_textures;

	size_t								_vCount;
	size_t								_vtCount;
	size_t								_vnCount;

public:
	Object();
	Object(const std::string &filepath);
	Object(const Object& other);
	Object& operator=(const Object& other);
	~Object();

	const std::vector<Vertex>&			getVertices();
	const std::vector<GLuint>&			getIndices();
	const std::vector<Texture>&			getTextures();

	void								setVertices(const std::vector<Vertex>& vertices);
	void								setIndices(const std::vector<GLuint>& indices);
	void								setTextures(const std::vector<Texture>& textures);

	void parse(const std::string& filepath);

private:
	static std::vector<std::string>		split(const std::string& str, const std::string& delims, const bool keepEmpty = false);
	static void							checkNumber(const std::string& str, const int type, size_t line);
	void								parseVertex(const std::vector<std::string>& tokens, size_t line);
	void								parseTexCoord(const std::vector<std::string>& tokens, size_t line);
	void								parseNormal(const std::vector<std::string>& tokens, size_t line);
	void								parseFace(const std::vector<std::string>& tokens, size_t line);
	void								parseMaterial(const std::vector<std::string>& tokens, size_t line);

	static const mapFunc				_fParser;

};


#endif //SCOP_OBJECT_HPP