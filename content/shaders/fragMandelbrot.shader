
uniform vec2 uCenter = vec2(0,0);
uniform float uScale = 2;
uniform float uIter = 20;

in vec2 UV;
out vec4 FragColor;

void main()
{
    vec2 z, c;

    c.x = 1.3333 * (UV.x - 0.5) * uScale - uCenter.x;
    c.y = (UV.y - 0.5) * uScale - uCenter.y;

    float i;
    z = c;
    for(i=0; i<uIter; i+=1.f)
    {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;

        if((x * x + y * y) > 4.0) break;
        z.x = x;
        z.y = y;
    }

    float tmp = float(i) / uIter;
    FragColor = vec4(tmp * tmp * tmp, tmp * tmp, tmp, 1);
}