//
// Created by bvasseur on 9/5/25.
//

#include "Object.hpp"

#include "math/Vec2.hpp"


const mapFunc Object::_objFunctionParser = {
			{"v", &Object::parseVertex},
			{"vt", &Object::parseTexCoord},
			{"vn", &Object::parseNormal},
			{"f", &Object::parseFace},
			{"mtllib", &Object::parseMaterialLib},
			{"usemtl", &Object::parseUseMaterial},
			{"g", &Object::parseGroup}
};

const mapFunc Object::_matFunctionParser = {
			{"newmtl", &Object::parseNewMaterial},
			{"Ns", &Object::parseNs},
			{"Ka", &Object::parseKa},
			{"Kd", &Object::parseKd},
			{"Ks", &Object::parseKs},
			{"Ke", &Object::parseKe},
			{"Ni", &Object::parseNi},
			{"d", &Object::parseD},
			{"map_Kd", &Object::parseMapKd}
};
matMap Object::_materials = {
	{"default", MaterialData({._name = "default"})}
};
std::string Object::_texturePath = "./resources/textures/";

/* ==================== CONSTRUCTORS ==================== */

Object::Object() {
}

Object::Object(const std::string &filepath) {
	_vCount = 0;
	_vnCount = 0;
	_vtCount = 0;
	_currentMaterial = &_materials["default"];
	_indicesGroups["default"]._material = _currentMaterial;
	_currentParsingMaterial = nullptr;
	parse(filepath);
	calculateNormals();
	assignTexCoords();
	calculateCenter();
}	

Object::Object(const Object &other) {
	*this = other;
}

Object & Object::operator=(const Object &other) {
	if (this != &other) {
		_objPath = other._objPath;
		_vertices = other._vertices;
		_indicesGroups = other._indicesGroups;
		_centerPoint = other._centerPoint;
		_currentMaterial = other._currentMaterial;
		_currentParsingMaterial = other._currentParsingMaterial;
		_vCount = other._vCount;
		_vtCount = other._vtCount;
		_vnCount = other._vnCount;
	}
	return *this;
}

Object::~Object() {
}


/* ==================== GETTERS / SETTERS ==================== */


const std::vector<Vertex>& Object::getVertices() const {
	return _vertices;
}

const faceGroupMap &Object::getIndicesGroup() const {
	return _indicesGroups;
}

const Vec3 & Object::getCenterPoint() const {
	return _centerPoint;
}

void Object::setVertices(const std::vector<Vertex> &vertices) {
	_vertices = vertices;
}

void Object::setIndicesGroup(const faceGroupMap &indices) {
	_indicesGroups = indices;
}


/* ==================== METHODS ==================== */


