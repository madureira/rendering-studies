#version 410 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 vertexColor;
out vec2 TexCoord;

// Uniforms for transformation matrices
uniform mat4 u_model;       // Model matrix: transforms from local space to world space
uniform mat4 u_view;        // View matrix: transforms from world space to camera space (view space)
uniform mat4 u_projection;  // Projection matrix: transforms from camera space to clip space

void main()
{
	// Apply the transformations to the vertex position
    gl_Position = u_projection * u_view * u_model * vec4(position, 1.0);

	vertexColor = color;

	TexCoord = vec2(texCoord.x, texCoord.y);
}
