#pragma once

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb/stb_image.h>

#include "Mesh.h"

class Model 
{
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> m_meshes;
	std::string m_directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, textureType typeName);
	unsigned int loadTextureFromFile(const char* path, const std::string &directory);

public:

	Model(const char* path);
	~Model();
	void addTexture(unsigned int id, textureType type, int index);
	void draw(Shader &shader);
};