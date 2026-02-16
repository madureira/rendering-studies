#version 410 core

in vec3 v_WorldPos;

uniform vec3 u_CameraPos;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;

uniform vec2 u_RippleCenterXZ;  // center on plane in world XZ
uniform float u_Time;           // seconds
uniform float u_Amp;            // amplitude, e.g. 0.08
uniform float u_K;              // wave number, e.g. 18.0
uniform float u_Omega;          // speed, e.g. 8.0
uniform float u_Damping;        // e.g. 1.2
uniform float u_NormalStrength; // e.g. 1.0..4.0

out vec4 frag_color;

// Damped radial sine wave height
float heightField(vec2 xz)
{
    vec2 d = xz - u_RippleCenterXZ;
    float r = length(d);
    // avoid singularity at r=0
    r = max(r, 1e-4);

    float wave = sin(u_K * r - u_Omega * u_Time);
    float env = exp(-u_Damping * r);
    return u_Amp * wave * env;
}

// Analytical derivatives dh/dx and dh/dz
vec2 heightDeriv(vec2 xz)
{
    vec2 d = xz - u_RippleCenterXZ;
    float r = length(d);
    r = max(r, 1e-4);

    // h = A * sin(k r - w t) * exp(-damp r)
    float phase = u_K * r - u_Omega * u_Time;
    float s = sin(phase);
    float c = cos(phase);
    float env = exp(-u_Damping * r);

    // dh/dr = A * env * (k*cos(phase) - damp*sin(phase))
    float dhdr = u_Amp * env * (u_K * c - u_Damping * s);

    // dr/dx = dx/r, dr/dz = dz/r
    vec2 drd = d / r;

    // dh/dx = dh/dr * dr/dx, dh/dz = dh/dr * dr/dz
    return dhdr * drd;
}

void main()
{
    vec2 xz = v_WorldPos.xz;

    // Derivatives and normal
    vec2 dh = heightDeriv(xz);
    vec3 N = normalize(vec3(-dh.x * u_NormalStrength, 1.0, -dh.y * u_NormalStrength));

    // Simple lighting (Blinn-Phong)
    vec3 L = normalize(u_LightPos - v_WorldPos);
    vec3 V = normalize(u_CameraPos - v_WorldPos);
    vec3 H = normalize(L + V);

    float NdotL = max(dot(N, L), 0.0);
    float spec = pow(max(dot(N, H), 0.0), 64.0);

    vec3 baseColor = vec3(0.08, 0.35, 0.55);
    vec3 color = baseColor * NdotL * u_LightColor + spec * u_LightColor * 0.35;

    frag_color = vec4(color, 1.0);
}
