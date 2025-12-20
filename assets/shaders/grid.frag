#version 410 core

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec3 v_Near;
in vec3 v_Far;

uniform float u_Near;
uniform float u_Far;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec4 frag_color;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis)
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));

    // z axis
    if (fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
    {
        color.z = 1.0;
    }
    // x axis
    if (fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
    {
        color.x = 1.0;
    }

    return color;
}

float computeDepth(vec3 pos)
{
    vec4 clipSpacePos = u_Projection * u_View * vec4(pos, 1.0);
    float clipSpaceDepth = (clipSpacePos.z / clipSpacePos.w);
    float far = gl_DepthRange.far;
    float near = gl_DepthRange.near;
    float depth = (((far - near) * clipSpaceDepth) + near + far) / 2.0;
    return depth;
}

float computeLinearDepth(vec3 pos)
{
    vec4 clipSpacePos = u_Projection * u_View * vec4(pos.xyz, 1.0);
    float clipSpaceDepth = (clipSpacePos.z / clipSpacePos.w) * 2.0 - 1.0;                              // put back between -1 and 1
    float linearDepth = (2.0 * u_Near * u_Far) / (u_Far + u_Near - clipSpaceDepth * (u_Far - u_Near)); // get linear value between 0.01 and 100
    return linearDepth / u_Far;                                                                        // normalize
}

void main()
{
    float t = -v_Near.y / (v_Far.y - v_Near.y);
    vec3 fragPos3D = v_Near + t * (v_Far - v_Near);
    float is_on = float(t > 0);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    frag_color = (grid(fragPos3D, 10, false) + grid(fragPos3D, 1, true)) * float(t > 0); // adding multiple resolution for the grid
    frag_color.a *= fading;

    // ===== Added: Y axis (green), anchored to the grid origin (world x=0,z=0) =====
    // We compute the closest point on the per-pixel view ray to the world Y axis.
    // Then we build a constant pixel-width mask (stable) from the distance in XZ.

    vec3 rayOrigin = v_Near;
    vec3 rayDir = normalize(v_Far - v_Near);

    // Project the problem into the XZ plane (Y axis is x=0,z=0 for all y).
    vec2 o = rayOrigin.xz;
    vec2 d = rayDir.xz;

    float dd = dot(d, d);
    if (dd > 1e-10)
    {
        // Closest approach along the ray to the Y axis in XZ.
        float tClosest = -dot(o, d) / dd;
        tClosest = max(tClosest, 0.0);

        vec3 axisPos = rayOrigin + tClosest * rayDir;

        // World distance from the ray to the Y axis (in XZ).
        float distWorld = length(axisPos.xz);

        // Convert world distance to pixel distance using derivatives.
        // This makes thickness stable (constant in pixels), reducing "dotted" while moving.
        vec2 dxz = fwidth(axisPos.xz);
        float worldPerPixel = max(max(dxz.x, dxz.y), 1e-6);

        float distPix = distWorld / worldPerPixel;

        // Thickness control (in pixels). Increase to make the axis thicker.
        float halfWidthPix = 1.0;

        // Anti-alias in pixel space (stable). The 1.0 range gives a soft edge ~1px.
        float yAxisAlpha = 1.0 - smoothstep(halfWidthPix - 0.5, halfWidthPix + 0.5, distPix);
        yAxisAlpha = clamp(yAxisAlpha, 0.0, 1.0);

        // Apply similar fading as the grid (optional but helps consistency).
        float axisLinearDepth = computeLinearDepth(axisPos);
        float axisFading = max(0.0, (0.5 - axisLinearDepth));
        yAxisAlpha *= axisFading;

        // Overlay green on top of the grid (no depth switching to avoid flicker/dots).
        if (yAxisAlpha > 0.0)
        {
            frag_color.rgb = mix(frag_color.rgb, vec3(0.0, 1.0, 0.0), yAxisAlpha);
            frag_color.a = max(frag_color.a, yAxisAlpha);
        }
    }
}
