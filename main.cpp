#include <glad/glad.h>
#include <GLFW/glfw3.h>
 

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <exception> 
#include <iostream>
#include <string>
#include <array>
#include <fstream>
#include <sstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

std::string readShaderCode(const std::string &shaderSourcePath);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
 
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


    /** Загрузка и создание текстуры **/
    unsigned int textureObject;
    glGenTextures(1, &textureObject);
    glBindTexture(GL_TEXTURE_2D, textureObject); // все последующие GL_TEXTURE_2D-операции теперь будут влиять на данный текстурный объект
	
    // Установка параметров наложения текстуры. ![Без этих параметров текстура не будет правильно рисоваться]!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // установка метода наложения текстуры GL_REPEAT (стандартный метод наложения)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
    // Установка параметров фильтрации текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
    // Загрузка изображения, создание текстуры и генерирование мипмап-уровней
    int width, height, nrChannels;
    unsigned char* data = stbi_load("../resources/tex2D.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    /** END TEXTURE LOAD **/


    /** Генерация буфера на GPU и заполнения буфера данными о вершинах **/
    unsigned int vertexBufferObject = 0;
    unsigned int elementBufferObject = 0;
    unsigned int colorBufferObject = 0;
    unsigned int colorArrayObject = 0;
    unsigned int vertexArrayObject = 0;
    unsigned int vertexSize = 3;
    unsigned int colorSize = 3;
    unsigned int indexSize = 3;
    unsigned int vertexCounter = 6;
    unsigned int textureSize = 2;

    
    float vertices[] = {
        // unique vertex coordinate |   vertex color        |   texture coordinate
        0.5f, 0.5f, 0.0f,               1.0f, 0.0f, 0.0f,       1.0f, 1.0f,
        0.5f, -0.5f, 0.0f,              0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,             0.0f, 0.0f, 1.0f,       0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,              0.0f, 0.0f, 0.0f,       0.0f, 1.0f
    };

    unsigned int indices[] = {
        2, 3, 0,
        0, 1, 2
    };

    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);
    glGenVertexArrays(1, &vertexArrayObject);

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
    

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, vertexSize, GL_FLOAT, GL_FALSE, sizeof(float) * (vertexSize + colorSize + textureSize), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, colorSize, GL_FLOAT, GL_FALSE, sizeof(float) * (vertexSize + colorSize + textureSize), (void*)(sizeof(float) * vertexSize));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, textureSize, GL_FLOAT, GL_FALSE, sizeof(float) * (vertexSize + colorSize + textureSize), (void*)(sizeof(float) * (vertexSize + colorSize)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
		
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
 

        glUseProgram(shader);
        glBindVertexArray(vertexArrayObject);
        glBindTexture(GL_TEXTURE2, textureObject);
        glDrawElements(GL_TRIANGLES, vertexCounter, GL_UNSIGNED_INT, 0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
 
    glfwTerminate();
    return 0;
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