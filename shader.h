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
#include<glm/gtc/type_ptr.hpp>
#include "Util.h"
class shader
{
	Util utilClass;
	void errosDeCompilacao(GLuint shader);
	void errosDeLigacao();
public:
		//id do shader program
		GLuint ID;
		//inicializa o shader com o seu respetivo vertex e fragment shaders
		shader(const char* vertexShaderFich, const char* fragmentShaderFich);
		//ativa o shader program
		void Ativar();
		//apaga o shader program
		void Apagar();

		void setLightColorUniform(glm::vec4 corLuz) {
			glUniform4f(glGetUniformLocation(ID, "lightColor"), corLuz.x, corLuz.y, corLuz.z, corLuz.w);
		};

		void setModelUniform(glm::mat4 model) {
			glUniformMatrix4fv(glGetUniformLocation(ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		}

		void setLightPosUniform(glm::vec3 luzPosicao) {
			glUniform3f(glGetUniformLocation(ID, "lightPos"), luzPosicao.x, luzPosicao.y, luzPosicao.z);

		}
};

