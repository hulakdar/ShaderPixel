
in vec2 UV;

out vec3 color;

uniform sampler2D   uInputTex;
uniform float       uTime;
uniform float       uBrightnessThreshold;

void main(){
    vec3 luma = vec3(0.2126, 0.7152, 0.0722);
    vec3 src = texture( uInputTex, UV ).xyz;
    float brightness = dot(src, luma);
    color = src * smoothstep(uBrightnessThreshold, 1.0, brightness);
}