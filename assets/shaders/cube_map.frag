#version 410 core

in vec3 v_TexDir;

uniform samplerCube u_CubeMap;

out vec4 frag_color;

void main()
{
    frag_color = texture(u_CubeMap, v_TexDir);
}
