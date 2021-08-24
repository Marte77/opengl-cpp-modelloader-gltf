#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <ios>
#include <sstream>  
#include <string>  
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>	
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <json/json.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"

using json = nlohmann::json;


struct header {
	uint32_t length, version;
	char magic[5], lengthChar[4]; //4 bytes mais o null char
};
struct chunk {
	uint32_t length; //tamanho do array data
	char type[5], lengthChar[4];
	unsigned char* data;
};
enum chunks {
	onlyjson, //so tem json chunk
	jsonbin //tem json chunk e bin chunk
};
struct glbfile {
	//especificacao https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#glb-file-format-specification
	header head;
	chunks chunksenum;
	chunk chunkArr[2];
};




//conjunto de mesh's
class Model
{
public:
	std::string fileType;
	Model(const char* fichModel);
	~Model() {
		if (fileType == "glb") {
			free(infoglb.chunkArr[0].data);
			if (infoglb.chunksenum == jsonbin) {
				free(infoglb.chunkArr[1].data);
			}
		}
	};
	glm::vec3 vetorMov = glm::vec3(0.0f);
	glm::vec3 vetorMovVelocidade = glm::vec3(0.0f);

	void Draw(shader& shader, Camera& camara);
	//igual ao Draw so que usa o vetorMov para mudar a posicao do modelo no espaco
	void move(shader& shader, Camera& camara);
	//vai definir o valor que vai ser usado para incrementar o valorMov na funcao Move
	void setVetorMov(float x = 0, float y = 0, float z = 0);


private:
	void loadFromGLTF(const char* file);
	void loadFromOBJ(const char* file);
	const char* file;
	std::vector<unsigned char> data;
	json JSON;
	
	//glb loader
	glbfile infoglb;
	void loadFromGLB(const char* file);
	void lerFicheiroGLB(const char* file);
	std::vector<unsigned char> getDataFromGLB();
	

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
	

	//load assimp
	void processNode(aiNode* node, const aiScene *scene);
	Mesh processMesh(aiMesh* mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};



