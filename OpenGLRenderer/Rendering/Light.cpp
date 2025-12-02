#include "Light.h"

Light::Light(glm::vec3 position, glm::vec4 colorAndType, bool castShadows)
	: m_position(position), m_colorAndType(colorAndType), m_castShadows(castShadows)
{

}

int Light::getLightType() { return glm::clamp((int)m_colorAndType.w, 0, 1); }//returns 4th component of color which equals the type

bool Light::castShadows() { return m_castShadows; }

glm::vec3 Light::getPos() { return m_position; }
glm::vec3 Light::getColor() { return glm::vec3(m_colorAndType.x, m_colorAndType.y, m_colorAndType.z); }

void Light::setShadowCastIndex(int index) { m_shadowListIndex = index; }
int Light::getShadowCastIndex() { return m_shadowListIndex; }