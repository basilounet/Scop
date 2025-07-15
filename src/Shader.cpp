//
// Created by bvasseur on 7/15/25.
//

#include "Shader.hpp"

std::string readFile(const std::string& filePath) {
	std::ifstream in(filePath, std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return (contents);
	}
	throw std::runtime_error("Failed to open file " + filePath);
}

Shader::Shader() : _id(0) {
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) : _id(0) {
	const std::string vertexCode = readFile(vertexPath);
	const std::string fragmentCode = readFile(fragmentPath);

	const char* vertexShaderSource = vertexCode.c_str();
	const char* fragmentShaderSource = fragmentCode.c_str();

	const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);

	const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);

	_id = glCreateProgram();
	glAttachShader(_id, vertexShader);
	glAttachShader(_id, fragmentShader);
	glLinkProgram(_id);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

Shader::Shader(const Shader &other) {
	*this = other;
}

Shader &Shader::operator=(const Shader &other) {
	if (this != &other) {
		_id = other._id;
	}
	return *this;
}

Shader::~Shader() {
	// glDeleteProgram(_id);
}

/* ==================== METHODS ==================== */

void Shader::activate() {
	glUseProgram(_id);
}

void Shader::deleteShader() {
	glDeleteProgram(_id);
}