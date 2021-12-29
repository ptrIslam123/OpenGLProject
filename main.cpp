#include <glad/glad.h>
#include <GLFW/glfw3.h>
 
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include "glm/gtc/type_ptr.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"

#include <exception> 
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <fstream>
#include <sstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

std::string readShaderCode(const std::string &shaderSourcePath);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::mat4 transformation();
void addVertices(std::vector<float>& vertices, const std::vector<float>& primitive);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
 
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL for Ravesli.com", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }     

    /** Создание шейдерной программы **/
    unsigned int shader = 0;
    unsigned int vertexShader = 0;
    unsigned int fragmentShader = 0;

    const std::string vertexShaderFileName = "../shaders/vertex.shader";
    const std::string fragmentShaderFileName = "../shaders/fragment.shader";

    std::string vertexSourceCode = readShaderCode(vertexShaderFileName);
    std::string fragmentSourceCode = readShaderCode(fragmentShaderFileName);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    int isSuccess = 1;
    std::array<char, 512> infoLog = {};

    const char *const vertexCode = vertexSourceCode.c_str();
    const char *const fragmentCode = fragmentSourceCode.c_str();

    glShaderSource(vertexShader, 1, &vertexCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isSuccess);

    if (!isSuccess) {
        glGetShaderInfoLog(vertexShader, infoLog.size(), NULL, infoLog.data());
        std::cout << "Compile error in a vertext shader: " << infoLog.data() << std::endl;
        return -1;
    }


    glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,  &isSuccess);

    if (!isSuccess) {
        glGetShaderInfoLog(fragmentShader, infoLog.size(), NULL, infoLog.data());
        std::cout << "Compile error in a fragment shader: " << infoLog.data() << std::endl;
        return -1;
    }

    shader = glCreateProgram();
    glAttachShader(shader, vertexShader);
    glAttachShader(shader, fragmentShader);
    glLinkProgram(shader);
    glGetShaderiv(shader,GL_LINK_STATUS,  &isSuccess);

    if (!isSuccess) {
        glGetShaderInfoLog(shader, infoLog.size(), NULL, infoLog.data());
        std::cout << "Link error in a shader program: " << infoLog.data() << std::endl;
        return -1;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    /** Шейдерная программа создана **/


    /** Генерация буфера на GPU и заполнения буфера данными о вершинах **/
    unsigned int vertexBufferObject = 0;
    unsigned int colorBufferObject = 0;
    unsigned int colorArrayObject = 0;
    unsigned int vertexArrayObject = 0;
    

    const size_t vertexSize = 3;
    std::vector<float> vertices;
    std::vector<float> colors; 

    std::vector<float> trinagle1 = {
        500.f, 500.f, -100.0f,
        -500.f, -500.f, -200.0f,
        500.f, -500.f, -1500.0f 
    };

    std::vector<float> color1 = {
        1.0, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };

    std::vector<float> trinagle2 = {
        -250.f, 250.f, -350.0f,
        -250.f, -250.f, -4000.0f,
        300.f, -700.f, 10.0f
    };

    std::vector<float> color2 = {
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };


    addVertices(vertices, trinagle1);
    addVertices(vertices, trinagle2);

    addVertices(colors, color1);
    addVertices(colors, color2);
    


    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

    glGenBuffers(1, &colorBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * colors.size(), colors.data(), GL_STATIC_DRAW);


    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
 
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferObject);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
		
        const std::string uniformMatrixName("mode");
        unsigned int uniformMatrix = glGetUniformLocation(shader, uniformMatrixName.c_str());
        if (uniformMatrix == -1) {
            std::cout << "Can`t found uniform variable: " << uniformMatrixName << std::endl;
            return -1;
        }
        glUniformMatrix4fv(uniformMatrix, 1, GL_FALSE, glm::value_ptr(transformation()));

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 

        glUseProgram(shader);
        glBindVertexArray(vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwTerminate();
    return 0;
}
 
glm::mat4 transformation() {
    glm::mat4 mode(1.0f);
    const float angle = glfwGetTime() * 2;

    mode = glm::scale(mode, glm::vec3(0.001f, 0.001f, 0.0001f));
    mode = glm::translate(mode, glm::vec3(0.5f, 0.5f, 0.5f));
    //  делаем поворот по оси Z
    mode = glm::rotate(mode, angle, glm::vec3(1.0f, 1.0f, 1.0f));

    return mode;
}

void addVertices(std::vector<float>& vertices, const std::vector<float>& primitive) {
    for (size_t i = 0; i < primitive.size(); ++i) {
        vertices.push_back(primitive[i]);
    }
}

std::string readShaderCode(const std::string &shaderSourcePath) {
    std::ifstream file(shaderSourcePath);

    if (!file.is_open()) {
        throw std::runtime_error(shaderSourcePath + ": file not fount");
    }
    std::stringstream streamData;
    streamData << file.rdbuf();
    return streamData.str();
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
 
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void printVector(const std::vector<float>& vertices) {
    for (size_t i = 0; i < vertices.size(); ++i) {
        std::cout << vertices[i];
    }
    std::cout << std::endl;
}