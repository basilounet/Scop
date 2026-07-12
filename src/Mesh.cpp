//
// Created by bvasseur on 9/4/25.
//

#include "Mesh.hpp"


/* ==================== CONSTRUCTORS ==================== */

Mesh::Mesh() {
}

Mesh::Mesh(const std::string& name, const std::vector<Vertex> &vertices, const faceGroupMap& indicesGroup) :
		_rawVertices(vertices),
		_indicesGroups(indicesGroup),
		_outlineSize(.2f),
		_name(name),
		_totalIndicesCount(0) {
	createFinalVertices();
	calculateNormals();
	assignTexCoords();
	calculateCenter();
	createMesh();
	for (auto& it: _indicesGroups)
		_totalIndicesCount += it.second._indices.size();

	_useColorPercentage = 1.0f;
	_useTexPercentage = 1.0f;
	_outlinePercentage = 1.0f;
	_flags = USE_COLORS | USE_TEX | HIDE_OUTLINE | OUTLINE_PER;
}

Mesh::Mesh(const Object& object) {
	*this = Mesh(object.getName(), object.getVertices(), object.getIndicesGroup());
}

Mesh::Mesh(const Mesh& other) {
	*this = other;
}

Mesh & Mesh::operator=(const Mesh &other) {
	if (this != &other) {
		_rawVertices = other._rawVertices;
		_finalVertices = other._finalVertices;
		_indicesGroups = other._indicesGroups;
		_pos = other._pos;
		_flags = other._flags;
		_centerPoint = other._centerPoint;
		_useTexPercentage = other._useTexPercentage;
		_useColorPercentage = other._useColorPercentage;
		_outlinePercentage = other._outlinePercentage;
		_useStaticTex = other._useStaticTex;
		_outlineSize = other._outlineSize;
		_name = other._name;
		_totalIndicesCount = other._totalIndicesCount;
		_vaoRaw = other._vaoRaw;
		_vboRaw = other._vboRaw;
		_eboRaw = other._eboRaw;
		_vaoFinal = other._vaoFinal;
		_vboFinal = other._vboFinal;
	}
	return *this;
}

Mesh::~Mesh() {
}

const std::string & Mesh::getName() const {
	return _name;
}

const std::vector<Vertex> & Mesh::getVertices() const {
	return _rawVertices;
}

const faceGroupMap & Mesh::getIndicesGroup() const {
	return _indicesGroups;
}

Vec3 & Mesh::getPos() {
	return _pos;
}

const size_t & Mesh::getTotalIndicesCount() const {
	return _totalIndicesCount;
}

float & Mesh::getOutlineSize() {
	return _outlineSize;
}

const Vec3 & Mesh::getCenterPoint() const {
	return _centerPoint;
}

void Mesh::setVertices(const std::vector<Vertex> &vertices) {
	_rawVertices = vertices;
}

void Mesh::setIndices(const faceGroupMap &indices) {
	_indicesGroups = indices;
	for (auto& it: _indicesGroups)
		_totalIndicesCount += it.second._indices.size();
}

void Mesh::setPos(const Vec3 &pos) {
	_pos = pos;
}

void Mesh::addPos(const Vec3 &pos) {
	_pos += pos;
}

void Mesh::switchFlags(const int flag) {
	_flags ^= flag;
}

void Mesh::addFlags(const int flag) {
	_flags |= flag;
}


/* ==================== METHODS ==================== */


void Mesh::createMesh() {
	_vaoRaw = VAO(true);
	_vaoRaw.bind();
	_vboRaw = VBO(_rawVertices);
	_eboRaw = EBO(_indicesGroups);
	_vaoRaw.linkAttrib(_vboRaw, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Position
	_vaoRaw.linkAttrib(_vboRaw, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(GLfloat))); // normal
	_vaoRaw.linkAttrib(_vboRaw, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(GLfloat))); // Color
	_vaoRaw.linkAttrib(_vboRaw, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(GLfloat))); // TexCoord
	_vaoRaw.unbind();
	_vboRaw.unbind();
	_eboRaw.unbind();

	_vaoFinal = VAO(true);
	_vaoFinal.bind();
	_vboFinal = VBO(_finalVertices);

	_vaoFinal.linkAttrib(_vboFinal, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0); // Position
	_vaoFinal.linkAttrib(_vboFinal, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(GLfloat))); // normal
	_vaoFinal.linkAttrib(_vboFinal, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(GLfloat))); // Color
	_vaoFinal.linkAttrib(_vboFinal, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(GLfloat))); // TexCoord
	_vaoFinal.unbind();
	_vboFinal.unbind();
}

