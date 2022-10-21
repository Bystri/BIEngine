#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D image;
uniform vec3 objectColor;

void main()
{    
    color = vec4(objectColor, 1.0) * texture(image, TexCoords);
}  