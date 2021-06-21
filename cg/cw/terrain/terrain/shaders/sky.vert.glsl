#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float scale;
uniform int rev;

void main()
{   
    vec3 pos = position;
    if (rev == 1) {
        pos.y *= -1;
    }
    gl_Position = projection * view * model * vec4(pos * scale, 1.0f);

    TexCoords = vec2(texCoord.x, 1.0f - texCoord.y);
}
