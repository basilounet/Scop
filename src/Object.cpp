//
// Created by bvasseur on 9/5/25.
//

#include "Object.hpp"

const mapFunc Object::_fParser = {
			{"v", &Object::parseVertex},
			{"vt", &Object::parseTexCoord},
			{"vn", &Object::parseNormal},
			{"f", &Object::parseFace},
			{"usemtl", &Object::parseMaterial}
};

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


/* ==================== METHODS ==================== */


void Object::parse(const std::string &filepath) {
	std::ifstream file;

	file.open(filepath.c_str());
	if (!file.is_open())
		throw std::runtime_error("Failed to open file: " + filepath);
	std::getline(file, _rawData, '\0');
	file.close();
	_rawData += '\n';

	std::vector<std::string>	lines = split(_rawData, "\n");
	std::vector<std::string>	elements;

	for (size_t i = 0; i < lines.size(); ++i) {
		elements = split(lines[i], " \t\r\n");
		// std::cout << RED"line : " << lines[i] << "$" << RESET << std::endl;
		// for (size_t i = 0; i < elements.size(); ++i)
			// std::cout << LIGTH_BLUE"elements["<<i<<"] : " << elements[i] << "$" << RESET << std::endl;
		// std::cout << std::endl;
		if (lines[i].empty() || elements.empty() || elements[0].empty() || elements[0].find('#') != std::string::npos)
			continue ;

		try {
			if (_fParser.find(elements[0]) != _fParser.end())
				(this->*(_fParser.at(elements[0])))(elements);
			else
				throw std::runtime_error(RED + elements[0] + YELLOW" : not recognized at line ===> " LIGTH_BLUE + std::to_string(i) + RESET);
		}
		catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
		}
	}
}

std::vector<std::string> Object::split(const std::string &str, const std::string& delims) {
	std::string	line;
	std::vector<std::string> tokens;
	size_t	pos0 = 0;
	size_t	pos1 = 0;

	while (pos0 != std::string::npos) {
		pos0 = str.find_first_not_of(delims, pos1);
		pos1 = str.find_first_of(delims, pos0);
		// std::cout << YELLOW"pos0: " << pos0 << ", pos1: " << pos1 << RESET << std::endl;
		// std::cout << "pos0["<<pos0<<"]: \'" << str[pos0] << "\', pos1["<<pos1<<"]: \'" << (pos1 != std::string::npos ? str[pos1] :'O') << "\'" <<  std::endl;
		if (pos0 == std::string::npos && pos1 == std::string::npos)
			break ;
		if (pos1 == std::string::npos)
			line = str.substr(pos0, str.length() - pos0);
		else
			line = str.substr(pos0, pos1 - pos0);
		pos0 = pos1 + 1;
		if (line.empty() || line[0] == '\n')
			continue ;
		tokens.push_back(line);
	}
	return tokens;
}

void Object::parseVertex(const std::vector<std::string>& tokens) {
	(void)tokens;
	std::cout << "vertex" << std::endl;
}

void Object::parseTexCoord(const std::vector<std::string>& tokens) {
	(void)tokens;
}

void Object::parseNormal(const std::vector<std::string>& tokens) {
	(void)tokens;
}

void Object::parseFace(const std::vector<std::string>& tokens) {
	(void)tokens;
}

void Object::parseMaterial(const std::vector<std::string>& tokens) {
	(void)tokens;
}
