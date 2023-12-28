#version 420 core

struct Material {
	sampler2D sprite;
	vec3 color;
};

in vec2 TexCoord;
out vec4 FragColor;

uniform Material material;

void main()
{    
    FragColor = vec4(material.color, 1.0) * texture(material.sprite, TexCoord);
}  