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


class Object;

typedef std::map<std::string, void (Object::*)(const std::vector<std::string>& tokens)> mapFunc;

class Object {
private:
	std::string							_rawData;
	std::vector<Vertex>					_vertices;
	std::vector<GLuint>					_indices;
	std::vector<Texture>				_textures;

public:
	Object();
	Object(const std::string &filepath);
	Object(const Object& other);
	Object& operator=(const Object& other);
	~Object();

	const std::vector<Vertex>&			getVertices();
	const std::vector<GLuint>&			getIndices();
	const std::vector<Texture>&			getTextures();

	void parse(const std::string& filepath);

private:
	static std::vector<std::string>		split(const std::string& str, const std::string& delims);
	void								parseVertex(const std::vector<std::string>& tokens);
	void								parseTexCoord(const std::vector<std::string>& tokens);
	void								parseNormal(const std::vector<std::string>& tokens);
	void								parseFace(const std::vector<std::string>& tokens);
	void								parseMaterial(const std::vector<std::string>& tokens);

	static const mapFunc				_fParser;

};


#endif //SCOP_OBJECT_HPP