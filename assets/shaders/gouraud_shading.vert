#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_CameraPosition;
uniform vec3 u_LightPosition;

out vec3 v_Color;

vec3 ADSLightModel(in vec3 normal, in vec3 position)
{
    const vec3 lightAmbient = vec3(0.2, 0.2, 0.2);
    const vec3 lightDiffuse = vec3(1.0, 1.0, 1.0);
    const vec3 lightSpecular = vec3(1.0, 1.0, 1.0);

    const vec3 materialAmbient = vec3(1.0, 0.5, 0.0);
    const vec3 materialDiffuse = vec3(0.8, 0.6, 0.4);
    const vec3 materialSpecular = vec3(0.6, 0.6, 0.6);

    const float materialShininess = 80;

    // normal, light, view, and light reflection vectors
    vec3 norm = normalize(normal);
    vec3 lightv = normalize(u_LightPosition);
    vec3 viewv = normalize(u_CameraPosition - position);
    vec3 refl = reflect(-lightv, norm);

    // ambient light computation
    vec3 ambient = materialAmbient * lightAmbient;

    // diffuse light computation
    vec3 diffuse = max(0.0, dot(lightv, norm)) * materialDiffuse * lightDiffuse;

    // specular light computation
    vec3 specular = vec3(0.0, 0.0, 0.0);

    if (dot(lightv, viewv) > 0.0)
    {
        specular = pow(max(0.0, dot(viewv, refl)), materialShininess) * materialSpecular * lightSpecular;
    }

    return clamp(ambient + diffuse + specular, 0.0, 1.0);
}

void main()
{
    // Transform normal (using normal matrix to handle non-uniform scaling)
    mat3 normalMatrix = mat3(transpose(inverse(u_Model)));
    vec3 transNorm = normalize(normalMatrix * a_Normal);

    // Transform position
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);

    v_Color = ADSLightModel(transNorm, worldPos.xyz);

    gl_Position = u_Projection * u_View * worldPos;
}
