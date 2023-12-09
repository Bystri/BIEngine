#version 420 core

struct Material {
	sampler2D diffuse;
    sampler2D specular;
	vec3 color;
    float shininess;
}; 

uniform Material material;

#include effects/common/lights_uniforms.glsl

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

vec4 CalculateDirectionalLight(DirLight light, vec3 normal, vec3 viewDir);
vec4 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir); 
vec4 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(-FragPos);
	
	FragColor = vec4(0.0, 0.0, 0.0, 1.0);
	
	for(int i = 0; i < dirLightsNum; i++) {
		FragColor += CalculateDirectionalLight(dirLights[i], norm, viewDir);
	}
	
	for(int i = 0; i < pointLightsNum; i++) {
		FragColor += CalculatePointLight(pointLights[i], norm, FragPos, viewDir);
	}
	
	for(int i = 0; i < spotLightsNum; i++) {
		FragColor += CalculateSpotLight(spotLights[i], norm, FragPos, viewDir);
	}
}


vec4 CalculateDirectionalLight(DirLight light, vec3 normal, vec3 viewDir)
{
	// ambient
    vec3 ambient = light.ambient * material.color * texture(material.diffuse, TexCoord).rgb;
  	
    // diffuse 
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
        
    vec3 result = ambient + diffuse + specular;
    return vec4(result, 1.0);
}


vec4 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	float distance = length(light.position-fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	// ambient
    vec3 ambient = light.ambient * material.color * texture(material.diffuse, TexCoord).rgb;
  	
    // diffuse 
    vec3 lightDir = normalize(light.position-fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));  
    
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
        
    vec3 result = ambient + diffuse + specular;
	result *= attenuation;
    return vec4(result, 1.0);
}


vec4 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position-fragPos);
	float theta     = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 
    
	float distance = length(light.position-fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// ambient
	vec3 ambient = light.ambient * material.color * texture(material.diffuse, TexCoord).rgb;

	// diffuse 
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));  
	diffuse *= intensity;

	// specular
	vec3 reflectDir = reflect(-lightDir, normal);  
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));
	specular *= intensity;

	vec3 result = ambient + diffuse + specular;
	result *= attenuation;
	return vec4(result, 1.0);
}