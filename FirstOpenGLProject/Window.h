#include <string>
#include <iostream>

#include <glfw/glfw3.h>
#include <glm/glm.hpp>
//#include <glad/glad.h>


class Window 
{
public:
	Window(int width, int height, std::string name = "App");

	void update();
	glm::vec2 getDimensions();
	GLFWwindow* getWindow();


private:
	int m_width, m_height;
	GLFWwindow* window = nullptr;
	

};