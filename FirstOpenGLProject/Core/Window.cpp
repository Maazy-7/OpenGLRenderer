#include "Core/Window.h"

Window::Window(int width, int height, std::string name) 
{
	glfwInit();
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);//will add support for later versions 
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	m_window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

	if (m_window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(m_window);
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	//if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		}
	);
}

glm::vec2 Window::getDimensions() 
{
	return glm::vec2((float)m_width, (float)m_height);
}

float Window::getAspect() 
{
	return (float)(m_width/m_height);
}

GLFWwindow* Window::getWindow() 
{
	return m_window;
}

void Window::swapBuffers() 
{
	glfwSwapBuffers(m_window);
}

void Window::pollEvents() 
{
	glfwPollEvents();
}

void Window::update() 
{

}

float Window::getTime() 
{
	return (float)glfwGetTime();
}

void Window::setWindowScrollCallBack(Camera* camera) 
{
	glfwSetWindowUserPointer(m_window, camera);
	glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
		Camera* camera = (Camera*)glfwGetWindowUserPointer(window);
		camera->scroll_callback(window, xoffset, yoffset);
		});
}

bool Window::windowClose() { return glfwWindowShouldClose(m_window); }