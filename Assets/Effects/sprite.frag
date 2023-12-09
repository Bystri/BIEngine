#version 420 core

in vec2 TexCoord;
out vec4 color;

uniform sampler2D sprite;
uniform vec3 objectColor;

void main()
{    
    color = vec4(objectColor, 1.0) * texture(sprite, TexCoord);
}  