void Object::parse(const std::string &filepath, const mapFunc& func, size_t lineCount) {
	std::ifstream file;

	if (lineCount == 0)
		_objPath = filepath;
	file.open(filepath.c_str());
	if (!file.is_open())
		throw std::runtime_error(RED "Failed to open file: " MAGENTA + filepath + YELLOW " at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	std::string rawData;
	std::getline(file, rawData, '\0');
	file.close();
	rawData += '\n';

	std::cout << std::endl << YELLOW "Parsing file: " MAGENTA + filepath + RESET << std::endl;

	std::vector<std::string>	lines = split(rawData, "\n", true);
	std::vector<std::string>	tokens;

	for (size_t i = 0; i < lines.size(); ++i) {
		tokens = split(lines[i], " \t\r\n", false);
		if (lines[i].empty() || tokens.empty() || tokens[0].empty() || tokens[0][0] == '#')
			continue ;
		try {
			// std::cout << "Line["<<i<<"]: " << lines[i] << "$" << std::endl;
			if (func.find(tokens[0]) != func.end())
				(this->*(func.at(tokens[0])))(tokens, i + 1);
			else
				throw std::runtime_error(RED + tokens[0] + YELLOW
					" : not recognized at line ===> " LIGTH_BLUE + std::to_string(i + 1) + RESET);
				// throw std::runtime_error(RED + elements[0] + YELLOW" : not recognized (" MAGENTA + filepath
					// + YELLOW ") at line ===> " LIGTH_BLUE + std::to_string(i + 1) + RESET);
		}
		catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}
}

void Object::createTexture() {
	for (auto&[fst, snd] : _materials) {
		try {
			snd._mapKdTexture = Texture(_texturePath + snd._mapKd, "texture", 0, GL_RGBA, GL_UNSIGNED_BYTE);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
}

void Object::calculateNormals() {
	std::vector<GLuint> indicesBuffer;

	for (auto &group : _indicesGroups) {
		indicesBuffer.insert(indicesBuffer.end(), group.second._indices.begin(), group.second._indices.end());
	}
	// for (auto & vertice : _vertices) {
		// vertice.normal = Vec3(0, 0, 0);
	// }
	for (size_t i = 0; i < indicesBuffer.size() - 2; i += 3) {
		Vec3 p = cross(
			_vertices[indicesBuffer[i + 1]].position - _vertices[indicesBuffer[i]].position,
			_vertices[indicesBuffer[i + 2]].position - _vertices[indicesBuffer[i]].position);
		// std::cout << "Face" YELLOW" [" <<indicesBuffer[i]<<", "<<indicesBuffer[i + 1]<<", "<<indicesBuffer[i + 2]<<
			// "]" RESET " product: " << p.x << ", " << p.y << ", " << p.z << std::endl;
		_vertices[indicesBuffer[i]].normal += p;
		_vertices[indicesBuffer[i + 1]].normal += p;
		_vertices[indicesBuffer[i + 2]].normal += p;

	}
	for (size_t i = 0; i < _vertices.size(); ++i) {
		// if (_vertices[i].normal.x == 0 && _vertices[i].normal.y == 0 && _vertices[i].normal.z == 0)
			// _vertices[i].normal = Vec3(0.0f, 0.0f, 1.0f);
		_vertices[i].normal = normalize(_vertices[i].normal).abs();
		_vertices[i].color = _vertices[i].normal;
		// std::cout << "Vertex " YELLOW"["<<i<<"]" RESET" normal: "
			// << _vertices[i].normal.x << ", " << _vertices[i].normal.y << ", " << _vertices[i].normal.z << std::endl;
	}
}

void Object::assignTexCoords() {
	for (Vertex & v : _vertices) {
		v.texCoord = Vec2(v.position.z, v.position.y);
	}
}

void Object::calculateCenter() {
	Vec3 max = Vec3(
		std::max_element(_vertices.begin(), _vertices.end(),
			[](const Vertex &a, const Vertex &b) {
			return a.position.x < b.position.x;})->position.x,
		std::max_element(_vertices.begin(), _vertices.end(),
			[](const Vertex &a, const Vertex &b) {
			return a.position.y < b.position.y; })->position.y,
		std::max_element(_vertices.begin(), _vertices.end(),
			[](const Vertex &a, const Vertex &b) {
			return a.position.z < b.position.z;	})->position.z);
	Vec3 min = Vec3(
	std::min_element(_vertices.begin(), _vertices.end(),
		[](const Vertex &a, const Vertex &b) {
		return a.position.x < b.position.x;})->position.x,
	std::min_element(_vertices.begin(), _vertices.end(),
		[](const Vertex &a, const Vertex &b) {
		return a.position.y < b.position.y; })->position.y,
	std::min_element(_vertices.begin(), _vertices.end(),
		[](const Vertex &a, const Vertex &b) {
		return a.position.z < b.position.z;	})->position.z);
	_centerPoint = (min + max) / 2.0f;
}

std::vector<std::string> Object::split(const std::string &str, const std::string& delims, const bool keepEmpty) {
	std::string	line;
	std::vector<std::string> tokens;
	size_t	pos0 = 0;
	size_t	pos1 = 0;

	while (true) {
		if (!keepEmpty)
			pos0 = str.find_first_not_of(delims, pos1);
		pos1 = str.find_first_of(delims, pos0);
		if (pos0 == std::string::npos && pos1 == std::string::npos)
			break ;
		// if (pos1 == std::string::npos)
		// 	line = str.substr(pos0, pos1);
		// else
		line = str.substr(pos0, pos1 - pos0);
		pos0 = pos1 + 1;
		tokens.push_back(line);
		if (pos0 == std::string::npos || pos1 == std::string::npos)
			break ;
	}
	return tokens;
}

bool isScientificNotationCorrect(const std::string &str, const char sign, size_t start = 0) {
	size_t pos = str.find(sign, start);
	if (str.find(sign, pos + 1) != std::string::npos)
		return false;
	return (pos != std::string::npos && pos > 0 && str[pos - 1] == 'e');
}

void Object::checkNumber(const std::string &str, const int type, const int sign, const size_t lineCount) {
	if (str.find_first_not_of("+-0123456789.e") != std::string::npos || str == "+" || str == "-" || str == ".")
		throw std::runtime_error(RED + str + YELLOW" : invalid char in number value at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	if (str.find('e') != str.rfind('e'))
		throw std::runtime_error(RED + str + YELLOW" : invalid scientific notation at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	if (type == INT && (str.find('.') != std::string::npos))
		throw std::runtime_error(RED + str + YELLOW" : not an integer value at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	if (type == FLOAT && (str.find('.') != str.rfind('.')))
		throw std::runtime_error(RED + str + YELLOW" : invalid float value at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	if (sign == POSITIVE && isScientificNotationCorrect(str, '-'))
		throw std::runtime_error(RED + str + YELLOW" : not a positive value at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	if (sign == NEGATIVE && str.find('-') != 0)
		throw std::runtime_error(RED + str + YELLOW" : not a negative value at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	if ((str.find('+', 1) != std::string::npos && !isScientificNotationCorrect(str, '+', 1)) ||
		(str.find('-', 1) != std::string::npos && !isScientificNotationCorrect(str, '-', 1)))
			throw std::runtime_error(RED + str + YELLOW" : invalid sign position at line ===> "
				LIGTH_BLUE + std::to_string(lineCount) + RESET);
}


/* ==================== OBJ ==================== */


void Object::parseVertex(const std::vector<std::string>& tokens, const size_t lineCount) {
		Vec3 vec;
	if (tokens.size() != 4)
		throw std::runtime_error(RED"Invalid number of values for vertex at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	for (size_t i = 1; i < tokens.size(); ++i) {
		checkNumber(tokens[i], FLOAT, ANY, lineCount);
		vec[i - 1] = std::stof(tokens[i]);
	}

	if (_vCount < _vertices.size())
		_vertices[_vCount].position = vec;
	else
		_vertices.push_back(Vertex{Vec3(vec), Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.5f, 0.20f, 0.9f), Vec2(1000000, -1000000)});
	++_vCount;
}

void Object::parseTexCoord(const std::vector<std::string>& tokens, const size_t lineCount) {
	if (tokens.size() < 3 || tokens.size() > 4)
		throw std::runtime_error(RED"Invalid number of values for vt at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, ANY, lineCount);
	checkNumber(tokens[2], FLOAT, ANY, lineCount);
	if (_vtCount < _vertices.size())
		_vertices[_vtCount].texCoord = Vec2(std::stof(tokens[1]), std::stof(tokens[2]));
	else
		_vertices.push_back(Vertex{Vec3(0.0f, 0.0f, 0.0f),
			Vec3(1.0f, 0.0f, 0.0f),
			Vec3(0.5f, 0.80f, 0.9f),
			Vec2(std::stof(tokens[1]), std::stof(tokens[2]))});
	++_vtCount;
}

void Object::parseNormal(const std::vector<std::string>& tokens, const size_t lineCount) {
	if (tokens.size() != 4)
		throw std::runtime_error(RED "Invalid number of values for vn at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, ANY, lineCount);
	checkNumber(tokens[2], FLOAT, ANY, lineCount);
	checkNumber(tokens[3], FLOAT, ANY, lineCount);
	if (_vnCount < _vertices.size())
		_vertices[_vnCount].normal = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
	else
		_vertices.push_back(Vertex{Vec3(0.0f, 0.0f, 0.0f),
			Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])),
			Vec3(0.5f, 0.80f, 0.9f),
			Vec2(1000000, -1000000)});
	++_vnCount;
}

void Object::parseFace(const std::vector<std::string>& tokens, const size_t lineCount) {
	std::vector<std::string> parts;
	int	numbers[tokens.size() - 1];

	if (tokens.size() < 4)
		throw std::runtime_error(RED"Invalid number of values for face at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	for (size_t i = 1; i < tokens.size(); ++i) {
		parts = split(tokens[i], "/", true); // TODO : handle v/vt/vn v//vn v/vt
		if (parts[0].empty())
			throw std::runtime_error(RED "Missing vertex index at line" YELLOW " ===> "
				LIGTH_BLUE + std::to_string(lineCount) + RESET);
		checkNumber(parts[0], INT, POSITIVE, lineCount);
		numbers[i - 1] = std::stoi(parts[0]);
	}
	if (!_currentMaterial)
		_currentMaterial = &_materials["default"];
	for (size_t i = 1; i < tokens.size() - 2; ++i) {
		_indicesGroups[_currentMaterial->_name]._indices.push_back(numbers[0] - 1);
		_indicesGroups[_currentMaterial->_name]._indices.push_back(numbers[i] - 1);
		_indicesGroups[_currentMaterial->_name]._indices.push_back(numbers[i + 1] - 1);
	}
}

void Object::parseGroup(const std::vector<std::string> &tokens, size_t lineCount) {
	(void)tokens, (void)lineCount;
}

void Object::parseMaterialLib(const std::vector<std::string>& tokens, const size_t lineCount) {
	std::ifstream file;

	if (tokens.size() < 2)
		throw std::runtime_error(RED"Invalid number of values for mtllib at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	for (size_t i = 1; i < tokens.size(); ++i) {
		try {
			std::filesystem::path fileDir = std::filesystem::path(_objPath).parent_path();
			std::filesystem::path fullPath = fileDir / tokens[i];
			parse(fullPath.string(), _matFunctionParser, lineCount);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
	std::cout << std::endl << YELLOW "Parsing file: " MAGENTA + _objPath + RESET << std::endl;
}

void Object::parseUseMaterial(const std::vector<std::string> &tokens, size_t lineCount) {
	if (tokens.size() < 2)
		throw std::runtime_error(RED "No material name specified at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	// std::cout << YELLOW "Using material: " MAGENTA + tokens[1] + RESET << std::endl;
	_currentMaterial = getMaterial(tokens[1]);
	_indicesGroups[_currentMaterial->_name]._material = _currentMaterial;
}


/* ==================== MTL ==================== */


void Object::parseNewMaterial(const std::vector<std::string> &tokens, size_t lineCount) {
	if (tokens.size() != 2)
		throw std::runtime_error(RED"Invalid number of values for newmtl at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	if (_materials.find(tokens[1]) != _materials.end())
		throw std::runtime_error(RED "WARNING : Material " LIGTH_BLUE + tokens[1] +
			YELLOW" already exists. Ignoring this at line ===> " LIGTH_BLUE + std::to_string(lineCount) + RESET);
	// std::cout << RED "New MaterialData" RESET << std::endl;
	_materials[tokens[1]] = MaterialData();
	_materials[tokens[1]]._name = tokens[1];
	_currentParsingMaterial = &_materials[tokens[1]];
}

void Object::checkCurrentParsingMaterial(size_t lineCount) {
	if (!_currentParsingMaterial)
		throw std::runtime_error(RED "No material currently being parsed" YELLOW " at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
}

void Object::parseNs(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 2)
		throw std::runtime_error(RED "Invalid number of values specified for Ns at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_ns = std::stof(tokens[1]);
}

void Object::parseKa(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 4)
		throw std::runtime_error(RED "Invalid number of values specified for Ka at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[2], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[3], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_ka = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}

void Object::parseKd(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 4)
		throw std::runtime_error(RED "Invalid number of values specified for Kd at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[2], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[3], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_kd = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}

void Object::parseKs(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 4)
		throw std::runtime_error(RED "Invalid number of values specified for Ks at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[2], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[3], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_ks = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}

void Object::parseKe(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 4)
		throw std::runtime_error(RED "Invalid number of values specified for Ke at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[2], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[3], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_ke = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}

void Object::parseNi(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 2)
		throw std::runtime_error(RED "Invalid number of values specified for Ni at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_ni = std::stof(tokens[1]);
}

void Object::parseD(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 2)
		throw std::runtime_error(RED "Invalid number of values specified for d at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_d = std::stof(tokens[1]);
}

void Object::parseMapKd(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 2)
		throw std::runtime_error(RED "Invalid number of values specified for map_Kd at line ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	_currentParsingMaterial->_mapKd = tokens[1];
}

MaterialData* Object::getMaterial(const std::string &name) {
	if (_materials.find(name) != _materials.end())
		return &_materials[name];
	throw std::runtime_error(RED "Material " + name + " not loaded or doesn't exists." RESET);
}
