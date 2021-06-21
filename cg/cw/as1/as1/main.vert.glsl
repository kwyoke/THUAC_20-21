#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool colorful;
uniform vec3 defaultColor;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    if (colorful) {
        vec3 c = 100 * color;
        ourColor = c - floor(c);
    } else {
        ourColor = defaultColor;
    }
}
