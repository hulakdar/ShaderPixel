
in vec3 FragPositionMS;
in vec2 UV;
in vec3 Normal;

layout(std140) uniform global
{
    mat4    lightView;
    vec3    lightDir;
    vec3    cameraPosition;
    float   uTime;
}           g;


uniform sampler3D   uVolume;
uniform uint    	uStepLimit = 128;
uniform uint    	uShadowSteps = 32;
uniform float   	uRayStepSize = 1/128.f;
uniform float   	uShadowThreshold = 0.3f;
uniform vec3        uCamPosMS;

float sampleVolume(vec3 p)
{
    ivec3 ts = textureSize(uVolume, 0);
    return texture(uVolume, p).r; //clamp(p + 0.5f, 1.0/ts, vec3(1.0) - 1.0/ts)).r;
}

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
		float cursample = sampleVolume(CurPos);

		//Sample Light Absorption and Scattering
		if( cursample > 0.001)
		{
			vec3 lpos = CurPos;
			float shadowdist = 0;

			for (int s = 0; s < uShadowSteps; s++)
			{
				lpos += LightVector;
				float lsample = sampleVolume(lpos);

				//vec3 shadowboxtest = floor( 0.5 + ( abs( 0.5 - lpos ) ) );
				//float exitshadowbox = shadowboxtest .x + shadowboxtest .y + shadowboxtest .z;
				shadowdist += lsample;

				//if(shadowdist > shadowthresh || exitshadowbox >= 1)
					//break;
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

uniform float uDensity = 10.0;
uniform float uShadowDensity = 1.0;

void main()
{
	// don't forget to add view-space plane-snapping 

    vec3 FragToCamDirMS = uCamPosMS - FragPositionMS;
	vec3 start = (FragPositionMS * 0.05) + 0.5;
	vec3 dir = normalize(FragToCamDirMS);

    //float theta = dot(dir, -Normal);

	//float tmp = trace(start, dir, uRayStepSize, length(FragToCamDirMS) * 0.05f);
    //tmp *= smoothstep(0.05, 0.09, theta);
	//FragColor = vec4(tmp * abs(start), 1.f); //NewTrace(start, dir, normalize(vec3(1)), 1.f, 50.f);
	FragColor = NewTrace(start, dir, g.lightDir, uShadowDensity, uDensity);
}
