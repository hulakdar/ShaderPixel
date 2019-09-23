
uniform sampler2D uInputTex; // 0
uniform vec2 rcpFrame;
uniform float FXAA_SPAN_MAX = 8.0;
uniform float FXAA_REDUCE_MUL = 1.0/8.0;

in vec4 FragPos;
out vec4 FragColor;

#define FxaaInt2 ivec2
#define FxaaFloat2 vec2
#define FxaaTexLod0(t, p) textureLod(t, p, 0.0)
#define FxaaTexOff(t, p, o, r) textureLodOffset(t, p, 0.0, o)

vec3 FxaaPixelShader( 
  vec4 FragPos, // Output of FxaaVertexShader interpolated across screen.
  sampler2D tex) // Input texture.
{   
/*---------------------------------------------------------*/
    #define FXAA_REDUCE_MIN   (1.0/128.0)
    //#define FXAA_REDUCE_MUL   (1.0/8.0)
    //#define FXAA_SPAN_MAX     8.0
/*---------------------------------------------------------*/
    vec3 rgbNW = FxaaTexLod0(tex, FragPos.zw).xyz;
    vec3 rgbNE = FxaaTexOff(tex, FragPos.zw, FxaaInt2(1,0), rcpFrame).xyz;
    vec3 rgbSW = FxaaTexOff(tex, FragPos.zw, FxaaInt2(0,1), rcpFrame).xyz;
    vec3 rgbSE = FxaaTexOff(tex, FragPos.zw, FxaaInt2(1,1), rcpFrame).xyz;
    vec3 rgbM  = FxaaTexLod0(tex, FragPos.xy).xyz;
/*---------------------------------------------------------*/
    vec3 luma = vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);
/*---------------------------------------------------------*/
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));
/*---------------------------------------------------------*/
    vec2 dir; 
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
/*---------------------------------------------------------*/
    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
        FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(FxaaFloat2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX), 
          max(FxaaFloat2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), 
          dir * rcpDirMin)) * rcpFrame;
/*--------------------------------------------------------*/
    vec3 rgbA = (1.0/2.0) * (
        FxaaTexLod0(tex, FragPos.xy + dir * (1.0/3.0 - 0.5)).xyz +
        FxaaTexLod0(tex, FragPos.xy + dir * (2.0/3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
        FxaaTexLod0(tex, FragPos.xy + dir * (0.0/3.0 - 0.5)).xyz +
        FxaaTexLod0(tex, FragPos.xy + dir * (3.0/3.0 - 0.5)).xyz);
    float lumaB = dot(rgbB, luma);
    if((lumaB < lumaMin) || (lumaB > lumaMax)) return rgbA;
    return rgbB;
}

vec4 PostFX(sampler2D tex, float time)
{
  vec4 c = vec4(0.0);
  c.rgb = FxaaPixelShader(FragPos, tex);
  //c.rgb = 1.0 - texture(tex, FragPos.xy).rgb;
  c.a = 1.0;
  return c;
}
    
void main() 
{ 
  FragColor = PostFX(uInputTex, 0.0);
}