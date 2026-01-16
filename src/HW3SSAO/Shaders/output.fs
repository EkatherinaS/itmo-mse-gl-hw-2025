#version 330 core

in vec3 FragPos;
in vec3 FragNormal;
in vec2 FragTexCoord;

const int MAX_LIGHT_NUMBER = 16;

struct AmbientLight {
    float intensity;
    vec3 color;
};

struct SpotLight {
    float intensity;
    vec3 color;
    vec3 position;
    vec3 direction;
    float constant;
    float linear;
    float quadratic;
    float specular;
    float cutoff;
    float outerCutoff;
};

uniform bool useTexture;
uniform vec3 viewPos;
uniform sampler2D ssaoTexture;

uniform int spotlightCount;
uniform SpotLight spotlights[MAX_LIGHT_NUMBER];
uniform AmbientLight ambientLight;

out vec4 FragColor;

void main() {
    vec3 normal = normalize(FragNormal);
    float AO = texture(ssaoTexture, FragTexCoord).r;

    vec4 color = vec4(0.8, 0.8, 0.8, 1.0);
    vec3 ambient = ambientLight.intensity * ambientLight.color * (useTexture ? AO : 1.0);
    vec4 result = vec4(ambient, 1.0) * color;

    for (int i = 0; i < spotlightCount; i++) {
        SpotLight light = spotlights[i];

        float lightDist = distance(light.position, FragPos);
        vec3 lightDir = normalize(light.position - FragPos);
        vec3 spotDir = normalize(light.position - light.direction);
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, normal);

        float diffuseFactor = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diffuseFactor * light.color;

        float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = light.specular * specularFactor * light.color;

        float theta = acos(dot(lightDir, spotDir));
        float epsilon = light.outerCutoff - light.cutoff;
        float softEdge = clamp((light.outerCutoff - theta) / epsilon, 0.0, 1.0);
        float attenuation = light.constant + light.linear * lightDist + light.quadratic * lightDist * lightDist;
        float intensity = (light.intensity * softEdge) / attenuation;

        result += vec4(intensity * (diffuse + specular), 1.0);
    }

    FragColor = result;
}
