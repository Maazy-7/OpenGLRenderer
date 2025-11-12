#pragma once

#include <vector>
#include <memory>

#include "Shader.h"
#include "Physics/Components/GameObject.h"

class LightingPass
{
public:
	LightingPass(Shader* shader);
	void execute(const std::vector<std::unique_ptr<GameObject>>&);

private:
	Shader* m_lightingShader;
};

