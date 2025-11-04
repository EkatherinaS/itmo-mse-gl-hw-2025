#version 330 core

layout(location=0) in vec2 vertCoord;

uniform mat4 mvp;

out vec2 fragCoord;

void main() {
    fragCoord = vertCoord;
    gl_Position = mvp * vec4(vertCoord, 0.0, 1.0);
}
