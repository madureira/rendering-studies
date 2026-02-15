#version 410 core

layout(location = 0) in vec3 a_Vertex;

void main()
{
    gl_Position = vec4(a_Vertex, 1.0);
}
