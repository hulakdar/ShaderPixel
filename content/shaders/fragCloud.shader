#define DITHER_16 1

in vec3 FragPositionMS;
in vec2 UV;
in vec3 Normal;

layout(std140) uniform global
{
    mat4    lightView;
    vec4    lightDir;
    vec4    cameraPosition;
    float   uTime;
}           g;


uniform sampler3D   uVolume;
uniform vec3        uCamPosMS;

const uint    cStepLimit = 64;
const uint    cShadowSteps = 8;
const float   cRayStepSize = 1.0 / float(cStepLimit);

float sampleVolume(vec3 p)
{
    return texture(uVolume, p).r;
}

uniform float uDensity = 1.0;
uniform float uShadowDensity = 1.0;

vec4 trace(vec3 from, vec3 direction, float rayStepSize, float distanceLimit)
{
    direction *= rayStepSize;
	float totalDistance = 0.0;
	vec3 lightVector = g.lightDir.xyz * rayStepSize * 4;
	vec3 p = from;
	vec3 lightEnergy = vec3(0.0);
	float transmittance = 1.0;
	for (uint i=0; i < cStepLimit; i++) {
		float currentSample = sampleVolume(p);
		if (currentSample > 0.001)
		{
			vec3 samplePoint = p;
			float shadowDistance = 0;

			for (int j = 0; j < cShadowSteps; j++)
			{
				samplePoint += lightVector;
				vec3 shadowboxtest = floor( vec3(0.5) + ( abs( vec3(0.5) - samplePoint ) ) );
				float exitshadowbox = shadowboxtest .x + shadowboxtest .y + shadowboxtest .z;
				if(shadowDistance > 2 || exitshadowbox >= 1)
					break;

				shadowDistance += sampleVolume(samplePoint);
			}
			float currentDensity = currentSample * uDensity;

			float shadow = exp(-shadowDistance * uShadowDensity);
			float absorbedLight = shadow * currentDensity;
			lightEnergy += absorbedLight * transmittance;
			transmittance *= 1 - currentDensity;
		}
		p += direction;
		totalDistance += rayStepSize;
        if (totalDistance > distanceLimit)
            break;
	}
	transmittance = min(transmittance, 1.0);
	return vec4(lightEnergy, transmittance);
}

out vec4 FragColor;

vec3 findIntersection(vec3 CamPosMS, vec3 CamToFragDirMS, out float Thickness)
{
	vec3 invRayDir = 1 / CamToFragDirMS;

	vec3 FirstIntersection = -CamPosMS * invRayDir;
	vec3 SecondIntersection = (1 - CamPosMS) * invRayDir;
	vec3 Closest = min(FirstIntersection, SecondIntersection);
	vec3 Furthest = max(FirstIntersection, SecondIntersection);

	float t0 = max(Closest.x, max(Closest.y, Closest.z));
	float t1 = min(Furthest.x, min(Furthest.y, Furthest.z));

	float PlaneOffset = 1-fract( ( t0 - length(CamPosMS-0.5) ) * cStepLimit );

	t0 += (PlaneOffset / cStepLimit);
	t0 = max(0, t0);

	Thickness = max(0, t1 - t0);
	vec3 entrypos = CamPosMS + (max(0,t0) * CamToFragDirMS);

	return entrypos;
}

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
#else
const float limits[16] = float[](
	0.0625, 0.5625, 0.1875, 0.6875,
	0.8125, 0.3125, 0.9375, 0.4375,
	0.25,	0.75,	0.125,	0.625,
	0.9999,	0.5,	0.875,	0.375
);
#endif

float getLimit()
{
#if DITHER_16
	const uint size = 16;
#else
	const uint size = 4;
#endif

    uint x = uint(gl_FragCoord.x) % size;
    uint y = uint(gl_FragCoord.y) % size;
    uint index = x + y * size;

#if DITHER_16
	uint entry = index / 4;
	uint byteIndex = index % 4;
	uint four = ArrayBlueNoiseA16x16[entry];
	uint byte = (four >> (byteIndex * 8)) & uint(0xff);
	float limit = byte / 255.0f;
#else
    float limit = limits[index];
#endif
    return limit;
}

vec3 dither(vec3 color)
{
    return color + getLimit() / 32.0 - 1.0/128.0;
}

void main()
{
    vec3 FragToCamDirMS = uCamPosMS - FragPositionMS;

	float thickness = 0;
	vec3 start = findIntersection(uCamPosMS * 0.05 + 0.5, -normalize(FragToCamDirMS), thickness);
	vec3 dir = normalize(-FragToCamDirMS);

	FragColor = trace(start, dir, cRayStepSize, thickness);
    FragColor.xyz = dither(FragColor.xyz);
}
