#version 330

layout (location = 0) in vec3 Position;

uniform mat4 mvp;
uniform mat4 mv;
uniform float maxSize;

out vec3 ViewPos;

void main()
{
    vec3 NormPos = (1.0 * Position) / maxSize;
    ViewPos = (mv * vec4(NormPos, 1.0)).xyz;
    gl_Position = mvp * vec4(NormPos, 1.0);
}
