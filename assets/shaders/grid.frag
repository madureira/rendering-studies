#version 410 core

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

in vec3 v_Near;
in vec3 v_Far;
in vec3 v_CameraPos;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec4 frag_color;

// Grid parameters
const float kLineWidth = 0.008;    // Line width as fraction of cell (thinner)
const vec3 kGridColor = vec3(0.4); // Gray grid lines

// Fog parameters
const vec3 kFogColor = vec3(0.18, 0.18, 0.10); // Dark gray fog (match background)
const float kFogStart = 1000.0;                // Distance where fog starts
const float kFogEnd = 2000.0;                  // Distance where fog is full

// Axis colors
const vec3 kAxisXColor = vec3(1.0, 0.2, 0.2); // Red for X axis
const vec3 kAxisZColor = vec3(0.2, 0.2, 1.0); // Blue for Z axis
const vec3 kAxisYColor = vec3(0.2, 1.0, 0.2); // Green for Y axis

// Axis line widths (in pixels)
const float kAxisXWidth = 3.5; // X axis (red) width
const float kAxisZWidth = 3.0; // Z axis (blue) width
const float kAxisYWidth = 2.0; // Y axis (green) width

// Simple anti-aliased grid - Ben Golus technique
// Uses camera-relative position for stable derivatives at large distances
float pristineGrid(vec2 worldPos, vec2 relativePos, float lineWidth)
{
    // Use relative position for derivatives (stable at large distances)
    vec2 uvDeriv = fwidth(relativePos);

    // Clamp derivatives to prevent issues at extreme distances
    uvDeriv = clamp(uvDeriv, vec2(1e-7), vec2(1.0));

    // Line width, clamped to derivative for proper AA
    vec2 targetWidth = vec2(lineWidth);
    vec2 drawWidth = clamp(targetWidth, uvDeriv, vec2(0.5));

    // AA range
    vec2 lineAA = uvDeriv * 1.5;

    // Distance from lines - use worldPos for pattern (fract handles large values)
    vec2 gridUV = abs(fract(worldPos) * 2.0 - 1.0) * 0.5;

    // Anti-aliased line
    vec2 grid2 = smoothstep(drawWidth + lineAA, drawWidth - lineAA, gridUV);
    grid2 = clamp(grid2, 0.0, 1.0);

    // Combine both axes
    float grid = max(grid2.x, grid2.y);

    // Sub-pixel fade to prevent moiré
    float maxDeriv = max(uvDeriv.x, uvDeriv.y);
    float fade = 1.0 - smoothstep(0.5, 1.0, maxDeriv);

    return grid * fade;
}

// Simple two-level grid with camera-relative coordinates
float simpleGrid(vec2 worldPos, vec2 relativePos)
{
    // Base grid (1 unit)
    float grid1 = pristineGrid(worldPos, relativePos, kLineWidth);

    // Major grid (10 units)
    float grid10 = pristineGrid(worldPos * 0.1, relativePos * 0.1, kLineWidth);

    // Combine: major grid is more prominent
    return max(grid1 * 0.4, grid10 * 0.7);
}

