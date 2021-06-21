#version 410 core

in vec3 myColour;

out vec4 color;

void main()
{
    color = vec4(myColour, 1.0f);
}