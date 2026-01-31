#version 410 core

precision highp float;

in vec3 v_Near;
in vec3 v_Far;
in vec3 v_CameraPos;

uniform mat4 u_View;
uniform mat4 u_Projection;

out vec4 frag_color;

// ============================================================================
// CONFIGURATION - Adjust these values to customize the grid appearance
// ============================================================================

// Colors
const vec3 kGridColor = vec3(0.5);              // Grid lines color
const vec3 kAxisXColor = vec3(0.9, 0.15, 0.15); // X axis (red)
const vec3 kAxisYColor = vec3(0.15, 0.9, 0.15); // Y axis (green)
const vec3 kAxisZColor = vec3(0.15, 0.3, 0.9);  // Z axis (blue)

// Grid line settings
const float kGridLineWidth = 0.02;              // Grid line width as fraction of cell (0.02 = 2%)
const float kGrid1Intensity = 0.5;              // 1-unit grid intensity
const float kGrid10Intensity = 0.65;            // 10-unit grid intensity
const float kGrid100Intensity = 0.8;            // 100-unit grid intensity

// Grid level fade thresholds (camera height)
const float kGrid10FadeStart = 2.0;             // Height where 10-unit grid starts appearing
const float kGrid10FadeEnd = 10.0;              // Height where 10-unit grid is fully visible
const float kGrid100FadeStart = 20.0;           // Height where 100-unit grid starts appearing
const float kGrid100FadeEnd = 100.0;            // Height where 100-unit grid is fully visible

// Axis line settings (in pixels)
const float kAxisXZLineWidth = 3.5;             // X and Z axis line width in pixels
const float kAxisYLineWidth = 1.0;              // Y axis line width in pixels
const float kAxisIntensity = 0.95;              // Axis line opacity
const float kAxisYBelowGroundIntensity = 0.5;   // Y axis opacity below ground

// Axis fade settings
const float kAxisFadeMultiplier = 40.0;         // Axis fade distance = camHeight * this value
const float kAxisYFadeMultiplier = 50.0;        // Y axis fade distance multiplier

// ============================================================================
// PRISTINE GRID - Based on "The Best Darn Grid Shader (Yet)" by Ben Golus
// https://bgolus.medium.com/the-best-darn-grid-shader-yet-727f9278b9d8
// ============================================================================

float pristineGrid(vec2 uv, vec2 lineWidth)
{
    // Use length of derivatives instead of fwidth for correct diagonal handling
    vec4 uvDDXY = vec4(dFdx(uv), dFdy(uv));
    vec2 uvDeriv = vec2(length(uvDDXY.xz), length(uvDDXY.yw));
    
    // Handle line widths > 0.5 by inverting
    bvec2 invertLine = greaterThan(lineWidth, vec2(0.5));
    vec2 targetWidth = mix(lineWidth, vec2(1.0) - lineWidth, vec2(invertLine));
    
    // Clamp draw width between derivatives and 0.5
    vec2 drawWidth = clamp(targetWidth, uvDeriv, vec2(0.5));
    
    // Anti-aliasing width (1.5 pixels for proper smoothstep AA)
    vec2 lineAA = uvDeriv * 1.5;
    
    // Grid UV - distance from line centers
    vec2 gridUV = abs(fract(uv) * 2.0 - 1.0);
    gridUV = mix(vec2(1.0) - gridUV, gridUV, vec2(invertLine));
    
    // Draw anti-aliased lines
    vec2 grid2 = smoothstep(drawWidth + lineAA, drawWidth - lineAA, gridUV);
    
    // Fade based on target vs actual width (phone-wire AA technique)
    grid2 *= clamp(targetWidth / drawWidth, 0.0, 1.0);
    
    // Suppress Moiré by fading to solid color when grid cells < 1 pixel
    grid2 = mix(grid2, targetWidth, clamp(uvDeriv * 2.0 - 1.0, 0.0, 1.0));
    
    // Invert back if needed
    grid2 = mix(grid2, vec2(1.0) - grid2, vec2(invertLine));
    
    // Combine axes with premultiplied alpha blend
    return mix(grid2.x, 1.0, grid2.y);
}

// ============================================================================
// DEPTH
// ============================================================================

float computeDepth(vec3 pos)
{
    vec4 clip = u_Projection * u_View * vec4(pos, 1.0);
    float ndc = clip.z / clip.w;
    return (gl_DepthRange.diff * ndc + gl_DepthRange.near + gl_DepthRange.far) * 0.5;
}

// ============================================================================
// MAIN
// ============================================================================

