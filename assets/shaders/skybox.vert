#version 410 core

layout(location = 0) in vec3 a_Vertex;

uniform mat4 u_VP;

out vec3 v_TexDir;

void main()
{
    v_TexDir = a_Vertex;
    vec4 pos = u_VP * vec4(a_Vertex, 1.0);
    gl_Position = pos.xyww;
}
