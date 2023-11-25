#version 420 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube cubemapTexture;

void main()
{   
    FragColor = texture(cubemapTexture, TexCoords);
}