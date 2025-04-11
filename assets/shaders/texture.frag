#version 410 core

in vec3 v_Color;
in vec2 v_TexCoord;

uniform sampler2D u_Texture1;

out vec4 frag_color;

void main()
{
    frag_color = texture(u_Texture1, v_TexCoord);
}
