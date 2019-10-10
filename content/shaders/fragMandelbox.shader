
#define DITHER_16 1

const float BOX_FOLD = 1.0;
const float SCALE = 2.1;
const float MIN_RADIUS = 0.5;
const float FIXED_RADIUS = 1.0;
const float LINEAR_SCALE = FIXED_RADIUS / MIN_RADIUS;

in vec3 FragPositionMS;
in vec3 FragToCamDirMS;
out vec4 FragColor;

const uint EstimatorIterations = 14;

float DistanceEstimator(vec3 position)
{
	// The running derivative is z.w
	vec4 z = vec4(position, 1.0);
	vec4 c = z;
	for (int i = 0; i < EstimatorIterations; i++) {
		// Boxfold
		z.xyz = clamp(z.xyz, -BOX_FOLD, BOX_FOLD) * 2.0 - z.xyz;
		// Spherefold
		float zDot = dot(z.xyz, z.xyz);
		if (zDot < MIN_RADIUS) z *= LINEAR_SCALE;
		else if (zDot < FIXED_RADIUS) z *= FIXED_RADIUS / zDot;
		z = SCALE * z + c;
	}
	return length(z.xyz) / abs(z.w);
}

const float Bailout = 8.f;
const float Power = 2.f;
const int Iterations = 14;

float DistanceEstimatorBulb(vec3 pos) {
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < Iterations ; i++) {
		r = dot(z,z);
		if (r>Bailout) break;
		
		// convert to polar coordinates
		float theta = acos(z.z/r);
		float phi = atan(z.y,z.x);
		dr =  pow( r, Power-1.0)*Power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow( r,Power);
		theta = theta*Power;
		phi = phi*Power;
		
		// convert back to cartesian coordinates
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		z+=pos;
	}
	return 0.5*log(r)*r/dr;
}

const uint MaxRaySteps = 36;
const float uMinimumDistance = 0.0002;

void trace(vec3 from, vec3 direction, out float AO, out float distance)
{
	float totalDistance = 0.0;
	uint steps;
	float limit = length(FragToCamDirMS);
	for (steps=0; steps < MaxRaySteps; steps++) {
		vec3 p = from + totalDistance * direction;
		float distance = DistanceEstimator(p);
		totalDistance += distance;
		if (totalDistance > limit)
			discard;
		if (distance < uMinimumDistance)
			break;
	}
	AO = 1.f - float(steps)/MaxRaySteps;
    distance = totalDistance;
}

uniform vec3 uCamPosMS;
uniform float uInvDistThreshold;


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
    return color + getLimit() / 32.0 - 1.0/64.0;
}

void main()
{
	vec3 start = uCamPosMS;
	vec3 dir = normalize(-FragToCamDirMS);
    float AO = 0;
    float distance = 0;
	trace(start, dir, AO, distance);
    AO = smoothstep(0.0,0.7,AO);
	AO = pow(AO, 0.4545);

    vec3 end = (start + dir * distance);
	FragColor = vec4(dither(vec3(AO))/* * abs(sin(end) + abs(cos(end))) */, 1);
}
