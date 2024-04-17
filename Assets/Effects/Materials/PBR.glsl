#ifndef PBR_GLSL
#define PBR_GLSL

#include effects/common/shadows.glsl

struct Material {
	sampler2D albedoMap;
    sampler2D normalMap;
	sampler2D metallicMap;
	sampler2D roughnessMap;
	sampler2D aoMap;
}; 

uniform Material material;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 PbrUtilCalcLo(vec3 normal, vec3 viewDir, vec3 L, vec3 H, vec3 radiance, vec3 F0, vec3 albedo, float metallic, float roughness) 
{
	// Cook-Torrance BRDF
	float NDF = DistributionGGX(normal, H, roughness);   
	float G   = GeometrySmith(normal, viewDir, L, roughness);      
	vec3 F    = fresnelSchlick(max(dot(H, viewDir), 0.0), F0);
	   
	vec3 numerator    = NDF * G * F; 
	float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
	vec3 specular = numerator / denominator;
	
	// kS is equal to Fresnel
	vec3 kS = F;
	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// multiply kD by the inverse metalness such that only non-metals 
	// have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
	kD *= 1.0 - metallic;	  

	// scale light by NdotL
	float NdotL = max(dot(normal, L), 0.0);        

	// add to outgoing radiance Lo
	return (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}

vec3 CalculateDirectionalLightLo(int index, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float metallic, float roughness)
{
	DirLight light = dirLights[index];

	// calculate per-light radiance
	vec3 L = normalize(-light.direction);
	vec3 H = normalize(viewDir + L);
	vec3 radiance = light.color * light.irradiance;

	vec3 Lo = PbrUtilCalcLo(normal, viewDir, L, H, radiance, F0, albedo, metallic, roughness);
	
	vec4 fragPosLightSpace = dirLightShadowInfos[index].dirLightSpaceMatrix * vec4(fragPos, 1.0);
	float shadow = CalculateDirShadow(index, normal, L, fragPosLightSpace);
    vec3 result = (1.0 - shadow) * Lo; 
	
	return result;
}


vec3 CalculatePointLightLo(int index, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float metallic, float roughness)
{
	PointLight light = pointLights[index];
	
	// calculate per-light radiance
	vec3 L = normalize(light.position-fragPos);
	vec3 H = normalize(viewDir + L);
	float distance = length(light.position-fragPos);
	float attenuation = 1.0 / (distance * distance);
	vec3 radiance = light.color * attenuation;

	vec3 Lo = PbrUtilCalcLo(normal, viewDir, L, H, radiance, F0, albedo, metallic, roughness);
	
	float shadow = CalculatePointShadow(index, fragPos);
    vec3 result = (1.0 - shadow) * Lo; 
	
	return result;
}


vec3 CalculateSpotLightLo(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 F0, vec3 albedo, float metallic, float roughness)
{
	vec3 lightDir = normalize(light.position-fragPos);
	float theta     = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 
	
	// calculate per-light radiance
	vec3 L = normalize(-light.direction);
	vec3 H = normalize(viewDir + L);
	float distance = length(light.position-fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	vec3 radiance = light.diffuse * attenuation;

	vec3 Lo = PbrUtilCalcLo(normal, viewDir, L, H, radiance, F0, albedo, metallic, roughness);
	
	return Lo;
}

#endif