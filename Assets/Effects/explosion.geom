#version 420 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexData {
    vec2 texCoords;
	vec3 fragPos;
	mat3 TBN;
} gs_in[];

out VertexData {
    vec2 texCoords;
	vec3 fragPos;
	mat3 TBN;
} gs_out;

#include effects/common/scene_uniforms.glsl

vec4 explode(vec4 position, vec3 normal) {
	float magnitude = 2.0;
    vec3 direction = normal * ((sin(totalTime) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);	
}


void prepareData(int idx, vec3 norm) {
	gs_out.fragPos = explode(vec4(gs_in[idx].fragPos, 1.0), norm).xyz;
	gl_Position = projection * view * vec4(gs_out.fragPos, 1.0);
    gs_out.texCoords = gs_in[idx].texCoords;
	gs_out.TBN = gs_in[idx].TBN;
}


void main() {    
    vec3 norm = normalize(cross(gs_in[1].fragPos - gs_in[0].fragPos, gs_in[2].fragPos - gs_in[0].fragPos));

    prepareData(0, norm);
    EmitVertex();
	
    prepareData(1, norm);
    EmitVertex();
	
    prepareData(2, norm);
    EmitVertex();
	
    EndPrimitive();
} 