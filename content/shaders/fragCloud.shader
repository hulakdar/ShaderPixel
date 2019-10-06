
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
const float   cRayStepSize = 1.0/cStepLimit;

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

void main()
{
    vec3 FragToCamDirMS = uCamPosMS - FragPositionMS;

	float thickness = 0;
	vec3 start = findIntersection(uCamPosMS * 0.05 + 0.5, -normalize(FragToCamDirMS), thickness);
	vec3 dir = normalize(-FragToCamDirMS);

	FragColor = trace(start, dir, cRayStepSize, thickness);
}
