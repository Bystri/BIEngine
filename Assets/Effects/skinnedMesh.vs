#version 420 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoord;
layout (location = 4) in ivec4 boneIds; 
layout (location = 5) in vec4 weights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

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
	
	vec4 totalPosition = vec4(aPos, 1.0);
	
	mat4 boneTransform = mat4(1.0);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
        if(boneIds[i] == -1) {
            continue;
		}
		
		if(boneIds[i] >= MAX_BONES) 
        {
            boneTransform = mat4(1.0);
            break;
        }
		
		if (i == 0) {
			boneTransform = mat4(0.0);
		}
		
        boneTransform += finalBonesMatrices[boneIds[i]] * weights[i];
    }
	
	vs_out.fragPos = vec3(model * boneTransform * totalPosition);
	vec3 T = normalize(vec3(model * boneTransform * vec4(aTangent, 0.0)));
	vec3 N = normalize(vec3(model * boneTransform * vec4(aNormal, 0.0)));
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	vs_out.TBN = mat3(T, B, N);
	gl_Position = projection * view * vec4(vs_out.fragPos, 1.0);
}