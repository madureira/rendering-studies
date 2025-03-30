#version 410 core

layout(location = 0) in vec4 vPosition;

uniform mat4 uProjection;

out vec2 vTexCoord;

void main()
{
    gl_Position = uProjection * vec4(vPosition.xy, 0.0, 1.0);
    vTexCoord = vPosition.zw;
}
