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
#include "Util.h"

struct Vertex {
	glm::vec3 position, normal, color;
	glm::vec2 textureUV;
	
	/*//usados para modelos obj
	// tangent
	glm::vec3 Tangent;
	// bitangent
	glm::vec3 Bitangent;*/
};

class VBO
{
public:
	//id do Vertex Buffer Object
	GLuint ID;
	//inicializa o VBO e liga-o aos vertices 
	VBO(std::vector<Vertex>&vertices);
	
	void Bind();
	void UnBind();
	void Delete();
};

