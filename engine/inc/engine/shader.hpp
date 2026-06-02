#pragma once

#include <glad/gl.h>

#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

class Shader
{
private:
    unsigned int program;

    unsigned int compileStage(GLenum type, const std::string &source);

    void linkProgram(unsigned int vertexShader, unsigned int fragmentShader);

public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);

    ~Shader();

    void bind();

    void setUniform(std::string name, glm::mat4 value);
};