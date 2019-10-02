
in vec2 UV;

out vec4 color;

uniform sampler2D   uInputTex;
uniform float       uAlpha = 1.0;

void main(){
    color = vec4(texture( uInputTex, UV ).xyz, uAlpha);
}