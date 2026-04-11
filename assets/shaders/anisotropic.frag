// ============================================================================
// Anisotropic PBR fragment shader
//
// Based on Google Filament's anisotropic BRDF (GGX distribution + Smith-GGX
// correlated visibility + Schlick Fresnel).
//
// References:
//   - Burley 2012, "Physically-Based Shading at Disney"
//   - Heitz 2014, "Understanding the Masking-Shadowing Function"
//   - Kulla 2017, "Revisiting Physically Based Shading at Imageworks"
//   - Romain Guy, https://gist.github.com/romainguy/39d4c1c6aac56623399dcd1e8da68337
//
// Coordinate space: all positions (v_WorldPos, u_CameraPosition) are in
// origin-relative world space (floating-origin pattern for precision).
// ============================================================================

in vec3 v_WorldPos;
in vec3 v_Normal;
in vec3 v_Tangent;
in vec3 v_Bitangent;

// Both u_CameraPosition and v_WorldPos are in origin-relative space
uniform vec3  u_CameraPosition;
// Directional light: normalized direction toward the light
uniform vec3  u_LightDir;
uniform vec3  u_LightColor;
uniform vec3  u_Albedo;         // base reflectance (F0 for metals)
uniform float u_Roughness;      // perceptual roughness [0..1]
uniform float u_Anisotropy;     // anisotropy amount [-1..1]; >0 stretches along T
uniform float u_Metallic;       // 0 = dielectric, 1 = metal

out vec4 frag_color;

const float PI            = 3.14159265358979;
const float MIN_ROUGHNESS = 0.045;

// ---------- helpers ---------------------------------------------------------

float sq(float x) { return x * x; }
float saturate(float x) { return clamp(x, 0.0, 1.0); }

// ---------- GGX Anisotropic NDF (Burley 2012) -------------------------------

float D_GGX_Anisotropic(float at, float ab, float ToH, float BoH, float NoH)
{
    float a2 = at * ab;
    vec3  d  = vec3(ab * ToH, at * BoH, a2 * NoH);
    float d2 = dot(d, d);
    float b2 = a2 / d2;
    return a2 * b2 * b2 * (1.0 / PI);
}

// ---------- Smith-GGX Correlated Visibility (Heitz 2014) --------------------

float V_SmithGGX_Anisotropic(float at, float ab,
                              float ToV, float BoV, float NoV,
                              float ToL, float BoL, float NoL)
{
    float lambdaV = NoL * length(vec3(at * ToV, ab * BoV, NoV));
    float lambdaL = NoV * length(vec3(at * ToL, ab * BoL, NoL));
    float v = 0.5 / max(lambdaV + lambdaL, 1e-7);
    return v;
}

// ---------- Fresnel (Schlick 1994) ------------------------------------------

vec3 F_Schlick(vec3 f0, float VoH)
{
    float f = pow(1.0 - VoH, 5.0);
    return f0 + (1.0 - f0) * f;
}

// ---------- Lambertian diffuse ----------------------------------------------

vec3 Fd_Lambert(vec3 albedo)
{
    return albedo * (1.0 / PI);
}

// ============================================================================

void main()
{
    vec3 N = normalize(v_Normal);
    // Gram-Schmidt re-orthogonalization after interpolation
    vec3 T = normalize(v_Tangent   - N * dot(v_Tangent,   N));
    vec3 B = normalize(v_Bitangent - N * dot(v_Bitangent, N));

    // V and L in origin-relative space (consistent with v_WorldPos)
    vec3 V = normalize(u_CameraPosition - v_WorldPos);
    vec3 L = u_LightDir;
    vec3 H = normalize(V + L);

    float NoV = abs(dot(N, V)) + 1e-5;
    float NoL = saturate(dot(N, L));
    float NoH = saturate(dot(N, H));
    float VoH = saturate(dot(V, H));

    float ToV = dot(T, V);
    float BoV = dot(B, V);
    float ToL = dot(T, L);
    float BoL = dot(B, L);
    float ToH = dot(T, H);
    float BoH = dot(B, H);

    // Derive anisotropic roughness from perceptual roughness + anisotropy
    // (Kulla 2017)
    float roughness = max(u_Roughness * u_Roughness, MIN_ROUGHNESS);
    float at = max(roughness * (1.0 + u_Anisotropy), MIN_ROUGHNESS);
    float ab = max(roughness * (1.0 - u_Anisotropy), MIN_ROUGHNESS);

    // F0: for metals F0 = albedo; for dielectrics F0 ~ 0.04
    vec3 f0 = mix(vec3(0.04), u_Albedo, u_Metallic);

    // ---- Specular (anisotropic GGX) ----
    float D   = D_GGX_Anisotropic(at, ab, ToH, BoH, NoH);
    float Vis = V_SmithGGX_Anisotropic(at, ab, ToV, BoV, NoV, ToL, BoL, NoL);
    vec3  F   = F_Schlick(f0, VoH);

    vec3 Fr = (D * Vis) * F;

    // ---- Diffuse ----
    // Metals have no diffuse; scale by (1 - metallic) and (1 - F)
    vec3 Fd = (1.0 - u_Metallic) * (vec3(1.0) - F) * Fd_Lambert(u_Albedo);

    // ---- Combine ----
    vec3 color = (Fd + Fr) * u_LightColor * NoL;

    // ---- Ambient / fake environment reflection ----
    // Without an environment map, metals look black because they have no
    // diffuse. Approximate with a hemisphere sky/ground blend.
    vec3 skyColor    = vec3(0.9, 0.95, 1.0);
    vec3 groundColor = vec3(0.3, 0.28, 0.25);
    float skyBlend   = 0.5 + 0.5 * N.y;
    vec3 envColor    = mix(groundColor, skyColor, skyBlend);

    // Fresnel: edges reflect more of the environment
    float envFresnel  = 1.0 - pow(1.0 - NoV, 5.0);
    float envStrength = mix(0.8, 0.35, envFresnel);
    vec3 ambient      = f0 * envColor * envStrength;

    color += ambient;

    frag_color = vec4(color, 1.0);
}
