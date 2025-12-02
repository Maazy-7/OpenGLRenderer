#include "mesh.h"
#include <iostream>
#include <stb/stb_image.h>


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) : m_vertices(vertices), m_indices(indices), m_textures(textures)
{
	innitMesh();
}

Mesh::~Mesh() 
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

Mesh::Mesh(Mesh&& other) noexcept
	: m_VAO(other.m_VAO),
	m_VBO(other.m_VBO),
	m_EBO(other.m_EBO),
	m_vertices(std::move(other.m_vertices)),
	m_indices(std::move(other.m_indices)),
	m_textures(std::move(other.m_textures))
{
	other.m_VAO = 0;
	other.m_VBO = 0;
	other.m_EBO = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
	if (this != &other)
	{
		if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
		if (m_VBO) glDeleteBuffers(1, &m_VBO);
		if (m_EBO) glDeleteBuffers(1, &m_EBO);

		m_VAO = other.m_VAO;
		m_VBO = other.m_VBO;
		m_EBO = other.m_EBO;

		m_vertices = std::move(other.m_vertices);
		m_indices = std::move(other.m_indices);
		m_textures = std::move(other.m_textures);

		other.m_VAO = 0;
		other.m_VBO = 0;
		other.m_EBO = 0;
	}
	return *this;
}

void Mesh::innitMesh() 
{
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_vertices.size(), m_vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indices.size(), m_indices.data(), GL_STATIC_DRAW);

	//vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	//normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	//texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(2);
	//tangent coords
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangentCoords));
	glEnableVertexAttribArray(3);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

}

unsigned int Mesh::loadTexture(const char* path)
{
	unsigned int texture;
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	return texture;
}

void Mesh::addTexture(Texture tex) 
{
	m_textures.push_back(tex);
}

void Mesh::Draw(Shader& shader) 
{

	for (int i = 0; i < m_textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0+i);

		int name = m_textures[i].type;

		switch (name) 
		{
			case diffuse:
				shader.setInt("diffuse", i);
				glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
				break;
			case specular:
				shader.setInt("specular", i);
				glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
				break;
			case depthMap:
				shader.setInt("depthMap", i);
				glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
				break;
			case cubeDepthMap:
				shader.setInt("cubeDepthMap", i);
				glBindTexture(GL_TEXTURE_CUBE_MAP, m_textures[i].id);
				break;
			case normalMap:
				shader.setInt("normalMap", i);
				glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
				break;
			case heightMap:
				shader.setInt("heightMap", i);
				glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
		}
	}

	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}