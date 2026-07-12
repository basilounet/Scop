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
	_totalIndicesCount = 0;
	_vCount = 0;
	_vnCount = 0;
	_vtCount = 0;
	_currentMaterial = &_materials["default"];
	_indicesGroups["default"]._material = _currentMaterial;
	_currentParsingMaterial = nullptr;
	parse(filepath);
	for (auto& it: _indicesGroups)
		_totalIndicesCount += it.second._indices.size();
}

Object::Object(const Object &other) {
	*this = other;
}

Object & Object::operator=(const Object &other) {
	if (this != &other) {
		_filePath = other._filePath;
		_name = other._name;
		_vertices = other._vertices;
		_indicesGroups = other._indicesGroups;
		_totalIndicesCount = other._totalIndicesCount;
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

const std::string & Object::getFilePath() const {
	return _filePath;
}

const std::string & Object::getName() const {
	return _name;
}


/* ==================== GETTERS / SETTERS ==================== */


const std::vector<Vertex>& Object::getVertices() const {
	return _vertices;
}

const faceGroupMap &Object::getIndicesGroup() const {
	return _indicesGroups;
}

const size_t & Object::getTotalIndicesCount() const {
	return _totalIndicesCount;
}

size_t Object::getTotalTextureCount() {
	return _materials.size();
}

matMap& Object::getMaterials() {
	return _materials;
}

MaterialData* Object::getMaterial(const std::string &name) {
	if (_materials.find(name) != _materials.end())
		return &_materials[name];
	throw std::runtime_error(RED "Material " + name + " not loaded or doesn't exists." RESET);
}

void Object::addMaterial(MaterialData &&mat) {
	if (_materials.find(mat._name) != _materials.end())
		_materials[mat._name]._mapKdTexture.deleteTexture();
	_materials[mat._name] = std::move(mat);
}

void Object::setVertices(const std::vector<Vertex> &vertices) {
	_vertices = vertices;
}

void Object::setIndicesGroup(const faceGroupMap &indices) {
	_indicesGroups = indices;
	for (auto& it: _indicesGroups)
		_totalIndicesCount += it.second._indices.size();
}


/* ==================== METHODS ==================== */


void Object::parse(const std::string &filepath, const mapFunc& func, size_t lineCount) {
	std::ifstream file;

	if (lineCount == 0)
		_filePath = filepath;
	_name = _filePath.substr(_filePath.find_last_of("/\\") + 1, _filePath.size() - 4);
	file.open(filepath.c_str());
	if (!file.is_open())
		throw std::runtime_error(RED "Failed to open file: " PRP + filepath + YLW " at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	std::string rawData;
	std::getline(file, rawData, '\0');
	file.close();
	rawData += '\n';

	std::cout << std::endl << YLW "Parsing file: " PRP + filepath + RESET << std::endl;

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
				throw std::runtime_error(RED + tokens[0] + YLW
					" : not recognized at line ===> " CYN + std::to_string(i + 1) + RESET);
				// throw std::runtime_error(RED + elements[0] + YLW" : not recognized (" PRP + filepath
					// + YLW ") at line ===> " CYN + std::to_string(i + 1) + RESET);
		}
		catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}
}

void Object::loadTextures() {
	stbi_set_flip_vertically_on_load(true);
	for (auto&[fst, snd] : _materials) {
		try {
			snd._mapKdTexture.deleteTexture();
			snd._mapKdTexture = Texture(_texturePath + snd._mapKd, "texture", 0, GL_UNSIGNED_BYTE);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
}

void Object::deleteTextures() {
	for (auto& [fst, snd] : _materials)
		snd._mapKdTexture.deleteTexture();
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
		throw std::runtime_error(RED + str + YLW" : invalid char in number value at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	if (str.find('e') != str.rfind('e'))
		throw std::runtime_error(RED + str + YLW" : invalid scientific notation at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	if (type == INT && (str.find('.') != std::string::npos))
		throw std::runtime_error(RED + str + YLW" : not an integer value at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	if (type == FLOAT && (str.find('.') != str.rfind('.')))
		throw std::runtime_error(RED + str + YLW" : invalid float value at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	if (sign == POSITIVE && isScientificNotationCorrect(str, '-'))
		throw std::runtime_error(RED + str + YLW" : not a positive value at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	if (sign == NEGATIVE && str.find('-') != 0)
		throw std::runtime_error(RED + str + YLW" : not a negative value at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	if ((str.find('+', 1) != std::string::npos && !isScientificNotationCorrect(str, '+', 1)) ||
		(str.find('-', 1) != std::string::npos && !isScientificNotationCorrect(str, '-', 1)))
			throw std::runtime_error(RED + str + YLW" : invalid sign position at line ===> "
				CYN + std::to_string(lineCount) + RESET);
}


/* ==================== OBJ ==================== */


void Object::parseVertex(const std::vector<std::string>& tokens, const size_t lineCount) {
		Vec3 vec;
	if (tokens.size() != 4)
		throw std::runtime_error(RED"Invalid number of values for vertex at line" YLW " ===> "
			CYN + std::to_string(lineCount) + RESET);
	for (size_t i = 1; i < tokens.size(); ++i) {
		checkNumber(tokens[i], FLOAT, ANY, lineCount);
		vec[i - 1] = std::stof(tokens[i]);
	}

	if (_vCount < _vertices.size())
		_vertices[_vCount].pos = vec;
	else
		_vertices.push_back(Vertex{Vec3(vec), Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.5f, 0.20f, 0.9f), Vec2(1000000, -1000000)});
	++_vCount;
}

void Object::parseTexCoord(const std::vector<std::string>& tokens, const size_t lineCount) {
	if (tokens.size() < 3 || tokens.size() > 4)
		throw std::runtime_error(RED"Invalid number of values for vt at line" YLW " ===> "
			CYN + std::to_string(lineCount) + RESET);
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
		throw std::runtime_error(RED "Invalid number of values for vn at line" YLW " ===> "
			CYN + std::to_string(lineCount) + RESET);
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
		throw std::runtime_error(RED"Invalid number of values for face at line" YLW " ===> "
			CYN + std::to_string(lineCount) + RESET);
	for (size_t i = 1; i < tokens.size(); ++i) {
		parts = split(tokens[i], "/", true); // TODO : handle v/vt/vn v//vn v/vt
		if (parts[0].empty())
			throw std::runtime_error(RED "Missing vertex index at line" YLW " ===> "
				CYN + std::to_string(lineCount) + RESET);
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
		throw std::runtime_error(RED"Invalid number of values for mtllib at line" YLW " ===> "
			CYN + std::to_string(lineCount) + RESET);
	for (size_t i = 1; i < tokens.size(); ++i) {
		try {
			std::filesystem::path fileDir = std::filesystem::path(_filePath).parent_path();
			std::filesystem::path fullPath = fileDir / tokens[i];
			parse(fullPath.string(), _matFunctionParser, lineCount);
		}
		catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
	std::cout << std::endl << YLW "Parsing file: " PRP + _filePath + RESET << std::endl;
}

void Object::parseUseMaterial(const std::vector<std::string> &tokens, size_t lineCount) {
	if (tokens.size() < 2)
		throw std::runtime_error(RED "No material name specified at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	// std::cout << YLW "Using material: " PRP + tokens[1] + RESET << std::endl;
	_currentMaterial = getMaterial(tokens[1]);
	_indicesGroups[_currentMaterial->_name]._material = _currentMaterial;
}


/* ==================== MTL ==================== */


void Object::parseNewMaterial(const std::vector<std::string> &tokens, size_t lineCount) {
	if (tokens.size() != 2)
		throw std::runtime_error(RED"Invalid number of values for newmtl at line" YLW " ===> "
			CYN + std::to_string(lineCount) + RESET);
	if (_materials.find(tokens[1]) != _materials.end())
		throw std::runtime_error(RED "WARNING : Material " CYN + tokens[1] +
			YLW" already exists. Ignoring this at line ===> " CYN + std::to_string(lineCount) + RESET);
	// std::cout << RED "New MaterialData" RESET << std::endl;
	_materials[tokens[1]] = MaterialData();
	_materials[tokens[1]]._name = tokens[1];
	_currentParsingMaterial = &_materials[tokens[1]];
}

void Object::checkCurrentParsingMaterial(size_t lineCount) {
	if (!_currentParsingMaterial)
		throw std::runtime_error(RED "No material currently being parsed" YLW " at line ===> "
			CYN + std::to_string(lineCount) + RESET);
}

void Object::parseNs(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 2)
		throw std::runtime_error(RED "Invalid number of values specified for Ns at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_ns = std::stof(tokens[1]);
}

void Object::parseKa(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 4)
		throw std::runtime_error(RED "Invalid number of values specified for Ka at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[2], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[3], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_ka = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}

void Object::parseKd(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 4)
		throw std::runtime_error(RED "Invalid number of values specified for Kd at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[2], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[3], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_kd = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}

void Object::parseKs(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 4)
		throw std::runtime_error(RED "Invalid number of values specified for Ks at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[2], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[3], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_ks = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}

void Object::parseKe(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 4)
		throw std::runtime_error(RED "Invalid number of values specified for Ke at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[2], FLOAT, POSITIVE, lineCount);
	checkNumber(tokens[3], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_ke = Vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
}

void Object::parseNi(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 2)
		throw std::runtime_error(RED "Invalid number of values specified for Ni at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_ni = std::stof(tokens[1]);
}

void Object::parseD(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 2)
		throw std::runtime_error(RED "Invalid number of values specified for d at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	checkNumber(tokens[1], FLOAT, POSITIVE, lineCount);
	_currentParsingMaterial->_d = std::stof(tokens[1]);
}

void Object::parseMapKd(const std::vector<std::string> &tokens, size_t lineCount) {
	checkCurrentParsingMaterial(lineCount);
	if (tokens.size() != 2)
		throw std::runtime_error(RED "Invalid number of values specified for map_Kd at line ===> "
			CYN + std::to_string(lineCount) + RESET);
	_currentParsingMaterial->_mapKd = tokens[1];
}
