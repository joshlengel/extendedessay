#version 400 core

in vec3 _vertex;
in vec3 _center;
in vec3 _camToCenter;

out vec4 _color;

uniform vec3 color;
uniform float radius;
uniform float glow_width;

void main()
{
    _color = vec4(color, 1.0);
}