#pragma once

#include <vector>
#include <memory>

#include "Rendering/Light.h"
#include "Rendering/ShadowCaster.h"
#include "Shader.h"
#include "Physics/Components/GameObject.h"

class ShadowPass
{
public:
	ShadowPass(std::shared_ptr<Shader> depthShader, std::shared_ptr<Shader> pointLightDepthShader);
	void execute(const std::vector<std::unique_ptr<GameObject>>& gameObjects, const std::vector<Light*>& lights, const std::vector<std::unique_ptr<ShadowCaster>>& shadowCasters);
	
private:
	std::shared_ptr<Shader> m_depthShader;
	std::shared_ptr<Shader> m_pointLightDepthShader;

	int SHADOW_WIDTH = 1024;
	int SHADOW_HEIGHT = 1024;
};

