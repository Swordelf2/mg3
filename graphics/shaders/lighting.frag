#version 330 core

in vec2 fragTexCoords;
in vec3 fragNormal;
in vec3 fragPosition;
in vec4 fragPosLightSpace;

out vec4 color;

uniform vec3 basicColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D texture0;
uniform sampler2D shadowMap;

float CalculateShadowFactor(vec4 fragPos)
{
    vec3 projCoords = fragPos.xyz / fragPos.w;
    projCoords = 0.5 * projCoords + 0.5;
    float depth = texture(shadowMap, projCoords.xy).r;
    float myDepth = projCoords.z;
    return myDepth - 0.005 > depth ? 1.0 : 0.0;
}

void main()
{
    vec3 fragNormalN = normalize(fragNormal);
    vec3 myColor;
    if (basicColor == vec3(0.0, 0.0, 0.0)) {
        myColor = vec3(texture(texture0, fragTexCoords));
    } else {
        myColor = basicColor;
    }

    // ambient
    vec3 ambientColor = 0.1 * lightColor;

    // diffuse
    vec3 lightDir = normalize(lightPos - fragPosition);
    vec3 diffuseColor = 0.8 * max(dot(lightDir, fragNormalN), 0.0) * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectedDir = reflect(-lightDir, fragNormalN);
    float angleCos = max(dot(reflectedDir, viewDir), 0.0);
    vec3 specularColor = 0.9 * pow(angleCos, 128) * lightColor;


    // shadow
    float shadowFactor = CalculateShadowFactor(fragPosLightSpace);

    vec3 resultColor = (ambientColor +
            (1 - shadowFactor) * (diffuseColor + specularColor))
          * myColor;
    color = vec4(resultColor, 1.0);
}
