layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in vec3 v_Normal[];

uniform mat4 u_Projection;
uniform bool u_FaceNormal;

const float MAGNITUDE = 0.2;

void generateLine(int index)
{
    gl_Position = u_Projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = u_Projection * (gl_in[index].gl_Position + vec4(v_Normal[index], 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    if (u_FaceNormal)
    {
        vec4 center = (gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position) / 3.0;
        vec3 edge0 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        vec3 edge1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
        vec3 faceNormal = normalize(cross(edge0, edge1));

        gl_Position = u_Projection * center;
        EmitVertex();
        gl_Position = u_Projection * (center + vec4(faceNormal, 0.0) * MAGNITUDE);
        EmitVertex();
        EndPrimitive();
    }
    else
    {
        generateLine(0);
        generateLine(1);
        generateLine(2);
    }
}
