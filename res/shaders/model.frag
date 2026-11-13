#version 450 core

in vec2 vUV;

layout(binding = 0) uniform sampler2D uTexture;

uniform vec3 uColor;
uniform bool uHasTexture;

out vec4 FragColor;

void main()
{
    vec4 color = vec4(uColor, 1.0);

    if (uHasTexture) color *= texture(uTexture, vUV);

    FragColor = color;
}