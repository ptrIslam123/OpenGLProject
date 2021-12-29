#version 330 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;

uniform mat4 mode;

out vec3 Color;

void main() {
    gl_Position = mode * vec4(vertexPos, 1.0);
    Color = vertexColor;
}
