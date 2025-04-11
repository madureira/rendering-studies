#version 410 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

uniform float u_Time;
uniform float u_Amplitude;
uniform float u_Frequency;

const float PHASE = 1.0;
const float DAMPING = 0.5;

out float v_Height;
out float v_Time;

void main()
{
    // Calculate distance from the origin (or center of the RippleEffect)
    float distance = length(a_Position.xz);

    // Apply damping to the amplitude based on distance
    float dampedAmplitude = u_Amplitude * exp(-DAMPING * distance);

    // Calculate the height of the vertex based on a sine wave
    float height = dampedAmplitude * sin(u_Frequency * distance - PHASE * u_Time);

    // Transform the vertex position
    vec4 worldPosition = u_Model * vec4(a_Position.x, a_Position.y + height, a_Position.z, 1.0);
    gl_Position = u_Projection * u_View * worldPosition;

    // Pass the height to the fragment shader
    v_Height = height;

    // Pass the time to the fragment shader
    v_Time = u_Time;
}
