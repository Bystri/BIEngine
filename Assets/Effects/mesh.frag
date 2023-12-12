#version 420 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

#include effects/common/lights.glsl

out vec4 FragColor;

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(-FragPos);
	
	FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	
	for(int i = 0; i < dirLightsNum; i++) {
		FragColor += CalculateDirectionalLight(dirLights[i], norm, viewDir, TexCoord);
	}
	
	for(int i = 0; i < pointLightsNum; i++) {
		FragColor += CalculatePointLight(pointLights[i], norm, FragPos, viewDir, TexCoord);
	}
	
	for(int i = 0; i < spotLightsNum; i++) {
		FragColor += CalculateSpotLight(spotLights[i], norm, FragPos, viewDir, TexCoord);
	}
}