#include"Model.h"

Model::Model(const char* file)
{
	std::string filestr(file);
	int indexExtension = filestr.find_last_of('.')+1;//ja tem em conta o index 0
	filestr = filestr.substr(indexExtension);// +1 para ignorar o '.'
	if (filestr.compare("gltf") == 0) {
		loadFromGLTF(file);
	}
	else if (filestr.compare("obj") == 0) {
		loadFromOBJ(file);
	}
	else if (filestr.compare("glb") == 0) {
		loadFromGLB(file);
	}
	Model::fileType = filestr;
}


void Model::move(shader& shader, Camera& camara)
{
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Mesh::Draw(shader, camara, matricesMeshes[i],
			vetorMov);
		vetorMov += vetorMovVelocidade;
	}
}

void Model::setVetorMov(float x, float y, float z)
{
	vetorMovVelocidade.x = x;
	vetorMovVelocidade.y = y;
	vetorMovVelocidade.z = z;
}

void Model::Draw(shader& shader, Camera& camera)
{
	int x = 0;
	// Go over all meshes and draw each one
	if(fileType == "gltf")
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Mesh::Draw(shader, camera, matricesMeshes[i]);
		}
	else
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Mesh::DrawOBJ(shader, camera);
		}
}



//regiao loadfromglb
#pragma region
void Model::loadFromGLB(const char* filePath) {
	
	lerFicheiroGLB(filePath);

	JSON = json::parse(infoglb.chunkArr[0].data);
	if (infoglb.chunksenum == onlyjson) {
		std::cout << "ERROR::MODEL::LOADFROMGLB::ONLY_JSON_INFO" << std::endl;
		throw(errno);
	}
	data = getDataFromGLB();
	//como o GLB é um ficheiro gltf com todas as outras infos combinadas
	// logo para carregar tudo é bastante parecido exceto as texturas, que estao todas no mesmo ficheiro	
	Model::file = filePath;
	//traverseNode(0);
	traverseNodeForLoop();
}
void Model::traverseNodeForLoop(glm::mat4 matrix)
{
	json nodes = JSON["nodes"];

	for (int i = 0; i < nodes.size(); i++) {
		json object = nodes[i];
		glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
		if (object.find("translation") != object.end())
		{
			float transValues[3];
			for (unsigned int i = 0; i < object["translation"].size(); i++)
				transValues[i] = (object["translation"][i]);
			translation = glm::make_vec3(transValues);
		}
		glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		if(object.contains("rotation")){
			float rotValues[4] =
			{
				object["rotation"][3],
				object["rotation"][0],
				object["rotation"][1],
				object["rotation"][2]
			};
			rotation = glm::make_quat(rotValues);
		}
		glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
		if (object.find("scale") != object.end())
		{
			float scaleValues[3];
			for (unsigned int i = 0; i < object["scale"].size(); i++)
				scaleValues[i] = (object["scale"][i]);
			scale = glm::make_vec3(scaleValues);
		}
		glm::mat4 matNode = glm::mat4(1.0f);
		if (object.find("matrix") != object.end())
		{
			float matValues[16];
			for (unsigned int i = 0; i < object["matrix"].size(); i++)
				matValues[i] = (object["matrix"][i]);
			matNode = glm::make_mat4(matValues);
		}
		// Initialize matrices
		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 rot = glm::mat4(1.0f);
		glm::mat4 sca = glm::mat4(1.0f);

		// Use translation, rotation, and scale to change the initialized matrices
		trans = glm::translate(trans, translation);
		rot = glm::mat4_cast(rotation);
		sca = glm::scale(sca, scale);

		// Multiply all matrices together
		glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;
		// Check if the node contains a mesh and if it does load it
		if (object.find("mesh") != object.end())
		{
			translationsMeshes.push_back(translation);
			rotationsMeshes.push_back(rotation);
			scalesMeshes.push_back(scale);
			matricesMeshes.push_back(matNextNode);
			loadMesh(object["mesh"]);
		}
	}
	
}
std::vector<unsigned char> Model::getDataFromGLB() {
	std::vector<unsigned char> data;
	for (int i = 0; i < infoglb.chunkArr[1].length; i++) {
		data.push_back(infoglb.chunkArr[1].data[i]);
	}
	return data;
}
void Model::lerFicheiroGLB(const char*filePath){
	FILE* file = fopen(filePath, "rb");
	fseek(file, 0, SEEK_END);
	long int fsize = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* str = (char*)malloc(fsize + 1);
	fread(str, 1, fsize, file);
	fclose(file);
	str[fsize] = 0;

	//glb é composto por uma header, uma chunk 0 em JSON e depois, se houver, outras chunks em binario
	// uint32 = 4bytes
	//header: magic (uint32) | version (uint32) | length (uint32)
	//chunks: chunkLength (uint32) | chunkType (uint32) | chunkData (ubyte[])
	//retirado de https://docs.fileformat.com/3d/glb/

	infoglb.chunkArr[0].length = -1;
	infoglb.chunkArr[1].length = -1;

	infoglb.head.length = 0; infoglb.head.version = 0;
	for (int i = 0; i < 4; i++) {
		infoglb.head.magic[i] = str[i];
		infoglb.head.version += str[i + 4 * UM_BYTE];
		infoglb.head.lengthChar[i] = str[i + 8];
	}
	infoglb.head.magic[4] = '\0';
	infoglb.head.length = *(uint32_t*)infoglb.head.lengthChar;//converter os 4 bytes no char array para um uint
														//!!!!!se bem que fazer isto e desnecessario porque a length indicada 
														//            no header é o tamanho do ficheiro todo, header incluida
														//mas vou deixar para mostrar que sei fazer :^) 
														//            (ps: parece que vou ter de usar isso para as outras lol)
	//std::cout << "<HEADER >" << std::endl
	//	<< "\tmagic: " << infoglb.head.magic << std::endl
	//	<< "\tversion: " << infoglb.head.version << std::endl
	//	<< "\tlength: " << infoglb.head.length << std::endl
	//	<< "\tlengthChar: " << infoglb.head.lengthChar << std::endl
	//	<< "<HEADER />" << std::endl;

	if (strcmp(infoglb.head.magic, "glTF") != 0) {
		std::cout << "ERROR::MODEL::GLB_LOADER::UNSUPORTED_BINARY_HEADER" << std::endl;
		free(str);
		throw("ERROR::MODEL::GLB_LOADER::UNSUPORTED_BINARY_HEADER");
	}
	if (infoglb.head.version < 2) {
		std::cout << "ERROR::MODEL::GLB_LOADER::UNSUPORTED_LEGACY_FILE_SUPORTED_VERSION" << infoglb.head.version << std::endl;
		free(str);
		throw("ERROR::MODEL::GLB_LOADER::UNSUPORTED_LEGACY_FILE_SUPORTED_VERSION");
	}

	int index = 12; //numero de bytes da header, vai ser incrementado por 4 ou pelo tamanho da chunk

	int nLoops = 0;
	while (index < infoglb.head.length) {
		nLoops++;
		chunk payload;
		for (int x = 0; x < 4; x++) {
			payload.lengthChar[x] = str[index + x];
			payload.type[x] = str[index + 4 + x];
		}
		payload.length = *(uint32_t*)payload.lengthChar;
		payload.type[4] = '\0';
		index += 4;//avancar para o chunktype
		index += 4;//avancar para o chunkdata

		payload.data = (unsigned char*)malloc(sizeof(unsigned char) * (payload.length + 1));

		for (int x = 0; x < payload.length; x++) {
			payload.data[x] = str[index + x];
		}
		payload.data[payload.length] = '\0';
		index += payload.length;
		//std::cout << "<Payload>" << std::endl
		//	<< "\tlength: " << payload.length << std::endl
		//	<< "\ttype: " << payload.type << std::endl
		//	<< "\tstrlen(data): " << strlen(payload.data) << std::endl
		//	<< "<Payload/>" << std::endl;


		infoglb.chunkArr[nLoops - 1] = payload;
	}
	if (nLoops == 1) {
		infoglb.chunksenum = onlyjson;
	}
	else {
		infoglb.chunksenum = jsonbin;
	}
	free(str);
}


