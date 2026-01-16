#version 330 core

layout(location=0) in vec3 Position;
layout(location=1) in vec3 Normal;

uniform mat4 mvp;
uniform mat4 model;

uniform float maxSize;

out vec3 FragPos;
out vec3 FragNormal;
out vec2 FragTexCoord;

void main() {
    vec3 NormPos = (1.0 * Position) / maxSize;

    FragPos = (model * vec4(NormPos, 1.0)).xyz;
    FragNormal = Normal;

    vec4 clipPos = mvp * vec4(NormPos, 1.0);
    vec3 ndc = clipPos.xyz / clipPos.w;
    FragTexCoord = ndc.xy * 0.5 + 0.5;

    gl_Position = mvp * vec4(NormPos, 1.0);
}

