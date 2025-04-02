#version 410 core

#ifdef GL_FRAGMENT_PRECISION_HIGH
  precision highp float;
#else
  precision mediump float;
#endif

layout (location = 0) in vec3 aPosition;

uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vNear;
out vec3 vFar;

vec3 unproject_point(float x, float y, float z)
{
    mat4 viewInv = inverse(uView);
    mat4 projInv = inverse(uProjection);
    vec4 unproj_point = viewInv * projInv * vec4(x, y, z, 1.f);
    return unproj_point.xyz / unproj_point.w;
}

void main()
{
    vNear = unproject_point(aPosition.x, aPosition.y, 0.f);
    vFar  = unproject_point(aPosition.x, aPosition.y,  1.f);
    gl_Position = vec4(aPosition, 1.0f);
}

/*
vec3 unproject_point(float x, float y, float z)
{
    mat4 inv = inverse(uProjection * uView);
    vec4 unproj_point = inv * vec4(x, y, z, 1.f);
    return unproj_point.xyz / unproj_point.w;
}

void main()
{
    vec2 p = aPosition;
    vNear = unproject_point(p.x, p.y, -1.f);
    vFar  = unproject_point(p.x, p.y,  1.f);
    gl_Position = vec4(aPosition, 0.0f, 1.0f);
}
*/
