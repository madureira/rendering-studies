#version 410 core

layout(location = 0) in vec3 a_Vertex;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_MVP;

// All outputs are in origin-relative world space
out vec3 v_WorldPos;
out vec3 v_Normal;
out vec3 v_Tangent;
out vec3 v_Bitangent;

void main()
{
    vec4 worldPos = u_Model * vec4(a_Vertex, 1.0);
    v_WorldPos = worldPos.xyz;

    // Normal matrix handles non-uniform scaling
    mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
    vec3 N = normalize(normalMatrix * a_Normal);

    // Construct a consistent tangent frame for the "brush direction".
    // Cross with a reference axis to get T tangent to the surface.
    // This gives circular grooves around the Y-axis (like a lathe-turned sphere).
    vec3 ref = (abs(N.y) < 0.999) ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
    vec3 T = normalize(cross(ref, N));
    vec3 B = cross(N, T);

    v_Normal    = N;
    v_Tangent   = T;
    v_Bitangent = B;

    gl_Position = u_MVP * vec4(a_Vertex, 1.0);
}
