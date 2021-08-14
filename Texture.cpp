#include "Texture.h"
Texture::Texture(const char* sourceImg, GLenum textype, GLenum slot, GLenum format, GLenum pixelType, 
	int upscaleMethodMin, int upscaleMethodMag, int repeatMethodS, int repeatMethodT, const char* tipotexturadesc_) {
	glEnumType = textype;
	tipoTexturaDesc = tipotexturadesc_;
							 //numero de canais de cores
	int widthImg, heightImg, numColCh;
	//o stbi e o opengl leem a imagem de maneira diferente sendo que sem isto a imagem aparece ao contrario
	stbi_set_flip_vertically_on_load(true);

	unsigned char* bytes = stbi_load(sourceImg, &widthImg, &heightImg, &numColCh, 0);
	std::cout << widthImg << "," << heightImg << ","<<numColCh << std::endl;

	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(textype, ID);
	//indicar como fazer upscale da imagem
	glTexParameteri(textype, GL_TEXTURE_MIN_FILTER, upscaleMethodMin);
	glTexParameteri(textype, GL_TEXTURE_MAG_FILTER, upscaleMethodMag);

	//indicar o que fazer com espaco de sobra (nao e obrigatorio)
	glTexParameteri(textype, GL_TEXTURE_WRAP_S, repeatMethodS);
	glTexParameteri(textype, GL_TEXTURE_WRAP_T, repeatMethodT);
	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(textype, GL_TEXTURE_BORDER_COLOR, flatColor);

	//ligar a imagem e as suas infos com o objeto de textura do opengl criado
	glTexImage2D(textype, 0,/*color channel*/ GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);
	// Generates MipMaps
	glGenerateMipmap(textype);

	//apagar imagem ja que ja esta no objeto da textura do opengl
	stbi_image_free(bytes);
	//unbind textura
	glBindTexture(textype, 0);
	unit = slot;
}

Texture::Texture(const char* image, const char* texType, GLuint slot)
{
	// Assigns the type of the texture ot the texture object
	tipoTexturaDesc = texType;
	glEnumType = GL_TEXTURE_2D;
	
	// Stores the width, height, and the number of color channels of the image
	int widthImg, heightImg, numColCh;
	// Flips the image so it appears right side up
	stbi_set_flip_vertically_on_load(true);
	// Reads the image from a file and stores it in bytes
	unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	// Assigns the texture to a Texture Unit
	glActiveTexture(GL_TEXTURE0 + slot);
	unit = slot;
	glBindTexture(GL_TEXTURE_2D, ID);

	// Configures the type of algorithm that is used to make the image smaller or bigger
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Configures the way the texture repeats (if it does at all)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Extra lines in case you choose to use GL_CLAMP_TO_BORDER
	// float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	// glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, flatColor);

	// Check what type of color channels the texture has and load it accordingly
	if (numColCh == 4)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 3)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else if (numColCh == 1)
		glTexImage2D
		(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			widthImg,
			heightImg,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			bytes
		);
	else
		throw std::invalid_argument("Automatic Texture type recognition failed");

	// Generates MipMaps
	glGenerateMipmap(GL_TEXTURE_2D);

	// Deletes the image data as it is already in the OpenGL Texture object
	stbi_image_free(bytes);

	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::texUnit(shader& shader, const char* uniform, GLuint unit) {
	GLuint texYUniform = glGetUniformLocation(shader.ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader.Ativar();
	glUniform1i(texYUniform, unit);
}

void Texture::Delete() {
	glDeleteTextures(1, &ID);
}

void Texture::Bind() {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(glEnumType, ID);
}

void Texture::UnBind() {
	glBindTexture(glEnumType, 0);
}