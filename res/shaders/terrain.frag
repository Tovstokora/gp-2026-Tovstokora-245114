#version 450 core
in vec3 vColor;
in vec2 vUV;
layout(binding = 0) uniform sampler2D uTexture;
out vec4 FragColor;
void main() {
    vec4 color = texture(uTexture, vUV);
    FragColor = vec4(color.rgb * vColor, color.a);
}