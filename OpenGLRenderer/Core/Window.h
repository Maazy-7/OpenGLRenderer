#pragma once

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Camera.h"


class Window 
{
public:
	Window(int width, int height, std::string name = "App");
	~Window();

	void update();
	glm::vec2 getDimensions() const;
	GLFWwindow* getWindow() const;
	void swapBuffers();
	void pollEvents();
	void setWindowScrollCallBack(Camera* camera);
	void setKeyCallback(void (*keyCallback)(GLFWwindow*,int,int,int, int));
	float getAspect() const;
	float getTime() const;
	bool windowClose();
	void setWindowShouldClose(int close);

	
private:
	int m_width, m_height;
	GLFWwindow* m_window = nullptr;
};