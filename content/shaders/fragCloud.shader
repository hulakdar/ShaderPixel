
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
uniform uint    	uStepLimit = 64;
uniform uint    	uShadowSteps = 16;
uniform float   	uRayStepSize = 1/64.f;
uniform vec3        uCamPosMS;

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
	vec3 lightVector = g.lightDir * rayStepSize * 2;
	vec3 p = from;
	vec3 lightEnergy = vec3(0.0);
	float transmittance = 1.0;
	for (uint i=0; i < uStepLimit; i++) {
		float currentSample = sampleVolume(p);
		if (currentSample > 0.001)
		{
			vec3 samplePoint = p;
			float shadowDistance = 0;

			for (int j = 0; j < uShadowSteps; j++)
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
	return vec4(lightEnergy, transmittance);
}

out vec4 FragColor;

void main()
{
    vec3 FragToCamDirMS = uCamPosMS - FragPositionMS;
	vec3 start = (uCamPosMS * 0.05) + 0.5;
	vec3 dir = normalize(-FragToCamDirMS);

	//FragColor = vec4(tmp, 1.f); //NewTrace(start, dir, normalize(vec3(1)), 1.f, 50.f);
	//FragColor = NewTrace(start, dir, g.lightDir, uShadowDensity, uDensity);
	FragColor = trace(start, dir, uRayStepSize, length(FragToCamDirMS));
}
