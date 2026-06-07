layout(location = 0) in vec3 a_Vertex;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform mat4 u_LightSpaceMatrix;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;
out vec4 v_FragPosLightSpace;

void main()
{
    vec4 worldPos = u_Model * vec4(a_Vertex, 1.0);
    v_FragPos = vec3(worldPos);
    v_Normal = u_NormalMatrix * a_Normal;
    v_TexCoords = a_TexCoords;
    v_FragPosLightSpace = u_LightSpaceMatrix * worldPos;
    gl_Position = u_Projection * u_View * worldPos;
}
