#pragma once

#include <glad/glad.h>
#include <vector>
#include <iostream>
class QuadRenderer
{
public:
	QuadRenderer();
	void renderQuad();

private:
	unsigned int m_VBO;
	unsigned int m_VAO;

	float m_vertices[20] = { 
	//positions       //texture Coords
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

};

