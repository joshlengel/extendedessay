#version 400 core

in vec3 _worldPosition;
in vec3 _viewPosition;
in vec3 _normal;

out vec4 _color;

uniform vec3 sunPosition;
uniform vec3 color;

void main()
{
    _color = vec4(color * max(dot(normalize(sunPosition - _worldPosition), _normal), 0.1), 1.0);
}