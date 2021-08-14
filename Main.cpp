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
#include <stb/stb_image.h>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include "Util.h"
#include "Mesh.h"
#include "Model.h"


						//red, green,blue, alpha
GLfloat cordefundo[] = { 0.0f, 0.0f, 0.4f, 0.0f };

Util util;
float FOV;

void atualizarcordefundo() {
	std::ifstream ficheirocordefundo;
	ficheirocordefundo.open("cordefundo.txt");
	GLfloat red, green, blue, alpha;
	if (!ficheirocordefundo.is_open()) {
		return;
	}
	std::string cores;
	std::getline(ficheirocordefundo, cores);
	for (int i = 0; i < 4; i++) {
		int index = cores.find(',');
		std::string cor = cores.substr(0, index);
		cores = cores.substr(index+1);
		GLfloat corF = std::stof(cor);
		cordefundo[i] = corF;
	}
	ficheirocordefundo.close();
	glClearColor(cordefundo[0], cordefundo[1], cordefundo[2], cordefundo[3]);
}

void callback_user_redimensionaJanela(GLFWwindow* window, int width, int height);
GLFWwindow* inicializarjanela(const char* nomejanela) {
	
	//inicializar glfw e configurar
	if (!glfwInit()) {
		std::cout << "failed to initialize glfw";
		return NULL;
	}
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);// configuracao da janela e da versao do opengl
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//criar janela
	
	GLFWwindow* janela = glfwCreateWindow(util.winW, util.winH, nomejanela, NULL, NULL);
	if (janela == NULL) {
		std::cout << "falha a criar a janela, vou abortar" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(janela);
	//inicializar glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return NULL;
	}
	glViewport(0, 0, util.winW, util.winH);
	glfwSetFramebufferSizeCallback(janela, callback_user_redimensionaJanela);//mudar o tamanho da janela e tamanho de onde e renderizado

	return janela;
}


Camera inicializarCamara(GLFWwindow* janela) {
	float sens =50.0f, velocidade = 0.005f;
	//float sens =100.0f, velocidade = 0.01f;
	glm::vec3 posini = glm::vec3(
		0.0f,
		0.0f,
		0.0f);
	std::string ficheirodefinicoesjsonstr = util.lerFicheiro("definicoes.json");
	const char* ficheirodefinicoesjson = ficheirodefinicoesjsonstr.c_str();

	rapidjson::Document definicoesjson;
	if (definicoesjson.Parse(ficheirodefinicoesjson).HasParseError()) {
		std::cout << definicoesjson.GetParseError();
		return Camera(util.winW, util.winH, posini, velocidade, sens,janela);
	}

	if (!definicoesjson.IsObject()) {
		return Camera(util.winW, util.winH, posini, velocidade, sens,janela);
	}

	std::string sensstr, velocidadestr;
	velocidade = definicoesjson["camara"]["velocidade"].GetFloat();
	sens = definicoesjson["camara"]["sens"].GetFloat();
	FOV = definicoesjson["camara"]["fov"].GetFloat();
	posini = glm::vec3(
		definicoesjson["camara"]["posini"][0].GetFloat(),
		definicoesjson["camara"]["posini"][1].GetFloat(),
		definicoesjson["camara"]["posini"][2].GetFloat()
	);
	bool vsyncstr = definicoesjson["vsync"].GetBool();
	if (vsyncstr) {
		// Use this to disable VSync (not advized)
		glfwSwapInterval(0);
	}
	return Camera(util.winW, util.winH, posini, velocidade, sens,janela);
	
}

void atualizarFPSCounter(GLFWwindow *janela,double *diferencaTempo_, double* tempAgr_, double* tempAnt_, unsigned int* contador_) {
	double diferencaTempo = (*diferencaTempo_), tempAgr = (*tempAgr_), tempAnt = (*tempAnt_);
	unsigned int contador = (*contador_);
	std::string FPS = std::to_string((1.0 / diferencaTempo) * contador);
	//std::string ms = std::to_string((diferencaTempo / contador) * 1000);
	std::string titulo = "Boas - FPS: " + FPS;//+ " / " + ms + " ms";
	glfwSetWindowTitle(janela, titulo.c_str());
}
GLFWwindow* janela = inicializarjanela("Boas");
Camera camara = inicializarCamara(janela);
int main() {
	
	if (janela == NULL) {
		return -1;
	}

	//shader e objetos opengl 
#pragma region
	//gerar shader usando os codigos do vertex shader e fragment shader
	shader shaderProgram("vertexShader.glsl", "fragmentShader.glsl");

	glm::vec4 corLuz = util.getCorLuz();
	glm::vec3 luzPosicao = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 luzModel = glm::mat4(1.0f);
	luzModel = glm::translate(luzModel, luzPosicao);
	
	shaderProgram.Ativar();
	//exportar o vetor da cor da luz
	shaderProgram.setLightColorUniform(corLuz);
	shaderProgram.setLightPosUniform(luzPosicao);
#pragma endregion

	//"organizar" a renderizacao de modo a nao haver sobreposicao de triangulos que deviam estar atras
	// 	   pode se fazer isto manualmente tambem
	//Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	
	
	//carregar model
	Model coelho("res/models/sword/scene.gltf");
	
	//Model espada("res/models/sword/scene.gltf");
	//espada.setVetorMov(0.0f);

	
	//variaveis para os fps
	double tempAnt = 0.0, tempAgr = 0.0, diferencaTempo;
	unsigned int contador = 0;
	while (!glfwWindowShouldClose(janela))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		atualizarcordefundo();
		
		
		//regiao fps
#pragma region
		tempAgr = glfwGetTime();
		diferencaTempo = tempAgr - tempAnt;
		contador++;
		if (diferencaTempo >= 1.0/30.0) {
			atualizarFPSCounter(janela,&diferencaTempo,&tempAgr,&tempAnt,&contador);
		}
#pragma endregion

		camara.Inputs(janela);
		camara.updateMatrix(FOV, 0.1f, 100.0f);
		
		
		coelho.Draw(shaderProgram, camara);
		//espada.move(shaderProgram, camara);
		
		glfwSwapBuffers(janela);
		glfwPollEvents();

		
	}

	shaderProgram.Apagar();

	glfwDestroyWindow(janela);
	glfwTerminate();
	return 0;
}


void callback_user_redimensionaJanela(GLFWwindow* window, int width, int height) {
	//alterar a janela de renderizacao quando o utilizador redimensiona a janela de modo a ficar sempre justo
	glViewport(0, 0, width, height);
	
	if (width != 0 || height != 0) {
		camara.height = height;
		camara.width = width;
		util.winH = height;
	util.winW = width;
	}
}