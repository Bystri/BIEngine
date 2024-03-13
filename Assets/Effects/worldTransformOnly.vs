#version 420 core
layout (location = 0) in vec3 aPos;

#include effects/common/scene_uniforms.glsl

uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}