#version 410 core

#ifdef GL_FRAGMENT_PRECISION_HIGH
  precision highp float;
#else
  precision mediump float;
#endif

in vec3 vNear;
in vec3 vFar;

uniform float uNear;
uniform float uFar;
uniform mat4 uView;
uniform mat4 uProjection;

out vec4 fColor;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
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
float computeDepth(vec3 pos) {
    vec4 clip_space_pos = uProjection * uView * vec4(pos, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w);
    float far  = gl_DepthRange.far;
    float near = gl_DepthRange.near;
    float depth = (((far - near) * clip_space_depth) + near + far) / 2.0;
    return depth;
}
float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = uProjection * uView * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * uNear * uFar) / (uFar + uNear - clip_space_depth * (uFar - uNear)); // get linear value between 0.01 and 100
    return linearDepth / uFar; // normalize
}
void main() {
    float t = -vNear.y / (vFar.y - vNear.y);
    vec3 fragPos3D = vNear + t * (vFar - vNear);
    float is_on = float(t > 0);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    fColor = (grid(fragPos3D, 10, false) + grid(fragPos3D, 1, true)) * float(t > 0); // adding multiple resolution for the grid
    fColor.a *= fading;
}

/*
vec4 grid(vec3 point, float scale, bool is_axis)
{
    vec2 coord = point.xz * scale;
    vec2 dd    = fwidth(coord);
    vec2 uv    = fract(coord - 0.5) - 0.5;
    vec2 grid  = abs(uv) / dd;  // TODO: figure this out, adjust the line thickness
    float line = min(grid.x, grid.y);
    float min_z = min(dd.y, 1.0);
    float min_x = min(dd.x, 1.0);
    vec4 col    = vec4(0.3);
    col.a       = 1.0 - min(line, 1.0);

    if (-1.0 * min_x < point.x && point.x < 0.1 * min_x && is_axis)
    {
        col.rgb = vec3(0.427, 0.792, 0.909);
    }

    if (-1.0 * min_z < point.z && point.z < 0.1 * min_z && is_axis)
    {
        col.rgb = vec3(0.984, 0.380, 0.490);
    }

    return col;
}

float compute_depth(vec3 point)
{
    vec4 clip_space = uProjection * uView * vec4(point, 1.0);
    float clip_space_depth = clip_space.z / clip_space.w;
    float far  = gl_DepthRange.far;
    float near = gl_DepthRange.near;
    float depth = (((far - near) * clip_space_depth) + near + far) / 2.0;
    return depth;
}

float compute_fade(vec3 point)
{
    vec4 clip_space = uProjection * uView * vec4(point, 1.0);
    float clip_space_depth = (clip_space.z / clip_space.w) * 2.0 - 1.0;
    float near = uNear;
    float far  = uFar;
    float linear_depth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near));
    return linear_depth / far;
}

void main()
{
    float t = -vNear.y / (vFar.y - vNear.y);
    vec3  R =  vNear + t * (vFar - vNear);
    float is_on = float(t > 0);

    float fade = max(0, 1.0 - compute_fade(R));
    //float fade = smoothstep(0.04, 0.0, compute_fade(R));
    fColor = grid(R, 1, true);
    //fColor += grid(R, 10, false) * 0.25;
    fColor *= fade;

    fColor *= is_on;
    gl_FragDepth = compute_depth(R);
}
*/
