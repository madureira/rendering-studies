#version 410 core

precision highp float;

// ============================================================================
// INPUTS FROM VERTEX SHADER
// ============================================================================

// Homogeneous coordinates (vec4) from vertex shader.
// We delay the perspective divide (w division) to the fragment shader to ensure
// correct interpolation. Dividing by w in the vertex shader and then interpolating
// causes visual artifacts (shimmer/jitter) because perspective projection is
// non-linear in screen space.
in vec4 v_NearH;     // Unprojected near plane point (homogeneous)
in vec4 v_FarH;      // Unprojected far plane point (homogeneous)
in vec3 v_CameraPos; // Camera position in relative coordinates

// ============================================================================
// UNIFORMS
// ============================================================================

uniform mat4 u_View;
uniform mat4 u_Projection;

// Grid origin in world coordinates (camera XZ position, Y=0).
// Used to reconstruct world coordinates for axis positioning.
uniform vec3 u_GridOrigin;

// Fractional parts of camera position for each grid scale.
// These are computed with double precision on the CPU to avoid float precision
// issues at large world coordinates. The grid pattern uses fract() which loses
// precision with large floats (mantissa ~23 bits), so we pre-compute the
// fractional offset and keep all shader calculations near zero.
uniform vec2 u_GridFract1;   // fract(cameraPos / 1.0)
uniform vec2 u_GridFract10;  // fract(cameraPos / 10.0)
uniform vec2 u_GridFract100; // fract(cameraPos / 100.0)
uniform bool u_ShowYAxis;    // when false, the green Y-axis line is not drawn

out vec4 frag_color;

// ============================================================================
// CONFIGURATION - Adjust these values to customize the grid appearance
// ============================================================================

// Colors
const vec3 kGridColor = vec3(0.5);              // Grid lines color
const vec3 kAxisXColor = vec3(0.9, 0.15, 0.15); // X axis (red)
const vec3 kAxisYColor = vec3(0.15, 0.9, 0.15); // Y axis (green)
const vec3 kAxisZColor = vec3(0.15, 0.3, 0.9);  // Z axis (blue)

// Axis line settings (in pixels)
const float kAxisXZLineWidth = 3.5;
const float kAxisYLineWidth = 1.0;
const float kAxisIntensity = 0.95;
const float kAxisYBelowGroundIntensity = 0.5;

// Axis fade settings
const float kAxisFadeMultiplier = 40.0;
const float kAxisYFadeMultiplier = 50.0;

// Grid line settings
const float kGridLineWidth = 0.02;   // Grid line width as fraction of cell (0.02 = 2%)
const float kGrid1Intensity = 0.5;   // 1-unit grid intensity
const float kGrid10Intensity = 0.65; // 10-unit grid intensity
const float kGrid100Intensity = 0.8; // 100-unit grid intensity

// Grid level fade thresholds (camera height)
const float kGrid10FadeStart = 2.0;
const float kGrid10FadeEnd = 10.0;
const float kGrid100FadeStart = 20.0;
const float kGrid100FadeEnd = 100.0;

// ============================================================================
// PRISTINE GRID
// Based on "The Best Darn Grid Shader (Yet)" by Ben Golus
// https://bgolus.medium.com/the-best-darn-grid-shader-yet-727f9278b9d8
//
// This algorithm produces anti-aliased grid lines that remain stable at any
// distance by using screen-space derivatives to calculate proper line widths.
// ============================================================================

float pristineGrid(vec2 uv, vec2 lineWidth)
{
    // Calculate screen-space derivatives (how much UV changes per pixel)
    // This tells us the "size" of one pixel in UV space
    vec4 uvDDXY = vec4(dFdx(uv), dFdy(uv));
    vec2 uvDeriv = vec2(length(uvDDXY.xz), length(uvDDXY.yw));

    // Handle case where line width > 0.5 (invert the pattern)
    bvec2 invertLine = greaterThan(lineWidth, vec2(0.5));
    vec2 targetWidth = mix(lineWidth, vec2(1.0) - lineWidth, vec2(invertLine));

    // Clamp line width to be at least as wide as one pixel (prevents aliasing)
    vec2 drawWidth = clamp(targetWidth, uvDeriv, vec2(0.5));

    // Anti-aliasing width (1.5 pixels of smooth transition)
    vec2 lineAA = uvDeriv * 1.5;

    // Convert UV to centered cell coordinates [-1, 1]
    vec2 gridUV = abs(fract(uv) * 2.0 - 1.0);
    gridUV = mix(vec2(1.0) - gridUV, gridUV, vec2(invertLine));

    // Smoothstep for anti-aliased edges
    vec2 grid2 = smoothstep(drawWidth + lineAA, drawWidth - lineAA, gridUV);

    // Correct intensity when line is wider than target
    grid2 *= clamp(targetWidth / drawWidth, 0.0, 1.0);

    // Fade to solid when cells become sub-pixel
    grid2 = mix(grid2, targetWidth, clamp(uvDeriv * 2.0 - 1.0, 0.0, 1.0));
    grid2 = mix(grid2, vec2(1.0) - grid2, vec2(invertLine));

    // Combine X and Y grid lines
    return mix(grid2.x, 1.0, grid2.y);
}

