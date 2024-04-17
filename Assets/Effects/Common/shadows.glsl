#ifndef SHADOWS_GLSL
#define SHADOWS_GLSL

#include effects/common/lights.glsl

struct RenderDirLightShadowInfo {
	sampler2D shadowMap;
	mat4 dirLightSpaceMatrix;
};

struct RenderPointLightShadowInfo {
	samplerCube shadowMap;
	vec3 lightPos;
};

uniform RenderDirLightShadowInfo dirLightShadowInfos[MAX_DIRECTIONAL_LIGHTS_NUM];
uniform RenderPointLightShadowInfo pointLightShadowInfos[MAX_POINT_LIGHTS_NUM];

float CalculateDirShadow(int index, vec3 normal, vec3 lightDir, vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(dirLightShadowInfos[index].shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float bias = max(0.015 * (1.0 - dot(normal, lightDir)), 0.0015);  

	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(dirLightShadowInfos[index].shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(dirLightShadowInfos[index].shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;

	if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float CalculatePointShadow(int index, vec3 fragPos)
{ 
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - pointLightShadowInfos[index].lightPos;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(pointLightShadowInfos[index].shadowMap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
	float far_plane = 25.0f;
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows

	vec3 sampleOffsetDirections[20] = vec3[]
	(
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);   
	
	float shadow = 0.0;
	float bias   = 0.15;
	int samples  = 20;
	float diskRadius = 0.05;
	for(int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(pointLightShadowInfos[index].shadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= far_plane;   // undo mapping [0;1]
		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);  

    return shadow;
}

#endif