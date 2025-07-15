//
// Created by bvasseur on 7/15/25.
//

#ifndef SHADER_HPP
# define SHADER_HPP

# include "glad/glad.h"
# include <iostream>
# include <string>
# include <fstream>
# include <iostream>
# include <sstream>
# include <cerrno>

std::string readFile(const std::string& filePath);

class Shader {
private:
	GLuint	_id;

public:
	Shader();
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	Shader(const Shader& other);
	~Shader();

	Shader& operator=(const Shader& other);

	void	activate();
	void	deleteShader();
};


#endif //SHADER_HPP
