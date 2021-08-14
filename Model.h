#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <ios>
#include <sstream>  
#include <string>  
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>	
#include<glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include<json/json.h>
#include "Mesh.h"

using json = nlohmann::json;

//conjunto de mesh's
class Model
{
public:
	Model(const char* fichModel);
	glm::vec3 vetorMov = glm::vec3(0.0f);
	glm::vec3 vetorMovVelocidade = glm::vec3(0.0f);

	void Draw(shader& shader, Camera& camara);
	//igual ao Draw so que usa o vetorMov para mudar a posicao do modelo no espaco
	void move(shader& shader, Camera& camara);
	//vai definir o valor que vai ser usado para incrementar o valorMov na funcao Move
	void setVetorMov(float x = 0, float y = 0, float z = 0);


private:
	const char* file;
	std::vector<unsigned char> data;
	json JSON;
	

	// All the meshes and transformations
public:
	std::vector<Mesh> meshes;
private:
	std::vector<glm::vec3> translationsMeshes;
	std::vector<glm::quat> rotationsMeshes;
	std::vector<glm::vec3> scalesMeshes;
	std::vector<glm::mat4> matricesMeshes;

	// Prevents textures from being loaded twice
	std::vector<std::string> loadedTexName;
	std::vector<Texture> loadedTex;


	// Loads a single mesh by its index
	void loadMesh(unsigned int indMesh);

	// Traverses a node recursively, so it essentially traverses all connected nodes
	void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f));


	// Gets the binary data from a file
	std::vector<unsigned char> getData();
	// Interprets the binary data into floats, indices, and textures
	std::vector<float> getFloats(json accessor);
	std::vector<GLuint> getIndices(json accessor);
	std::vector<Texture> getTextures();
	// Assembles all the floats into vertices
	std::vector<Vertex> assembleVertices
	(
		std::vector<glm::vec3> positions,
		std::vector<glm::vec3> normals,
		std::vector<glm::vec2> texUVs
	);
	// Helps with the assembly from above by grouping floats
	std::vector<glm::vec2> groupFloatsVec2(std::vector<float>floatVec);
	std::vector<glm::vec3> groupFloatsVec3(std::vector<float>floatVec);
	std::vector<glm::vec4> groupFloatsVec4(std::vector<float>floatVec);
	
};



