layout(location = 0) in vec3 a_Vertex;

uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * vec4(a_Vertex, 1.0);
}
