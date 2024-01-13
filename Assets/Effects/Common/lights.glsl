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
	sampler2D normal;
	vec3 color;
    float shininess;
}; 

#define MAX_DIRECTIONAL_LIGHTS_NUM 1
#define MAX_POINT_LIGHTS_NUM 1
#define MAX_SPOT_LIGHTS_NUM 1

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

vec4 CalculateDirectionalLight(int index, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoord)
{
	DirLight light = dirLights[index];

	// ambient
	vec3 diffuseColor = texture(material.diffuse, texCoord).rgb;
	
    vec3 ambient = light.ambient * material.color;
  	
    // diffuse 
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;  
    
    // specular
	vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
        
	vec4 fragPosLightSpace = dirLightShadowInfos[index].dirLightSpaceMatrix * vec4(fragPos, 1.0);
	float shadow = CalculateDirShadow(index, normal, lightDir, fragPosLightSpace);
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * diffuseColor;
    return vec4(result, 1.0);
}


vec4 CalculatePointLight(int index, PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoord)
{
	float distance = length(light.position-fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	
	// ambient
    vec3 ambient = light.ambient * material.color * texture(material.diffuse, texCoord).rgb * 0.3;
  	
    // diffuse 
    vec3 lightDir = normalize(light.position-fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff;  
    
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoord));
        
	float shadow = CalculatePointShadow(index, fragPos);
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * texture(material.diffuse, texCoord).rgb;    
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