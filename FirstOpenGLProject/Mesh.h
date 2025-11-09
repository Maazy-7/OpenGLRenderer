#pragma once

#include <glad/glad.h>
#include <vector>
#include "Shader.h"


enum textureType
{
	diffuse,
	specular,
	depthMap,
	cubeDepthMap,
	normalMap,
	heightMap
};

struct Vertex 
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::vec3 tangentCoords;
};

struct Texture 
{
	unsigned int id;
	textureType type;
	std::string path;
};


class Mesh 
{
	unsigned int m_VAO, m_VBO, m_EBO;
	//std::vector<float> m_vertices;
	int m_faces;
	std::vector<Vertex> m_vertices_;
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;
	
	
	//void innitBuffers();
	void innitMesh();

	unsigned int loadTexture(const char*);

public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	//Mesh(std::vector<float> vertices, int faces = 6);
	void addTexture(Texture tex);
	void draw(Shader& shader);
	void Draw(Shader& shader);

};