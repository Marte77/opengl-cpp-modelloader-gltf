#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;


out vec3 color;//variavel de cor para o fragment shader
out vec2 texturaCoords;
out vec3 Normal;
out vec3 currentPos;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
	color = aColor;									
	texturaCoords = /*mat2(0.0,1.0,1.0,0.0)*/ mat2(0.0, -1.0, 1.0, 0.0)* aTex;	
	Normal = aNormal;								
	currentPos = vec3(model *translation * -rotation * scale* vec4(aPos, 1.0f));
	
	//color = aColor;
	//texturaCoords = aTex;
	//Normal = aNormal;
	//currentPos = vec3(model * vec4(aPos, 1.0f));

	//output a posicao de todos os vertices
	gl_Position =camMatrix * vec4(currentPos, 1.0);
}