// Axis line with camera-relative anti-aliasing
// worldCoord: actual world coordinate (for detecting axis at 0)
// relativeCoord: camera-relative coordinate (for stable derivatives)
float axisLine(float worldCoord, float relativeCoord, float pixelWidth)
{
    // Use relative coordinate for stable derivative
    float deriv = fwidth(relativeCoord);
    float stableDeriv = clamp(deriv, 1e-7, 1.0);

    // Distance in pixels from the axis (use world coord for actual distance from origin)
    float distInPixels = abs(worldCoord) / stableDeriv;

    // Half width in pixels
    float halfWidthPix = pixelWidth * 0.5;

    // AA range in pixels (fixed, not dependent on world scale)
    float aaPix = 1.0;

    // Calculate line alpha with stable pixel-space AA
    float lineAlpha = 1.0 - smoothstep(halfWidthPix - aaPix, halfWidthPix + aaPix, distInPixels);

    return clamp(lineAlpha, 0.0, 1.0);
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


void main()
{
    vec3 rayOrigin = v_Near;
    vec3 rayDir = normalize(v_Far - v_Near);

    // --- Y axis (green) - rendered independently of grid plane ---
    // Compute closest point on the view ray to the world Y axis (x=0, z=0)
    // Split into positive (above grid) and negative (below grid) parts
    float yAxisAlphaPositive = 0.0; // Y >= 0, rendered on top
    float yAxisAlphaNegative = 0.0; // Y < 0, rendered behind grid
    {
        vec2 o = rayOrigin.xz;
        vec2 d = rayDir.xz;

        float dd = dot(d, d);
        if (dd > 1e-10)
        {
            float tClosest = -dot(o, d) / dd;

            // Only render if in front of camera
            if (tClosest > 0.0)
            {
                vec3 axisPos = rayOrigin + tClosest * rayDir;

                // Camera-relative position for stable derivatives
                vec2 relativeXZ = axisPos.xz - v_CameraPos.xz;

                float distWorld = length(axisPos.xz);
                vec2 dxz = fwidth(relativeXZ);
                float worldPerPixel = clamp(max(dxz.x, dxz.y), 1e-7, 1.0);

                // Convert to stable pixel-space distance
                float distInPixels = distWorld / worldPerPixel;

                // Half width in pixels
                float halfWidthPix = kAxisYWidth * 0.5;

                // Fixed AA range in pixels
                float aaPix = 0.75;

                // Calculate line alpha with stable pixel-space AA
                float yAxisAlpha = 1.0 - smoothstep(halfWidthPix - aaPix, halfWidthPix + aaPix, distInPixels);
                yAxisAlpha = clamp(yAxisAlpha, 0.0, 1.0);

                // Split based on Y position
                if (axisPos.y >= 0.0)
                {
                    yAxisAlphaPositive = yAxisAlpha;
                }
                else
                {
                    yAxisAlphaNegative = yAxisAlpha;
                }
            }
        }
    }

    // --- Grid on Y=0 plane ---
    float t = -v_Near.y / (v_Far.y - v_Near.y);
    vec3 fragPos3D = v_Near + t * (v_Far - v_Near);

    // Check if we hit the grid plane
    bool onGridPlane = (t > 0.0);

    // Start with Y axis negative part (behind the grid)
    if (yAxisAlphaNegative > 0.0)
    {
        frag_color = vec4(kAxisYColor, yAxisAlphaNegative * 0.6); // Slightly dimmer behind grid
        gl_FragDepth = 0.999;                                     // Far depth so grid can occlude it
    }
    else
    {
        frag_color = vec4(0.0);
        gl_FragDepth = 1.0;
    }

    if (onGridPlane)
    {
        gl_FragDepth = computeDepth(fragPos3D);

        // Camera-relative position for stable calculations at large distances
        vec2 relativePos = fragPos3D.xz - v_CameraPos.xz;

        // --- Simple Grid (1 unit + 10 unit) ---
        // Use world position for pattern, relative position for derivatives
        float gridAlpha = simpleGrid(fragPos3D.xz, relativePos);

        // Base grid color
        vec3 gridColor = kGridColor;

        // --- X/Z Axis lines (on the grid plane) ---
        // Z axis (blue) - where X = 0
        float zAxisAlpha = axisLine(fragPos3D.x, relativePos.x, kAxisZWidth);

        // X axis (red) - where Z = 0
        float xAxisAlpha = axisLine(fragPos3D.z, relativePos.y, kAxisXWidth);

        // Build final color
        vec3 finalColor = gridColor;
        float finalAlpha = gridAlpha;

        // Overlay Z axis (blue)
        if (zAxisAlpha > 0.0)
        {
            finalColor = mix(finalColor, kAxisZColor, zAxisAlpha);
            finalAlpha = max(finalAlpha, zAxisAlpha);
        }

        // Overlay X axis (red)
        if (xAxisAlpha > 0.0)
        {
            finalColor = mix(finalColor, kAxisXColor, xAxisAlpha);
            finalAlpha = max(finalAlpha, xAxisAlpha);
        }

        // --- Fog effect ---
        // Calculate distance from camera to fragment (using relative position)
        float distFromCamera = length(fragPos3D - v_CameraPos);
        float fogFactor = smoothstep(kFogStart, kFogEnd, distFromCamera);

        // Apply fog: blend color towards fog color and reduce alpha
        finalColor = mix(finalColor, kFogColor, fogFactor);
        finalAlpha = finalAlpha * (1.0 - fogFactor * 0.8); // Fade out but not completely

        // Blend grid on top of any Y axis negative part
        frag_color.rgb = mix(frag_color.rgb, finalColor, finalAlpha);
        frag_color.a = max(frag_color.a, finalAlpha);
    }

    // --- Overlay Y axis positive part (above grid) on top of everything ---
    if (yAxisAlphaPositive > 0.0)
    {
        frag_color.rgb = mix(frag_color.rgb, kAxisYColor, yAxisAlphaPositive);
        frag_color.a = max(frag_color.a, yAxisAlphaPositive);
    }
}
