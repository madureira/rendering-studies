in float v_Height;
in float v_Time;

out vec4 frag_color;

void main()
{
    float r = 0.5 + 0.5 * sin(v_Height * 10.0 + v_Time);
    float g = 0.5 + 0.5 * sin(v_Height * 10.0 + v_Time + 2.0);
    float b = 0.5 + 0.5 * sin(v_Height * 10.0 + v_Time + 4.0);

    frag_color = vec4(r, g, b, 1.0);
}
