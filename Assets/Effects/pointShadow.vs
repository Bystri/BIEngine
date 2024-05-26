#version 420 core
layout (location = 0) in vec3 aPos;

uniform mat4 shadowMatrix;
uniform mat4 model;

out vec4 FragPos; 

void main()
{
    FragPos = model * vec4(aPos, 1.0);
    gl_Position = shadowMatrix * FragPos;
} 