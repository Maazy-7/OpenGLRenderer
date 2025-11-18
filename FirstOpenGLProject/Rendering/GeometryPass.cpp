#include "GeometryPass.h"

GeometryPass::GeometryPass(std::shared_ptr<Shader> shader)
	: m_geometryShader(shader)
{

}

void GeometryPass::execute(const std::vector<std::unique_ptr<GameObject>>&)
{

}