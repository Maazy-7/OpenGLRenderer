#pragma once

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <iostream>

class Camera 
{
	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_up;
	glm::vec3 m_position;
	glm::mat4 m_view, m_proj;
	float m_aspect;
	float m_pitch;
	float m_yaw;
	float m_fov = 90.f;
	float m_sens = 0.075f;
	float m_speed = 3.5;
	const glm::vec3 initialLookDirection = {0.f,0.f,-1.f};
	
	GLFWwindow* m_window;

	void calculateFront();
	void calculateProjection();

	void processMouse();
	
public:
	Camera(GLFWwindow* window, glm::vec3 pos, float aspect, float pitch = 0, float yaw = 0, glm::vec3 up = { 0.f,1.f,0.f });

	glm::mat4* getProjection();
	glm::mat4* getView();
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	void update(float dt);

	glm::vec3 getPos() const { return m_position; }
	glm::vec3 getFront() const { return m_front; }
};