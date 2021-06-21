#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec2 texCoord2;

out vec2 TexCoords;
out vec2 TexCoords2;
out float clip;

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
    if ((rev == 1 && pos.y > -0.05) || (rev == 0 && pos.y < 0.043))
    {
        clip = 0.0;
    }
    else 
    {
        clip = 1.0;
    }
   
    TexCoords = vec2(texCoord.x, 1.0f - texCoord.y);
    TexCoords2 = vec2(texCoord2.x, 1.0f - texCoord2.y);
}