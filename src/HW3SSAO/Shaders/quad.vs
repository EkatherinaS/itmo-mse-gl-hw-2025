#version 330

layout(location=0) in vec2 Position;
layout(location=1) in vec2 TexCoord;

uniform mat4 mvp;

out vec2 FragTexCoord;

void main() {
    FragTexCoord = TexCoord;
    gl_Position = mvp * vec4(Position.xy, 0.0, 1.0);
}
