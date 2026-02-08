#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out VS_OUT {
    vec3 WorldPos;
    vec3 Normal;
    vec3 Tangent;
    vec3 Bitangent;
} vs_out;

void main()
{
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    vs_out.WorldPos = worldPos.xyz;

    mat3 normalMat = transpose(inverse(mat3(u_Model)));
    vec3 N = normalize(normalMat * a_Normal);

    // Construct a consistent tangent frame for the "brush direction".
    // We pick a reference axis and project it onto the surface to get T.
    // This gives circular grooves around the Y-axis (like a lathe-turned sphere).
    vec3 ref = (abs(N.y) < 0.999) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 T = normalize(cross(ref, N));
    vec3 B = cross(N, T);

    vs_out.Normal    = N;
    vs_out.Tangent   = T;
    vs_out.Bitangent = B;

    gl_Position = u_Projection * u_View * worldPos;
}
