#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

out vec2 fragTexCoord;

void main()
{
    fragTexCoord = texCoord;
    gl_Position = vec4(pos.xy, 0.9999999, 1.0);
}
