#version 420 core

in vec3 Normal;

in VertexData {
    vec2 texCoords;
	vec3 fragPos;
	mat3 TBN;
} fs_in;

#include effects/materials/PBR.glsl
#include effects/common/scene_uniforms.glsl

out vec4 FragColor;

void main()
{		
    vec3 albedo     = pow(texture(material.albedoMap, fs_in.texCoords).rgb, vec3(2.2));
    float metallic  = texture(material.metallicMap, fs_in.texCoords).r;
    float roughness = texture(material.roughnessMap, fs_in.texCoords).r;
    float ao        = texture(material.aoMap, fs_in.texCoords).r;

	vec3 viewDir = normalize(viewPos-fs_in.fragPos);

    vec3 norm = texture(material.normalMap, fs_in.texCoords).rgb;
	norm = normalize(norm * 2.0 - 1.0);
	norm = fs_in.TBN * norm;
	norm = normalize(norm);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < dirLightsNum; i++) {
		Lo += CalculateDirectionalLightLo(i, norm, fs_in.fragPos, viewDir, F0, albedo, metallic, roughness);
	}
	
	for(int i = 0; i < pointLightsNum; i++) {
		Lo += CalculatePointLightLo(i, norm, fs_in.fragPos, viewDir, F0, albedo, metallic, roughness);
	}
	
	for(int i = 0; i < spotLightsNum; i++) {
		Lo += CalculateSpotLightLo(spotLights[i], norm, fs_in.fragPos, viewDir, F0, albedo, metallic, roughness);
	}
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;
    
    vec3 color = ambient + Lo;

    FragColor = vec4(Lo, 1.0);
}