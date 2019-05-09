#version 330 core

in vec2 fragTexCoords;

out vec4 color;

uniform sampler2D texture0;

void main()
{
    float depthVal = texture(texture0, fragTexCoords).r;
    color = vec4(vec3(depthVal), 1.0);
}