#pragma endregion


//regiao loadfromobj
#pragma region
void Model::loadFromOBJ(const char* file) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs);
	/*
    outras opcoes para o segundo parametro do readfile
	aiProcess_GenNormals: creates normal vectors for each vertex if the model doesn't contain normal vectors.
    aiProcess_SplitLargeMeshes: splits large meshes into smaller sub-meshes which is useful if your rendering has a maximum number of vertices allowed and can only process smaller meshes.
    aiProcess_OptimizeMeshes: does the reverse by trying to join several meshes into one larger mesh, reducing drawing calls for optimization.
	*/

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERRO::ASSIMP::"<< importer.GetErrorString() << std::endl;
		return;
	}

	Model::file = file;
	processNode(scene->mRootNode, scene);
	
}

void Model::processNode(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	using namespace std;
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		/*Vertex
		* 
			{	glm::vec3 position, normal, color;
	glm::vec2 textureUV;
				positions[i],
				normals[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				texUVs[i]
			}*/
		vertex.position = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);
		vertex.normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);
		if (mesh->mTextureCoords[0]) {
			vertex.textureUV = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}
		else 
			vertex.textureUV = glm::vec2(0.0f, 0.0f);
		vertex.color = glm::vec3(1.0f, 1.0f, 1.0f);
		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		vector<Texture> diffuseMap = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, "diffuse");
		textures.insert(textures.end(), diffuseMap.begin(), diffuseMap.end());

		vector<Texture> specularMap = loadMaterialTextures(material,
			aiTextureType_SPECULAR, "specular");
		textures.insert(textures.end(), specularMap.begin(), specularMap.end());

	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	using namespace std;
	vector<Texture> texturas;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		std::string texPath(str.C_Str());
		const char* tipo = typeName.c_str();
		/*Texture textura(str.C_Str(), tipo, 0);
		texturas.push_back(textura);*/
		bool skip = false;
		for (unsigned int j = 0; j < loadedTexName.size(); j++)
		{
			if (loadedTexName[j] == texPath)
			{
				texturas.push_back(loadedTex[j]);
				skip = true;
				break;
			}
		}
		if (!skip) {
			std::string pathcompleto(this->file);
			pathcompleto = pathcompleto.substr(0, pathcompleto.find_last_of('/'));
			pathcompleto.append("/");
			pathcompleto.append(str.C_Str());
			Texture textura(pathcompleto.c_str(), tipo, loadedTex.size());
			loadedTex.push_back(textura);
			loadedTexName.push_back(texPath);
			texturas.push_back(textura);
		}
	}
	return texturas;
}

