#version 330 core

in vec3 fragNormal;
in vec3 fragPosition;

out vec4 color;

uniform vec3 basicColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vec3 fragNormalN = normalize(fragNormal);

    // ambient
    vec3 ambientColor = 0.1 * lightColor;

    // diffuse
    vec3 lightDir = normalize(lightPos - fragPosition);
    vec3 diffuseColor = max(dot(lightDir, fragNormalN), 0.0) * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectedDir = reflect(-lightDir, fragNormalN);
    float angleCos = max(dot(reflectedDir, viewDir), 0.0);
    vec3 specularColor = 0.5 * pow(angleCos, 32) * lightColor;

    vec3 resultColor = (ambientColor + diffuseColor + specularColor)
            * basicColor;
    color = vec4(resultColor, 1.0);
}
