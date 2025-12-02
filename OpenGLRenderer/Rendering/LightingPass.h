#pragma once

#include <vector>
#include <memory>
#include <string>

#include "Shader.h"
#include "Rendering/QuadRenderer.h"
#include "Rendering/Light.h"
#include "Physics/Components/GameObject.h"
#include "Camera.h"

class LightingPass
{
public:
	LightingPass(std::shared_ptr<Shader> shader);
	void execute(const std::vector<std::unique_ptr<Light>>& lights, Camera* camera);
	void useLightingShader();

private:
	std::shared_ptr<Shader> m_lightingShader;
	std::unique_ptr<QuadRenderer> m_LightPassQuad;
};

