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

// Pass homogeneous coordinates - divide by w in fragment shader
out vec4 v_NearH;
out vec4 v_FarH;
out vec3 v_CameraPos;

vec4 unprojectPointH(float x, float y, float z)
{
    // OpenGL NDC: z = -1 at near, z = +1 at far
    return u_ViewInv * u_ProjectionInv * vec4(x, y, z, 1.0);
}

void main()
{
    // Use correct OpenGL NDC z range: -1 (near) to +1 (far)
    v_NearH = unprojectPointH(a_Position.x, a_Position.y, -1.0);
    v_FarH = unprojectPointH(a_Position.x, a_Position.y, 1.0);

    v_CameraPos = u_CameraPos;
    gl_Position = vec4(a_Position, 1.0);
}
