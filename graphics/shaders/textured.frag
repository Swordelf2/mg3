#version 330 core

in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D texture0;

void main()
{
    //color = vec4(1.0, 0.0, 1.0, 1.0);
    color = texture(texture0, fragTexCoord);
}