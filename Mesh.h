#pragma once
#include <string>
#include "EBO.h"
#include "VAO.h"
#include "Camera.h"
#include "Texture.h"

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> texturas;

	VAO VAO;

	Mesh(std::vector<Vertex>& vertices_, std::vector<GLuint>& indices_, std::vector<Texture>& textures_);
	//outdated
	void DrawOutdated(shader& shader, Camera& camara);
	//novo
	void Draw(shader& shader, Camera& camara, glm::mat4 matrix = glm::mat4(1.0f),
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f),
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));
	void DrawOBJ(shader& shader, Camera& camara);
};

