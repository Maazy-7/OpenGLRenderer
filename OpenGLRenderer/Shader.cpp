#include "Shader.h";

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	createShader(GL_VERTEX_SHADER, vertexPath, vertex);
	createShader(GL_FRAGMENT_SHADER, fragmentPath, fragment);

	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex);
	glAttachShader(m_ID, fragment);
	glLinkProgram(m_ID);

	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath) 
{
	unsigned int vertex, geometry, fragment;
	int success;
	char infoLog[512];

	createShader(GL_VERTEX_SHADER, vertexPath, vertex);
	createShader(GL_GEOMETRY_SHADER, geometryPath, geometry);
	createShader(GL_FRAGMENT_SHADER, fragmentPath, fragment);
	
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex);
	glAttachShader(m_ID, geometry);
	glAttachShader(m_ID, fragment);
	glLinkProgram(m_ID);

	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(geometry);
	glDeleteShader(fragment);
}

void Shader::createShader(GLenum shaderType, const char* shaderPath, unsigned int& shader)
{
	//getting shader code from file

	//code to put into shader program
	std::string shaderCode;

	//shader file
	std::ifstream shaderFile;

	//handling exceptions for when we read shader files
	shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		//opening files
		shaderFile.open(shaderPath);

		//stream stores the contents of the file (the shader code)
		std::stringstream shaderStream;

		//adding content of file into the string stream
		shaderStream << shaderFile.rdbuf();

		//closing file
		shaderFile.close();

		//converting the shader code to a string
		shaderCode = shaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n";
	}

	//making shader code into a char pointer
	const char* shaderCodeChar = shaderCode.c_str();

	//creating shader object and compiling its code
	int success;
	char infoLog[512];

	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCodeChar, NULL);
	glCompileShader(shader);

	//check for any compile errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		std::string shaderName;
		if (shaderType == GL_FRAGMENT_SHADER) { shaderName = "FRAGMENT"; }
		else if (shaderType == GL_VERTEX_SHADER) { shaderName = "VERTEX"; }
		else if (shaderType == GL_GEOMETRY_SHADER) { shaderName = "GEOMETRY"; }
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::" << shaderName << "::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

void Shader::use() 
{
	glUseProgram(m_ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
}

void Shader::setMatrix4f(const std::string& name, const glm::mat4& value) const 
{
	glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}

void Shader::setMatrix4f(const std::string& name, int amount, const glm::mat4* value) const 
{
	glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), amount, GL_FALSE, glm::value_ptr(value[0]));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const 
{
	glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, int amount, const std::vector<glm::vec3> value) const
{
	glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), amount, glm::value_ptr(value[0]));
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
	glUniform2fv(glGetUniformLocation(m_ID, name.c_str()), 1, &value[0]);
}