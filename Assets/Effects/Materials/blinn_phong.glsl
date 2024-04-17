#ifndef BLINN_PHONG_GLSL
#define BLINN_PHONG_GLSL

#include effects/common/shadows.glsl

struct Material {
	sampler2D diffuse;
    sampler2D specular;
	sampler2D normal;
	sampler2D displacement;
	vec4 color;
    float shininess;
}; 

uniform Material material;

vec4 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoord)
{
	vec3 lightDir = normalize(light.position-fragPos);
	float theta     = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 
    
	float distance = length(light.position-fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    vec3 matColor = material.color.rgb;

	// ambient
	vec3 ambient = light.ambient * matColor * texture(material.diffuse, texCoord).rgb;

	// diffuse 
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));  
	diffuse *= intensity;

	// specular  
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
	specular *= intensity;

	vec3 result = ambient + diffuse + specular;
	result *= attenuation;
	return vec4(result, 1.0);
}

vec4 CalculatePointLight(int index, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoord)
{
	PointLight light = pointLights[index];
	
	vec3 distVec = light.position-fragPos;
	vec3 lightDir = normalize(distVec);
	float geometryFactor = max(dot(normal, lightDir), 0.0) / dot(distVec, distVec);
	vec3 E = light.color * light.intensity * geometryFactor;
	
	vec3 matColor = material.color.rgb;
	
	// diffuse
    vec3 diffuseComponent = matColor * texture(material.diffuse, texCoord).rgb;
    
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float specular = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    vec3 specularComponent = specular * vec3(texture(material.specular, texCoord));
        
	float shadow = CalculatePointShadow(index, fragPos);
    vec3 result = (1.0 - shadow) * (diffuseComponent + specularComponent) * E;    
    return vec4(result, 1.0);
}

vec4 CalculateDirectionalLight(int index, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoord)
{
	DirLight light = dirLights[index];
	vec3 lightDir = normalize(-light.direction);
	vec3 E = light.color * light.irradiance * max(dot(normal, lightDir), 0.0);
	
	vec3 matColor = material.color.rgb;
	
    // diffuse 
    vec3 diffuseComponent = matColor * texture(material.diffuse, texCoord).rgb; 
    
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float specular = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    vec3 specularComponent = specular * vec3(texture(material.specular, texCoord));
        
	vec4 fragPosLightSpace = dirLightShadowInfos[index].dirLightSpaceMatrix * vec4(fragPos, 1.0);
	float shadow = CalculateDirShadow(index, normal, lightDir, fragPosLightSpace);
    vec3 result = (1.0 - shadow) * (diffuseComponent + specularComponent) * E;
    return vec4(result, 1.0);
}

#endif