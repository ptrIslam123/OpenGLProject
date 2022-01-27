#include <glad/glad.h>
#include <GLFW/glfw3.h>
 
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"

#include <cmath>

#include <iostream>
#include <exception> 
#include <iostream>
#include <string>
#include <array>
#include <fstream>
#include <sstream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_pos_callback(GLFWwindow* window, double xPos, double yPos);

std::string readShaderCode(const std::string &shaderSourcePath);

void scaleVertices(float* vertices, unsigned int verticesSize, float scaleVal);
void translateVertices(float* vertices, unsigned int verticesSize, float translateVal);
void rotateVertices(float* vertices, unsigned int verticesSize, float rotateVal);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
double curMouseXPos = 0.;
double curMouseYPos = 0.;
 

// Получения базисных векторов системы координат камеры
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f,  3.0f);
glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f;	// время между текущим кадром и последним кадром
float lastFrame = 0.0f;

/**
 * Углы Эйлера.
 * Тангаж (pitch) - вращение объекта вокруг оси Oz
 * Рыскание (yaw) - вращение объекта вокруг оси Ox
 */

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
    // Установить функция-колбэк для получения данных о позиции курсора мыши(тачпада)
    glfwSetCursorPosCallback(window, mouse_pos_callback);

    // Сообщаем GLFW, чтобы он захватил наш курсор (Делает просто не видимым для пользователя!)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
 
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
    unsigned int modelLocation = 0;
    unsigned int viewLocation = 0;
    unsigned int projLocation = 0;
    unsigned int vertexBufferObject = 0;
    unsigned int elementBufferObject = 0;
    unsigned int colorBufferObject = 0;
    unsigned int colorArrayObject = 0;
    unsigned int vertexArrayObject = 0;
    unsigned int vertexSize = 3;
    unsigned int colorSize = 3;
    unsigned int indexSize = 3;
    unsigned int vertexCounter = 36;


    // вершины куба (6 граней * 2 треугольника * 3 вершины каждый)
    float vertices[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
 
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
 
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
 
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
 
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
 
    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f
    };
    

    glGenBuffers(1, &vertexBufferObject);
    glGenBuffers(1, &elementBufferObject);
    glGenVertexArrays(1, &vertexArrayObject);

    glBindVertexArray(vertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, vertexSize, GL_FLOAT, GL_FALSE, sizeof(float) * (vertexSize), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        // Логическая часть работы со временем для каждого кадра
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
 
        glUseProgram(shader);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);

        model = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f)) * 
                glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(1.0f, 0.5f, 0.0f)) * 
                glm::scale(model, glm::vec3(0.75f, 0.75f, 0.75f));

        view = glm::lookAt(
            cameraPosition,                             // вектор позиции камеры 
            cameraPosition + cameraDirection,           // вектор цели камеры
            cameraUp                                    // вектор вверх камеры
        );

        proj = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        
        modelLocation = glGetUniformLocation(shader, "model");
        viewLocation = glGetUniformLocation(shader, "view");
        projLocation = glGetUniformLocation(shader, "proj");

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, vertexCounter);

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

void mouse_pos_callback(GLFWwindow* window, const double xPos, const double yPos) {
    std::cout << "xPos := " << curMouseXPos << "\tyPos := " << curMouseYPos << std::endl;
    curMouseXPos = xPos;
    curMouseYPos = yPos;
    std::cout << "xPos` := " << curMouseXPos << "\tyPos` := " << curMouseYPos << std::endl;
    std::cout << std::endl;
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