#version 410 core

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform int u_UseTint;
uniform vec4 u_Tint;

out vec4 frag_color;

void main()
{
    vec4 base = texture(u_Texture, v_TexCoord);
    frag_color = (u_UseTint != 0) ? (base * u_Tint) : base;
}
