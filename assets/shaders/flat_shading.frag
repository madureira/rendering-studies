#version 410 core

flat in vec3 v_Normal;
in vec3 v_WorldPos;

// Directional light: normalized direction toward the light
uniform vec3 u_LightDir;

out vec4 frag_color;

void main()
{
    vec3 normal = normalize(v_Normal);
    float diffuse = max(dot(normal, u_LightDir), 0.0);

    float ambient = 0.3;
    float lighting = ambient + diffuse * 0.7;

    vec3 objectColor = vec3(0.8, 0.6, 0.4);
    frag_color = vec4(objectColor * lighting, 1.0);
}
