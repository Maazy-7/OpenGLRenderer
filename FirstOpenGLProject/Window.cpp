#include <Window.h>

Window::Window(int width, int height, std::string name) 
{
	glfwInit();
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);//will add support for later versions 
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		}
	);
}

glm::vec2 Window::getDimensions() 
{
	return glm::vec2((float)m_width, (float)m_height);
}

GLFWwindow* Window::getWindow() 
{
	return window;
}