void main()
{
    float t = -v_Near.y / (v_Far.y - v_Near.y);
    vec3 fragPos3D = v_Near + t * (v_Far - v_Near);
    
    frag_color = vec4(0.0);
    gl_FragDepth = 1.0;
    
    if (t > 0.0)
    {
        gl_FragDepth = computeDepth(fragPos3D);
        
        // ============================================================
        // MULTI-SCALE GRID using Pristine Grid technique
        // ============================================================
        
        vec2 lineWidth = vec2(kGridLineWidth);
        
        // Calculate grid at multiple scales
        float grid1 = pristineGrid(fragPos3D.xz, lineWidth);
        float grid10 = pristineGrid(fragPos3D.xz * 0.1, lineWidth);
        float grid100 = pristineGrid(fragPos3D.xz * 0.01, lineWidth);
        
        // Camera height determines which grid levels are visible
        float camHeight = max(abs(v_CameraPos.y), 0.5);
        
        // Fade factors for each level based on camera height
        float fade1 = 1.0;
        float fade10 = smoothstep(kGrid10FadeStart, kGrid10FadeEnd, camHeight);
        float fade100 = smoothstep(kGrid100FadeStart, kGrid100FadeEnd, camHeight);
        
        // Combine grids with height-based visibility
        float gridAlpha = max(max(
            grid1 * kGrid1Intensity * fade1, 
            grid10 * kGrid10Intensity * fade10), 
            grid100 * kGrid100Intensity * fade100);
        
        vec3 color = kGridColor;
        float alpha = gridAlpha;
        
        // ============================================================
        // AXIS LINES (X and Z)
        // ============================================================
        
        vec4 uvDDXY_x = vec4(dFdx(fragPos3D.xz), dFdy(fragPos3D.xz));
        vec2 derivX = vec2(length(uvDDXY_x.xz), length(uvDDXY_x.yw));
        
        // X axis (red) - where Z = 0
        float xAxisWidth = derivX.y * kAxisXZLineWidth;
        float xAxis = 1.0 - smoothstep(0.0, xAxisWidth, abs(fragPos3D.z));
        
        // Z axis (blue) - where X = 0
        float zAxisWidth = derivX.x * kAxisXZLineWidth;
        float zAxis = 1.0 - smoothstep(0.0, zAxisWidth, abs(fragPos3D.x));
        
        // Distance fade for axes
        float distFromCamera = length(fragPos3D - v_CameraPos);
        float axisFadeDist = camHeight * kAxisFadeMultiplier;
        float axisFade = 1.0 - smoothstep(axisFadeDist * 0.3, axisFadeDist, distFromCamera);
        
        // Apply X axis
        if (xAxis > 0.01)
        {
            float xAxisFinal = xAxis * axisFade;
            color = mix(color, kAxisXColor, xAxisFinal);
            alpha = max(alpha, xAxisFinal * kAxisIntensity);
        }
        
        // Apply Z axis
        if (zAxis > 0.01)
        {
            float zAxisFinal = zAxis * axisFade;
            color = mix(color, kAxisZColor, zAxisFinal);
            alpha = max(alpha, zAxisFinal * kAxisIntensity);
        }
        
        frag_color = vec4(color, alpha);
    }
    
    // ============================================================
    // Y AXIS (vertical line at origin)
    // ============================================================
    vec3 rayDir = normalize(v_Far - v_Near);
    vec2 rayXZ = rayDir.xz;
    float denom = dot(rayXZ, rayXZ);
    
    if (denom > 1e-8)
    {
        float tY = -dot(v_Near.xz, rayXZ) / denom;
        if (tY > 0.0)
        {
            vec3 closest = v_Near + tY * rayDir;
            float distXZ = length(closest.xz);
            
            // Compute line width based on distance to camera
            float camDist = length(closest - v_CameraPos);
            float pixelWorldSize = camDist * 0.001;
            float lineW = pixelWorldSize * kAxisYLineWidth;
            float lineAA = pixelWorldSize * 0.6;
            
            float yAxis = 1.0 - smoothstep(lineW - lineAA, lineW + lineAA, distXZ);
            
            // Distance fade
            float camHeight = max(abs(v_CameraPos.y), 0.5);
            float yAxisFadeDist = camHeight * kAxisYFadeMultiplier;
            float yAxisFade = 1.0 - smoothstep(yAxisFadeDist * 0.3, yAxisFadeDist, camDist);
            
            yAxis *= yAxisFade;
            
            if (yAxis > 0.01)
            {
                float intensity = closest.y >= 0.0 ? kAxisIntensity : kAxisYBelowGroundIntensity;
                frag_color.rgb = mix(frag_color.rgb, kAxisYColor, yAxis * intensity);
                frag_color.a = max(frag_color.a, yAxis * intensity);
            }
        }
    }
}
