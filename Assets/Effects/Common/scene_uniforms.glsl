#ifndef UNIFORMS_GLSL
#define UNIFORMS_GLSL

layout (std140, binding = 0) uniform Global
{
    // trtansformations
	mat4 projection;
    mat4 view;
	vec3 viewPos;
	float totalTime;
};

#endif