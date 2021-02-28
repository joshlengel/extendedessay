#version 400 core

in vec3 worldPosition;
in vec3 camPosition;
in vec3 normal;

out vec4 color;

const vec3 LIGHT_DIR = normalize(vec3(-0.2, -0.7, -0.3));
const vec3 LIGHT_COLOR = vec3(1.0);

void main()
{
    color = vec4(LIGHT_COLOR * max(dot(-LIGHT_DIR, normal), 0.1), 1.0);
}