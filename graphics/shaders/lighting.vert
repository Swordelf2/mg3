#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec2 fragTexCoords;
out vec3 fragNormal;
out vec3 fragPosition;

uniform mat4 fullTransform;
uniform mat4 modelTransform;

void main()
{
    fragTexCoords = texCoords;
    fragPosition = vec3(modelTransform * vec4(position, 1.0));
    fragNormal = mat3(transpose(inverse(modelTransform))) * normal;
    gl_Position = fullTransform * vec4(position, 1.0);
}

