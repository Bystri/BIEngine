#version 420 core

struct Material {
	sampler2D sprite;
	vec4 color;
};

in vec2 TexCoord;
out vec4 FragColor;

uniform Material material;

void main()
{    
    FragColor = material.color * texture(material.sprite, TexCoord);
}  