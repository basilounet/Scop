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
	compileErrors(vertexShader, "VERTEX");

	const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	glCompileShader(fragmentShader);
	compileErrors(fragmentShader, "FRAGMENT");

	_id = glCreateProgram();
	glAttachShader(_id, vertexShader);
	glAttachShader(_id, fragmentShader);
	glLinkProgram(_id);
	compileErrors(_id, "PROGRAM");
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

/* ==================== GETTERS / SETTERS ==================== */

GLuint Shader::getID() const {
	return _id;
}

/* ==================== METHODS ==================== */

void Shader::activate() const {
	glUseProgram(_id);
}

void Shader::deleteShader() const {
	glDeleteProgram(_id);
}

void Shader::compileErrors(unsigned int shader, const std::string& type)
{
	GLint hasCompiled;
	// Character array to store error message in
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_COMPILATION_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
		if (hasCompiled == GL_FALSE)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "SHADER_LINKING_ERROR for:" << type << "\n" << infoLog << std::endl;
		}
	}
}