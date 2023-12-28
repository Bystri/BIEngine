#ifndef LIGHT_UNIFORMS_GLSL
#define LIGHT_UNIFORMS_GLSL

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

struct PointLight {    
    vec3 position;
    float constant;
    vec3 ambient;
	float linear;
    vec3 diffuse;
	float quadratic; 
    vec3 specular;
}; 

struct SpotLight {    
    vec3 position;
	float constant;
    vec3 direction;
    float linear;
    vec3 ambient;
	float quadratic;  
    vec3 diffuse;
	float cutOff;
    vec3 specular;
	float outerCutOff;
}; 

struct Material {
	sampler2D diffuse;
    sampler2D specular;
	vec3 color;
    float shininess;
}; 

#define MAX_DIRECTIONAL_LIGHTS_NUM 4
#define MAX_POINT_LIGHTS_NUM 16
#define MAX_SPOT_LIGHTS_NUM 12

layout (std140, binding = 1) uniform Light
{
	int dirLightsNum;
	int pointLightsNum;
	int spotLightsNum;
	DirLight dirLights[MAX_DIRECTIONAL_LIGHTS_NUM];  
	PointLight pointLights[MAX_POINT_LIGHTS_NUM];
	SpotLight spotLights[MAX_SPOT_LIGHTS_NUM];
};

uniform Material material;

vec4 CalculateDirectionalLight(DirLight light, vec3 normal, vec3 viewDir, vec2 texCoord)
{
	// ambient
    vec3 ambient = light.ambient * material.color * texture(material.diffuse, texCoord).rgb;
  	
    // diffuse 
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));  
    
    // specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
        
    vec3 result = ambient + diffuse + specular;
    return vec4(result, 1.0);
}


vec4 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoord)
{
	float distance = length(light.position-fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	// ambient
    vec3 ambient = light.ambient * material.color * texture(material.diffuse, texCoord).rgb;
  	
    // diffuse 
    vec3 lightDir = normalize(light.position-fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoord));  
    
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
        
    vec3 result = ambient + diffuse + specular;
	result *= attenuation;
    return vec4(result, 1.0);
}


vec4 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoord)
{
	vec3 lightDir = normalize(light.position-fragPos);
	float theta     = dot(lightDir, normalize(-light.direction));
	float epsilon   = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 
    
	float distance = length(light.position-fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// ambient
	vec3 ambient = light.ambient * material.color * texture(material.diffuse, texCoord).rgb;

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

#endif