void Mesh::updateStates(const double &deltaTime) {
	_useTexPercentage = std::clamp(_useTexPercentage + (_flags & USE_TEX ? 1.0f : -1.0f) * (float)deltaTime * 0.5f, 0.0f, 1.0f);
	_useColorPercentage = std::clamp(_useColorPercentage + (_flags & USE_COLORS ? 1.0f : -1.0f) * (float)deltaTime * 0.5f, 0.0f, 1.0f);
	_outlinePercentage = std::clamp(_outlinePercentage + (_flags & OUTLINE_PER ? 1.0f : -1.0f) * (float)deltaTime * 0.5f, 0.0f, 1.0f);
}

void Mesh::draw(const Shader &shader, const Camera &camera, const Mat4& model, const std::string& type, const bool forceOutline) {
	if ((type == "mesh" && (_flags & HIDE_MESH)) ||
		(type == "outline" && !forceOutline && (_flags & HIDE_OUTLINE)))
		return;
	shader.activate();

	if (type == "outline")
		_vaoRaw.bind();
	else
		_vaoFinal.bind();

	size_t offset = 0;
	Texture *texture = nullptr;
	size_t i = 0;

	camera.sendUniforms(shader);
	glUniform3f(glGetUniformLocation(shader.getID(), "translation"), _pos.x, _pos.y, _pos.z);
	glUniformMatrix4fv(glGetUniformLocation(shader.getID(), "model"), 1, GL_FALSE, model.m);
	glUniform1f(glGetUniformLocation(shader.getID(), "iTime"), glfwGetTime());
	glUniform3f(glGetUniformLocation(shader.getID(), "iResolution"), (float)camera.getWidth(), (float)camera.getHeight(), 0.f);
	glUniform1f(glGetUniformLocation(shader.getID(), "useTexturePercentage"), _useTexPercentage);
	glUniform1f(glGetUniformLocation(shader.getID(), "useColorPercentage"), _useColorPercentage);
	glUniform1f(glGetUniformLocation(shader.getID(), "outlinePercentage"), _outlinePercentage);
	glUniform1i(glGetUniformLocation(shader.getID(), "staticTex"), (_flags & USE_STATIC_TEX) ? 1 : 0);
	glUniform1f(glGetUniformLocation(shader.getID(), "outlining"), _outlineSize);

	for (auto&[fst, snd] : _indicesGroups) {
		texture = &snd._material->_mapKdTexture;
		texture->texUnit(shader, texture->getType() + std::to_string(i), i);
		texture->bind();
		if (type == "outline")
			glDrawElements(GL_TRIANGLES, snd._indices.size(), GL_UNSIGNED_INT, (void*)(offset * sizeof(GLuint)));
		else
			glDrawArrays(GL_TRIANGLES, offset, snd._indices.size());
		offset += snd._indices.size();
		++i;
		texture->unbind();
	}
	_vaoRaw.unbind();
}

void Mesh::createFinalVertices() {
	_finalVertices.clear();
	for (auto &[str, faceGroup] : _indicesGroups) {
		for (auto &index : faceGroup._indices) {
			_finalVertices.push_back(_rawVertices[index]);
		}
	}
}

void Mesh::calculateCenter() {
	Vec3 max = Vec3(
		std::max_element(_rawVertices.begin(), _rawVertices.end(),
			[](const Vertex &a, const Vertex &b) {
			return a.pos.x < b.pos.x;})->pos.x,
		std::max_element(_rawVertices.begin(), _rawVertices.end(),
			[](const Vertex &a, const Vertex &b) {
			return a.pos.y < b.pos.y; })->pos.y,
		std::max_element(_rawVertices.begin(), _rawVertices.end(),
			[](const Vertex &a, const Vertex &b) {
			return a.pos.z < b.pos.z;	})->pos.z);
	Vec3 min = Vec3(
	std::min_element(_rawVertices.begin(), _rawVertices.end(),
		[](const Vertex &a, const Vertex &b) {
		return a.pos.x < b.pos.x;})->pos.x,
	std::min_element(_rawVertices.begin(), _rawVertices.end(),
		[](const Vertex &a, const Vertex &b) {
		return a.pos.y < b.pos.y; })->pos.y,
	std::min_element(_rawVertices.begin(), _rawVertices.end(),
		[](const Vertex &a, const Vertex &b) {
		return a.pos.z < b.pos.z;	})->pos.z);
	_centerPoint = (min + max) / 2.0f;
}

