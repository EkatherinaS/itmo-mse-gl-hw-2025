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
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;

    vec3 dir = normalize(pos);

    float curR = sqrt(x*x + y*y + z*z);
    float dist = radius - curR;

    return vec3(
        x + dir.x * (dist * morphingScale),
        y + dir.y * (dist * morphingScale),
        z + dir.z * (dist * morphingScale)
    );
}

void main() {
    vec3 normPos = pos / maxSize;
    vec3 morphedPos = getMorphed(normPos, morphingScale, 0.7);

    fragPos = vec3(model * vec4(morphedPos, 1.0));
    fragNormal = normal;
    fragTexCoord = texCoord;

    gl_Position = mvp * vec4(morphedPos, 1.0);
}
