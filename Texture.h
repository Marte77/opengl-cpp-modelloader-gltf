#pragma once
#include <iostream>

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
#include <stb/stb_image.h>
#include "Util.h"
#include "shader.h"

class Texture
{
public:
	GLuint ID;
	GLenum glEnumType;
	const char* tipoTexturaDesc;
	GLuint unit;
	//o param tipotexturadesc_ indica o tipo de textura: "diffuse" ou "specular"
	Texture(const char* sourceImg, GLenum textype, GLenum slot, GLenum format, GLenum pixelType,
		int upscaleMethodMin, int upscaleMethodMag, int repeatMethodS, int repeatMethodT, const char* tipotexturadesc_);
	Texture(const char* image, const char* texType, GLuint slot);
	//atribui uma unidade de textura a textura
	void texUnit(shader& shader, const char* uniform, GLuint unit);

	void Bind();
	void UnBind();
	void Delete();
};

