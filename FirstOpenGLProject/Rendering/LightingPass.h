#pragma once

#include <vector>
#include <memory>

#include "Shader.h"
#include "Physics/Components/GameObject.h"

class LightingPass
{
public:
	LightingPass(std::shared_ptr<Shader> shader);
	void execute(const std::vector<std::unique_ptr<GameObject>>&);

private:
	std::shared_ptr<Shader> m_lightingShader;
};

