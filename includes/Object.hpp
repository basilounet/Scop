//
// Created by bvasseur on 9/5/25.
//

#ifndef SCOP_OBJECT_HPP
# define SCOP_OBJECT_HPP

#include <map>
# include <string>
# include <vector>
# include "glad/glad.h"
# include "glm/glm.hpp"
# include <filesystem>

# include "Texture.hpp"
# include "VAO.hpp"
# include "Colors.hpp"

# define INT 0
# define POSITIVE_INT 1
# define FLOAT 2

class Object;

struct MaterialData {
	float		_ns;
	glm::vec3	_ka;
	glm::vec3	_kd;
	glm::vec3	_ks;
	glm::vec3	_ke;
	float		_ni;
	float		_d;
	std::string	_mapKd = "./resources/textures/image.png";
};

typedef std::map<std::string, void (Object::*)(const std::vector<std::string>& tokens, size_t line)> mapFunc;
typedef std::map<std::string, MaterialData> matMap;


class Object {
private:
	static matMap						_materials;

	std::string							_rawData;
	std::string							_objPath;
	std::vector<Vertex>					_vertices;
	std::vector<GLuint>					_indices;
	std::vector<Texture>				_textures;

	MaterialData						_currentMaterial;
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

	void parse(const std::string& filepath, const mapFunc& func = _objFunctionParser, size_t lineCount = 0);

private:
	static std::vector<std::string>		split(const std::string& str, const std::string& delims, const bool keepEmpty = false);
	static void							checkNumber(const std::string& str, const int type, size_t lineCount);

	void								parseVertex(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseTexCoord(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseNormal(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseFace(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseMaterialLib(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseUseMaterial(const std::vector<std::string>& tokens, size_t lineCount);

	void								parseNewMaterial(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseNs(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseKa(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseKd(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseKs(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseKe(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseNi(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseD(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseMapKd(const std::vector<std::string>& tokens, size_t lineCount);
	const MaterialData&					getMaterial(const std::string &name) const;

	static const mapFunc				_objFunctionParser;
	static const mapFunc				_matFunctionParser;

};


#endif //SCOP_OBJECT_HPP