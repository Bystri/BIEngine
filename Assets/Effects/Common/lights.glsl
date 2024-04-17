#ifndef LIGHT_UNIFORMS_GLSL
#define LIGHT_UNIFORMS_GLSL

struct DirLight {
    vec3 direction;
	float irradiance;
	vec3 color;
};  

struct PointLight {    
    vec3 position;
    float intensity;
    vec3 color;
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

#define MAX_DIRECTIONAL_LIGHTS_NUM 1
#define MAX_POINT_LIGHTS_NUM 1
#define MAX_SPOT_LIGHTS_NUM 2

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