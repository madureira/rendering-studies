#version 410 core

layout(location = 0) in vec4 a_Position;

uniform mat4 u_Projection;

out vec2 v_TexCoord;

void main()
{
    gl_Position = u_Projection * vec4(a_Position.xy, 0.0, 1.0);
    v_TexCoord = a_Position.zw;
}
