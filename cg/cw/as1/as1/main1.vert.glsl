#version 410 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 colour;

out vec3 myColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool colorful;
uniform vec3 defaultColor;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    if (colorful) {
        vec3 ctmp = 1000 * colour;
        myColour = ctmp - floor(ctmp);
    } else {
        myColour = defaultColor;
    }
}
