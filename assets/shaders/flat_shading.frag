#version 410 core

flat in vec3 v_Normal;
in vec3 v_FragPos;

out vec4 frag_color;

void main()
{
    // Simple directional light from above-front
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));

    // Compute diffuse lightig using the flat normal
    float diffuse = max(dot(v_Normal, lightDir), 0.0);

    // Add ambient term so back faces aren't completely black
    float ambient = 0.3;
    float lighting = ambient + diffuse * 0.7;

    // Base color
    vec3 objectColor = vec3(0.8, 0.6, 0.4);

    // Final color based on lighting
    vec3 color = objectColor * lighting;

    frag_color = vec4(color, 1.0);
}
