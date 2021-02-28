#version 400 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;

out vec3 _worldPosition;
out vec3 _viewPosition;
out vec3 _normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec4 worldPosition = model * vec4(vertex, 1.0);
    vec4 viewPosition = view * worldPosition;

    _worldPosition = worldPosition.xyz;
    _viewPosition = viewPosition.xyz;
    _normal = normal;

    gl_Position = projection * viewPosition;
}