// ============================================================================
// DEPTH CALCULATION
// Computes the proper depth buffer value for a world position so the grid
// correctly interacts with other geometry in the scene.
// ============================================================================

float computeDepth(vec3 pos)
{
    vec4 clip = u_Projection * u_View * vec4(pos, 1.0);
    float ndc = clip.z / clip.w; // Normalized device coordinate [-1, 1]
    // Convert from NDC to depth buffer range
    return (gl_DepthRange.diff * ndc + gl_DepthRange.near + gl_DepthRange.far) * 0.5;
}

// ============================================================================
// MAIN
// ============================================================================

void main()
{
    // ------------------------------------------------------------------------
    // RAY-PLANE INTERSECTION
    // We render a fullscreen quad and cast rays from the camera through each
    // pixel to find where they intersect the ground plane (Y=0).
    // ------------------------------------------------------------------------

    // Perform perspective divide here (not in vertex shader) for correct interpolation.
    // This converts from homogeneous clip space to 3D world-relative coordinates.
    vec3 nearP = v_NearH.xyz / v_NearH.w;
    vec3 farP = v_FarH.xyz / v_FarH.w;

    // Ground plane is at Y=0 in relative coordinates.
    // (Camera-relative rendering: all calculations happen near origin to preserve
    // float precision. gridOrigin.y is always 0.)
    float planeY = 0.0;

    // Calculate ray direction (from near to far plane)
    float denomY = farP.y - nearP.y;

    // Discard if ray is nearly parallel to ground (would never intersect)
    if (abs(denomY) < 1e-6)
    {
        discard;
    }

    // Parametric intersection: find t where ray.y = planeY
    // ray = nearP + t * (farP - nearP)
    float t = (planeY - nearP.y) / denomY;

    // Calculate the 3D position where ray hits the ground plane.
    // This is in RELATIVE coordinates (centered around camera XZ position),
    // so values stay small and float precision is preserved.
    vec3 fragPos3D = nearP + t * (farP - nearP);

    // Reconstruct WORLD position by adding back the grid origin.
    // Needed for axis positioning (axes are at world X=0, Y=0, Z=0).
    vec3 worldPos = fragPos3D + u_GridOrigin;

    // Initialize output
    frag_color = vec4(0.0);
    gl_FragDepth = 1.0; // Default to far plane (invisible)

    // Only render if intersection is in front of camera (t > 0)
    if (t > 0.0)
    {
        gl_FragDepth = computeDepth(fragPos3D);

        // ====================================================================
        // MULTI-SCALE GRID (1-unit, 10-unit, 100-unit)
        // ====================================================================

        // Grid UV coordinates: use relative position + pre-computed fractional offset.
        // This aligns the grid to world coordinates without large float values.
        //
        // Instead of: fract(worldPos.xz) which fails at large coordinates,
        // We use: fragPos3D.xz + u_GridFract (both are small values)
        //
        // The fractional offsets are computed on CPU with double precision,
        // ensuring the grid pattern is perfectly aligned to world coordinates.
        vec2 gridUV1 = fragPos3D.xz + u_GridFract1;
        vec2 gridUV10 = (fragPos3D.xz / 10.0) + u_GridFract10;
        vec2 gridUV100 = (fragPos3D.xz / 100.0) + u_GridFract100;

        vec2 lineWidth = vec2(kGridLineWidth);

        // Calculate grid pattern for each scale
        float grid1 = pristineGrid(gridUV1, lineWidth);
        float grid10 = pristineGrid(gridUV10, lineWidth);
        float grid100 = pristineGrid(gridUV100, lineWidth);

        // Height-based LOD: fade between grid scales as camera moves up/down.
        // Close to ground: show 1-unit grid
        // Higher up: fade to 10-unit, then 100-unit grid
        float camHeight = max(abs(v_CameraPos.y), 0.5);

        float fade1 = 1.0;
        float fade10 = smoothstep(kGrid10FadeStart, kGrid10FadeEnd, camHeight);
        float fade100 = smoothstep(kGrid100FadeStart, kGrid100FadeEnd, camHeight);

        // Combine all grid scales, taking the maximum intensity
        float gridAlpha = max(max(
                                  grid1 * kGrid1Intensity * fade1,
                                  grid10 * kGrid10Intensity * fade10),
            grid100 * kGrid100Intensity * fade100);

        vec3 color = kGridColor;
        float alpha = gridAlpha;

        // ====================================================================
        // AXIS LINES (X and Z) - Horizontal axes on the ground plane
        // ====================================================================

        // Calculate screen-space derivatives for consistent line width in pixels
        vec4 uvDDXY_x = vec4(dFdx(fragPos3D.xz), dFdy(fragPos3D.xz));
        vec2 derivX = vec2(length(uvDDXY_x.xz), length(uvDDXY_x.yw));

        // X axis: horizontal line where Z=0 in WORLD coordinates (red)
        float xAxisWidth = derivX.y * kAxisXZLineWidth;
        float xAxis = 1.0 - smoothstep(0.0, xAxisWidth, abs(worldPos.z));

        // Z axis: horizontal line where X=0 in WORLD coordinates (blue)
        float zAxisWidth = derivX.x * kAxisXZLineWidth;
        float zAxis = 1.0 - smoothstep(0.0, zAxisWidth, abs(worldPos.x));

        // Fade axes with distance from camera to avoid visual noise at horizon
        float distFromCamera = length(fragPos3D - v_CameraPos);
        float axisFadeDist = camHeight * kAxisFadeMultiplier;
        float axisFade = 1.0 - smoothstep(axisFadeDist * 0.3, axisFadeDist, distFromCamera);

        // Apply X axis color and alpha
        if (xAxis > 0.01)
        {
            float xAxisFinal = xAxis * axisFade;
            color = mix(color, kAxisXColor, xAxisFinal);
            alpha = max(alpha, xAxisFinal * kAxisIntensity);
        }

        // Apply Z axis color and alpha
        if (zAxis > 0.01)
        {
            float zAxisFinal = zAxis * axisFade;
            color = mix(color, kAxisZColor, zAxisFinal);
            alpha = max(alpha, zAxisFinal * kAxisIntensity);
        }

        frag_color = vec4(color, alpha);
    }

    // ========================================================================
    // Y AXIS (vertical line at world origin)
    // This is rendered separately because it's not on the ground plane.
    // We find the closest point on the ray to the Y axis line.
    // Disabled when u_ShowYAxis is false.
    // ========================================================================

    if (u_ShowYAxis)
    {
        vec3 rayDir = normalize(farP - nearP);
        vec2 rayXZ = rayDir.xz;
        float denom = dot(rayXZ, rayXZ);

        if (denom > 1e-8) // Ray has horizontal component
        {
            // Y axis is at world origin (0, Y, 0).
            // In relative coordinates, that's at position (-gridOrigin.x, Y, -gridOrigin.z)
            vec2 yAxisRelPos = -u_GridOrigin.xz;

            // Find parameter t where ray is closest to the Y axis line
            float tY = dot(yAxisRelPos - nearP.xz, rayXZ) / denom;

            if (tY > 0.0) // Point is in front of camera
            {
                // Closest point on ray to Y axis
                vec3 closest = nearP + tY * rayDir;

                // Distance from closest point to the Y axis (XZ distance only)
                float distXZ = length(closest.xz - yAxisRelPos);

                // Calculate line width in world units based on distance from camera
                // This keeps the line a consistent pixel width on screen
                float camDist = length(closest - v_CameraPos);
                float pixelWorldSize = camDist * 0.001; // Approximate world size of one pixel
                float lineW = pixelWorldSize * kAxisYLineWidth;
                float lineAA = pixelWorldSize * 0.6; // Anti-aliasing width

                // Smooth falloff from line center
                float yAxis = 1.0 - smoothstep(lineW - lineAA, lineW + lineAA, distXZ);

                // Fade with distance
                float camHeight = max(abs(v_CameraPos.y), 0.5);
                float yAxisFadeDist = camHeight * kAxisYFadeMultiplier;
                float yAxisFade = 1.0 - smoothstep(yAxisFadeDist * 0.3, yAxisFadeDist, camDist);

                yAxis *= yAxisFade;

                if (yAxis > 0.01)
                {
                    // Determine if point is above or below ground (Y=0)
                    // Below ground: render with reduced intensity for visual distinction
                    float worldY = closest.y; // World Y = relative Y (since gridOrigin.y = 0)
                    float intensity = worldY >= 0.0 ? kAxisIntensity : kAxisYBelowGroundIntensity;

                    // Blend Y axis color over existing content
                    frag_color.rgb = mix(frag_color.rgb, kAxisYColor, yAxis * intensity);
                    frag_color.a = max(frag_color.a, yAxis * intensity);
                }
            }
        }
    }
}