#pragma endregion



//regiao loadfromgltf
#pragma region
void Model::loadFromGLTF(const char* file) {
	Util a;
	// Make a JSON object
	std::string text = a.lerFicheiro(file);
	JSON = json::parse(text);

	// Get the binary data
	Model::file = file;
	data = getData();

	// Traverse all nodes
	traverseNode(0);
}



void Model::loadMesh(unsigned int indMesh)
{
	// Get all accessor indices
	unsigned int posAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
	unsigned int normalAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
	unsigned int texAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
	unsigned int indAccInd = JSON["meshes"][indMesh]["primitives"][0]["indices"];

	// Use accessor indices to get all vertices components
	std::vector<float> posVec = getFloats(JSON["accessors"][posAccInd]);
	std::vector<glm::vec3> positions = groupFloatsVec3(posVec);
	std::vector<float> normalVec = getFloats(JSON["accessors"][normalAccInd]);
	std::vector<glm::vec3> normals = groupFloatsVec3(normalVec);
	std::vector<float> texVec = getFloats(JSON["accessors"][texAccInd]);
	std::vector<glm::vec2> texUVs = groupFloatsVec2(texVec);

	// Combine all the vertex components and also get the indices and textures
	std::vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
	std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);
	std::vector<Texture> textures = getTextures();

	std::cout << "MODEL::LOADMESH Nvertices: "<<vertices.size() << std::endl;
	std::cout << "MODEL::LOADMESH Nindices: "<<indices.size() << std::endl;
	std::cout << "MODEL::LOADMESH Ntexturas: "<<textures.size() << std::endl;
	// Combine the vertices, indices, and textures into a mesh
	meshes.push_back(Mesh(vertices, indices, textures));
}

