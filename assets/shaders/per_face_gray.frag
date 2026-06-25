out vec4 frag_color;

const float GOLDEN_RATIO_CONJUGATE = 0.61803398875;

void main()
{
    float gray = 0.3 + 0.4 * fract(float(gl_PrimitiveID) * GOLDEN_RATIO_CONJUGATE);
    frag_color = vec4(gray, gray, gray, 1.0);
}
