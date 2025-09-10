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
		_vertices = other._vertices;
		_indices = other._indices;
		_textures = other._textures;
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


void Object::parse(const std::string &filepath) {
	std::ifstream file;

	file.open(filepath.c_str());
	if (!file.is_open())
		throw std::runtime_error("Failed to open file: " + filepath);
	std::getline(file, _rawData, '\0');
	file.close();
	_rawData += '\n';

	std::vector<std::string>	lines = split(_rawData, "\n", true);
	std::vector<std::string>	elements;

	for (size_t i = 0; i < lines.size(); ++i) {
		elements = split(lines[i], " \t\r\n");
		// std::cout << MAGENTA"line : " << lines[i] << "$" << RESET << std::endl;
		// for (size_t i = 0; i < elements.size(); ++i)
			// std::cout << LIGTH_BLUE"elements["<<i<<"] : " << elements[i] << "$" << RESET << std::endl;
		// std::cout << std::endl;
		if (lines[i].empty() || elements.empty() || elements[0].empty() || elements[0].find('#') != std::string::npos)
			continue ;

		try {
			if (_fParser.find(elements[0]) != _fParser.end())
				(this->*(_fParser.at(elements[0])))(elements, i + 1);
			else
				throw std::runtime_error(RED + elements[0] + YELLOW" : not recognized at line ===> " LIGTH_BLUE + std::to_string(i + 1) + RESET);
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
		// std::cout << "pos0["<<pos0<<"]: \'" << (str[pos0] == '\n' ? 'n' : str[pos0])
		// << "\', pos1["<<pos1<<"]: \'" << (pos1 != std::string::npos ? str[pos1] :'O') << "\'" <<  std::endl;
		if (pos0 == std::string::npos && pos1 == std::string::npos)
			break ;
		if (pos1 == std::string::npos)
			line = str.substr(pos0, str.length() - pos0);
		else
			line = str.substr(pos0, pos1 - pos0);
		pos0 = pos1 + 1;
		// if (line.empty() || (keepEmpty && line[0] == '\n'))
			// continue ;
		tokens.push_back(line);
		if (pos0 == std::string::npos || pos1 == std::string::npos)
			break ;
	}
	return tokens;
}

void Object::checkNumber(const std::string &str, const int type, const size_t line) {
	switch (type) {
		case INT:
			if (str.find_first_not_of("+-0123456789") != std::string::npos ||
				str == "+" || str == "-" ||
				(str.rfind('+') != std::string::npos && str.rfind('+') != 0) ||
				(str.rfind('-') != std::string::npos && str.rfind('-') != 0))
				throw std::runtime_error(RED + str + YELLOW" : not a valid integer value at line ===> "
					LIGTH_BLUE + std::to_string(line) + RESET);
			break ;
		case POSITIVE_INT:
			if (str.find_first_not_of("0123456789") != std::string::npos)
				throw std::runtime_error(RED + str + YELLOW" : not a valid positive integer value at line ===> "
					LIGTH_BLUE + std::to_string(line) + RESET);
			break ;
		case FLOAT:
			if (str.find_first_not_of("+-0123456789.") != std::string::npos ||
				str.find('.') != str.rfind('.') || str == "." || str == "+" || str == "-" ||
				(str.rfind('+') != std::string::npos && str.rfind('+') != 0) ||
				(str.rfind('-') != std::string::npos && str.rfind('-') != 0))
				throw std::runtime_error(RED + str + YELLOW" : not a valid float value at line ===> "
					LIGTH_BLUE + std::to_string(line) + RESET);
			break ;
		default:
			break ;
	}
}

void Object::parseVertex(const std::vector<std::string>& tokens, const size_t line) {
		glm::vec3 vec;
	if (tokens.size() != 4)
		throw std::runtime_error(RED"Invalid number of values for vertex at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(line) + RESET);
	for (size_t i = 1; i < tokens.size(); ++i) {
		checkNumber(tokens[i], FLOAT, line);
		vec[i - 1] = std::stof(tokens[i]);
		// std::cout << "tokens["<<i<<"]: " << tokens[i] << std::endl;
	}
	// std::cout << vec.x << ',' << vec.y << ',' << vec.z << std::endl;

	if (_vCount >= _vertices.size())
		_vertices.push_back(Vertex{glm::vec3(vec), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1, 1, 1), glm::vec2(0.0f, 0.0f)});
	else
		_vertices[_vCount].position = vec;
	++_vCount;
}

void Object::parseTexCoord(const std::vector<std::string>& tokens, const size_t line) {
	(void)tokens, (void)line;
}

void Object::parseNormal(const std::vector<std::string>& tokens, const size_t line) {
	(void)tokens, (void)line;
}

void Object::parseFace(const std::vector<std::string>& tokens, const size_t line) {
	int	numbers[tokens.size() - 1];

	if (tokens.size() < 4)
		throw std::runtime_error(RED"Invalid number of values for face at line" YELLOW " ===> "
			LIGTH_BLUE + std::to_string(line) + RESET);
	for (size_t i = 1; i < tokens.size(); ++i) {
		checkNumber(tokens[i], POSITIVE_INT, line);
		numbers[i - 1] = std::stoi(tokens[i]);
	}
	for (size_t i = 1; i < tokens.size() - 2; ++i) {
		_indices.push_back(numbers[0] - 1);
		_indices.push_back(numbers[i] - 1);
		_indices.push_back(numbers[i + 1] - 1);
	}
}

void Object::parseMaterial(const std::vector<std::string>& tokens, const size_t line) {
	(void)tokens, (void)line;
}
