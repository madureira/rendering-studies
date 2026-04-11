layout(vertices = 4) out;

uniform int u_TessLevel = 128;

void main()
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = float(u_TessLevel);
        gl_TessLevelOuter[1] = float(u_TessLevel);
        gl_TessLevelOuter[2] = float(u_TessLevel);
        gl_TessLevelOuter[3] = float(u_TessLevel);
        gl_TessLevelInner[0] = float(u_TessLevel);
        gl_TessLevelInner[1] = float(u_TessLevel);
    }

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
