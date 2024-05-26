#version 420 core

in vec4 FragPos;

uniform vec3 lightPos;

out float FragColor;

void main()
{
    // get distance between fragment and light source
    FragColor = length(FragPos.xyz - lightPos);
}  