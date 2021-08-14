#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "shader.h"



class Camera
{
public:
	int cursormode = GLFW_CURSOR_HIDDEN;
	glm::vec3 posicao, //posicao da camara no espaco
		orientacao= glm::vec3(0.0f,0.0f,-1.0f) ,//para onde esta a olhar
		up=glm::vec3(0.0f,1.0f,0.0f) /*up dimension*/;
	glm::mat4 cameraMatrix = glm::mat4(1.0f);
	int width, height;

	float velocidade ;
	float sensivity ;

	Camera(int width_, int height_, glm::vec3 pos_,float velocidade_, float sens,GLFWwindow *janela);
	void updateMatrix(float FOVdegree, float nearPlane, float farPlane);
	void Matrix(shader& shader, const char* uniform);
	void Inputs(GLFWwindow* janela);
	void Inputs(GLFWwindow* janela, shader model);

	void logPos(int ncoisas) {
		if (ncoisas == 3) {
			std::cout << "xyz = (" << posicao.x << " ," << posicao.y << " ," << posicao.z << ")" << std::endl;
			std::cout << "prst = (" << posicao.p<< " ," << posicao.r << " ," << posicao.s << " ," << posicao.t << ")" << std::endl;
			std::cout << "bg = (" << posicao.b<< " ," << posicao.g << ")" << std::endl;
		}
		else if (ncoisas == 2) {
			std::cout << "xyz = (" << posicao.x << " ," << posicao.y << " ," << posicao.z << ")" << std::endl;
			std::cout << "prst = (" << posicao.p << " ," << posicao.r << " ," << posicao.s << " ," << posicao.t << ")" << std::endl;
		}
		else if (ncoisas == 1) {
			std::cout << "xyz = (" << posicao.x << " ," << posicao.y << " ," << posicao.z << ")" << std::endl;
		}
	}
	void logOri(int ncoisas) {
		if (ncoisas == 3) {
			std::cout << "xyz = (" << orientacao.x << " ," << orientacao.y << " ," << orientacao.z << ")" << std::endl;
			std::cout << "prst = (" << orientacao.p << " ," << orientacao.r << " ," << orientacao.s << " ," << orientacao.t << ")" << std::endl;
			std::cout << "bg = (" << orientacao.b << " ," << orientacao.g << ")" << std::endl;
		}
		else if (ncoisas == 2) {
			std::cout << "xyz = (" << orientacao.x << " ," << orientacao.y << " ," << orientacao.z << ")" << std::endl;
			std::cout << "prst = (" << orientacao.p << " ," << orientacao.r << " ," << orientacao.s << " ," << orientacao.t << ")" << std::endl;
		}
		else if (ncoisas == 1) {
			std::cout << "xyz = (" << orientacao.x << " ," << orientacao.y << " ," << orientacao.z << ")" << std::endl;
		}
	}

};

