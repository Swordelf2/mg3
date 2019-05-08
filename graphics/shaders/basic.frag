#version 330 core

out vec4 color;

uniform vec3 basicColor;

void main()
{
    color = vec4(basicColor, 1.0);
}