void Model::traverseNode(unsigned int nextNode, glm::mat4 matrix)
{
	// Current node
	json node = JSON["nodes"][nextNode];

	// Get translation if it exists
	glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
	if (node.find("translation") != node.end())
	{
		float transValues[3];
		for (unsigned int i = 0; i < node["translation"].size(); i++)
			transValues[i] = (node["translation"][i]);
		translation = glm::make_vec3(transValues);
	}
	// Get quaternion if it exists
	glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	if (node.find("rotation") != node.end())
	{
		float rotValues[4] =
		{
			node["rotation"][3],
			node["rotation"][0],
			node["rotation"][1],
			node["rotation"][2]
		};
		rotation = glm::make_quat(rotValues);
	}
	// Get scale if it exists
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
	if (node.find("scale") != node.end())
	{
		float scaleValues[3];
		for (unsigned int i = 0; i < node["scale"].size(); i++)
			scaleValues[i] = (node["scale"][i]);
		scale = glm::make_vec3(scaleValues);
	}
	// Get matrix if it exists
	glm::mat4 matNode = glm::mat4(1.0f);
	if (node.find("matrix") != node.end())
	{
		float matValues[16];
		for (unsigned int i = 0; i < node["matrix"].size(); i++)
			matValues[i] = (node["matrix"][i]);
		matNode = glm::make_mat4(matValues);
	}

	// Initialize matrices
	glm::mat4 trans = glm::mat4(1.0f);
	glm::mat4 rot = glm::mat4(1.0f);
	glm::mat4 sca = glm::mat4(1.0f);

	// Use translation, rotation, and scale to change the initialized matrices
	trans = glm::translate(trans, translation);
	rot = glm::mat4_cast(rotation);
	sca = glm::scale(sca, scale);

	// Multiply all matrices together
	glm::mat4 matNextNode = matrix * matNode * trans * rot * sca;

	// Check if the node contains a mesh and if it does load it
	if (node.find("mesh") != node.end())
	{
		translationsMeshes.push_back(translation);
		rotationsMeshes.push_back(rotation);
		scalesMeshes.push_back(scale);
		matricesMeshes.push_back(matNextNode);
		loadMesh(node["mesh"]);
	}

	// Check if the node has children, and if it does, apply this function to them with the matNextNode
	if (node.find("children") != node.end())
	{
		for (unsigned int i = 0; i < node["children"].size(); i++)
			traverseNode(node["children"][i], matNextNode);
	}
}

std::vector<unsigned char> Model::getData()
{
	// Create a place to store the raw text, and get the uri of the .bin file
	std::string bytesText;
	std::string uri = JSON["buffers"][0]["uri"];

	// Store raw text data into bytesText
	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
	Util a;
	bytesText = a.lerFicheiro((fileDirectory + uri).c_str());

	// Transform the raw text data into bytes and put them in a vector
	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
	return data;
}

std::vector<float> Model::getFloats(json accessor)
{
	std::vector<float> floatVec;

	// Get properties from the accessor
	unsigned int buffViewInd = accessor.value("bufferView", 1);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	std::string type = accessor["type"];

	// Get properties from the bufferView
	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	// Interpret the type and store it into numPerVert
	unsigned int numPerVert;
	if (type == "SCALAR") numPerVert = 1;
	else if (type == "VEC2") numPerVert = 2;
	else if (type == "VEC3") numPerVert = 3;
	else if (type == "VEC4") numPerVert = 4;
	else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");

	// Go over all the bytes in the data at the correct place using the properties from above
	unsigned int beginningOfData = byteOffset + accByteOffset;
	unsigned int lengthOfData = count * 4 * numPerVert;
	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i)
	{
		unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
		float value;
		std::memcpy(&value, bytes, sizeof(float));
		floatVec.push_back(value);
	}

	return floatVec;
}

std::vector<GLuint> Model::getIndices(json accessor)
{
	std::vector<GLuint> indices;

	// Get properties from the accessor
	unsigned int buffViewInd = accessor.value("bufferView", 0);
	unsigned int count = accessor["count"];
	unsigned int accByteOffset = accessor.value("byteOffset", 0);
	unsigned int componentType = accessor["componentType"];

	// Get properties from the bufferView
	json bufferView = JSON["bufferViews"][buffViewInd];
	unsigned int byteOffset = bufferView["byteOffset"];

	// Get indices with regards to their type: unsigned int, unsigned short, or short
	unsigned int beginningOfData = byteOffset + accByteOffset;
	if (componentType == 5125)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i)
		{
			unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
			unsigned int value;
			std::memcpy(&value, bytes, sizeof(unsigned int));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5123)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
		{
			unsigned char bytes[] = { data[i++], data[i++] };
			unsigned short value;
			std::memcpy(&value, bytes, sizeof(unsigned short));
			indices.push_back((GLuint)value);
		}
	}
	else if (componentType == 5122)
	{
		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
		{
			unsigned char bytes[] = { data[i++], data[i++] };
			short value;
			std::memcpy(&value, bytes, sizeof(short));
			indices.push_back((GLuint)value);
		}
	}

	return indices;
}

