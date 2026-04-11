in float v_StripeCoord;

out vec4 frag_color;

// Stripe frequency: higher = more, thinner stripes
const float stripeFrequency = 4.0;
const vec4 stripeWhite = vec4(1.0, 1.0, 1.0, 1.0);
const vec4 stripeRed = vec4(1.0, 0.0, 0.0, 1.0);

void main()
{
    float stripe = floor(v_StripeCoord * stripeFrequency);
    bool evenStripe = mod(stripe, 2.0) < 1.0;
    frag_color = evenStripe ? stripeWhite : stripeRed;
}
