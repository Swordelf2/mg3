#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec2 fragTexCoords;
out vec3 fragNormal;
out vec3 fragPosition;
out vec4 fragPosLightSpace;

uniform mat4 fullTransform;
uniform mat4 modelTransform;
uniform mat4 lightSpaceTransform;

void main()
{
    fragTexCoords = texCoords;
    fragPosition = vec3(modelTransform * vec4(position, 1.0));
    fragNormal = mat3(transpose(inverse(modelTransform))) * normal;
    fragPosLightSpace = lightSpaceTransform * vec4(fragPosition, 1.0);
    gl_Position = fullTransform * vec4(position, 1.0);
}

