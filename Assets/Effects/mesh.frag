#version 420 core

in VertexData {
    vec2 texCoords;
	vec3 fragPos;
	mat3 TBN;
} fs_in;

#include effects/common/lights.glsl
#include effects/common/scene_uniforms.glsl

out vec4 FragColor;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, vec3 surfNorm)
{ 
	const float height_scale = 0.1;

    const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = mix(maxLayers, minLayers, max(abs(dot(surfNorm, viewDir)), 0.0));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    vec2 P = viewDir.xy * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
	
	vec2  currentTexCoords     = texCoords;
	float currentDepthMapValue = texture(material.displacement, currentTexCoords).r;
	  
	while(currentLayerDepth < currentDepthMapValue)
	{
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(material.displacement, currentTexCoords).r;  
		currentLayerDepth += layerDepth;  
	}

	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(material.displacement, prevTexCoords).r - currentLayerDepth + layerDepth;
	 
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords; 
} 

void main()
{
	vec3 viewDir = normalize(viewPos-fs_in.fragPos);
	vec2 texCoords = ParallaxMapping(fs_in.texCoords, viewDir, fs_in.TBN[2]);

	vec3 norm = texture(material.normal, texCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	norm = fs_in.TBN * norm;
	norm = normalize(norm);
	
	FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	
	for(int i = 0; i < dirLightsNum; i++) {
		FragColor += CalculateDirectionalLight(i, norm, fs_in.fragPos, viewDir, texCoords);
	}
	
	for(int i = 0; i < pointLightsNum; i++) {
		FragColor += CalculatePointLight(i, norm, fs_in.fragPos, viewDir, texCoords);
	}
	
	for(int i = 0; i < spotLightsNum; i++) {
		FragColor += CalculateSpotLight(spotLights[i], norm, fs_in.fragPos, viewDir, texCoords);
	}
}