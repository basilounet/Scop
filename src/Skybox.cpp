//
// Created by bvasseur on 6/30/26.
//

# include <Skybox.hpp>
# include <functional>

#include "math/Mat4.hpp"

std::vector<Vertex> Skybox::_vertices = {
	Vertex{Vec3(-1.0f, -1.0f,  1.0f), Vec3(0), Vec3(0), Vec2(0)},
	Vertex{Vec3( 1.0f, -1.0f,  1.0f), Vec3(0), Vec3(0), Vec2(0)},
	Vertex{Vec3( 1.0f, -1.0f, -1.0f), Vec3(0), Vec3(0), Vec2(0)},
	Vertex{Vec3(-1.0f, -1.0f, -1.0f), Vec3(0), Vec3(0), Vec2(0)},
	Vertex{Vec3(-1.0f,  1.0f,  1.0f), Vec3(0), Vec3(0), Vec2(0)},
	Vertex{Vec3( 1.0f,  1.0f,  1.0f), Vec3(0), Vec3(0), Vec2(0)},
	Vertex{Vec3( 1.0f,  1.0f, -1.0f), Vec3(0), Vec3(0), Vec2(0)},
	Vertex{Vec3(-1.0f,  1.0f, -1.0f), Vec3(0), Vec3(0), Vec2(0)}
};

std::vector<GLuint> Skybox::_indices = {
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};


/* ==================== CONSTRUCTORS ==================== */


Skybox::Skybox(const int width, const int height) : _width(width), _height(height) {
}

Skybox::Skybox(const Skybox &other) {
	*this = other;
}

Skybox::~Skybox() {
}

Skybox& Skybox::operator=(const Skybox &other) {
	if (this != &other) {
		_width = other._width;
		_height = other._height;
		_textureID = other._textureID;
		_vao = other._vao;
		_vbo = other._vbo;
		_ebo = other._ebo;
		_shader = other._shader;
	}
	return *this;
}


/* ==================== GETTERS ==================== */


const std::vector<Vertex> &Skybox::getVertices() {
	return _vertices;
}

const std::vector<GLuint> &Skybox::getIndices() {
	return _indices;
}


/* ==================== METHODS ==================== */


void Skybox::createSkybox() {
	_shader = Shader("./src/shaders/skybox.vert", "./src/shaders/skybox.frag");

	_vao = VAO(true);
	_vbo = VBO(_vertices);
	glGenBuffers(1, &_ebo);
	_vao.bind();
	_vbo.bind();
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo); // bind EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), _indices.data(), GL_STATIC_DRAW);
	_vao.linkAttrib(_vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
	_vao.unbind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // unbind EBO

	// All the faces of the cubemap (make sure they are in this exact order)
	std::string facesCubemap[6] =
	{
		"./resources/textures/px2.jpg",
		"./resources/textures/nx2.jpg",
		"./resources/textures/py2.jpg",
		"./resources/textures/ny2.jpg",
		"./resources/textures/pz2.jpg",
		"./resources/textures/nz2.jpg"
	};

	// Creates the cubemap texture object
	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Cycles through all the textures and attaches them to the cubemap object
	stbi_set_flip_vertically_on_load(false);
	for (GLuint i = 0; i < 6; ++i)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
		}
		else
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
		stbi_image_free(data);
	}
}

void Skybox::drawSkybox(const Camera &camera) {
	// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_CULL_FACE);

	_shader.activate();
	Mat4 view = Mat4(1.0f);
	Mat4 projection = Mat4(1.0f);
	// We make the Mat4 into a Mat3 and then a Mat4 again in order to get rid of the last row and column
	// The last row and column affect the translation of the skybox (which we don't want to affect)
	view = Mat4(lookAt(camera.getPos(), camera.getPos() + camera.getOrientation(), camera.getUp()));
	for (int i = 0; i < 4; ++i) {
		view(3, i) = 0.0f;
		view(i, 3) = 0.0f;
	}
	// view(3, 3) = 1.f;
	// view = mat4(mat3(lookAt(camera.Position, camera.Position + camera.Orientation, camera.Up)));
	projection = perspective(45.0f, (float)_width / _height, 0.1f, 100.0f);
	glUniformMatrix4fv(glGetUniformLocation(_shader.getID(), "view"), 1, GL_FALSE, view.m);
	glUniformMatrix4fv(glGetUniformLocation(_shader.getID(), "projection"), 1, GL_FALSE, projection.m);

	//! Draws the cubemap as the last object so we can save a bit of performance by discarding all fragments
	//! where an object is present (a depth of 1.0f will always fail against any object's depth value)
	_vao.bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	_vao.unbind();
	// Switch back to the normal depth function
	glDepthFunc(GL_LESS);
	// glEnable(GL_CULL_FACE);
}

void Skybox::destroy() {
	_vao.deleteVAO();
	_vbo.deleteVBO();
	glDeleteBuffers(1, &_ebo);
	_shader.deleteShader();
	glDeleteTextures(1, &_textureID);
}
