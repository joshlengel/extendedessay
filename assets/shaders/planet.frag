#version 400 core

in vec3 _worldPosition;
in vec3 _viewPosition;
in vec3 _normal;

out vec4 color;

uniform vec3 sunPosition;

void main()
{
    color = vec4(vec3(1.0) * max(dot(normalize(sunPosition - _worldPosition), _normal), 0.1), 1.0);
    color = vec4(1.0, 0.0, 0.0, 1.0);
}