#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoord;

uniform mat4 mvp;
uniform mat4 model;

uniform float maxSize;
uniform float morphingScale;

out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoord;


vec3 getMorphed(vec3 pos, float morphingScale, float radius) {
    vec3 dir = normalize(pos);
    float dist = radius - length(pos);
    return pos + dir * dist * morphingScale;
}

void main() {
    vec3 normPos = pos / maxSize;
    vec3 morphedPos = getMorphed(normPos, morphingScale, 0.7);

    fragPos = vec3(model * vec4(morphedPos, 1.0));
    fragNormal = normal;
    fragTexCoord = texCoord;

    gl_Position = mvp * vec4(morphedPos, 1.0);
}
