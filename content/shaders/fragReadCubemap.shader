
in vec3 FragPositionMS;

out vec3 color;

uniform samplerCube   uInputTex;

void main(){
    color = texture( uInputTex, normalize(FragPositionMS) ).xyz;
}
