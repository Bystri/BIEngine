#version 420 core

layout (location = 0) in vec3 aPos;

#include effects/common/scene_uniforms.glsl

out vec3 TexCoords;

uniform mat4 skyboxView;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * skyboxView * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}