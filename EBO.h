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
#include "Util.h"

class EBO
{
public:
	//id do Elements Buffer Object
	GLuint ID;
	//cria um EBO e liga o aos indices
	EBO(std::vector<GLuint>&indices);
	void Bind();
	void UnBind();
	void Delete();
};

