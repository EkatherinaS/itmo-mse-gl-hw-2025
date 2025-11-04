#version 330 core

in vec2 fragCoord;
out vec4 fragColor;

uniform vec2 center;
uniform float zoom;

uniform vec3 color;
uniform float maxIteration;
uniform float treshold;
uniform float brightness;


float mandelbrot_set (float cx, float cy, float steps, float treshold) {
    float x = 0.0;
    float y = 0.0;
    float i;
    for (i = 0.0; i < steps; i++) {
        if ((x*x + y*y) > treshold) {
          break;
        }
        float xtemp = x*x - y*y + cx;
        y = 2.0*x*y + cy;
        x = xtemp;
    }
    return i;
}

void main() {
    float steps = maxIteration;
    float treshold = treshold;

    vec2 uv = center + fragCoord / zoom;

    float iter = mandelbrot_set(uv.x, uv.y, steps, treshold);
    float value;

    if (iter == maxIteration) {
        value = 0.0;
    }
    else {
        value = iter / steps;
    }
    float shade = brightness * value / (value + 1.0);
    fragColor = vec4(shade * color, 1.0);
}
