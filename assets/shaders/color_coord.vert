#version 410 core

layout(location = 0) in vec3 a_Vertex;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Projection;
uniform mat4 u_MV;
uniform bool u_UseModelCoords;

out float vStripeCoord; // X in model or eye space for vertical stripes

void main()
{
    vec4 worldPos = u_MV * vec4(a_Vertex, 1.0);
    vec3 modelCoordPosition = a_Vertex.xyz;
    vec3 eyeCoordPosition = worldPos.xyz;

    // Use X in chosen space for vertical stripes (color varies left-right)
    if (u_UseModelCoords)
    {
        vStripeCoord = modelCoordPosition.x;
    }
    else
    {
        vStripeCoord = eyeCoordPosition.x;
    }

    gl_Position = u_Projection * worldPos;
}
