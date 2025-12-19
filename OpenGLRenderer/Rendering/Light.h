#pragma once

#include "glm/glm.hpp"

class Light
{
public:
	Light(glm::vec3 position, glm::vec4 colorAndType, bool castShadows);
	int getLightType();
	bool castShadows();
	glm::vec3 getPos();
	glm::vec3 getColor();
	void setShadowCastIndex(int index);
	int getShadowCastIndex();
	
private:
	glm::vec3 m_position;
	glm::vec4 m_colorAndType;//1 for directional light, 0 for point light
	bool m_castShadows;
	int m_shadowListIndex;//used to index shadowCaster list
};

