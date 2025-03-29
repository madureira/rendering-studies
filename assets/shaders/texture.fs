#version 410 core

in vec3 vColor;
in vec2 vTexCoord;

uniform sampler2D uTexture1;

out vec4 fColor;

void main()
{
    fColor = texture(uTexture1, vTexCoord);
}
