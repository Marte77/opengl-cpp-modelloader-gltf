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
#include "VBO.h"
class VAO
{
public:
	//id do Vertex Array Object
	GLuint ID;
	//construtor gera um VAO e o seu ID
	VAO();

	//liga o VBO ao VAO usando um layout (layout usado no GLSL), eliga o attributo que tem infos como cor e posicao ao VAO
	void linkAttribute(VBO& VBO, GLuint layout, GLuint nComponents,GLenum type, GLsizeiptr stride, void *offset);
	void Bind();
	void UnBind();
	void Delete();
};

