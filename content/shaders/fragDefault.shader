
#define DITHER_16 1

in VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec4 FragPosLightSpace;
}   vs_out;

out vec4 fragColor;

uniform float uTime;
uniform sampler2D uDiffuse;
uniform sampler2D uAlpha;
uniform sampler2DShadow uShadow;

#if DITHER_16
// bytes packed in uint fo less cache misses
const uint ArrayBlueNoiseA16x16[64] = uint[](
	0x3083e0fc, 0x449303da, 0x8010e831, 0x17d6ee5d, // 0
	0x486e593a, 0xa4cf26b5, 0xdca9661a, 0x954fc534, // 1
	0x11ca21ba, 0xf0547de9, 0x2091c188, 0x7c2c9e6f, // 2
	0xa9f48ed4, 0x0b396397, 0xf93d4ce1, 0x0de4b405, // 3
	0x733f5ea1, 0x6dd5bd1c, 0xd17a28b1, 0x69468756, // 4
	0x50de1032, 0x82a0fd2d, 0x9a61ca15, 0xf6c614ea, // 5
	0x89b0c082, 0x334a08cc, 0xbc0aa7f5, 0x25ab7336, // 6
	0x6816e74b, 0x5a9478eb, 0xdd418be5, 0xd7925122, // 7
	0x38589979, 0xc418b7a2, 0x7e6b5324, 0x1a5feeb3, // 8
	0xd206aaf1, 0x70db4326, 0xed0fd3af, 0x3ccd049c, // 9
	0xfa72c42b, 0x01ef6184, 0x2ec0973b, 0x6ab88a44, // 10
	0x1d8e47df, 0xa89051ba, 0x5d1cf87c, 0xa012e476, // 11
	0x37b20c5a, 0x2fc80ee6, 0xadc7674d, 0x803454d9, // 12
	0x65d098ec, 0x23d89e7a, 0x133e85e2, 0xcb1ff790, // 13
	0xf34e2875, 0xaf6c4019, 0x29f29c08, 0x41bda56e, // 14
	0xc3a407ad, 0xbefb5c8c, 0xb6cf5676, 0x64880246  // 15
);
#endif

const float limits[16] = float[](
	0.0625, 0.5625, 0.1875, 0.6875,
	0.8125, 0.3125, 0.9375, 0.4375,
	0.25,	0.75,	0.125,	0.625,
	0.9999,	0.5,	0.875,	0.375
);

bool ditheredAlphaTest(float opacity)
{
#if DITHER_16
	const int size = 16;
#else
	const int size = 4;
#endif

    int x = int(gl_FragCoord.x) % size;
    int y = int(gl_FragCoord.y) % size;
    int index = x + y * size;

#if DITHER_16
	uint entry = index / 4;
	uint byteIndex = index % 4;
	uint four = ArrayBlueNoiseA16x16[entry];
	uint byte = (four >> (byteIndex * 8)) & 0xff;
	float limit = byte / 255.0f;
#else
    float limit = limits[index];
#endif

	return (opacity <= limit);
}

bool alphaTest(float a)
{
#if DITHERED
	return (ditheredAlphaTest(a));
#else
	return (a < 0.1f);
#endif
}

uniform vec3 uLightDir = vec3(0.6f);

float shadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    //if (currentDepth > 1.f)
        //return 0.f;
    float bias = 0.; // max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;

    vec2 texelSize = 1.0 / textureSize(uShadow, 0);
    for(int x = -3; x <= 3; ++x)
    {
        for(int y = -3; y <= 3; ++y)
        {
            float pcfDepth = texture(uShadow, projCoords + vec3(x, y, 0.f) * vec3(texelSize * 2.7, 1.f)); 
            shadow += float(currentDepth - bias > pcfDepth);
        }    
    }
    shadow *= 0.0202;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
     if(projCoords.z > 1.0)
         shadow = 0.0;
        
    return shadow;
}

void main()
{
	vec4 albedo_a = texture(uDiffuse, vs_out.TexCoord);
	vec3 albedo = albedo_a.rgb;

#if MASKED

    float D = abs(dot(uLightDir, vs_out.Normal));

	// get alpha from somewhere
# if ALPHA_TEXTURE
	float a = texture(uAlpha, vs_out.TexCoord).r;
# else
	float a = albedo_a.a;
# endif
	//

	float currentDepth = gl_FragCoord.z;
	a = min(a, currentDepth * currentDepth * currentDepth * currentDepth * currentDepth);

	if (alphaTest(smoothstep(.0, .9, a)))
		discard;

#else
	// default opaque case
	const float a = 1;
    float D = max(dot(uLightDir, vs_out.Normal), 0.f);
#endif


    float shadow = shadowCalculation(vs_out.FragPosLightSpace);

    vec3 diffuse = albedo * D * shadow * 0.5;
    vec3 ambient = albedo * 0.15;
	fragColor = vec4(diffuse + ambient, a);
}
