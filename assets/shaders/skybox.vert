#version 410 core

layout(location = 0) in vec3 a_Vertex;

uniform mat4 u_VP;

out vec3 v_TexCoord;

void main()
{
    v_TexCoord = a_Vertex;
    vec4 pos = u_VP * vec4(a_Vertex, 1.0);
    gl_Position = pos.xyww;
}
