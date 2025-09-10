//
// Created by bvasseur on 9/5/25.
//

#include "Object.hpp"


const mapFunc Object::_objFunctionParser = {
			{"v", &Object::parseVertex},
			{"vt", &Object::parseTexCoord},
			{"vn", &Object::parseNormal},
			{"f", &Object::parseFace},
			{"mtllib", &Object::parseMaterialLib},
			{"usemtl", &Object::parseUseMaterial}
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
matMap Object::_materials = {};

/* ==================== CONSTRUCTORS ==================== */

Object::Object() {
}

Object::Object(const std::string &filepath) {
	parse(filepath);
}

Object::Object(const Object &other) {
	*this = other;
}

Object & Object::operator=(const Object &other) {
	if (this != &other) {
		_rawData = other._rawData;
		_objPath = other._objPath;
		_vertices = other._vertices;
		_indices = other._indices;
		_textures = other._textures;
		_currentMaterial = other._currentMaterial;
		_vCount = other._vCount;
		_vtCount = other._vtCount;
		_vnCount = other._vnCount;
	}
	return *this;
}

Object::~Object() {
}


/* ==================== GETTERS / SETTERS ==================== */


const std::vector<Vertex>& Object::getVertices() {
	return _vertices;
}

const std::vector<GLuint>& Object::getIndices() {
	return _indices;
}

const std::vector<Texture>& Object::getTextures() {
	return _textures;
}

void Object::setVertices(const std::vector<Vertex> &vertices) {
	_vertices = vertices;
}

void Object::setIndices(const std::vector<GLuint> &indices) {
	_indices = indices;
}

void Object::setTextures(const std::vector<Texture> &textures) {
	_textures = textures;
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
	std::getline(file, _rawData, '\0');
	file.close();
	_rawData += '\n';

	std::cout << std::endl << YELLOW "Parsing file: " MAGENTA + filepath + RESET << std::endl;

	std::vector<std::string>	lines = split(_rawData, "\n", true);
	std::vector<std::string>	elements;

	for (size_t i = 0; i < lines.size(); ++i) {
		elements = split(lines[i], " \t\r\n");
		if (lines[i].empty() || elements.empty() || elements[0].empty() || elements[0].find('#') != std::string::npos)
			continue ;
		try {
			if (func.find(elements[0]) != func.end())
				(this->*(func.at(elements[0])))(elements, i + 1);
			else
				throw std::runtime_error(RED + elements[0] + YELLOW
					" : not recognized at line ===> " LIGTH_BLUE + std::to_string(i + 1) + RESET);
				// throw std::runtime_error(RED + elements[0] + YELLOW" : not recognized (" MAGENTA + filepath
					// + YELLOW ") at line ===> " LIGTH_BLUE + std::to_string(i + 1) + RESET);
		}
		catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}
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
		if (pos1 == std::string::npos)
			line = str.substr(pos0, str.length() - pos0);
		else
			line = str.substr(pos0, pos1 - pos0);
		pos0 = pos1 + 1;
		tokens.push_back(line);
		if (pos0 == std::string::npos || pos1 == std::string::npos)
			break ;
	}
	return tokens;
}

void Object::checkNumber(const std::string &str, const int type, const size_t lineCount) {
	switch (type) {
		case INT:
			if (str.find_first_not_of("+-0123456789") != std::string::npos ||
				str == "+" || str == "-" ||
				(str.rfind('+') != std::string::npos && str.rfind('+') != 0) ||
				(str.rfind('-') != std::string::npos && str.rfind('-') != 0))
				throw std::runtime_error(RED + str + YELLOW" : not a valid integer value at line ===> "
					LIGTH_BLUE + std::to_string(lineCount) + RESET);
			break ;
		case POSITIVE_INT:
			if (str.find_first_not_of("0123456789") != std::string::npos)
				throw std::runtime_error(RED + str + YELLOW" : not a valid positive integer value at line ===> "
					LIGTH_BLUE + std::to_string(lineCount) + RESET);
			break ;
		case FLOAT:
			if (str.find_first_not_of("+-0123456789.") != std::string::npos ||
				str.find('.') != str.rfind('.') || str == "." || str == "+" || str == "-" ||
				(str.rfind('+') != std::string::npos && str.rfind('+') != 0) ||
				(str.rfind('-') != std::string::npos && str.rfind('-') != 0))
				throw std::runtime_error(RED + str + YELLOW" : not a valid float value at line ===> "
					LIGTH_BLUE + std::to_string(lineCount) + RESET);
			break ;
		default:
			break ;
	}
}


