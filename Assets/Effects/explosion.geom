#version 420 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VertexData {
    vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
} gs_in[];

out VertexData {
    vec2 texCoords;
	vec3 fragPos;
	vec3 normal;
} gs_out;

#include effects/common/scene_uniforms.glsl

vec4 explode(vec4 position, vec3 normal) {
	float magnitude = 2.0;
    vec3 direction = normal * ((sin(totalTime) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);	
}


void prepareData(int idx) {
	gs_out.fragPos = explode(vec4(gs_in[idx].fragPos, 1.0), gs_in[idx].normal).xyz;
	gl_Position = projection * vec4(gs_out.fragPos, 1.0);
    gs_out.texCoords = gs_in[idx].texCoords;
	gs_out.normal = gs_in[idx].normal;
}


void main() {    
    prepareData(0);
    EmitVertex();
	
    prepareData(1);
    EmitVertex();
	
    prepareData(2);
    EmitVertex();
	
    EndPrimitive();
} 