void Mesh::assignTexCoords() {
	std::function <void(Vertex&)> assignTexCoord = [](Vertex& v) {
		const Vec3 absN = v.normal.abs();
		int biggestIndex = absN.x > absN.y ? (absN.x > absN.z ? 0 : 2) : (absN.y > absN.z ? 1 : 2);
		if (biggestIndex == 0) // x
			v.texCoord = Vec2(v.pos.z, v.pos.y);
		else if (biggestIndex == 1) // y
			v.texCoord = Vec2(v.pos.x, v.pos.z);
		else if (biggestIndex == 2) // z
			v.texCoord = Vec2(v.pos.x, v.pos.y);
	};
	for (Vertex& v : _rawVertices)
		assignTexCoord(v);
	for (Vertex& v : _finalVertices)
		assignTexCoord(v);
}

void Mesh::calculateNormals() {
	for (size_t i = 0; i < _finalVertices.size(); i += 3) {
		Vec3 v0 = _finalVertices[i].pos;
		Vec3 v1 = _finalVertices[i + 1].pos;
		Vec3 v2 = _finalVertices[i + 2].pos;
		const Vec3 normal = normalize(cross(v1 - v0, v2 - v0));
		_finalVertices[i].normal = _finalVertices[i].color = normal;
		_finalVertices[i + 1].normal = _finalVertices[i + 1].color = normal;
		_finalVertices[i + 2].normal = _finalVertices[i + 2].color = normal;
	}

	std::vector<GLuint> indicesBuffer;
	for (auto &group : _indicesGroups)
		indicesBuffer.insert(indicesBuffer.end(), group.second._indices.begin(), group.second._indices.end());
	for (size_t i = 0; i < indicesBuffer.size() - 2; i += 3) {
		const Vec3 p = cross(
			_rawVertices[indicesBuffer[i + 1]].pos - _rawVertices[indicesBuffer[i]].pos,
			_rawVertices[indicesBuffer[i + 2]].pos - _rawVertices[indicesBuffer[i]].pos);
		for (short j = 0; j < 3; ++j)
			_rawVertices[indicesBuffer[i + j]].normal += p; // xyz
	}
	for (size_t i = 0; i < _rawVertices.size(); ++i)
		_rawVertices[i].normal = _rawVertices[i].color = normalize(_rawVertices[i].normal);
}

void Mesh::imGuiMeshInfos() {
	ImGui::Text("Name : %s", _name.c_str());
	drag3(_pos, "posOffset", 0.01f, -FLT_MAX, FLT_MAX, ImGui::GetContentRegionAvail().x * .6f);
	ImGui::DragFloat("Ouline Size", &_outlineSize, .01f, 0.f, FLT_MAX);

	if (ImGui::Button("Flip tex (R)"))
		_flags ^= USE_TEX;
	ImGui::SameLine();
	ImGui::Text("%.0f%%", _useTexPercentage * 100);
	ImGui::SameLine();
	if (ImGui::Button("Flip colors (F)"))
		_flags ^= USE_COLORS;
	ImGui::SameLine();
	ImGui::Text("%.0f%%", _useColorPercentage * 100);
	ImGui::SameLine();
	if (ImGui::Button("Flip Outline (T)"))
		_flags ^= OUTLINE_PER;
	if (ImGui::Button("Hide mesh (G)"))
		_flags ^= HIDE_MESH;
	ImGui::SameLine();
	if (ImGui::Button("Hide Outline (H)"))
		_flags ^= HIDE_OUTLINE;
	ImGui::SameLine();
	if (ImGui::Button("Use Static Tex (X)"))
		_flags ^= USE_STATIC_TEX;
}

void Mesh::destroy() const {
	_vaoRaw.deleteVAO();
	_vboRaw.deleteVBO();
	_eboRaw.deleteEBO();

	_vaoFinal.deleteVAO();
	_vboFinal.deleteVBO();
}

