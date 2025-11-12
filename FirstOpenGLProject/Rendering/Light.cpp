#include "Light.h"

Light::Light(glm::vec3 position, glm::vec4 colorAndType, bool castShadows)
	: m_position(position), m_colorAndType(colorAndType), m_castShadows(castShadows)
{

}

int Light::getLightType() { return (int)m_colorAndType.w; }//returns 4th component of color which equals the type