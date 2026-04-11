in vec2 v_TexCoord;

uniform sampler2D u_Text;
uniform vec3 u_TextColor;

out vec4 frag_color;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Text, v_TexCoord).r);
    if (sampled.a < 0.1)
    {
        discard;
    }
    frag_color = vec4(u_TextColor, 1.0) * sampled;
}
