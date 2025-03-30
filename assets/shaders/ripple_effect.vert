#version 410 core

layout(location = 0) in vec3 aPosition;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform float uTime;
uniform float uAmplitude;
uniform float uFrequency;

const float cPhase = 1.0;
const float cDamping = 0.5;

out float vHeight;
out float vTime;

void main()
{
    // Calculate distance from the origin (or center of the RippleEffect)
    float distance = length(aPosition.xz);

    // Apply damping to the amplitude based on distance
    float dampedAmplitude = uAmplitude * exp(-cDamping * distance);

    // Calculate the height of the vertex based on a sine wave
    float height = dampedAmplitude * sin(uFrequency * distance - cPhase * uTime);

    // Transform the vertex position
    vec4 worldPosition = uModel * vec4(aPosition.x, aPosition.y + height, aPosition.z, 1.0);
    gl_Position = uProjection * uView * worldPosition;

    // Pass the height to the fragment shader
    vHeight = height;

    // Pass the time to the fragment shader
    vTime = uTime;
}
