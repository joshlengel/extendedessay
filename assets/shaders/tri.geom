#version 400 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 _worldPosition[];
in vec3 _camPosition[];

out vec3 worldPosition;
out vec3 camPosition;
out vec3 normal;

void main()
{
    vec3 norm = normalize(cross(_worldPosition[1] - _worldPosition[0], _worldPosition[2] - _worldPosition[0]));

    gl_Position = gl_in[0].gl_Position;
    worldPosition = _worldPosition[0];
    camPosition = _camPosition[0];
    normal = norm;
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    worldPosition = _worldPosition[1];
    camPosition = _camPosition[1];
    normal = norm;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    worldPosition = _worldPosition[2];
    camPosition = _camPosition[2];
    normal = norm;
    EmitVertex();

    EndPrimitive();
}