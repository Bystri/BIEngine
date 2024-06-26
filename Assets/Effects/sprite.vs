#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoord;

#include effects/common/scene_uniforms.glsl

out vec2 TexCoord;

uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}