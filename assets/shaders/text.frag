#version 410 core

in vec2 vTexCoord;

uniform sampler2D uText;
uniform vec3 uTextColor;

out vec4 fColor;

void main()
{
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(uText, vTexCoord).r);
    fColor = vec4(uTextColor, 1.0) * sampled;
}
