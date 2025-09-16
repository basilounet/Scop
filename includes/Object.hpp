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
# include "utils.hpp"
# include <algorithm>

# define INT 0
# define FLOAT 1
# define POSITIVE 2
# define NEGATIVE 3
# define ANY 4

class Object;

struct MaterialData {
	std::string	_name;
	float		_ns;
	glm::vec3	_ka;
	glm::vec3	_kd;
	glm::vec3	_ks;
	glm::vec3	_ke;
	float		_ni;
	float		_d;
	std::string	_mapKd = "jiji.png";
	Texture		_mapKdTexture;
};

struct FaceGroup {
	std::vector<GLuint> _indices;
	MaterialData*		_material = nullptr;
};

typedef std::map<std::string, void (Object::*)(const std::vector<std::string>& tokens, size_t line)> mapFunc;
typedef std::map<std::string, MaterialData> matMap;
typedef std::map<std::string, FaceGroup> faceGroupMap;

class Object {
private:
	static matMap						_materials;
	static std::string					_texturePath;

	std::string							_rawData;
	std::string							_objPath;
	std::vector<Vertex>					_vertices;
	faceGroupMap						_indicesGroups;

	MaterialData*						_currentMaterial;
	MaterialData*						_currentParsingMaterial;
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
	const faceGroupMap&					getIndicesGroup();

	void								setVertices(const std::vector<Vertex>& vertices);
	void								setIndicesGroup(const faceGroupMap &indices);

	static void							createTexture(); // to be called after parsing all .mtl files

private:
	void								parse(const std::string& filepath,
												const mapFunc& func = _objFunctionParser,
												size_t lineCount = 0);
	void								calculateNormals();
	void								assignTexCoords();
	static std::vector<std::string>		split(const std::string& str, const std::string& delims, const bool keepEmpty = false);
	static void							checkNumber(const std::string& str, const int type, const int sign, size_t lineCount);

	void								parseVertex(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseTexCoord(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseNormal(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseFace(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseGroup(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseMaterialLib(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseUseMaterial(const std::vector<std::string>& tokens, size_t lineCount);

	void								parseNewMaterial(const std::vector<std::string>& tokens, size_t lineCount);
	void								checkCurrentParsingMaterial(size_t lineCount);
	void								parseNs(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseKa(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseKd(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseKs(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseKe(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseNi(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseD(const std::vector<std::string>& tokens, size_t lineCount);
	void								parseMapKd(const std::vector<std::string>& tokens, size_t lineCount);
	static MaterialData*			getMaterial(const std::string &name);

	static const mapFunc				_objFunctionParser;
	static const mapFunc				_matFunctionParser;

};


#endif //SCOP_OBJECT_HPP