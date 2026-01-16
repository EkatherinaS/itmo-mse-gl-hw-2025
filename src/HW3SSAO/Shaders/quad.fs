#version 330

uniform sampler2D outputTexture;

in vec2 FragTexCoord;
out vec4 FragColor;

void main() {
    float depth = texture(outputTexture, FragTexCoord).r;
    FragColor = vec4(vec3(depth), 1.0);
}
