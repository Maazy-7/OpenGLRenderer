#include "GeometryPass.h"

GeometryPass::GeometryPass(Shader* shader)
	: m_geometryShader(shader)
{

}

void GeometryPass::execute(const std::vector<std::unique_ptr<GameObject>>&)
{

}