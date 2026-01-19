#version 410 core

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

layout(location = 0) in vec3 a_Position;

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_ViewInv;
uniform mat4 u_ProjectionInv;
uniform vec3 u_CameraPos;

out vec3 v_Near;
out vec3 v_Far;
out vec3 v_CameraPos;

vec3 unprojectPoint(float x, float y, float z)
{
    vec4 unprojPoint = u_ViewInv * u_ProjectionInv * vec4(x, y, z, 1.f);
    return unprojPoint.xyz / unprojPoint.w;
}

void main()
{
    v_Near = unprojectPoint(a_Position.x, a_Position.y, 0.f);
    v_Far = unprojectPoint(a_Position.x, a_Position.y, 1.f);
    v_CameraPos = u_CameraPos;
    gl_Position = vec4(a_Position, 1.0f);
}
