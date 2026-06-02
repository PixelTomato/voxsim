#include <engine/shader.hpp>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath)
{
    std::string vertexSource;
    std::string fragmentSource;

    try
    {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        std::stringstream stream;

        file.open(vertexPath);

        stream << file.rdbuf();
        vertexSource = stream.str();
    }
    catch (std::ifstream::failure &error)
    {
        std::cout << "Failed to open a vertex source: " << vertexPath << "\n";
    }

    try
    {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        std::stringstream stream;

        file.open(fragmentPath);

        stream << file.rdbuf();
        fragmentSource = stream.str();
    }
    catch (std::ifstream::failure &error)
    {
        std::cout << "Failed to open a fragment source: " << fragmentPath << "\n";
    }

    unsigned int vertexShader = compileStage(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = compileStage(GL_FRAGMENT_SHADER, fragmentSource);

    linkProgram(vertexShader, fragmentShader);
}

Shader::~Shader()
{
    glDeleteProgram(program);
}

void Shader::bind()
{
    glUseProgram(program);
}

unsigned int Shader::compileStage(GLenum type, const std::string &source)
{
    unsigned int shader = glCreateShader(type);

    const char *shaderSource = source.c_str();

    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        std::cout << "Failed to compile shader:\n"
                  << infoLog
                  << "\n";
    }

    return shader;
}

void Shader::linkProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);

        std::cout << "Failed to link shader program:\n"
                  << infoLog
                  << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::setUniform(std::string name, glm::mat4 value)
{
    glUniformMatrix4fv(glGetUniformLocation(program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}