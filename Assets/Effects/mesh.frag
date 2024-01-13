#version 420 core

in VertexData {
    vec2 texCoords;
	vec3 fragPos;
	mat3 normalTransform;
} fs_in;

#include effects/common/lights.glsl
#include effects/common/scene_uniforms.glsl

out vec4 FragColor;

void main()
{
	vec3 norm = texture(material.normal, fs_in.texCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	norm = fs_in.normalTransform * norm;
	norm = normalize(norm);
	
	vec3 viewDir = normalize(viewPos-fs_in.fragPos);
	
	FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	
	for(int i = 0; i < dirLightsNum; i++) {
		FragColor += CalculateDirectionalLight(i, norm, fs_in.fragPos, viewDir, fs_in.texCoords);
	}
	
	for(int i = 0; i < pointLightsNum; i++) {
		FragColor += CalculatePointLight(i, pointLights[i], norm, fs_in.fragPos, viewDir, fs_in.texCoords);
	}
	
	for(int i = 0; i < spotLightsNum; i++) {
		FragColor += CalculateSpotLight(spotLights[i], norm, fs_in.fragPos, viewDir, fs_in.texCoords);
	}
}