/* ==================== OBJ ==================== */


void Object::parseVertex(const std::vector<std::string>& tokens, const size_t lineCount) {
		glm::vec3 vec;
	if (tokens.size() != 4)
		throw std::runtime_error(RED"Invalid number of values for vertex at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	for (size_t i = 1; i < tokens.size(); ++i) {
		checkNumber(tokens[i], FLOAT, lineCount);
		vec[i - 1] = std::stof(tokens[i]);
		// std::cout << "tokens["<<i<<"]: " << tokens[i] << std::endl;
	}
	// std::cout << vec.x << ',' << vec.y << ',' << vec.z << std::endl;

	if (_vCount <= _vertices.size())
		_vertices[_vCount].position = vec;
	else
		_vertices.push_back(Vertex{glm::vec3(vec), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.5f, 0.80f, 0.9f), glm::vec2(0.0f, 0.0f)});
	++_vCount;
}

void Object::parseTexCoord(const std::vector<std::string>& tokens, const size_t lineCount) {
	(void)tokens, (void)lineCount;
}

void Object::parseNormal(const std::vector<std::string>& tokens, const size_t lineCount) {
	(void)tokens, (void)lineCount;
}

void Object::parseFace(const std::vector<std::string>& tokens, const size_t lineCount) {
	int	numbers[tokens.size() - 1];

	if (tokens.size() < 4)
		throw std::runtime_error(RED"Invalid number of values for face at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	for (size_t i = 1; i < tokens.size(); ++i) {
		checkNumber(tokens[i], POSITIVE_INT, lineCount);
		numbers[i - 1] = std::stoi(tokens[i]);
	}
	for (size_t i = 1; i < tokens.size() - 2; ++i) {
		_indices.push_back(numbers[0] - 1);
		_indices.push_back(numbers[i] - 1);
		_indices.push_back(numbers[i + 1] - 1);
	}
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
			// parse(tokens[i], _matFunctionParser, lineCount);
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
}


/* ==================== MTL ==================== */


void Object::parseNewMaterial(const std::vector<std::string> &tokens, size_t lineCount) {
	(void)tokens, (void)lineCount;
	if (tokens.size() != 2)
		throw std::runtime_error(RED"Invalid number of values for newmtl at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(lineCount) + RESET);
	if (_materials.find(tokens[1]) != _materials.end())
		throw std::runtime_error(RED "WARNING : Material " LIGTH_BLUE + tokens[1] +
			YELLOW" already exists at line ===> " LIGTH_BLUE + std::to_string(lineCount) + RESET);
	_materials[tokens[1]] = MaterialData();
	_materials[tokens[1]]._mapKd = "./resources/textures/image.png";
}

void Object::parseNs(const std::vector<std::string> &tokens, size_t lineCount) {
	(void)tokens, (void)lineCount;
}

void Object::parseKa(const std::vector<std::string> &tokens, size_t lineCount) {
	(void)tokens, (void)lineCount;
}

void Object::parseKd(const std::vector<std::string> &tokens, size_t lineCount) {
	(void)tokens, (void)lineCount;
}

void Object::parseKs(const std::vector<std::string> &tokens, size_t lineCount) {
	(void)tokens, (void)lineCount;
}

void Object::parseKe(const std::vector<std::string> &tokens, size_t lineCount) {
	(void)tokens, (void)lineCount;
}

void Object::parseNi(const std::vector<std::string> &tokens, size_t lineCount) {
	(void)tokens, (void)lineCount;
}

void Object::parseD(const std::vector<std::string> &tokens, size_t lineCount) {
	(void)tokens, (void)lineCount;
}

void Object::parseMapKd(const std::vector<std::string> &tokens, size_t lineCount) {
	(void)tokens, (void)lineCount;
}

const MaterialData & Object::getMaterial(const std::string &name) const {
	if (_materials.find(name) != _materials.end())
		return _materials[name];
	throw std::runtime_error(RED "Material " + name + " not loaded or doesn't exists." RESET);
}
