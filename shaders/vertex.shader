#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec4 vertexColor;

out vec4 color; // необходимо что бы название выходных переменных совпадали с входными след. шейдера!   

void main() {
    gl_Position = vec4(vertexPos.x, vertexPos.y, vertexPos.z, 1.0);
    color = vertexColor;
}
