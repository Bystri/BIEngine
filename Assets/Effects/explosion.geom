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

vec3 getNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal) {
	float magnitude = 2.0;
    vec3 direction = normal * ((sin(totalTime) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);	
}


void prepareData(int idx, vec3 normal) {
	gl_Position = explode(gl_in[idx].gl_Position, normal);
	gs_out.fragPos = explode(vec4(gs_in[idx].fragPos, 1.0), normal).xyz;
    gs_out.texCoords = gs_in[idx].texCoords;
	gs_out.normal = gs_in[idx].normal;
}


void main() {    
    vec3 normal = getNormal();

    prepareData(0, normal);
    EmitVertex();
	
    prepareData(1, normal);
    EmitVertex();
	
    prepareData(2, normal);
    EmitVertex();
	
    EndPrimitive();
} 