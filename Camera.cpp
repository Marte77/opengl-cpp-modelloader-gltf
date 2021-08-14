#include "Camera.h"

Camera::Camera(int width_, int height_, glm::vec3 pos_, float velocidade_, float sens, GLFWwindow* janela) {
	width = width_;
	height = height_;
	posicao = pos_;
	velocidade = velocidade_;
	sensivity = sens;
	// METER O CURSOR INVISIVEL QUANDOE STA EM CIMA DA JANELA
	glfwSetInputMode(janela, GLFW_CURSOR, cursormode);

}

void Camera::updateMatrix(float FOVdegree, float nearPlane, float farPlane) {
	//inicializar as matrizes caso contrario seriam nulas
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 proj = glm::mat4(1.0f);

	//fazer a camara olhar para o sitio certo
	view = glm::lookAt(posicao, posicao + orientacao, up);
	//adicionar perspetiva
	proj = glm::perspective(glm::radians(FOVdegree), (float)(width / height), nearPlane, farPlane);
	cameraMatrix = proj * view;
	//glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(proj * view));
}

void Camera::Inputs(GLFWwindow* janela, shader model)
{
	if (glfwGetKey(janela, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(janela, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		if (cursormode == GLFW_CURSOR_HIDDEN) {
			cursormode = GLFW_CURSOR_NORMAL;
		}
		else {
			cursormode = GLFW_CURSOR_HIDDEN;
		}
		glfwSetInputMode(janela, GLFW_CURSOR, cursormode);
	}
	if (cursormode == GLFW_CURSOR_HIDDEN) {
		double mX, mY;
		glfwGetCursorPos(janela, &mX, &mY);

		//normalizar as coordenadas do cursor de modo a que fiquem no meio do ecra e depois converte para degree
		float rotacaoX = sensivity * (float)(mY - (height / 2)) / height
			, rotacaoY = sensivity * (float)(mX - (width / 2)) / width;

		//calcular a nova orientacao e depois decidir se esta é 'legal' ou nao
		glm::vec3 newOrientation = glm::rotate(orientacao, glm::radians(-rotacaoX), glm::normalize(glm::cross(orientacao, up)));
		if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
			orientacao = newOrientation;
		}

		orientacao = glm::rotate(orientacao, glm::radians(-rotacaoY), up);

		glfwSetCursorPos(janela, (width / 2), (height / 2));
	}

	if (glfwGetKey(janela, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		
	}
}

void Camera::Matrix(shader& shader, const char* uniform){
	//mandar a matriz da camara para o vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* janela) {
	if (glfwGetKey(janela, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(janela, GLFW_KEY_UP) == GLFW_PRESS)
	{
		posicao += velocidade * orientacao;
	}
	if (glfwGetKey(janela, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(janela, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		posicao += velocidade * -glm::normalize(glm::cross(orientacao, up));
	}
	if (glfwGetKey(janela, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(janela, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		posicao += velocidade * -orientacao;
	}
	if (glfwGetKey(janela, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(janela, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		posicao += velocidade * glm::normalize(glm::cross(orientacao, up));
	}
	if (glfwGetKey(janela, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(janela, true);
	}
	if (glfwGetKey(janela, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		posicao += velocidade * up;
	}
	if (glfwGetKey(janela, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		posicao += velocidade * -up;
	}
	

	if (glfwGetKey(janela, GLFW_KEY_Y) == GLFW_PRESS) {
		std::cout << orientacao.x << "," << orientacao.y << "," << orientacao.z << std::endl;
	}
	if (glfwGetKey(janela, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(janela, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		if (cursormode == GLFW_CURSOR_HIDDEN) {
			cursormode = GLFW_CURSOR_NORMAL;
		}
		else {
			cursormode = GLFW_CURSOR_HIDDEN;
		}
		glfwSetInputMode(janela, GLFW_CURSOR, cursormode);
	}
	if(cursormode == GLFW_CURSOR_HIDDEN){
		double mX, mY;
		glfwGetCursorPos(janela, &mX, &mY);

		//normalizar as coordenadas do cursor de modo a que fiquem no meio do ecra e depois converte para degree
		float rotacaoX = sensivity * (float)(mY - (height / 2)) / height
			, rotacaoY = sensivity * (float)(mX - (width / 2)) / width;

		//calcular a nova orientacao e depois decidir se esta é 'legal' ou nao
		glm::vec3 newOrientation = glm::rotate(orientacao, glm::radians(-rotacaoX), glm::normalize(glm::cross(orientacao, up)));
		if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f)) {
			orientacao = newOrientation;
		}

		orientacao = glm::rotate(orientacao, glm::radians(-rotacaoY), up);

		glfwSetCursorPos(janela, (width / 2), (height / 2));
	}
}
