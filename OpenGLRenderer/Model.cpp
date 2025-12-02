#include "Model.h"
//#include "util.h"

Model::Model(const char* path) 
{
	loadModel(path);
}

Model::~Model() 
{
	std::cout << "Model Destroyed\n";
}

void Model::loadModel(std::string path) 
{
	Assimp::Importer importer;
	//creating scene object which stores the root node which stores other nodes and those nodes store different meshes
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	//checking for any loading errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
	{
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	m_directory = path.substr(0,path.find_last_of('/'));//storing the path of the model to use later to when loading textures
	processNode(scene->mRootNode, scene);//processing the 
}

void Model::processNode(aiNode* node, const aiScene* scene) 
{
	//process all meshes from the node
	for (unsigned int i = 0; i < node->mNumMeshes; i++) 
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(processMesh(mesh,scene));
	}

	//if node has any children nodes call the function again with the children node as the main node to process the children node's meshes
	for (unsigned int i = 0; i < node->mNumChildren; i++) 
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) 
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//processing each vertex form the mesh
	
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		//vertices
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.position = vector;

		//normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.normal = vector;

		//texture coords;
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.texCoords = vec;
		}
		else
		{
			vertex.texCoords = glm::vec2(0.f, 0.f);
		}

		//tangent coords
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.tangentCoords = vector;

		vertices.push_back(vertex);
	}

	//processing the mesh's indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) 
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) 
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//processing any materials contained in the meshes
	if (mesh->mMaterialIndex >= 0) 
	{
		//getting the material object from the scene using an index from the mesh object
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		//getting the diffuse and specular textures from the material
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, diffuse);
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, specular);
		
		//adding the textures to the texture array which will be passed to the final Mesh
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, textureType typeName)
{
	std::vector<Texture> textures;
	
	//looping through each texture from the given type then loading it and storing it in the textures list
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) 
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++) 
		{
			if (std::strcmp(textures_loaded[j].path.data(),str.C_Str()) == 0) 
			{
				textures_loaded.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if(!skip)
		{
			Texture texture;
			texture.id = loadTextureFromFile(str.C_Str(), m_directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
	}
	return textures;
}

unsigned int Model::loadTextureFromFile(const char* path, const std::string &directory) 
{
	//getting full file location
	std::string filename = std::string(path);
	filename = std::string("Assets") + '/' + filename;
	
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;

	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

	if (data) 
	{
		GLenum format;
		if (nrComponents == 1) { format = GL_RED; }
		else if (nrComponents == 3) { format = GL_RGB; }
		else if (nrComponents == 4) { format = GL_RGBA; }

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//setting texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Model::addTexture(unsigned int id, textureType type, int index = 0) 
{
	//index to specify a specific mesh from the model
	m_meshes[index].addTexture({id,type,""});
}

void Model::draw(Shader &shader) 
{
	for (Mesh& m : m_meshes) 
	{
		m.Draw(shader);
	}
}