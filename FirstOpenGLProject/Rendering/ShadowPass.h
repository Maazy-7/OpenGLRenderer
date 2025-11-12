#pragma once

#include <vector>
#include <memory>

#include "Rendering/Light.h"
#include "Shader.h"
#include "Physics/Components/GameObject.h"

class ShadowPass
{
public:
	ShadowPass(Shader* depthShader, Shader* pointLightDepthShader);
	void execute(const std::vector<std::unique_ptr<GameObject>>&, const std::vector<std::unique_ptr<Light>>&);
	
private:
	Shader* m_depthShader;
	Shader* m_pointLightDepthShader;

	int SHADOW_WIDTH, SHADOW_HEIGHT;
};

