#version 410 core

in vec3 v_TexCoord;

uniform samplerCube u_SkyboxTexture;
uniform float u_Exposure;

out vec4 frag_color;

void main()
{
    vec3 color = texture(u_SkyboxTexture, v_TexCoord).rgb;

    // Exposure
    color *= u_Exposure;

    // Tone mapping
    //color = color / (color + vec3(1.0));

    // Gamma correction
    //color = pow(color, vec3(1.0 / 2.2));

    frag_color = vec4(color, 1.0);
}
