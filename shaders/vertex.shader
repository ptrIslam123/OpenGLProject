#version 330 core

layout(location = 0) in vec3 vertexPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 Color;

void main() {
    gl_Position = proj * view * model * vec4(vertexPos, 1.0);
    Color = vec3(1.0, 0.0, 1.0);
}