#include "shader.h"

shader::shader(const char* vertexShaderFich, const char* fragmentShaderFich) {
	std::string vertexShaderSourcestr = utilClass.lerFicheiro(vertexShaderFich);
	std::string fragmentShaderSourcestr = utilClass.lerFicheiro(fragmentShaderFich);
	const char* vertexShaderSource = vertexShaderSourcestr.c_str();
	const char* fragmentShaderSource = fragmentShaderSourcestr.c_str();
	//criar objeto do Vertex Shader (o seu ponteiro fica guardado na variavel)
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//ligar o codigo do shader com o shader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	//compilar shader 
	glCompileShader(vertexShader);
	//verificar se houve erros de compilacao
	errosDeCompilacao(vertexShader);


	//criar objeto do Vertex Shader (o seu ponteiro fica guardado na variavel)
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//ligar o codigo do shader com o shader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	//compilar shader 
	glCompileShader(fragmentShader);
	//verificar se houve erros de compilacao
	errosDeCompilacao(fragmentShader);
	//criar objeto do shader program (ponteiro fica guardado na variavel
	ID = glCreateProgram();
	//ligar o vertex shader e fragment shader ao programa
	glAttachShader(ID,vertexShader);
	glAttachShader(ID,fragmentShader);
	glLinkProgram(ID);
	// verificar erros de ligacao
	errosDeLigacao();


	//apagar os shaders criados para libertar memoria ja que nao vao ser usados
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void shader::Ativar() {
	//ativar shader program
	glUseProgram(ID);
}
void shader::Apagar() {
	//delete shader program
	glDeleteProgram(ID);
}

void shader::errosDeCompilacao(GLuint shader) {
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}
void shader::errosDeLigacao() {
	int success;
	char infoLog[512];
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
}