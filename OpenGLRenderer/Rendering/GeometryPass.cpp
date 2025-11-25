#include "GeometryPass.h"

GeometryPass::GeometryPass(std::shared_ptr<Shader> shader)
	: m_geometryShader(shader)
{

}

void GeometryPass::execute(const std::vector<std::unique_ptr<GameObject>>& gameObjects)
{
	//gBuffer currently binded
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (size_t i = 0; i < gameObjects.size(); i++) 
	{
		gameObjects[i]->render(*m_geometryShader);
	}
	//gBuffer gets unbinded after method call
}