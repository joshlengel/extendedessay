#version 400 core

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 textureCoords;

out vec2 _textureCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewPosition = view * vec4(vertex, 0.0);
    gl_Position = projection * viewPosition;

    _textureCoords = textureCoords;
}