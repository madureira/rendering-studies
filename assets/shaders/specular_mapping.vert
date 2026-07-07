layout(location = 0) in vec3 a_Vertex;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in vec3 a_Tangent;

uniform mat4 u_MVP;
uniform mat4 u_ModelView;
uniform vec3 u_LightPos;

out vec2 v_TexCoord;
out vec3 v_TangentLightPos;
out vec3 v_TangentFragPos;

void main()
{
    vec3 fragPosView = vec3(u_ModelView * vec4(a_Vertex, 1.0));
    vec3 lightPosView = u_LightPos; // already in view space

    mat3 normalMatrix = transpose(inverse(mat3(u_ModelView)));
    vec3 N = normalize(normalMatrix * a_Normal);
    vec3 T = normalMatrix * a_Tangent;

    // Fallback: synthesize a tangent when a_Tangent is zero or degenerate
    if (dot(T, T) < 0.001)
    {
        vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
        T = normalize(cross(up, N));
    }
    else
    {
        T = normalize(T - dot(T, N) * N);
    }

    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    v_TexCoord = a_TexCoord;
    v_TangentFragPos = TBN * fragPosView;
    v_TangentLightPos = TBN * lightPosView;

    gl_Position = u_MVP * vec4(a_Vertex, 1.0);
}
