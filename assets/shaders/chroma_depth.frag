uniform float u_ChromaBlue; // z-depth corresponding to blue
uniform float u_ChromaRed;  // z-depth corresponding to red

in float v_LightIntensity; // from lighting model
in float v_Z;              // depth in eye coordinates

out vec4 frag_color;

vec3 ChromaDepth(float t)
{
    t = clamp(t, 0.0, 1.0);

    float r = 1.0;
    float g = 0.0;
    float b = 1.0 - 6.0 * (t - (5.0 / 6.0));

    if (t <= (5.0 / 6.0))
    {
        r = 6.0 * (t - (4.0 / 6.0));
        g = 0.0;
        b = 1.0;
    }

    if (t <= (4.0 / 6.0))
    {
        r = 0.0;
        g = 1.0 - 6.0 * (t - (3.0 / 6.0));
        b = 1.0;
    }

    if (t <= (3.0 / 6.0))
    {
        r = 0.0;
        g = 1.0;
        b = 6.0 * (t - (2.0 / 6.0));
    }


    if (t <= (2.0 / 6.0))
    {
        r = 1.0 - 6.0 * (t - (1.0 / 6.0));
        g = 1.0;
        b = 0.0;
    }

    if (t <= (1.0 / 6.0))
    {
        r = 1.0;
        g = 6.0 * t;
    }

    return vec3(r, g, b);
}

void main()
{
    float t = (v_Z - u_ChromaRed) / (u_ChromaBlue - u_ChromaRed);
    vec3 color = ChromaDepth(t);
    frag_color = vec4(v_LightIntensity * color, 1.0);
}