std::vector<Texture> Model::getTextures()
{
	std::vector<Texture> textures;

	std::string fileStr = std::string(file);
	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

	// Go over all images
	for (unsigned int i = 0; i < JSON["images"].size(); i++)
	{
		// uri of current texture
		std::string texPath;
		try {
			texPath = JSON["images"][i]["uri"];
		}
		catch (std::exception exception) {
			//uri nao existe
			//deve ser "name"
			
			texPath = JSON["images"][i]["name"];//so que o ficheiro nao tem a sua extensao, logo preciso de encontra-la na diretoria
			std::string extension = JSON["images"][i]["mimeType"];
			extension = extension.substr(extension.find_last_of('/') + 1, extension.size() - 1);
			texPath += '.';
			texPath.append(extension);

			//o seguinte bloco comentado so funciona para c++17
			//namespace fs = std::filesystem;
			//using Path = fs::path; using string = std::string;
			//bool didAppend = false;
			//for (const auto& entry : fs::directory_iterator(fileDirectory)) {
			//	Path path = entry.path();
			//	string file(path.filename().string());
			//	file = file.substr(0, file.find_last_of("."));
			//	if (file.compare(texPath) == 0) {
			//		texPath.append(path.extension().string());
			//		didAppend = true;
			//		break;
			//	}
			//}
			//if (!didAppend) {
			//	std::cout << "ERROR::MODEL::GETTEXTURES::NO_FILE_FOR_TEXTURE_EXISTS" << std::endl;
			//}
		}

		
		// Check if the texture has already been loaded
		bool skip = false;
		for (unsigned int j = 0; j < loadedTexName.size(); j++)
			{
				if (loadedTexName[j] == texPath)
				{
					textures.push_back(loadedTex[j]);
					skip = true;
					break;
				}
			}

		// If the texture has been loaded, skip this
		if (!skip)
		{
			// Load diffuse texture
			//std::cout << (fileDirectory + texPath).c_str() << std::endl;
			if (texPath.find("baseColor") != std::string::npos || texPath.find("BaseColor") != std::string::npos)
			{
				Texture diffuse = Texture((fileDirectory + texPath).c_str(), "diffuse", loadedTex.size());
				textures.push_back(diffuse);
				loadedTex.push_back(diffuse);
				loadedTexName.push_back(texPath);
			}
			// Load specular texture
			else if (texPath.find("metallicRoughness") != std::string::npos || texPath.find("Metallic-Gladiator") != std::string::npos || texPath.find("Metallic_Gladiator") != std::string::npos)
			{
				Texture specular = Texture((fileDirectory + texPath).c_str(), "specular", loadedTex.size());
				textures.push_back(specular);
				loadedTex.push_back(specular);
				loadedTexName.push_back(texPath);
			}
			else {
				Texture diffuse = Texture((fileDirectory + texPath).c_str(), "diffuse", loadedTex.size());
				textures.push_back(diffuse);
				loadedTex.push_back(diffuse);
				loadedTexName.push_back(texPath);
			}
		}
	}
	//std::cout << "NTEXTURAS!   " << textures.size() << std::endl;
	return textures;

}

std::vector<Vertex> Model::assembleVertices
(
	std::vector<glm::vec3> positions,
	std::vector<glm::vec3> normals,
	std::vector<glm::vec2> texUVs
)
{
	std::vector<Vertex> vertices;
	for (int i = 0; i < positions.size(); i++)
	{
		vertices.push_back
		(
			Vertex
			{
				positions[i],
				normals[i],
				glm::vec3(1.0f, 1.0f, 1.0f),
				texUVs[i]
			}
		);
	}
	return vertices;
}

std::vector<glm::vec2> Model::groupFloatsVec2(std::vector<float> floatVec)
{
	std::vector<glm::vec2> vectors;
	for (int i = 0; i < floatVec.size(); i)
	{
		vectors.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
	}
	return vectors;
}
std::vector<glm::vec3> Model::groupFloatsVec3(std::vector<float> floatVec)
{
	std::vector<glm::vec3> vectors;
	for (int i = 0; i < floatVec.size(); i)
	{
		vectors.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return vectors;
}
std::vector<glm::vec4> Model::groupFloatsVec4(std::vector<float> floatVec)
{
	std::vector<glm::vec4> vectors;
	for (int i = 0; i < floatVec.size(); i)
	{
		vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
	}
	return vectors;
}
#pragma endregion