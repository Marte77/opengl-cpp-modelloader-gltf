#version 330 core
in vec3 color;
in vec2 texturaCoords;
in vec3 Normal;
in vec3 currentPos;

out vec4 FragColor;


uniform sampler2D diffuse0;//diz ao opengl que unidade de textura deve usar
uniform sampler2D specular0;

uniform vec4 lightColor;
uniform vec3 lightPos;
uniform vec3 camPos;

//existem 3 tipos de luz, direcional, point e spotlight, esta funcao calcula a point light
vec4 pointLight(float a, float b){
	vec3 lightVec = lightPos - currentPos;
	float dist = length(lightVec);

	float intensidade = 1.0f / (a * dist * dist + b * dist + 1.0f);


	float luzAmbiente = 0.2f; //simula a reflexao da luz em outros objetos
	
	//normalizar o vec Normal e a obter a direcao da luz
	vec3 normal = normalize(Normal);
	vec3 direcaoLuz = normalize(lightVec);

	float diffuse = max(dot(normal, direcaoLuz),0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - currentPos);
	vec3 reflectionDirection = reflect(-direcaoLuz, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;
    //FragColor = vec4(color, 1.0f);

    /*para fazer com que a textura tenha relevo soma-se a segunda texture(...).r,
																fica com o r pois a textura do 
																specular so tem um canal de cor*/
	return (texture(diffuse0, texturaCoords) * (diffuse * intensidade + luzAmbiente) + texture(specular0, texturaCoords).r * specular * intensidade) * lightColor;
}

vec4 directionalLight()
{
// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - currentPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(diffuse0, texturaCoords) * (diffuse + ambient) + texture(specular0, texturaCoords).r * specular) * lightColor;
}
	
vec4 spotLight(float outerCone, float innerCone)
{
// outer cone and inner cone control how big the area that is lit up is


	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos - currentPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - currentPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(diffuse0, texturaCoords) * (diffuse * inten + ambient) + texture(specular0, texturaCoords).r * specular * inten) * lightColor;
}

void main()
{
	FragColor = spotLight(0.90f,0.95f);
	//FragColor = pointLight(3.0f,0.7f);
	//FragColor = directionalLight();
}