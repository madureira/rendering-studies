in vec2 v_TexCoord;
in vec3 v_TangentLightPos;
in vec3 v_TangentFragPos;

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_SpecularMap;
uniform float u_NormalStrength;
uniform float u_SpecularStrength;

out vec4 frag_color;

void main()
{
    vec3 normal = texture(u_NormalMap, v_TexCoord).rgb;
    normal = normal * 2.0 - 1.0;
    normal.xy *= u_NormalStrength;
    normal = normalize(normal);

    vec3 diffuse = texture(u_DiffuseMap, v_TexCoord).rgb;

    vec3 ambient = 0.1 * diffuse;

    vec3 lightDir = normalize(v_TangentLightPos - v_TangentFragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseLight = diff * diffuse;

    vec3 viewDir = normalize(-v_TangentFragPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0);
    float specFactor = texture(u_SpecularMap, v_TexCoord).r;
    vec3 specular = vec3(specFactor) * spec * u_SpecularStrength;

    frag_color = vec4(ambient + diffuseLight + specular, 1.0);
}
