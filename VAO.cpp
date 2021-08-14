#include "VAO.h"
VAO::VAO() {
	glGenVertexArrays(1, &ID);
}

void VAO::linkAttribute(VBO& VBO, GLuint layout, GLuint nComponents, GLenum type, GLsizeiptr stride, void* offset) {
	VBO.Bind();
	glVertexAttribPointer(layout, nComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.UnBind();
}

void VAO::Bind() {
	glBindVertexArray(ID);
}

void VAO::UnBind() {
	glBindVertexArray(0);

}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}