#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader
{
public:
    unsigned int ID;

    Shader() { }

    Shader& Use();

    void Compile(const char* vertexSource, const char* fragmentSource);

    void SetInteger(const char* name, int value, bool useShader = true);
    void SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader = true);
    void SetVector3f(const char* name, const glm::vec3& value, bool useShader = true);


private:
    void checkCompileErrors(unsigned int object, std::string type);
};
