#version 400 core

in vec2 _textureCoords;

out vec4 _color;

uniform vec3 color;

void main()
{
    _color = vec4(color, 1.0 - clamp(length(_textureCoords * 2.0 - 1.0), 0.0, 1.0));
}