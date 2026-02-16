#version 410 core

layout(location = 0) in vec3 a_Vertex;

uniform mat4 u_Model;
uniform mat4 u_VP;

out vec3 v_WorldPos;

void main()
{
    vec4 w = u_Model * vec4(a_Vertex, 1.0);
    v_WorldPos = w.xyz;

    gl_Position = u_VP * w;
}
