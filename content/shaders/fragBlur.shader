
in vec2 UV;

out vec4 color;

uniform sampler2D   uInputTex;
uniform vec2        uBlurDir;

uniform float offset[3] = float[](0.0, 1.3846153846, 3.2307692308);
uniform float weight[3] = float[](0.2270270270, 0.3162162162, 0.0702702703);

void main(){
    vec2 tex_offset = 1.0 / textureSize(uInputTex, 0); // gets size of single texel
    vec3 result = texture(uInputTex, UV).rgb * weight[0]; // current fragment's contribution
    for(int i = 1; i < 3; i++)
    {
        result += texture(uInputTex, vec2(UV) + uBlurDir * offset[i] * tex_offset).rgb * weight[i];
        result += texture(uInputTex, vec2(UV) - uBlurDir * offset[i] * tex_offset).rgb * weight[i];
    }
    color = vec4(result, 1.0);
}