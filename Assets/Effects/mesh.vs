#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out VertexData {
    vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
} vs_out;

#include effects/common/scene_uniforms.glsl

uniform mat4 model;

void main()
{	
	vs_out.texCoords = aTexCoord;
	vs_out.fragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.normal = mat3(transpose(inverse(model))) * aNormal; 
	
	gl_Position = projection * view * vec4(vs_out.fragPos, 1.0);
}