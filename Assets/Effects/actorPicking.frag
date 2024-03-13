#version 420 core

uniform int actorIndex;

out float FragColor;

void main()
{
    FragColor = float(actorIndex + 1);
}