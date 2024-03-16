#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoord;

out VertexData {
    vec2 texCoords;
	vec3 fragPos;
	mat3 TBN;
} vs_out;

#include effects/common/scene_uniforms.glsl

uniform mat4 model;

void main()
{	
	vs_out.texCoords = aTexCoord;
	vs_out.fragPos = vec3(model * vec4(aPos, 1.0));
	vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	vs_out.TBN = mat3(T, B, N);
	gl_Position = projection * view * vec4(vs_out.fragPos, 1.0);
}