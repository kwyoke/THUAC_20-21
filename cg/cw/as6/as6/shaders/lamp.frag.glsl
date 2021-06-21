#version 330 core
out vec4 FragColor;

uniform vec3 lightCol;

void main()
{
    FragColor = vec4(lightCol, 1.0); // set alle 4 vector values to 1.0
}