#pragma once

#include "glm/glm.hpp"

class Light
{
public:
	Light(glm::vec3 position, glm::vec4 colorAndType, bool castShadows);
	int getLightType();
	
private:
	glm::vec3 m_position;
	glm::vec4 m_colorAndType;//1 for directional light, 0 for spot light
	bool m_castShadows;
};

