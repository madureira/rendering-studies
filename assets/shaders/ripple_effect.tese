layout(quads, equal_spacing, ccw) in;

uniform mat4 u_Model;
uniform mat4 u_VP;
uniform float u_Time;
uniform float u_Amplitude;
uniform float u_Frequency;

const float PHASE = 1.0;
const float DAMPING = 0.5;

out float v_Height;
out float v_Time;

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 p0 = gl_in[0].gl_Position;
    vec4 p1 = gl_in[1].gl_Position;
    vec4 p2 = gl_in[2].gl_Position;
    vec4 p3 = gl_in[3].gl_Position;

    vec4 pos = (1.0 - u) * (1.0 - v) * p0
             + u * (1.0 - v) * p1
             + u * v * p2
             + (1.0 - u) * v * p3;

    vec3 worldPos = (u_Model * vec4(pos.xyz, 1.0)).xyz;

    float distance = length(worldPos.xz);
    float dampedAmplitude = u_Amplitude * exp(-DAMPING * distance);
    float height = dampedAmplitude * sin(u_Frequency * distance - PHASE * u_Time);

    vec4 worldPosition = u_Model * vec4(pos.x, pos.y + height, pos.z, 1.0);

    v_Height = height;
    v_Time = u_Time;

    gl_Position = u_VP * worldPosition;
}
