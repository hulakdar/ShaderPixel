
const float BOX_FOLD = 1.f;
const float MIN_RADIUS = 0.5f;
const float FIXED_RADIUS = 1.f;

in vec3 FragPos;
out vec4 FragColor;

float DistanceEstimator(vec3 pos) {
	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < Iterations ; i++) {
		r = length(z);
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


float trace(vec3 from, vec3 direction)
{
	float totalDistance = 0.0;
	int steps;
	for (steps=0; steps < MaximumRaySteps; steps++) {
		vec3 p = from + totalDistance * direction;
		float distance = DistanceEstimator(p);
		totalDistance += distance;
		if (distance < MinimumDistance) break;
	}
	return 1.0-float(steps)/float(MaxRaySteps);
}

void main()
{
    vec3 start = FragPos;
    vec3 dir = FragPos;

    float tmp = trace(start, dir);
    FragColor = vec4(tmp, tmp * tmp, tmp * tmp * tmp, 1);
}