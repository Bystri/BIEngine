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
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}; 

struct SpotLight {    
    vec3 position;
    vec3 direction;
	
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
	float cutOff;
	float outerCutOff;
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

#endif