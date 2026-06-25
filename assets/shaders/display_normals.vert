layout(location = 0) in vec3 a_Vertex;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_ModelView;

out vec3 v_Normal;

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(u_ModelView)));
    v_Normal = normalMatrix * a_Normal;
    gl_Position = u_ModelView * vec4(a_Vertex, 1.0);
}
