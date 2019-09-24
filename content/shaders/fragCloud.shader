
in vec3 FragPositionMS;
//in vec3 FragToCamDirMS;
in vec2 UV;
in vec3 Normal;

uniform sampler3D   uVolume;
uniform uint    	uStepLimit = 64;
uniform uint    	uShadowSteps = 32;
uniform float   	uRayStepSize = 1/128.f;
uniform float   	uShadowThreshold = 0.3f;
uniform vec3        uCamPosMS;

vec4 NewTrace(vec3 CurPos, vec3 localcamvec, vec3 LightVector, float ShadowDensity, float Density)
{
	float accumdist = 0;
	float curdensity = 0;
	float transmittance = 1;

	float shadowstepsize = 1 / uShadowSteps;
	LightVector *= shadowstepsize;
	ShadowDensity *= shadowstepsize;

	Density *= uRayStepSize;
	vec3 lightenergy = vec3(0);
	float shadowthresh = -log(uShadowThreshold) / ShadowDensity;

	for (int i = 0; i < uStepLimit; i++)
	{
		float cursample = texture(uVolume, clamp(CurPos, 0.f, 1.f)).r;

		//Sample Light Absorption and Scattering
		if( cursample > 0.001)
		{
			vec3 lpos = CurPos;
			float shadowdist = 0;

			for (int s = 0; s < uShadowSteps; s++)
			{
				lpos += LightVector;
				float lsample = texture(uVolume, clamp(lpos, 0.f, 1.f)).r;

				vec3 shadowboxtest = floor( 0.5 + ( abs( 0.5 - lpos ) ) );
				float exitshadowbox = shadowboxtest .x + shadowboxtest .y + shadowboxtest .z;
				shadowdist += lsample;

				if(shadowdist > shadowthresh || exitshadowbox >= 1)
					break;
			}

			curdensity = clamp(cursample * Density, 0.f, 1.f);
			float shadowterm = exp(-shadowdist * ShadowDensity);
			vec3 absorbedlight = vec3(shadowterm * curdensity);
			lightenergy += absorbedlight * transmittance;
			transmittance *= 1-curdensity;
		}
		CurPos += localcamvec;
	}

	return vec4(lightenergy, transmittance);
}

float sampleVolume(vec3 p)
{
    return texture(uVolume, p + 0.5f).r;
}

float trace(vec3 from, vec3 direction, float rayStepSize, float distanceLimit)
{
    direction *= rayStepSize;
	float totalDistance = 0.0;
	float Density = 0;
	vec3 p = from;
	for (uint i=0; i < uStepLimit; i++) {
		Density += sampleVolume(p);
		p += direction;
		totalDistance += rayStepSize;
        if (totalDistance > distanceLimit)
            break;
	}
	return Density / uStepLimit;
}

out vec4 FragColor;

void main()
{
	// don't forget to add view-space plane-snapping 

    vec3 FragToCamDirMS = uCamPosMS - FragPositionMS;
	vec3 start = (FragPositionMS * 0.05f);
	vec3 dir = normalize(FragToCamDirMS);

    float theta = dot(dir, -Normal);

	float tmp = pow(trace(start, dir, uRayStepSize, length(FragToCamDirMS) * 0.05f), 0.4545);
    tmp *= smoothstep(0.05, 0.09, theta);
	FragColor = vec4(abs(start) * tmp, 1.f); //NewTrace(start, dir, normalize(vec3(1)), 1.f, 50.f);
}
