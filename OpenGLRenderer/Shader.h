#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader 
{
private:
    void createShader(GLenum shaderType, const char* shaderPath, unsigned int& shader);

public:
    unsigned int m_ID;
	Shader(const char* vertexPath, const char* fragmentPath);
    Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
    ~Shader();

    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMatrix4f(const std::string& name, const glm::mat4& value) const;
    void setMatrix4f(const std::string& name, int amount,const glm::mat4* value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, int amount, const std::vector<glm::vec3> value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
};