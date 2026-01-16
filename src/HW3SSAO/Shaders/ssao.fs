#version 330

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D positionMap;
uniform float sampleRadius;
uniform float kernelRadius;
uniform float kernelSize;
uniform mat4 projection;

const int MAX_KERNEL_SIZE = 128;
uniform vec3 gKernel[MAX_KERNEL_SIZE];

void main()
{
    vec3 Pos = texture(positionMap, TexCoord).xyz;

    float AO = 0.0;

    for (int i = 0 ; i < MAX_KERNEL_SIZE ; i++) {
        vec3 samplePos = Pos + gKernel[i] * kernelRadius;
        vec4 sampleOffset = vec4(samplePos, 1.0);
        sampleOffset = projection * sampleOffset;
        sampleOffset.xy /= sampleOffset.w;
        sampleOffset.xy = sampleOffset.xy * 0.5 + vec2(0.5);

        float sampleDepth = texture(positionMap, sampleOffset.xy).b;

        if (abs(Pos.z - sampleDepth) < sampleRadius) {
            AO += step(sampleDepth,samplePos.z);
        }
    }

    AO = 1.0 - AO/128.0;
    FragColor = vec4(pow(AO, 2.0));
}
