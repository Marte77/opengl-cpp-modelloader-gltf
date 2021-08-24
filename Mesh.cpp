#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex>& vertices_, std::vector<GLuint>& indices_, std::vector<Texture>& textures_) {
	Mesh::vertices = vertices_;
	Mesh::indices = indices_;
	Mesh::texturas = textures_;

	//gerar VAO e dar bind
	VAO.Bind();

	//gerar VBO e ligar lhe os vertices
	VBO VBO(vertices_);
	//gerar EBO e ligar lhe os indices dos vertices (a ordem em que sao renderizados
	EBO EBO(indices_);

	////ligar o VBO ao VAO
	//VAO1.linkVBO(VBO1, 0);
	//ligar os atributos, o primeiro especifica as coordenadas do vertice, o segundo especifica as cores, o terceiro as coords relativas a textura etc...
							//layout para o glsl			//offset das coords, ou seja os primeiros 3 numeros sao vertices
	VAO.linkAttribute(VBO, 0, 3, GL_FLOAT,sizeof(Vertex), (void*)0);
							//layout para o glsl			//offset das cores, ou seja, os 3 numeros a seguir as coords referem-se as cores
	VAO.linkAttribute(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
							//layout para a textura			//offset das cores, ou seja, os 3 numeros a seguir as cores sao da textura
	VAO.linkAttribute(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
							//layout para os normais		//offset das texturas, ou seja, os 2 numeros a seguir as texturas sao das normais
	VAO.linkAttribute(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));
	//visualizacao [coord.x,coord.y,coord.z,cor.r,cor.g,cor.b,tex.x,tex.y,tex.z,normal.s,normal.t]

	
	
	//remover a ligacao dos VBO, VAO e EBO para prevenir modificacao acidental
	VAO.UnBind();
	VBO.UnBind();
	EBO.UnBind();
}

void Mesh::DrawOutdated(shader& shader, Camera& camara) {
	shader.Ativar();
	VAO.Bind();

	unsigned int numDiffuse = 0, numSpecular = 0;//variaveis para guardar o numero de texturas normais e specular
	for (unsigned int i = 0; i < texturas.size(); i++) {
		std::string num, type = texturas[i].tipoTexturaDesc;
		if (type == "diffuse") {
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular") {
			num = std::to_string(numSpecular++);
		}
		
		texturas[i].texUnit(shader, (type + num).c_str(),i);
		texturas[i].Bind();
	}
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camara.posicao.x, camara.posicao.y, camara.posicao.z);
	camara.Matrix(shader, "camMatrix");

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::Draw(shader& shader,	Camera& camera, glm::mat4 matrix,
	glm::vec3 translation, glm::quat rotation, glm::vec3 scale)
{
	// Bind shader to be able to access uniforms
	shader.Ativar();
	VAO.Bind();

	// Keep track of how many of each type of textures we have
	unsigned int numDiffuse = 0;
	unsigned int numSpecular = 0;

	for (unsigned int i = 0; i < texturas.size(); i++)
	{
		std::string num;
		std::string type = texturas[i].tipoTexturaDesc;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		texturas[i].texUnit(shader, (type + num).c_str(), i);
		texturas[i].Bind();
	}
	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.posicao.x, camera.posicao.y, camera.posicao.z);
	camera.Matrix(shader, "camMatrix");

	// Initialize matrices
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	// Transform the matrices to their correct form
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::DrawOBJ(shader& shader, Camera& camera)
{
	glm::mat4 matrix = glm::mat4(1.0f);
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

	shader.Ativar();
	VAO.Bind();
	unsigned int numDiffuse = 1, numSpecular = 1;
	for (unsigned int i = 0; i < texturas.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		std::string num;
		std::string type = texturas[i].tipoTexturaDesc;
		if (type == "diffuse")
		{
			num = std::to_string(numDiffuse++);
		}
		else if (type == "specular")
		{
			num = std::to_string(numSpecular++);
		}
		texturas[i].texUnit(shader, (type + num).c_str(), i);
		texturas[i].Bind();
	}
	// Take care of the camera Matrix
	glUniform3f(glGetUniformLocation(shader.ID, "camPos"), camera.posicao.x, camera.posicao.y, camera.posicao.z);
	camera.Matrix(shader, "camMatrix");
	// Initialize matrices
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	// Transform the matrices to their correct form
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	// Push the matrices to the vertex shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

	// Draw the actual mesh
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}
