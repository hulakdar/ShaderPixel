
const float BOX_FOLD = 1.f;
const float SCALE = 2.5f;
const float MIN_RADIUS = 0.5f;
const float FIXED_RADIUS = 1.f;
const float LINEAR_SCALE = FIXED_RADIUS / MIN_RADIUS;

in vec3 FragPositionMS;
in vec3 FragToCamDirMS;
out vec4 FragColor;

const uint EstimatorIterations = 32;

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

const uint MaxRaySteps = 64;
const float uMinimumDistance = 0.000001;

void trace(vec3 from, vec3 direction, out float AO, out float distance)
{
	float totalDistance = 0.0;
	uint steps;
	for (steps=0; steps < MaxRaySteps; steps++) {
		vec3 p = from + totalDistance * direction;
		float distance = DistanceEstimator(p);
		totalDistance += distance;
		if (distance < uMinimumDistance)
			break;
	}
	AO = 1.f - float(steps)/MaxRaySteps;
    distance = totalDistance;
}

uniform vec3 uCamPosMS;
uniform float uInvDistThreshold;

void main()
{
	vec3 start = uCamPosMS;
	vec3 dir = normalize(-FragToCamDirMS);
    float AO = 0;
    float distance = 0;
	trace(start, dir, AO, distance);
    AO = smoothstep(0.0,1.0,AO);

	float tmp = pow(AO, 0.4545);
	FragColor = vec4(tmp, tmp, tmp, 1);
}
