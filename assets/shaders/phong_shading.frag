#version 410 core

in vec3 v_Normal;
in vec3 v_Position;

uniform vec3 u_CameraPosition;
uniform vec3 u_LightPosition;

out vec4 frag_color;

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
    frag_color = vec4(ADSLightModel(v_Normal, v_Position), 1.0);
}
