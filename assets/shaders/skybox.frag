#version 410 core

in vec3 v_TexDir;

uniform samplerCube u_Skybox;

out vec4 frag_color;

void main()
{
    frag_color = texture(u_Skybox, v_TexDir);
}
