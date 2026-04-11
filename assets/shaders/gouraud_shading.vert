layout(location = 0) in vec3 a_Vertex;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_VP;

// Both u_CameraPosition and worldPos are in origin-relative space
uniform vec3 u_CameraPosition;
// Directional light: normalized direction toward the light
uniform vec3 u_LightDir;

out vec3 v_Color;

vec3 ADSLightModel(in vec3 normal, in vec3 worldPos)
{
    const vec3 lightAmbient  = vec3(0.2);
    const vec3 lightDiffuse  = vec3(1.0);
    const vec3 lightSpecular = vec3(1.0);

    const vec3 materialAmbient  = vec3(1.0, 0.5, 0.0);
    const vec3 materialDiffuse  = vec3(0.8, 0.6, 0.4);
    const vec3 materialSpecular = vec3(0.6);

    const float materialShininess = 80.0;

    vec3 N = normalize(normal);
    vec3 L = u_LightDir;
    vec3 V = normalize(u_CameraPosition - worldPos);
    vec3 R = reflect(-L, N);

    vec3 ambient  = materialAmbient * lightAmbient;
    vec3 diffuse  = max(0.0, dot(L, N)) * materialDiffuse * lightDiffuse;

    vec3 specular = vec3(0.0);
    if (dot(L, N) > 0.0)
    {
        specular = pow(max(0.0, dot(V, R)), materialShininess) * materialSpecular * lightSpecular;
    }

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(u_Model)));
    vec3 N = normalize(normalMatrix * a_Normal);

    vec4 worldPos = u_Model * vec4(a_Vertex, 1.0);

    v_Color = ADSLightModel(N, worldPos.xyz);

    gl_Position = u_VP * worldPos;
}
