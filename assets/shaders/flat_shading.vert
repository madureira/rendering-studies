#version 410 core

layout(location = 0) in vec3 a_Vertex;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_MVP;

// All outputs are in origin-relative world space
flat out vec3 v_Normal;
out vec3 v_WorldPos;

void main()
{
    gl_Position = u_MVP * vec4(a_Vertex, 1.0);

    // Normal matrix handles non-uniform scaling
    mat3 normalMatrix = mat3(transpose(inverse(u_Model)));
    v_Normal = normalize(normalMatrix * a_Normal);
}
