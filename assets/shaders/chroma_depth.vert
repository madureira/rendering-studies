layout(location = 0) in vec3 a_Vertex;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_ModelView;
uniform mat4 u_MVP;

out float v_Z;
out float v_LightIntensity;

void main()
{
    vec4 eyePos = u_ModelView * vec4(a_Vertex, 1.0);
    v_Z = eyePos.z;

    vec3 N = normalize(mat3(transpose(inverse(u_ModelView))) * a_Normal);
    vec3 L = normalize(vec3(0.3, 0.5, 0.8));
    v_LightIntensity = max(0.0, dot(N, L)) * 0.7 + 0.3;

    gl_Position = u_MVP * vec4(a_Vertex, 1.0);
}
