#version 410 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform bool u_UseModelCoords;

out float vStripeCoord; // X in model or eye space for vertical stripes

void main()
{
    vec3 modelCoordPosition = a_Position.xyz;
    vec3 eyeCoordPosition = (u_Model * u_View * vec4(a_Position, 1.0)).xyz;

    // Use X in chosen space for vertical stripes (color varies left-right)
    if (u_UseModelCoords)
    {
        vStripeCoord = modelCoordPosition.x;
    }
    else
    {
        vStripeCoord = eyeCoordPosition.x;
    }

    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}
