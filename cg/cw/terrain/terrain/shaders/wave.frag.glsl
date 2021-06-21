#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D textureSea;

void main()
{
    color = texture(textureSea, TexCoords) * vec4(1.0f,1.0f, 1.0f, 0.7f);
}