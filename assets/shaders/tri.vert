#version 400 core

layout(location = 0) in vec3 vertex;

out vec3 _worldPosition;
out vec3 _camPosition;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vec4 worldPosition = model * vec4(vertex, 1.0);
    _worldPosition = worldPosition.xyz;
    gl_Position = projection * view * worldPosition;

    _camPosition = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
}