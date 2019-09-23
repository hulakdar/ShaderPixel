
in vec2 UV;

out vec3 color;

uniform sampler2D   uInputTex;
uniform float       uTime;

void main(){
    //color = texture( uInputTex, UV + 0.005*vec2( sin(uTime+1024.0*UV.x),cos(uTime+768.0*UV.y)) ).xyz;
    color = texture( uInputTex, UV ).xyz;
}