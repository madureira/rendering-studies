layout(location = 0) in vec3 a_Vertex;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_VP;

// All outputs are in origin-relative world space
out vec3 v_Normal;
out vec3 v_WorldPos;

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(u_Model)));

    vec4 worldPos = u_Model * vec4(a_Vertex, 1.0);

    v_Normal   = normalize(normalMatrix * a_Normal);
    v_WorldPos = worldPos.xyz;

    gl_Position = u_VP * worldPos;
}
