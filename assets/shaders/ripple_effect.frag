#version 410 core

in float vHeight; // Receive height from vertex shader
in float vTime; // Receive time from vertex shader

out vec4 fColor;

void main()
{
    // Calculate the color based on the height and time
    float r = 0.5 + 0.5 * sin(vHeight * 10.0 + vTime);
    float g = 0.5 + 0.5 * sin(vHeight * 10.0 + vTime + 2.0);
    float b = 0.5 + 0.5 * sin(vHeight * 10.0 + vTime + 4.0);

    fColor = vec4(r, g, b, 1.0); // Set the fragment color
}
